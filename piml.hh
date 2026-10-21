/*
Creative Commons Legal Code

CC0 1.0 Universal

    CREATIVE COMMONS CORPORATION IS NOT A LAW FIRM AND DOES NOT PROVIDE
    LEGAL SERVICES. DISTRIBUTION OF THIS DOCUMENT DOES NOT CREATE AN
    ATTORNEY-CLIENT RELATIONSHIP. CREATIVE COMMONS PROVIDES THIS
    INFORMATION ON AN "AS-IS" BASIS. CREATIVE COMMONS MAKES NO WARRANTIES
    REGARDING THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS
    PROVIDED HEREUNDER, AND DISCLAIMS LIABILITY FOR DAMAGES RESULTING FROM
    THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS PROVIDED
    HEREUNDER.

Statement of Purpose

The laws of most jurisdictions throughout the world automatically confer
exclusive Copyright and Related Rights (defined below) upon the creator
and subsequent owner(s) (each and all, an "owner") of an original work of
authorship and/or a database (each, a "Work").

Certain owners wish to permanently relinquish those rights to a Work for
the purpose of contributing to a commons of creative, cultural and
scientific works ("Commons") that the public can reliably and without fear
of later claims of infringement build upon, modify, incorporate in other
works, reuse and redistribute as freely as possible in any form whatsoever
and for any purposes, including without limitation commercial purposes.
These owners may contribute to the Commons to promote the ideal of a free
culture and the further production of creative, cultural and scientific
works, or to gain reputation or greater distribution for their Work in
part through the use and efforts of others.

For these and/or other purposes and motivations, and without any
expectation of additional consideration or compensation, the person
associating CC0 with a Work (the "Affirmer"), to the extent that he or she
is an owner of Copyright and Related Rights in the Work, voluntarily
elects to apply CC0 to the Work and publicly distribute the Work under its
terms, with knowledge of his or her Copyright and Related Rights in the
Work and the meaning and intended legal effect of CC0 on those rights.

1. Copyright and Related Rights. A Work made available under CC0 may be
protected by copyright and related or neighboring rights ("Copyright and
Related Rights"). Copyright and Related Rights include, but are not
limited to, the following:

  i. the right to reproduce, adapt, distribute, perform, display,
     communicate, and translate a Work;
 ii. moral rights retained by the original author(s) and/or performer(s);
iii. publicity and privacy rights pertaining to a person's image or
     likeness depicted in a Work;
 iv. rights protecting against unfair competition in regards to a Work,
     subject to the limitations in paragraph 4(a), below;
  v. rights protecting the extraction, dissemination, use and reuse of data
     in a Work;
 vi. database rights (such as those arising under Directive 96/9/EC of the
     European Parliament and of the Council of 11 March 1996 on the legal
     protection of databases, and under any national implementation
     thereof, including any amended or successor version of such
     directive); and
vii. other similar, equivalent or corresponding rights throughout the
     world based on applicable law or treaty, and any national
     implementations thereof.

2. Waiver. To the greatest extent permitted by, but not in contravention
of, applicable law, Affirmer hereby overtly, fully, permanently,
irrevocably and unconditionally waives, abandons, and surrenders all of
Affirmer's Copyright and Related Rights and associated claims and causes
of action, whether now known or unknown (including existing as well as
future claims and causes of action), in the Work (i) in all territories
worldwide, (ii) for the maximum duration provided by applicable law or
treaty (including future time extensions), (iii) in any current or future
medium and for any number of copies, and (iv) for any purpose whatsoever,
including without limitation commercial, advertising or promotional
purposes (the "Waiver"). Affirmer makes the Waiver for the benefit of each
member of the public at large and to the detriment of Affirmer's heirs and
successors, fully intending that such Waiver shall not be subject to
revocation, rescission, cancellation, termination, or any other legal or
equitable action to disrupt the quiet enjoyment of the Work by the public
as contemplated by Affirmer's express Statement of Purpose.

3. Public License Fallback. Should any part of the Waiver for any reason
be judged legally invalid or ineffective under applicable law, then the
Waiver shall be preserved to the maximum extent permitted taking into
account Affirmer's express Statement of Purpose. In addition, to the
extent the Waiver is so judged Affirmer hereby grants to each affected
person a royalty-free, non transferable, non sublicensable, non exclusive,
irrevocable and unconditional license to exercise Affirmer's Copyright and
Related Rights in the Work (i) in all territories worldwide, (ii) for the
maximum duration provided by applicable law or treaty (including future
time extensions), (iii) in any current or future medium and for any number
of copies, and (iv) for any purpose whatsoever, including without
limitation commercial, advertising or promotional purposes (the
"License"). The License shall be deemed effective as of the date CC0 was
applied by Affirmer to the Work. Should any part of the License for any
reason be judged legally invalid or ineffective under applicable law, such
partial invalidity or ineffectiveness shall not invalidate the remainder
of the License, and in such case Affirmer hereby affirms that he or she
will not (i) exercise any of his or her remaining Copyright and Related
Rights in the Work or (ii) assert any associated claims and causes of
action with respect to the Work, in either case contrary to Affirmer's
express Statement of Purpose.

4. Limitations and Disclaimers.

 a. No trademark or patent rights held by Affirmer are waived, abandoned,
    surrendered, licensed or otherwise affected by this document.
 b. Affirmer offers the Work as-is and makes no representations or
    warranties of any kind concerning the Work, express, implied,
    statutory or otherwise, including without limitation warranties of
    title, merchantability, fitness for a particular purpose, non
    infringement, or the absence of latent or other defects, accuracy, or
    the present or absence of errors, whether or not discoverable, all to
    the greatest extent permissible under applicable law.
 c. Affirmer disclaims responsibility for clearing rights of other persons
    that may apply to the Work or any use thereof, including without
    limitation any person's Copyright and Related Rights in the Work.
    Further, Affirmer disclaims responsibility for obtaining any necessary
    consents, permissions or other rights required for any use of the
    Work.
 d. Affirmer understands and acknowledges that Creative Commons is not a
    party to this document and has no duty or obligation with respect to
    this CC0 or use of the Work.
*/


