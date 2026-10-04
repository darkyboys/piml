# PIML

**Precision Image Manipulation Library**

PIML is a C++ image manipulation library focused on precise, selection-based image processing. It provides a small API for working with images, selecting pixels, refining selections, and applying effects to those selections.

The library is intended to remain lightweight and portable while providing enough control for more advanced image manipulation through its selection system.

## Features

* Selection-based image manipulation
* Fine-grained pixel control
* Luma-based pixel selection
* Channel-specific selection
* Composable selection rules
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

    piml::pimlio_write(image, "test_output.png");

}
```

The image is first loaded from disk and then a selection is created:

```cpp
image.select(piml::EVERYTHING);
```

`EVERYTHING` selects all pixels in the image.

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

An `Image` maintains a current selection of pixels. Effects operate only on the pixels currently contained in that selection.

The selection system is divided conceptually into two parts:

```text
WHERE → Which pixels?
WHAT  → Which channel?
HOW   → Which effect?
```

A selection can first be created with `EVERYTHING` and then refined using selection predicates.

### Selecting all pixels

```cpp
image.select(piml::EVERYTHING);
```

This initializes the current selection with every pixel in the image.

### Luma-based selection

PIML can refine the current selection using the average RGB value of each pixel as its luma:

```cpp
luma = (r + g + b) / 3.0;
```

Alpha does not participate in luma calculations.

Currently available luma predicates are:

```cpp
piml::WHERE_LUMA_GREATER_THAN
piml::WHERE_LUMA_SMALLER_THAN
piml::WHERE_LUMA_GREATER_THAN_OR_EQUALS
piml::WHERE_LUMA_SMALLER_THAN_OR_EQUALS
piml::WHERE_LUMA_EQUALS
piml::WHERE_LUMA_BETWEEN
```

Threshold values are specified as percentages.

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN, 50);
```

This selects pixels whose luma is greater than 50%.

Similarly:

```cpp
image.select(piml::WHERE_LUMA_SMALLER_THAN, 50);
```

selects pixels whose luma is smaller than 50%.

### Luma ranges

`WHERE_LUMA_BETWEEN` accepts a lower and upper percentage:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_BETWEEN, 30, 70);
```

This selects pixels whose luma is within the inclusive range:

```text
30% <= luma <= 70%
```

The lower bound must be supplied first. If the upper bound is smaller than the lower bound, the selection is considered invalid.

### Selection refinement

Selection predicates refine the current selection rather than operating independently on the entire image.

For example:

```cpp
image.select(piml::EVERYTHING);

image.select(piml::WHERE_LUMA_GREATER_THAN, 50);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 80);
```

The second predicate operates only on the pixels remaining after the first predicate.

Conceptually, this behaves like:

```text
ALL PIXELS
    ↓
LUMA > 50%
    ↓
LUMA < 80%
    ↓
50% < LUMA < 80%
```

This allows multiple selection conditions to be composed to create more precise regions.

### Channel selection

PIML can also select which channel an effect should operate on:

```cpp
piml::CHANNEL_RED
piml::CHANNEL_GREEN
piml::CHANNEL_BLUE
piml::CHANNEL_ALPHA
```

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::CHANNEL_GREEN);

effect.brightness(-20);
```

This applies the brightness operation specifically to the selected channel.

Channel selection and pixel selection are separate concepts. A luma predicate determines **which pixels** are selected, while a channel selection determines **which part of those pixels** an effect operates on.

This allows operations such as:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 40);
image.select(piml::CHANNEL_BLUE);

effect.brightness(10);
```

The effect is therefore applied only to the blue channel of darker pixels.

### Selection reset

Calling:

```cpp
image.select(piml::EVERYTHING);
```

can be used to start a new selection from all pixels.

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 50);

effect.brightness(-20);

image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN, 50);

effect.brightness(20);
```

The first operation affects darker pixels, while the second operation starts from the complete image and affects brighter pixels.

## Effects

Effects are provided through `piml::Effect`.

For example:

```cpp
piml::Effect effect = image;

effect.brightness(10);
```

Effects operate on the image's current selection.

The effect system is intentionally independent of the selection system. Effects do not need to know why a pixel was selected or which selection predicate was used.

### Brightness

The brightness effect accepts a percentage value:

```cpp
effect.brightness(10);
```

Positive values increase brightness, while negative values decrease it.

The value is normalized and constrained to the supported range internally.

Brightness can operate on the normal RGB channels or on a specifically selected channel.

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::CHANNEL_GREEN);

effect.brightness(-20);
```

Effects only operate on the pixels and channels currently selected.

### Selection-driven effects

Because effects are independent from selection, the same effect can be used for many different operations.

For example, a simple contrast-like operation can be created using luma selection:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 50);

effect.brightness(-20);

image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN, 50);

effect.brightness(20);
```

This darkens pixels below 50% luma and brightens pixels above 50% luma.

More advanced color-grading operations can be constructed by combining luma selection with channel selection.

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 40);
image.select(piml::CHANNEL_BLUE);

effect.brightness(10);

image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN, 60);
image.select(piml::CHANNEL_RED);

effect.brightness(10);
```

This demonstrates the general PIML model:

```text
WHERE → select pixels
WHAT  → select channel
HOW   → apply effect
```

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

PIML uses more memory than a minimal image buffer because the selection system maintains explicit pointers to selected pixels.

This is a deliberate design trade-off.

The additional memory allows the library to provide fine-grained selection and gives the effect system a straightforward way to operate only on the selected pixels.

Selection filtering is performed using temporary pointer buffers rather than repeatedly removing elements from the middle of the selection vector. This allows selection refinement to remain efficient while avoiding unnecessary movement of vector elements.

For applications where memory usage is critical, this is an important consideration when choosing how large an image to process.

## Performance

PIML is designed to keep image operations fast while retaining the selection-based architecture.

Selection predicates currently perform linear scans over the current selection.

Performance depends on factors such as:

* Image dimensions
* Number of selected pixels
* Number of selection predicates
* Number of operations performed
* Image format
* Available memory
* Compiler optimizations

The selection system trades some additional memory for flexibility and fine-grained control.

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

| File                        | Description                      |
| --------------------------- | -------------------------------- |
| `piml.hh`                   | Main public API                  |
| `pimlio.hh`                 | Image input/output API           |
| `src/pimlio.cc`             | Image I/O implementation         |
| `src/classes/Image.cc`      | `Image` implementation           |
| `src/classes/Debugger.cc`   | Debugger implementation          |
| `src/effects/brightness.cc` | Brightness effect implementation |
| `test/test.cc`              | Test program                     |
| `build.cc`                  | Build implementation             |
| `build.sh`                  | Build script                     |
| `test.sh`                   | Build and run tests              |
| `clean.sh`                  | Clean generated files            |

## Design

PIML is built around a selection-driven architecture:

```text
Image
  │
  ├── Pixel Data
  │
  ├── Selection
  │      │
  │      ├── WHERE → Which pixels?
  │      │
  │      └── WHAT  → Which channel?
  │
  └── Effect
         │
         └── HOW → What operation?
```

`Image` owns the image data and current selection.

The selection system determines which pixels should be affected and, when requested, which channel of those pixels an effect should operate on.

`Effect` performs the actual image operation.

Keeping these responsibilities separate means that effects do not need to implement their own selection logic.

A new selection predicate can therefore be introduced without requiring existing effects to be modified.

Likewise, a new effect can operate on existing selection types without needing to understand how those selections were constructed.

This separation is a core part of PIML's design.

## Roadmap

PIML is still under development.

Planned areas of development include:

* Additional image effects
* More selection types
* More channel-based selection mechanisms
* Complex selection rules
* Conditional selections
* Combining multiple selection conditions
* Additional image formats
* Performance improvements
* Memory usage improvements
* Expanded test coverage
* More advanced pixel-property predicates

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

[GitHub Repository](https://github.com/darkyboys/piml)
