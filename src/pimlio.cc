#include "../piml.hh"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace piml {

    namespace {
    
        using byte = std::uint8_t;
        
        // ------------------------------------------------------------
        // Basic helpers
        // ------------------------------------------------------------
        
        static const std::uint8_t PNG_SIGNATURE[8] = {
            137, 80, 78, 71, 13, 10, 26, 10
        };
        
        std::uint32_t read_be32(const std::vector<byte>& data, std::size_t pos)
        {
            if (pos + 4 > data.size())
                throw std::runtime_error("Unexpected end of PNG.");
        
            return (std::uint32_t(data[pos]) << 24) |
                   (std::uint32_t(data[pos + 1]) << 16) |
                   (std::uint32_t(data[pos + 2]) << 8) |
                   std::uint32_t(data[pos + 3]);
        }
        
        std::uint16_t read_be16(const std::vector<byte>& data, std::size_t pos)
        {
            if (pos + 2 > data.size())
                throw std::runtime_error("Unexpected end of PNG.");
        
            return (std::uint16_t(data[pos]) << 8) |
                   std::uint16_t(data[pos + 1]);
        }
        
        void write_be32(std::vector<byte>& out, std::uint32_t value)
        {
            out.push_back(static_cast<byte>((value >> 24) & 0xff));
            out.push_back(static_cast<byte>((value >> 16) & 0xff));
            out.push_back(static_cast<byte>((value >> 8) & 0xff));
            out.push_back(static_cast<byte>(value & 0xff));
        }
        
        // ------------------------------------------------------------
        // CRC32
        // ------------------------------------------------------------
        
        std::uint32_t crc32(const byte* data, std::size_t size)
        {
            std::uint32_t crc = 0xffffffffu;
        
            for (std::size_t i = 0; i < size; ++i) {
                crc ^= data[i];
            
                for (int j = 0; j < 8; ++j) {
                    if (crc & 1u)
                        crc = (crc >> 1) ^ 0xedb88320u;
                    else
                        crc >>= 1;
                }
            }
        
            return crc ^ 0xffffffffu;
        }
        
        std::uint32_t crc32_chunk(const char type[4],
                                  const std::vector<byte>& data)
        {
            std::vector<byte> buffer;
            buffer.reserve(4 + data.size());
        
            for (int i = 0; i < 4; ++i)
                buffer.push_back(static_cast<byte>(type[i]));
        
            buffer.insert(buffer.end(), data.begin(), data.end());
        
            return crc32(buffer.data(), buffer.size());
        }
        
        // ------------------------------------------------------------
        // Adler32
        // ------------------------------------------------------------
        
        std::uint32_t adler32(const std::vector<byte>& data)
        {
            constexpr std::uint32_t MOD = 65521;
        
            std::uint32_t a = 1;
            std::uint32_t b = 0;
        
            for (byte value : data) {
                a += value;
                if (a >= MOD)
                    a -= MOD;
            
                b += a;
                if (b >= MOD)
                    b -= MOD;
            }
        
            return (b << 16) | a;
        }
        
        // ------------------------------------------------------------
        // Bit reader for DEFLATE
        // ------------------------------------------------------------
        
        class BitReader {
            const std::vector<byte>& data;
            std::size_t byte_pos = 0;
            std::uint64_t buffer = 0;
            unsigned bits = 0;
        
        public:
            explicit BitReader(const std::vector<byte>& input)
                : data(input)
            {}
        
            std::uint32_t get(unsigned count)
            {
                if (count == 0)
                    return 0;
            
                if (count > 32)
                    throw std::runtime_error("Invalid DEFLATE bit count.");
            
                while (bits < count) {
                    if (byte_pos >= data.size())
                        throw std::runtime_error("Unexpected end of DEFLATE stream.");
                
                    buffer |=
                        (std::uint64_t(data[byte_pos++]) << bits);
                
                    bits += 8;
                }
            
                std::uint32_t result =
                    static_cast<std::uint32_t>(buffer &
                        (count == 32
                            ? 0xffffffffull
                            : ((1ull << count) - 1ull)));
                        
                buffer >>= count;
                bits -= count;
                        
                return result;
            }
        
            void align()
            {
                unsigned drop = bits % 8;
            
                if (drop != 0) {
                    buffer >>= drop;
                    bits -= drop;
                }
            }
        };
        
        // ------------------------------------------------------------
        // Huffman decoder
        // ------------------------------------------------------------
        
        std::uint32_t reverse_bits(std::uint32_t value, int count)
        {
            std::uint32_t result = 0;
        
            for (int i = 0; i < count; ++i) {
                result = (result << 1) | (value & 1u);
                value >>= 1;
            }
        
            return result;
        }
        
        class HuffmanTable {
            struct Entry {
                std::uint32_t code = 0;
                int length = 0;
                int symbol = -1;
            };
        
            std::vector<Entry> entries;
        
        public:
            void build(const std::vector<int>& lengths)
            {
                entries.clear();
            
                int max_bits = 0;
            
                for (int len : lengths) {
                    if (len < 0 || len > 15)
                        throw std::runtime_error("Invalid Huffman code length.");
                
                    max_bits = std::max(max_bits, len);
                }
            
                if (max_bits == 0)
                    throw std::runtime_error("Empty Huffman tree.");
            
                std::vector<int> count(max_bits + 1, 0);
            
                for (int len : lengths) {
                    if (len > 0)
                        ++count[len];
                }
            
                std::vector<int> next_code(max_bits + 1, 0);
            
                int code = 0;
            
                for (int bits = 1; bits <= max_bits; ++bits) {
                    code = (code + count[bits - 1]) << 1;
                    next_code[bits] = code;
                }
            
                for (std::size_t symbol = 0;
                     symbol < lengths.size();
                     ++symbol) {
                    
                    int len = lengths[symbol];
                    
                    if (len == 0)
                        continue;
                    
                    std::uint32_t canonical =
                        static_cast<std::uint32_t>(next_code[len]++);
                    
                    Entry entry;
                    entry.code = reverse_bits(canonical, len);
                    entry.length = len;
                    entry.symbol = static_cast<int>(symbol);
                    
                    entries.push_back(entry);
                }
            }
        
            int decode(BitReader& reader) const
            {
                std::uint32_t code = 0;
            
                for (int bits = 1; bits <= 15; ++bits) {
                    code |= reader.get(1) << (bits - 1);
                
                    for (const Entry& entry : entries) {
                        if (entry.length == bits &&
                            entry.code == code) {
                            return entry.symbol;
                        }
                    }
                }
            
                throw std::runtime_error("Invalid Huffman code.");
            }
        };
        
        // ------------------------------------------------------------
        // Fixed Huffman trees
        // ------------------------------------------------------------
        
        void build_fixed_trees(HuffmanTable& literal,
                               HuffmanTable& distance)
        {
            std::vector<int> literal_lengths(288);
        
            for (int i = 0; i <= 143; ++i)
                literal_lengths[i] = 8;
        
            for (int i = 144; i <= 255; ++i)
                literal_lengths[i] = 9;
        
            for (int i = 256; i <= 279; ++i)
                literal_lengths[i] = 7;
        
            for (int i = 280; i <= 287; ++i)
                literal_lengths[i] = 8;
        
            std::vector<int> distance_lengths(32, 5);
        
            literal.build(literal_lengths);
            distance.build(distance_lengths);
        }
        
        // ------------------------------------------------------------
        // DEFLATE tables
        // ------------------------------------------------------------
        
        const int LENGTH_BASE[29] = {
             3,   4,   5,   6,   7,   8,   9,  10,
            11,  13,  15,  17,  19,  23,  27,  31,
            35,  43,  51,  59,  67,  83,  99, 115,
           131, 163, 195, 227, 258
        };
        
        const int LENGTH_EXTRA[29] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 3, 3, 4, 4, 4, 4,
            5, 5, 5, 5, 0
        };
        
        const int DIST_BASE[30] = {
               1,    2,    3,    4,    5,    7,    9,   13,
              17,   25,   33,   49,   65,   97,  129,  193,
             257,  385,  513,  769, 1025, 1537, 2049, 3073,
            4097, 6145, 8193,12289,16385,24577
        };
        
        const int DIST_EXTRA[30] = {
            0, 0, 0, 0, 1, 1, 2, 2,
            3, 3, 4, 4, 5, 5, 6, 6,
            7, 7, 8, 8, 9, 9,10,10,
           11,11,12,12,13,13
        };
        
        // ------------------------------------------------------------
        // Dynamic Huffman trees
        // ------------------------------------------------------------
        
        void build_dynamic_trees(BitReader& reader,
                                 HuffmanTable& literal,
                                 HuffmanTable& distance)
        {
            int HLIT  = static_cast<int>(reader.get(5)) + 257;
            int HDIST = static_cast<int>(reader.get(5)) + 1;
            int HCLEN = static_cast<int>(reader.get(4)) + 4;
        
            if (HLIT > 286 || HDIST > 32 || HCLEN > 19)
                throw std::runtime_error("Invalid dynamic DEFLATE header.");
        
            static const int order[19] = {
                16, 17, 18, 0, 8, 7, 9, 6, 10,
                 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
            };
        
            std::vector<int> code_lengths(19, 0);
        
            for (int i = 0; i < HCLEN; ++i)
                code_lengths[order[i]] =
                    static_cast<int>(reader.get(3));
        
            HuffmanTable code_length_tree;
            code_length_tree.build(code_lengths);
        
            std::vector<int> lengths;
            lengths.reserve(HLIT + HDIST);
        
            while (static_cast<int>(lengths.size()) < HLIT + HDIST) {
                int symbol = code_length_tree.decode(reader);
            
                if (symbol >= 0 && symbol <= 15) {
                    lengths.push_back(symbol);
                }
                else if (symbol == 16) {
                    if (lengths.empty())
                        throw std::runtime_error(
                            "Invalid DEFLATE repeat code.");
                        
                    int repeat =
                        static_cast<int>(reader.get(2)) + 3;
                        
                    int value = lengths.back();
                        
                    if (static_cast<int>(lengths.size()) + repeat >
                        HLIT + HDIST)
                        throw std::runtime_error(
                            "DEFLATE code-length overflow.");
                        
                    for (int i = 0; i < repeat; ++i)
                        lengths.push_back(value);
                }
                else if (symbol == 17) {
                    int repeat =
                        static_cast<int>(reader.get(3)) + 3;
                
                    if (static_cast<int>(lengths.size()) + repeat >
                        HLIT + HDIST)
                        throw std::runtime_error(
                            "DEFLATE code-length overflow.");
                        
                    for (int i = 0; i < repeat; ++i)
                        lengths.push_back(0);
                }
                else if (symbol == 18) {
                    int repeat =
                        static_cast<int>(reader.get(7)) + 11;
                
                    if (static_cast<int>(lengths.size()) + repeat >
                        HLIT + HDIST)
                        throw std::runtime_error(
                            "DEFLATE code-length overflow.");
                        
                    for (int i = 0; i < repeat; ++i)
                        lengths.push_back(0);
                }
                else {
                    throw std::runtime_error(
                        "Invalid DEFLATE code-length symbol.");
                }
            }
        
            std::vector<int> literal_lengths(
                lengths.begin(),
                lengths.begin() + HLIT
            );
        
            std::vector<int> distance_lengths(
                lengths.begin() + HLIT,
                lengths.end()
            );
        
            // A distance tree with no usable symbols cannot decode a
            // distance, but a block containing only literals is valid.
            bool has_distance = false;
        
            for (int len : distance_lengths) {
                if (len != 0) {
                    has_distance = true;
                    break;
                }
            }
        
            if (!has_distance)
                distance_lengths[0] = 1;
        
            literal.build(literal_lengths);
            distance.build(distance_lengths);
        }
        
        // ------------------------------------------------------------
        // Raw DEFLATE decompression
        // ------------------------------------------------------------
        
        std::vector<byte> inflate_raw(const std::vector<byte>& input,
                                      std::size_t max_output_size)
        {
            BitReader reader(input);
            std::vector<byte> output;
        
            while (true) {
                int final_block =
                    static_cast<int>(reader.get(1));
            
                int block_type =
                    static_cast<int>(reader.get(2));
            
                if (block_type == 0) {
                    // Stored block.
                    reader.align();
                
                    std::uint32_t len = reader.get(16);
                    std::uint32_t nlen = reader.get(16);
                
                    if ((len ^ 0xffffu) != nlen)
                        throw std::runtime_error(
                            "Invalid DEFLATE stored block.");
                        
                    if (output.size() + len > max_output_size)
                        throw std::runtime_error(
                            "PNG decompressed data is too large.");
                        
                    for (std::uint32_t i = 0; i < len; ++i)
                        output.push_back(
                            static_cast<byte>(reader.get(8)));
                }
                else if (block_type == 1 ||
                         block_type == 2) {
                        
                    HuffmanTable literal_tree;
                    HuffmanTable distance_tree;
                        
                    if (block_type == 1) {
                        build_fixed_trees(
                            literal_tree,
                            distance_tree
                        );
                    }
                    else {
                        build_dynamic_trees(
                            reader,
                            literal_tree,
                            distance_tree
                        );
                    }
                
                    while (true) {
                        int symbol =
                            literal_tree.decode(reader);
                    
                        if (symbol < 256) {
                            if (output.size() >= max_output_size)
                                throw std::runtime_error(
                                    "PNG decompressed data is too large.");
                                
                            output.push_back(
                                static_cast<byte>(symbol));
                        }
                        else if (symbol == 256) {
                            break;
                        }
                        else if (symbol >= 257 &&
                                 symbol <= 285) {
                                
                            int index = symbol - 257;
                                
                            int length =
                                LENGTH_BASE[index];
                                
                            int extra =
                                LENGTH_EXTRA[index];
                                
                            if (extra != 0)
                                length +=
                                    static_cast<int>(
                                        reader.get(extra));
                                    
                            int distance_symbol =
                                distance_tree.decode(reader);
                                    
                            if (distance_symbol < 0 ||
                                distance_symbol >= 30)
                                throw std::runtime_error(
                                    "Invalid DEFLATE distance symbol.");
                                
                            int distance_value =
                                DIST_BASE[distance_symbol];
                                
                            int distance_extra =
                                DIST_EXTRA[distance_symbol];
                                
                            if (distance_extra != 0)
                                distance_value +=
                                    static_cast<int>(
                                        reader.get(distance_extra));
                                    
                            if (distance_value <= 0 ||
                                static_cast<std::size_t>(
                                    distance_value) > output.size())
                                throw std::runtime_error(
                                    "Invalid DEFLATE back-reference.");
                                
                            if (output.size() +
                                static_cast<std::size_t>(length)
                                > max_output_size)
                                throw std::runtime_error(
                                    "PNG decompressed data is too large.");
                                
                            for (int i = 0; i < length; ++i) {
                                std::size_t source =
                                    output.size() -
                                    static_cast<std::size_t>(
                                        distance_value);
                                    
                                output.push_back(output[source]);
                            }
                        }
                        else {
                            throw std::runtime_error(
                                "Invalid DEFLATE literal/length symbol.");
                        }
                    }
                }
                else {
                    throw std::runtime_error(
                        "Reserved DEFLATE block type.");
                }
            
                if (final_block)
                    break;
            }
        
            return output;
        }
        
        // ------------------------------------------------------------
        // PNG unfiltering
        // ------------------------------------------------------------
        
        byte paeth_predictor(byte a, byte b, byte c)
        {
            int p =
                static_cast<int>(a) +
                static_cast<int>(b) -
                static_cast<int>(c);
        
            int pa = std::abs(p - static_cast<int>(a));
            int pb = std::abs(p - static_cast<int>(b));
            int pc = std::abs(p - static_cast<int>(c));
        
            if (pa <= pb && pa <= pc)
                return a;
        
            if (pb <= pc)
                return b;
        
            return c;
        }
        
        void unfilter_png(std::vector<byte>& raw,
                          std::uint32_t width,
                          std::uint32_t height,
                          std::size_t bytes_per_pixel,
                          std::size_t row_bytes)
        {
            const std::size_t expected =
                static_cast<std::size_t>(height) *
                (row_bytes + 1);
        
            if (raw.size() != expected)
                throw std::runtime_error(
                    "PNG decompressed size does not match image dimensions.");
                
            std::vector<byte> previous(row_bytes, 0);
            std::vector<byte> current(row_bytes);
                
            std::vector<byte> result;
            result.reserve(
                static_cast<std::size_t>(height) * row_bytes
            );
        
            std::size_t pos = 0;
        
            for (std::uint32_t y = 0; y < height; ++y) {
                byte filter = raw[pos++];
            
                std::copy(
                    raw.begin() + pos,
                    raw.begin() + pos + row_bytes,
                    current.begin()
                );
            
                pos += row_bytes;
            
                for (std::size_t x = 0; x < row_bytes; ++x) {
                    byte left =
                        (x >= bytes_per_pixel)
                            ? current[x - bytes_per_pixel]
                            : 0;
                
                    byte up = previous[x];
                
                    byte upper_left =
                        (x >= bytes_per_pixel)
                            ? previous[x - bytes_per_pixel]
                            : 0;
                
                    switch (filter) {
                        case 0:
                            break;
                    
                        case 1:
                            current[x] =
                                static_cast<byte>(
                                    current[x] + left);
                            break;
                                
                        case 2:
                            current[x] =
                                static_cast<byte>(
                                    current[x] + up);
                            break;
                                
                        case 3:
                            current[x] =
                                static_cast<byte>(
                                    current[x] +
                                    static_cast<byte>(
                                        (static_cast<int>(left) +
                                         static_cast<int>(up)) / 2));
                            break;
                                    
                        case 4:
                            current[x] =
                                static_cast<byte>(
                                    current[x] +
                                    paeth_predictor(
                                        left,
                                        up,
                                        upper_left));
                            break;
                                    
                        default:
                            throw std::runtime_error(
                                "Unsupported PNG filter.");
                    }
                }
            
                result.insert(
                    result.end(),
                    current.begin(),
                    current.end()
                );
            
                previous.swap(current);
            }
        
            raw.swap(result);
        }
        
        // ------------------------------------------------------------
        // Zlib wrapper
        // ------------------------------------------------------------
        
        std::vector<byte> inflate_zlib(const std::vector<byte>& input,
                                       std::size_t max_output_size)
        {
            if (input.size() < 6)
                throw std::runtime_error(
                    "Invalid zlib stream.");
                
            byte CMF = input[0];
            byte FLG = input[1];
                
            if ((CMF & 0x0f) != 8)
                throw std::runtime_error(
                    "PNG uses unsupported zlib compression.");
                
            if (((std::uint16_t(CMF) << 8) | FLG) % 31 != 0)
                throw std::runtime_error(
                    "Invalid zlib header.");
                
            if (FLG & 0x20)
                throw std::runtime_error(
                    "Preset zlib dictionaries are unsupported.");
                
            std::vector<byte> deflate_data(
                input.begin() + 2,
                input.end() - 4
            );
        
            std::vector<byte> output =
                inflate_raw(
                    deflate_data,
                    max_output_size
                );
            
            std::uint32_t expected_adler =
                read_be32(input, input.size() - 4);
            
            std::uint32_t actual_adler =
                adler32(output);
            
            if (expected_adler != actual_adler)
                throw std::runtime_error(
                    "PNG zlib Adler-32 checksum mismatch.");
                
            return output;
        }
        
        // ------------------------------------------------------------
        // Zlib stored-block writer
        // ------------------------------------------------------------
        
        std::vector<byte> zlib_store(
            const std::vector<byte>& input)
        {
            std::vector<byte> output;
        
            // CMF = 0x78
            // FLG = 0x01
            // Together they satisfy the zlib FCHECK requirement.
            output.push_back(0x78);
            output.push_back(0x01);
        
            std::size_t pos = 0;
        
            do {
                std::size_t remaining =
                    input.size() - pos;
            
                std::size_t block_size =
                    std::min<std::size_t>(
                        remaining,
                        65535
                    );
                
                bool final_block =
                    (pos + block_size == input.size());
                
                // Stored DEFLATE block header.
                // Since stored blocks are byte-aligned,
                // this is one complete byte.
                output.push_back(
                    final_block ? 0x01 : 0x00
                );
            
                std::uint16_t len =
                    static_cast<std::uint16_t>(
                        block_size);
                    
                std::uint16_t nlen =
                    static_cast<std::uint16_t>(
                        ~len);
                    
                output.push_back(
                    static_cast<byte>(len & 0xff));
                
                output.push_back(
                    static_cast<byte>((len >> 8) & 0xff));
                
                output.push_back(
                    static_cast<byte>(nlen & 0xff));
                
                output.push_back(
                    static_cast<byte>((nlen >> 8) & 0xff));
                
                output.insert(
                    output.end(),
                    input.begin() + pos,
                    input.begin() + pos + block_size
                );
            
                pos += block_size;
            
                // Empty input needs one final empty stored block.
                if (input.empty())
                    break;
            
            } while (pos < input.size());
        
            std::uint32_t checksum =
                adler32(input);
        
            write_be32(output, checksum);
        
            return output;
        }
        
        // ------------------------------------------------------------
        // PNG chunk writer
        // ------------------------------------------------------------
        
        void write_chunk(std::ofstream& file,
                         const char type[4],
                         const std::vector<byte>& data)
        {
            if (data.size() >
                std::numeric_limits<std::uint32_t>::max())
                throw std::runtime_error(
                    "PNG chunk is too large.");
                
            std::vector<byte> type_and_data;
            type_and_data.reserve(4 + data.size());
                
            for (int i = 0; i < 4; ++i)
                type_and_data.push_back(
                    static_cast<byte>(type[i]));
                
            type_and_data.insert(
                type_and_data.end(),
                data.begin(),
                data.end()
            );
        
            std::uint32_t length =
                static_cast<std::uint32_t>(
                    data.size());
                
            std::uint32_t checksum =
                crc32(
                    type_and_data.data(),
                    type_and_data.size()
                );
            
            byte length_bytes[4] = {
                static_cast<byte>((length >> 24) & 0xff),
                static_cast<byte>((length >> 16) & 0xff),
                static_cast<byte>((length >> 8) & 0xff),
                static_cast<byte>(length & 0xff)
            };
        
            file.write(
                reinterpret_cast<const char*>(length_bytes),
                4
            );
        
            file.write(type, 4);
        
            if (!data.empty()) {
                file.write(
                    reinterpret_cast<const char*>(data.data()),
                    static_cast<std::streamsize>(data.size())
                );
            }
        
            byte crc_bytes[4] = {
                static_cast<byte>((checksum >> 24) & 0xff),
                static_cast<byte>((checksum >> 16) & 0xff),
                static_cast<byte>((checksum >> 8) & 0xff),
                static_cast<byte>(checksum & 0xff)
            };
        
            file.write(
                reinterpret_cast<const char*>(crc_bytes),
                4
            );
        
            if (!file)
                throw std::runtime_error(
                    "Failed while writing PNG chunk.");
        }
        
        // ------------------------------------------------------------
        // Numeric conversion
        // ------------------------------------------------------------
        
        std::uint8_t pixel_to_u8(double value)
        {
            if (!std::isfinite(value))
                throw std::runtime_error(
                    "Pixel contains NaN or infinity.");
                
            value = std::clamp(value, 0.0, 1.0);
                
            return static_cast<std::uint8_t>(
                std::lround(value * 255.0)
            );
        }
        
        std::uint16_t pixel_to_u16(double value)
        {
            if (!std::isfinite(value))
                throw std::runtime_error(
                    "Pixel contains NaN or infinity.");
                
            value = std::clamp(value, 0.0, 1.0);
                
            return static_cast<std::uint16_t>(
                std::lround(value * 65535.0)
            );
        }
        
        double u8_to_pixel(byte value)
        {
            return static_cast<double>(value) / 255.0;
        }
        
        double u16_to_pixel(std::uint16_t value)
        {
            return static_cast<double>(value) / 65535.0;
        }
        
        // ------------------------------------------------------------
        // Safe size calculation
        // ------------------------------------------------------------
        
        std::size_t checked_multiply(std::size_t a,
                                     std::size_t b,
                                     const char* message)
        {
            if (b != 0 &&
                a > std::numeric_limits<std::size_t>::max() / b)
                throw std::runtime_error(message);
            
            return a * b;
        }
    
    } // anonymous namespace
    
    // ============================================================
    // PNG READER
    // ============================================================
    
    Image pimlio_read(const std::string& filename)
    {
        std::ifstream file(
            filename,
            std::ios::binary
        );
    
        if (!file)
            throw std::runtime_error(
                "Could not open PNG file: " + filename);
            
        file.seekg(0, std::ios::end);
            
        std::streamoff file_size =
            file.tellg();
            
        if (file_size < 0)
            throw std::runtime_error(
                "Could not determine PNG file size.");
            
        file.seekg(0, std::ios::beg);
            
        std::vector<byte> png(
            static_cast<std::size_t>(file_size)
        );
    
        if (!png.empty()) {
            file.read(
                reinterpret_cast<char*>(png.data()),
                static_cast<std::streamsize>(png.size())
            );
        }
    
        if (!file && !png.empty())
            throw std::runtime_error(
                "Could not read PNG file.");
            
        if (png.size() < 8 ||
            std::memcmp(
                png.data(),
                PNG_SIGNATURE,
                8
            ) != 0)
            throw std::runtime_error(
                "Invalid PNG signature.");
            
        std::size_t pos = 8;
            
        bool got_IHDR = false;
        bool got_IEND = false;
            
        std::uint32_t width = 0;
        std::uint32_t height = 0;
            
        byte bit_depth = 0;
        byte color_type = 0;
        byte compression = 0;
        byte filter_method = 0;
        byte interlace = 0;
            
        std::vector<byte> compressed;
            
        while (pos < png.size()) {
            if (png.size() - pos < 12)
                throw std::runtime_error(
                    "Truncated PNG chunk.");
                
            std::uint32_t chunk_size =
                read_be32(png, pos);
                
            pos += 4;
                
            if (pos + 4 > png.size())
                throw std::runtime_error(
                    "Truncated PNG chunk type.");
                
            char type[5] = {
                static_cast<char>(png[pos]),
                static_cast<char>(png[pos + 1]),
                static_cast<char>(png[pos + 2]),
                static_cast<char>(png[pos + 3]),
                '\0'
            };
        
            std::size_t type_pos = pos;
        
            pos += 4;
        
            std::size_t data_size =
                static_cast<std::size_t>(chunk_size);
        
            if (data_size > png.size() - pos)
                throw std::runtime_error(
                    "PNG chunk extends past end of file.");
                
            std::size_t data_pos = pos;
                
            pos += data_size;
                
            if (png.size() - pos < 4)
                throw std::runtime_error(
                    "Missing PNG chunk CRC.");
                
            std::uint32_t stored_crc =
                read_be32(png, pos);
                
            pos += 4;
                
            // CRC covers chunk type + chunk data.
            std::vector<byte> crc_data;
            crc_data.reserve(4 + data_size);
                
            crc_data.insert(
                crc_data.end(),
                png.begin() + type_pos,
                png.begin() + type_pos + 4
            );
        
            crc_data.insert(
                crc_data.end(),
                png.begin() + data_pos,
                png.begin() + data_pos + data_size
            );
        
            std::uint32_t calculated_crc =
                crc32(
                    crc_data.data(),
                    crc_data.size()
                );
            
            if (stored_crc != calculated_crc)
                throw std::runtime_error(
                    std::string("PNG CRC mismatch in ") +
                    type + " chunk.");
                
            if (std::strcmp(type, "IHDR") == 0) {
                if (got_IHDR)
                    throw std::runtime_error(
                        "PNG contains multiple IHDR chunks.");
                    
                if (data_size != 13)
                    throw std::runtime_error(
                        "Invalid PNG IHDR size.");
                    
                if (type_pos != 12)
                    throw std::runtime_error(
                        "IHDR must be the first PNG chunk.");
                    
                width =
                    read_be32(png, data_pos);
                    
                height =
                    read_be32(png, data_pos + 4);
                    
                bit_depth =
                    png[data_pos + 8];
                    
                color_type =
                    png[data_pos + 9];
                    
                compression =
                    png[data_pos + 10];
                    
                filter_method =
                    png[data_pos + 11];
                    
                interlace =
                    png[data_pos + 12];
                    
                got_IHDR = true;
                    
                if (width == 0 || height == 0)
                    throw std::runtime_error(
                        "PNG has invalid dimensions.");
                    
                if (width >
                        static_cast<std::uint32_t>(
                            std::numeric_limits<int>::max()) ||
                    height >
                        static_cast<std::uint32_t>(
                            std::numeric_limits<int>::max()))
                    throw std::runtime_error(
                        "PNG dimensions exceed Image limits.");
                    
                if (bit_depth != 8 &&
                    bit_depth != 16)
                    throw std::runtime_error(
                        "Only 8-bit and 16-bit PNGs are supported.");
                    
                if (color_type != 2 &&
                    color_type != 6)
                    throw std::runtime_error(
                        "Only RGB and RGBA PNGs are supported.");
                    
                if (compression != 0)
                    throw std::runtime_error(
                        "Unsupported PNG compression method.");
                    
                if (filter_method != 0)
                    throw std::runtime_error(
                        "Unsupported PNG filter method.");
                    
                if (interlace != 0)
                    throw std::runtime_error(
                        "Interlaced PNGs are not supported.");
            }
            else if (std::strcmp(type, "IDAT") == 0) {
                if (!got_IHDR)
                    throw std::runtime_error(
                        "IDAT appears before IHDR.");
                    
                compressed.insert(
                    compressed.end(),
                    png.begin() + data_pos,
                    png.begin() + data_pos + data_size
                );
            }
            else if (std::strcmp(type, "IEND") == 0) {
                if (data_size != 0)
                    throw std::runtime_error(
                        "Invalid IEND chunk.");
                    
                got_IEND = true;
                break;
            }
            else {
                // Ancillary chunks are intentionally ignored.
                // This implementation only decodes RGB/RGBA images.
            }
        }
    
        if (!got_IHDR)
            throw std::runtime_error(
                "PNG is missing IHDR.");
            
        if (!got_IEND)
            throw std::runtime_error(
                "PNG is missing IEND.");
            
        if (compressed.empty())
            throw std::runtime_error(
                "PNG is missing IDAT data.");
            
        const std::size_t channels =
            (color_type == 6) ? 4 : 3;
            
        const std::size_t bytes_per_sample =
            (bit_depth == 16) ? 2 : 1;
            
        const std::size_t bytes_per_pixel =
            channels * bytes_per_sample;
            
        const std::size_t row_bytes =
            checked_multiply(
                static_cast<std::size_t>(width),
                bytes_per_pixel,
                "PNG row size overflow."
            );
        
        const std::size_t filtered_row_size =
            row_bytes + 1;
        
        const std::size_t expected_raw_size =
            checked_multiply(
                static_cast<std::size_t>(height),
                filtered_row_size,
                "PNG image size overflow."
            );
        
        std::vector<byte> raw =
            inflate_zlib(
                compressed,
                expected_raw_size
            );
        
        if (raw.size() != expected_raw_size)
            throw std::runtime_error(
                "PNG decompressed size mismatch.");
            
        unfilter_png(
            raw,
            width,
            height,
            bytes_per_pixel,
            row_bytes
        );
    
        const std::size_t pixel_count =
            checked_multiply(
                static_cast<std::size_t>(width),
                static_cast<std::size_t>(height),
                "PNG pixel count overflow."
            );
        
        std::vector<Pixel> pixels;
        pixels.resize(pixel_count);
        
        std::size_t offset = 0;
        
        for (std::uint32_t y = 0; y < height; ++y) {
            for (std::uint32_t x = 0; x < width; ++x) {
                Pixel& pixel =
                    pixels[
                        static_cast<std::size_t>(y) *
                        static_cast<std::size_t>(width) +
                        static_cast<std::size_t>(x)
                    ];
                
                if (bit_depth == 8) {
                    pixel.r =
                        u8_to_pixel(raw[offset++]);
                
                    pixel.g =
                        u8_to_pixel(raw[offset++]);
                
                    pixel.b =
                        u8_to_pixel(raw[offset++]);
                
                    if (color_type == 6)
                        pixel.a =
                            u8_to_pixel(raw[offset++]);
                    else
                        pixel.a = 1.0;
                }
                else {
                    std::uint16_t r =
                        read_be16(raw, offset);
                    offset += 2;
                
                    std::uint16_t g =
                        read_be16(raw, offset);
                    offset += 2;
                
                    std::uint16_t b =
                        read_be16(raw, offset);
                    offset += 2;
                
                    pixel.r =
                        u16_to_pixel(r);
                
                    pixel.g =
                        u16_to_pixel(g);
                
                    pixel.b =
                        u16_to_pixel(b);
                
                    if (color_type == 6) {
                        std::uint16_t a =
                            read_be16(raw, offset);
                    
                        offset += 2;
                    
                        pixel.a =
                            u16_to_pixel(a);
                    }
                    else {
                        pixel.a = 1.0;
                    }
                }
            }
        }
    
        // IMPORTANT:
        // Constructor is now (pixels, depth, width, height).
        Image image(
            pixels,
            bit_depth,
            static_cast<int>(width),
            static_cast<int>(height)
        );
    
        return image;
    }
    
    // ============================================================
    // PNG WRITER
    // ============================================================
    
    void pimlio_write(const Image& image,
                      const std::string& filename)
    {
        if (image.width <= 0 ||
            image.height <= 0)
            throw std::runtime_error(
                "Cannot write image with invalid dimensions.");
            
        if (image.bitdepth != 8 &&
            image.bitdepth != 16)
            throw std::runtime_error(
                "Only 8-bit and 16-bit PNG output is supported.");
            
        const std::size_t width =
            static_cast<std::size_t>(image.width);
            
        const std::size_t height =
            static_cast<std::size_t>(image.height);
            
        const std::size_t pixel_count =
            checked_multiply(
                width,
                height,
                "Image dimensions overflow."
            );
        
        if (image.pixel_vector.size() != pixel_count)
            throw std::runtime_error(
                "Image pixel count does not match dimensions.");
            
        const bool is_16bit =
            image.bitdepth == 16;
            
        const std::size_t bytes_per_sample =
            is_16bit ? 2 : 1;
            
        // We always write RGBA PNG.
        constexpr std::size_t channels = 4;
            
        const std::size_t bytes_per_pixel =
            channels * bytes_per_sample;
            
        const std::size_t row_bytes =
            checked_multiply(
                width,
                bytes_per_pixel,
                "PNG row size overflow."
            );
        
        const std::size_t raw_size =
            checked_multiply(
                height,
                row_bytes + 1,
                "PNG output size overflow."
            );
        
        std::vector<byte> raw;
        raw.reserve(raw_size);
        
        for (std::size_t y = 0; y < height; ++y) {
            // Filter type 0 = None.
            raw.push_back(0);
        
            for (std::size_t x = 0; x < width; ++x) {
                const Pixel& pixel =
                    image.pixel_vector[
                        y * width + x
                    ];
                
                if (!std::isfinite(pixel.r) ||
                    !std::isfinite(pixel.g) ||
                    !std::isfinite(pixel.b) ||
                    !std::isfinite(pixel.a))
                    throw std::runtime_error(
                        "Cannot write NaN/infinite pixel values.");
                    
                if (!is_16bit) {
                    raw.push_back(
                        pixel_to_u8(pixel.r)
                    );
                
                    raw.push_back(
                        pixel_to_u8(pixel.g)
                    );
                
                    raw.push_back(
                        pixel_to_u8(pixel.b)
                    );
                
                    raw.push_back(
                        pixel_to_u8(pixel.a)
                    );
                }
                else {
                    std::uint16_t r =
                        pixel_to_u16(pixel.r);
                
                    std::uint16_t g =
                        pixel_to_u16(pixel.g);
                
                    std::uint16_t b =
                        pixel_to_u16(pixel.b);
                
                    std::uint16_t a =
                        pixel_to_u16(pixel.a);
                
                    raw.push_back(
                        static_cast<byte>((r >> 8) & 0xff)
                    );
                    raw.push_back(
                        static_cast<byte>(r & 0xff)
                    );
                
                    raw.push_back(
                        static_cast<byte>((g >> 8) & 0xff)
                    );
                    raw.push_back(
                        static_cast<byte>(g & 0xff)
                    );
                
                    raw.push_back(
                        static_cast<byte>((b >> 8) & 0xff)
                    );
                    raw.push_back(
                        static_cast<byte>(b & 0xff)
                    );
                
                    raw.push_back(
                        static_cast<byte>((a >> 8) & 0xff)
                    );
                    raw.push_back(
                        static_cast<byte>(a & 0xff)
                    );
                }
            }
        }
    
        if (raw.size() != raw_size)
            throw std::runtime_error(
                "Internal PNG raw-data size error.");
            
        std::vector<byte> compressed =
            zlib_store(raw);
            
        // --------------------------------------------------------
        // IHDR
        // --------------------------------------------------------
            
        std::vector<byte> ihdr;
        ihdr.reserve(13);
            
        write_be32(
            ihdr,
            static_cast<std::uint32_t>(width)
        );
    
        write_be32(
            ihdr,
            static_cast<std::uint32_t>(height)
        );
    
        // Bit depth.
        ihdr.push_back(
            is_16bit ? 16 : 8
        );
    
        // Color type 6 = RGBA.
        ihdr.push_back(6);
    
        // Compression method.
        ihdr.push_back(0);
    
        // Filter method.
        ihdr.push_back(0);
    
        // No interlace.
        ihdr.push_back(0);
    
        // --------------------------------------------------------
        // Write file
        // --------------------------------------------------------
    
        std::ofstream file(
            filename,
            std::ios::binary
        );
    
        if (!file)
            throw std::runtime_error(
                "Could not open output PNG: " + filename);
            
        file.write(
            reinterpret_cast<const char*>(
                PNG_SIGNATURE
            ),
            8
        );
    
        if (!file)
            throw std::runtime_error(
                "Failed to write PNG signature.");
            
        write_chunk(
            file,
            "IHDR",
            ihdr
        );
    
        write_chunk(
            file,
            "IDAT",
            compressed
        );
    
        std::vector<byte> empty;
    
        write_chunk(
            file,
            "IEND",
            empty
        );
    
        file.close();
    
        if (!file)
            throw std::runtime_error(
                "Failed to finish writing PNG.");
    }

} // namespace piml