#include <algorithm>
#ifndef PIML
#define PIML 1.001

#include <vector>
#include <string>
#include <cmath>


namespace piml {
    struct Pixel{
        double r,
               g,
               b,
               a;
        double* s = nullptr; // selected channel, This will be used if not nullptr
        unsigned long long index; // This will only store the index of the pixel in an image to select it properly.

        inline void add(
            double value
        ){
            if (s != nullptr){
                *s += value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                r += value;
                g += value;
                b += value;

                r = std::clamp(r, 0.0, 1.0);
                g = std::clamp(g, 0.0, 1.0);
                b = std::clamp(b, 0.0, 1.0);
            }
        }

        inline void add_r(
            double value
        ){
            if (s != nullptr){
                *s += value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                r += value;
                r = std::clamp(r, 0.0, 1.0);
            }
        }

        inline void add_g(
            double value
        ){
            if (s != nullptr){
                *s += value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                g += value;
                g = std::clamp(g, 0.0, 1.0);
            }
        }

        inline void add_b(
            double value
        ){
            if (s != nullptr){
                *s += value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                b += value;
                b = std::clamp(b, 0.0, 1.0);
            }
        }

        inline void add_a(
            double value
        ){
            if (s != nullptr){
                *s += value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                a += value;
                a = std::clamp(a, 0.0, 1.0);
            }
        }

        inline void subtract(
            double value
        ){
            if (s != nullptr){
                *s -= value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                r -= value;
                g -= value;
                b -= value;

                r = std::clamp(r, 0.0, 1.0);
                g = std::clamp(g, 0.0, 1.0);
                b = std::clamp(b, 0.0, 1.0);
            }
        }

        inline void subtract_r(
            double value
        ){
            if (s != nullptr){
                *s -= value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                r -= value;
                r = std::clamp(r, 0.0, 1.0);
            }
        }

