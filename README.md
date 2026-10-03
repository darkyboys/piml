# PIML

**Precision Image Manipulation Library**

PIML is a C++ image manipulation library focused on precise, selection-based image processing. It provides a small API for working with images, selecting pixels, and applying effects to those selections.

The library is intended to remain lightweight and portable while providing enough control for more advanced image manipulation as the selection system develops.

## Features

* Selection-based image manipulation
* Fine-grained pixel control
* Floating-point pixel representation
* Built-in image input/output
* 8-bit and 16-bit output support
* Built-in logging and debugging
* Minimal external requirements
* Designed for portability
* Fast image operations
* Extensible effect system
* Future support for more complex selection rules

## Requirements

PIML only requires:

* A C++ STL implementation
* An STL-compatible C++ compiler

There are no platform-specific dependencies required by the core library.

Because of this, PIML can be built on a wide range of systems, including Linux and environments such as Termux.

## Building

Clone the repository:

```bash
git clone https://github.com/darkyboys/piml.git
cd piml
```

Build PIML using:

```bash
./build.sh
```

The build script uses `build.cc` for the actual build process.

To build and run the test program:

```bash
./test.sh
```

Generated files can be removed with:

```bash
./clean.sh
```

## Basic Usage

PIML exposes its main API through `piml.hh` and image I/O through `pimlio.hh`.

```cpp
#include "piml.hh"
#include "pimlio.hh"

int main() {
    piml::Image image = piml::pimlio_read("img/test.png");

    image.select(piml::EVERYTHING);

    piml::Effect effect = image;
    effect.brightness(10);
    effect.brightness(-90);

    piml::pimlio_write(image, "test_output.png");
}
```

The image is first loaded from disk and then a selection is created:

```cpp
image.select(piml::EVERYTHING);
```

`EVERYTHING` currently selects all pixels in the image.

An effect can then be created for the image and applied to the current selection:

```cpp
piml::Effect effect = image;
effect.brightness(10);
```

Multiple operations can be applied to the same selection.

Finally, the modified image can be written back to disk:

```cpp
piml::pimlio_write(image, "test_output.png");
```

## Pixel Representation

PIML stores each pixel as four `double` values:

```cpp
struct Pixel {
    double r;
    double g;
    double b;
    double a;
};
```

Color channel values are represented internally in the range:

```text
0.0 - 1.0
```

This allows image operations to work with normalized floating-point values rather than being tied directly to a particular integer pixel format.

## Selection

Selection is a central part of PIML's design.

An `Image` maintains its current selection, and effects operate on the selected pixels.

Currently available:

```cpp
image.select(piml::EVERYTHING);
```

The selection API is intentionally kept separate from effects. This allows new selection mechanisms to be added without requiring each effect to implement its own selection logic.

More complex selection rules are planned for future versions, including selections based on pixel properties and combinations of multiple conditions.

## Effects

Effects are provided through `piml::Effect`.

For example:

```cpp
piml::Effect effect = image;

effect.brightness(10);
```

### Brightness

The brightness effect accepts a percentage value:

```cpp
effect.brightness(10);
```

Positive values increase brightness, while negative values decrease it.

The value is normalized and constrained to the supported range internally.

Effects only operate on the pixels currently included in the image's selection.

## Image I/O

Image input/output is provided by `pimlio.hh`.

### Reading an image

```cpp
piml::Image image = piml::pimlio_read("image.png");
```

Image channels are stored internally as normalized `double` values.

The source image's bit depth is preserved when it can be determined.

### Writing an image

```cpp
piml::pimlio_write(image, "output.png");
```

The output format is selected from the filename extension.

PIML currently supports output at:

* 8 bits per channel
* 16 bits per channel

Values are clamped to `[0.0, 1.0]` before being converted to the output format.

## Debugging

PIML includes a small logging system for debugging and development.

Debugging can be disabled or enabled on an image:

```cpp
image.disable_debugging();
```

```cpp
image.enable_debugging();
```

Logs are categorized as:

```cpp
piml::Log::MESSAGE
piml::Log::WARNING
piml::Log::ERROR
piml::Log::ACTIVITY
```

The `Debugger` class can be used to inspect and clear these logs.

```cpp
piml::Debugger debugger(image);

debugger.show_everything();
```

Individual log categories can also be accessed:

```cpp
debugger.show_messages();
debugger.show_warnings();
debugger.show_errors();
debugger.show_activities();
```

## Memory Usage

PIML uses more memory than a minimal image buffer would because the selection system maintains explicit information about selected pixels.

This is a deliberate design trade-off.

The additional memory allows the library to provide fine-grained selection and gives the effect system a straightforward way to operate only on the selected pixels.

For applications where memory usage is critical, this is an important consideration when choosing how large an image to process.

## Performance

PIML is designed to keep image operations fast while retaining the selection-based architecture.

Actual performance depends on factors such as:

* Image dimensions
* Number of selected pixels
* Number of operations performed
* Image format
* Available memory
* Compiler optimizations

The library does, however, trade some memory usage for the flexibility provided by explicit pixel selection.

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
│
├── img/
│   └── test.png
│
├── src/
│   ├── pimlio.cc
│   │
│   ├── classes/
│   │   ├── Debugger.cc
│   │   └── Image.cc
│   │
│   └── effects/
│       └── brightness.cc
│
└── test/
    └── test.cc
```

### Source files

| File                        | Description              |
| --------------------------- | ------------------------ |
| `piml.hh`                   | Main public API          |
| `pimlio.hh`                 | Image input/output API   |
| `src/pimlio.cc`             | Image I/O implementation |
| `src/classes/Image.cc`      | `Image` implementation   |
| `src/classes/Debugger.cc`   | Debugger implementation  |
| `src/effects/brightness.cc` | Brightness effect        |
| `test/test.cc`              | Test program             |
| `build.cc`                  | Build implementation     |
| `build.sh`                  | Build script             |
| `test.sh`                   | Build and run tests      |
| `clean.sh`                  | Clean generated files    |

## Design

PIML is built around three main components:

```text
Image
  │
  ├── Selection
  │
  └── Effect
```

`Image` owns the image data and current selection.

`Selection` determines which pixels an operation should affect.

`Effect` performs an operation on the selected pixels.

Keeping these responsibilities separate makes it possible to add effects without coupling them to a particular selection method.

The same design also leaves room for more advanced selection rules in the future.

## Roadmap

PIML is still under development.

Planned areas of development include:

* Additional image effects
* More selection types
* Complex selection rules
* Conditional selections
* Combining multiple selection conditions
* Additional image formats
* Performance improvements
* Memory usage improvements
* Expanded test coverage

The selection system is expected to become considerably more expressive as the library develops.

## Contributing

Contributions are welcome.

If you want to work on PIML, useful areas include:

* Adding new effects
* Improving image I/O
* Implementing new selection mechanisms
* Optimizing existing operations
* Adding tests
* Improving documentation

For larger changes, opening an issue first can help keep development coordinated.

## License

PIML is released under **CC0 1.0 Universal**.

See the license text included in the project for the complete terms.

## Repository

[https://github.com/darkyboys/piml](https://github.com/darkyboys/piml?utm_source=chatgpt.com)
