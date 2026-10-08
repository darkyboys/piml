# PIML

**PIML (Precision Image Manipulation Library)** is a lightweight C++ image manipulation library focused on precise, selection-based image processing.

PIML is designed around a simple idea:

> **Select what you want to modify, then decide how you want to modify it.**

The library provides a flexible system for selecting pixels, refining selections, targeting individual channels, and applying image effects to the current selection.

## Highlights

- Selection-based image manipulation
- Fine-grained pixel control
- Luma and intensity-based pixel selection
- Channel-specific processing
- Composable selection predicates
- Normalized floating-point pixel representation
- Built-in image I/O
- 8-bit and 16-bit image output
- Built-in debugging and logging
- Lightweight and portable C++ design
- Extensible effect system
- Selection-driven image effects

## Documentation

The complete documentation and API reference are available in:

```text
docs/index.html
````

Open **`docs/index.html`** in your browser for the complete documentation, including API usage, examples, effects, selection predicates, channels, image I/O, debugging, and architecture details.

## Quick Start

### Include PIML

```cpp
#include "piml.hh"
#include "pimlio.hh"
```

### Read an image

```cpp
auto image = piml::pimlio_read("img/test.png");
```

### Select pixels

```cpp
image.select(piml::EVERYTHING);
```

### Apply an effect

```cpp
image.apply_effect.brightness(10);
```

### Write the result

```cpp
piml::pimlio_write(image, "output.png");
```

A complete example:

```cpp
#include "piml.hh"
#include "pimlio.hh"

int main()
{
    using namespace piml;

    auto image = pimlio_read("img/test.png");

    image.select(EVERYTHING);
    image.apply_effect.brightness(10);

    pimlio_write(image, "output.png");
}
```

## Requirements

PIML requires:

* A STL-compatible C++ compiler
* Standard C++ library support
* A compatible build environment

The core library is designed to avoid unnecessary platform-specific dependencies.

Linux and environments such as Termux are supported.

## Building

Clone the repository:

```bash
git clone https://github.com/darkyboys/piml.git
cd piml
```

Build:

```bash
./build.sh
```

Run tests:

```bash
./test.sh
```

Clean build artifacts:

```bash
./clean.sh
```

## Project Structure

```text
piml/
├── build.cc
├── build.sh
├── clean.sh
├── piml.hh
├── pimlio.hh
├── README.md
├── test.sh
├── docs/
│   └── index.html
├── img/
│   └── test.png
├── src/
│   ├── pimlio.cc
│   ├── classes/
│   │   ├── Debugger.cc
│   │   └── Image.cc
│   └── effects/
│       └── brightness.cc
└── test/
    └── test.cc
```

## Contributing

Contributions, bug fixes, new effects, selection predicates, tests, optimizations, and documentation improvements are welcome.

For implementation details and API usage, refer to:

```text
docs/index.html
```

## License

PIML is released into the **Public Domain** under **CC0 1.0**.

You are free to use, modify, distribute, and build upon the project under the terms of the CC0 dedication.

## Repository

GitHub:

[https://github.com/darkyboys/piml](https://github.com/darkyboys/piml)

For complete documentation and API reference:

```text
docs/index.html