        inline void subtract_g(
            double value
        ){
            if (s != nullptr){
                *s -= value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                g -= value;
                g = std::clamp(g, 0.0, 1.0);
            }
        }

        inline void subtract_b(
            double value
        ){
            if (s != nullptr){
                *s -= value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                b -= value;
                b = std::clamp(b, 0.0, 1.0);
            }
        }

        inline void subtract_a(
            double value
        ){
            if (s != nullptr){
                *s -= value;
                *s = std::clamp(*s, 0.0, 1.0);
            }
            else {
                a -= value;
                a = std::clamp(a, 0.0, 1.0);
            }
        }

        inline double average(){
            return std::clamp((r + g + b) / 3.0, 0.0, 1.0);
        }

        inline double get_r(){
            if (s != nullptr) return *s;
            else return r;
        }

        inline double get_g(){
            if (s != nullptr) return *s;
            else return g;
        }

        inline double get_b(){
            if (s != nullptr) return *s;
            else return b;
        }

        inline double get_a(){
            if (s != nullptr) return *s;
            else return a;
        }


        inline void set_r(double value){
            if (s != nullptr) *s = std::clamp(value, 0.0, 1.0);
            else r = std::clamp(value, 0.0, 1.0);
        }

        inline void set_g(double value){
            if (s != nullptr) *s = std::clamp(value, 0.0, 1.0);
            else g = std::clamp(value, 0.0, 1.0);
        }

        inline void set_b(double value){
            if (s != nullptr) *s = std::clamp(value, 0.0, 1.0);
            else b = std::clamp(value, 0.0, 1.0);
        }

        inline void set_a(double value){
            if (s != nullptr) *s = std::clamp(value, 0.0, 1.0);
            else a = std::clamp(value, 0.0, 1.0);
        }

        inline double get_selected(){
            return *s;
        }

        inline void max(){
            if (s != nullptr);
            else {
                if (r >= g && r >= b) {s = &r;return;}
                if (g >= b) {s = &g;return;}
                s = &b;return;
            }
        }

        inline void min(){
            if (s != nullptr);
            else {
                if (r <= g && r <= b) {s = &r;return;}
                if (g <= b) {s = &g;return;}
                s = &b;return;
            }
        }

        inline void clear_selection(){ // They both does the same thing to keep the API consistent
            s = nullptr;
        }

        inline void clean_selection(){
            s = nullptr;
        }

        inline double luma(){
            constexpr double RED_LUMA = 0.2126;
            constexpr double GREEN_LUMA = 0.7152;
            constexpr double BLUE_LUMA = 0.0722;
            if (s != nullptr){
                return *s*RED_LUMA + *s*GREEN_LUMA + *s*BLUE_LUMA;
            }
            return (r*RED_LUMA + g*GREEN_LUMA + b*BLUE_LUMA);
        }

        Pixel() = default; // Useless default constructor

        Pixel(const Pixel& other): // Copy constructor
            r(other.r),
            g(other.g),
            b(other.b),
            a(other.a),
            s(nullptr),
            index{other.index}
        {}

        Pixel(Pixel&& other) = default;

        Pixel& operator=(const Pixel& other) // Copy assignment
        {
            if (this == &other)
                return *this;
        
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            index = other.index;
        
            // Don't touch s.
            return *this;
        }

        Pixel& operator=(Pixel&& other) = default;
    };

    struct Color{
        double r, g, b;
    };

    inline double to_positive(double value){
        if (value < 0) return (value*-1);
        else return value;
    }

    inline double nonzero_value(double value, double result = 0.000001){
        if (value == 0)
            return result;
        else return value;
    }

    inline double invert_zero(double value){
        if (value == 0.0)
            return 1.0;
        return value;
    }

    /*
    * Calculates the brightness-independent distance between two RGB colors.
    *
    * The algorithm compares the relative proportions of the RGB channels
    * rather than their absolute intensities. This allows colors with the
    * same RGB proportions but different overall brightness to have a
    * distance of zero.
    *
    * To compare the colors, the algorithm first selects a channel that is
    * nonzero in both colors, prioritizing red, then green, then blue.
    * The first usable channel becomes the reference channel.
    *
    * Color A is then scaled proportionally so that its reference channel
    * matches the corresponding channel in Color B. The same scaling factor
    * is applied to all three channels, preserving Color A's RGB proportions.
    *
    * The Euclidean distance between the scaled Color A and Color B is
    * calculated using their RGB channel differences. This distance is
    * normalized by the magnitude of Color B and clamped to the range
    * [0.0, 1.0], where 0.0 represents a perfect proportional match and
    * 1.0 represents the maximum reported distance.
    *
    * If no common nonzero reference channel exists, identical colors
    * return 0.0; otherwise, the algorithm returns 1.0.
    *
    * Note: Reference-channel priority can affect the result, and the
    * normalized distance is a custom similarity metric, not a perceptual
    * color difference measurement.
    */
    inline double get_distance_between_colors(
        const Color& a, const Color& b)
    {
        double ar = a.r, ag = a.g, ab = a.b;
        double br = b.r, bg = b.g, bb = b.b;

        // Choose the first usable reference channel.
        double scale = 0.0;

        if (ar > 0.0 && br > 0.0)
            scale = br / ar;
        else if (ag > 0.0 && bg > 0.0)
            scale = bg / ag;
        else if (ab > 0.0 && bb > 0.0)
            scale = bb / ab;
        else
            return (ar == br && ag == bg && ab == bb)
                ? 0.0 : 1.0;

        // Scale A to match B's reference channel.
        const double dr = ar * scale - br;
        const double dg = ag * scale - bg;
        const double db = ab * scale - bb;

        // Normalize distance to [0, 1].
        const double distance =
            std::sqrt(dr * dr + dg * dg + db * db);

        const double magnitude =
            std::sqrt(br * br + bg * bg + bb * bb);

        if (magnitude == 0.0)
            return distance == 0.0 ? 0.0 : 1.0;

        return std::clamp(distance / magnitude, 0.0, 1.0);
    }

    inline double invert_positive(double value){
        return 1.0 - std::clamp(value, 0.0, 1.0);
    }

    enum Selection{
        EVERYTHING,
        CHANNEL_RED,
        CHANNEL_BLUE,
        CHANNEL_GREEN,
        CHANNEL_ALPHA,
        WHERE_INTENSITY_GREATER_THAN,
        WHERE_INTENSITY_GREATER_THAN_OR_EQUALS,
        WHERE_INTENSITY_SMALLER_THAN_OR_EQUALS,
        WHERE_INTENSITY_SMALLER_THAN,
        WHERE_INTENSITY_BETWEEN,
        WHERE_INTENSITY_EQUALS,
        WHERE_LUMA_GREATER_THAN,
        WHERE_LUMA_GREATER_THAN_OR_EQUALS,
        WHERE_LUMA_SMALLER_THAN_OR_EQUALS,
        WHERE_LUMA_SMALLER_THAN,
        WHERE_LUMA_BETWEEN,
        WHERE_LUMA_EQUALS,
        REMAINING,
        WARM_COLORS,
        WHERE_COLORS_MATCHES
    };

    struct Log{
        enum Type{
            MESSAGE,
            WARNING,
            ERROR,
            ACTIVITY
        };

        std::string name;
        std::string message;
        Type type;
    };


    inline double normalize_percentage(
        double value
    ){
        return std::clamp(value, -100.0, 100.0) / 100.0;
    }

    class Image;
    class Effect{

        Image& image;

        public:
            Effect(
                Image& img
            ) : image(img){}

            void brightness( // Applies the brightness to the image.
                double value
            );

            void color_brightness( // Applies the brightness to the image.
                double value,
                const Color& color,
                double distance = 80
            );

            void smoothing_brightness( // Lifts the darkest areas more compared to the lightest areas
                double value
            );

            void smoothing_brightness_opposite( // Lifts the brightest areas more compared to the darkest areas
                double value
            );

            void intensity_lift( // Lifts up the dark pixels linearly.
                double value,
                double point = 0.5
            );
            
            void intensity_gain( // Lifts up the bright pixels linearly.
                double value,
                double point = 0.5
            );

            void intensity_corners( // Manipulates the corner point 0 and the corner point 1 on the linear intensity graph. 
                double value,
                double point = 0.5
            );

            void intensity_contrast( // Applies the linear contrast
                double value,
                double point = 0.5
            );

            void corners( // Manipulates the corner point 0 and the corner point 1 on the linear intensity graph per channel.
                double value,
                double point = 0.5
            );

            void contrast( // Applies the linear contrast
                double value,
                double point = 0.5
            );


            Effect* operator ->();
    };
    

    class Image{
        // Private section of the class! - This is not directly accessible by the programmer
        std::vector <Pixel> pixel_vector; // This will only store the pixels.
        std::vector <Pixel*> selected_pixels; // This will only store the pixels.
        std::vector <Pixel*> pixel_buffer; // This will only store the pixels.
        std::vector <Log> logs; // Debug logs when needed.
        std::vector <unsigned long long> index_buffer;

        unsigned char bitdepth = 0; // This must not be zero!
        bool debugging_allowed = true; // This state will toggle if the debugging is allowed or not

        int height,
            width;

        friend struct Debugger; // Debugger can access the private information.
        friend struct Effect; // Effects must be able to access this area to manipulate the image.
        friend Image pimlio_read(const std::string& filename);;
        friend void pimlio_write(const Image& image, const std::string& filename);
        friend Image pimlio_give_lut_image(int size);

        friend void pimlio_export_lut(
            const Image& image,
            const std::string& outputCube,
            int size
        );
        Effect effect;

        public:
            Image(
                const std::vector <Pixel>& input_pixels,
                unsigned char depth,
                int w,
                int h
            ) : pixel_vector(input_pixels), bitdepth(depth) , height(h), width(w), effect(*this) {}

            Image(
                const Image& img
            ) : effect(*this){
                // We will only copy what's necessary, Not the logs or selected pixels of the other image.
                for (const Pixel& px : img.pixel_vector){
                    pixel_vector.push_back(px);
                }
                bitdepth = img.bitdepth;
                if (debugging_allowed){ // Debugging
                    log(
                        "Image::Image",
                        "Copied the Image.",
                        Log::ACTIVITY
                    );
                } // Debugging
            }

            void select(
                Selection selection,
                double a = 0.0,
                double b = 0.0,
                double c = 0.0,
                double e = 0.0
            );

            void select(
                Selection selection,
                const Color& color,
                double distance = 80 // one of the best calibrated value for most of the images.
            );

            inline Effect& apply_effect(){ // Allows effect application via debugging
                if (debugging_allowed){
                    if (selected_pixels.size() == 0){
                        log("piml::Image::apply_effect()", "Applying effect on " + std::to_string(selected_pixels.size()) + " pixels.", Log::WARNING);
                    }
                    else {
                        log("piml::Image::apply_effect()", "Applying effect on " + std::to_string(selected_pixels.size()) + " pixels.", Log::MESSAGE);
                    }
                }
                return effect;
            }

            void disable_debugging();
            void enable_debugging();
            
            void clear_selection(); // Actually empty the selection
            void clean_selection(); // To clean any junk

            void log(
                const std::string& name,
                const std::string& message,
                Log::Type type
            );

            ~Image();
    };


    struct Debugger{
        private:
            Image& image;
        public:
            Debugger(
                Image& img
            );
        
            void show_messages();
            void show_warnings();
            void show_errors();
            void show_activities();
            void show_everything();
            void clear_everything();
            void clear_messages();
            void clear_warnings();
            void clear_errors();
            void clear_activities();
            void show_pixels();
        
            ~Debugger();

    };

}

#endif