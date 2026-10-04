# PIML

**Precision Image Manipulation Library**

PIML is a lightweight C++ image manipulation library focused on precise, selection-based image processing.

It provides a small API for:

* Loading and writing images
* Selecting pixels
* Refining pixel selections
* Selecting individual color channels
* Applying effects to the current selection
* Debugging image operations

PIML is designed around the idea that **selection and effects should remain separate**. This allows the same effect to operate on completely different parts of an image without requiring the effect itself to understand how those pixels were selected.

The library is intended to remain lightweight and portable while providing enough control for increasingly advanced image manipulation.

---

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
* Selection-driven image effects
* Future support for more complex selection rules

---

## Requirements

PIML only requires:

* A C++ STL implementation
* An STL-compatible C++ compiler

There are no platform-specific dependencies required by the core library.

Because of this, PIML can be built on a wide range of systems, including Linux and environments such as Termux.

---

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

---

# Basic Usage

PIML exposes its main API through `piml.hh` and image I/O through `pimlio.hh`.

A basic program looks like this:

```cpp
#include "piml.hh"
#include "pimlio.hh"

int main() {

    piml::Image image = piml::pimlio_read("img/test.png");

    image.select(piml::EVERYTHING);

    image.apply_effect.brightness(10);

    piml::pimlio_write(image, "test_output.png");

}
```

An image is first loaded from disk:

```cpp
piml::Image image = piml::pimlio_read("img/test.png");
```

A selection can then be created:

```cpp
image.select(piml::EVERYTHING);
```

`EVERYTHING` selects all pixels in the image.

Effects are exposed directly through `Image::apply_effect`:

```cpp
image.apply_effect.brightness(10);
```

There is no need to manually create an `Effect` object.

Finally, the modified image can be written back to disk:

```cpp
piml::pimlio_write(image, "test_output.png");
```

---

# Pixel Representation

PIML stores each pixel using four `double` values:

```cpp
struct Pixel {

    double r;
    double g;
    double b;
    double a;

};
```

The channels represent:

* `r` — Red
* `g` — Green
* `b` — Blue
* `a` — Alpha

Color channel values are represented internally using normalized floating-point values:

```text
0.0 - 1.0
```

This allows image operations to work with normalized values rather than being directly tied to a particular integer pixel format.

For example:

```text
0.0 = 0%
0.5 = 50%
1.0 = 100%
```

---

# Selection

Selection is the central part of PIML's design.

An `Image` maintains a current selection of pixels. Effects operate on that selection.

The selection system can be understood using three concepts:

```text
WHERE → Which pixels?
WHAT  → Which channel?
HOW   → Which effect?
```

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 50);
image.select(piml::CHANNEL_BLUE);

image.apply_effect.brightness(10);
```

This means:

```text
WHERE → pixels with luma < 50%
WHAT  → blue channel
HOW   → increase brightness
```

The effect does not need to know why those pixels were selected.

---

## Selecting All Pixels

```cpp
image.select(piml::EVERYTHING);
```

`EVERYTHING` initializes the current selection with every pixel in the image.

It can also be used to start a new selection after a previous selection has already been processed.

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 50);

image.apply_effect.brightness(-20);

image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN, 50);

image.apply_effect.brightness(20);
```

The second `EVERYTHING` starts the second selection from the complete image rather than from the previously filtered selection.

---

# Luma Selection

PIML provides selection predicates based on pixel luma.

Currently, PIML defines luma as the average of the red, green, and blue channels:

```text
luma = (r + g + b) / 3.0
```

Alpha does not participate in the luma calculation.

Because channels are normalized, luma is also represented in the range:

```text
0.0 - 1.0
```

Selection thresholds are specified using percentages.

For example:

```cpp
image.select(piml::WHERE_LUMA_GREATER_THAN, 50);
```

means:

```text
luma > 50%
```

---

## Available Luma Predicates

PIML currently provides:

```cpp
piml::WHERE_LUMA_GREATER_THAN
piml::WHERE_LUMA_GREATER_THAN_OR_EQUALS
piml::WHERE_LUMA_SMALLER_THAN
piml::WHERE_LUMA_SMALLER_THAN_OR_EQUALS
piml::WHERE_LUMA_BETWEEN
piml::WHERE_LUMA_EQUALS
```

---

## Greater Than

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN, 50);
```

Selects pixels where:

```text
luma > 50%
```

---

## Smaller Than

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 50);
```

Selects pixels where:

```text
luma < 50%
```

---

## Greater Than or Equals

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN_OR_EQUALS, 50);
```

Selects pixels where:

```text
luma >= 50%
```

---

## Smaller Than or Equals

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN_OR_EQUALS, 50);
```

Selects pixels where:

```text
luma <= 50%
```

---

## Equals

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_EQUALS, 50);
```

Selects pixels where the calculated luma is equal to the specified value.

---

# Luma Ranges

`WHERE_LUMA_BETWEEN` accepts two percentage values.

The first value is the lower bound and the second value is the upper bound:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_BETWEEN, 30, 70);
```

This selects pixels where:

```text
30% <= luma <= 70%
```

The range is inclusive.

The lower bound must be supplied before the upper bound.

For example:

```cpp
image.select(piml::WHERE_LUMA_BETWEEN, 70, 30);
```

is invalid because the upper bound is smaller than the lower bound.

---

# Selection Refinement

Selection predicates refine the **current selection**.

They do not automatically create a new selection from the entire image.

For example:

```cpp
image.select(piml::EVERYTHING);

image.select(piml::WHERE_LUMA_GREATER_THAN, 30);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 70);
```

The resulting selection contains pixels where:

```text
30% < luma < 70%
```

Conceptually:

```text
ALL PIXELS
    │
    ▼
LUMA > 30%
    │
    ▼
LUMA < 70%
    │
    ▼
SELECTED PIXELS
```

This allows multiple selection predicates to be combined.

The selection system therefore behaves similarly to a filtering or query system:

```text
Select everything
       ↓
Apply condition
       ↓
Apply another condition
       ↓
Apply effect
```

---

# Channel Selection

PIML separates **pixel selection** from **channel selection**.

Available channel selections are:

```cpp
piml::CHANNEL_RED
piml::CHANNEL_GREEN
piml::CHANNEL_BLUE
piml::CHANNEL_ALPHA
```

A channel selection determines which channel an effect operates on for the currently selected pixels.

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::CHANNEL_GREEN);

image.apply_effect.brightness(-20);
```

This selects every pixel but applies the brightness operation only to the green channel.

---

## Combining Pixel and Channel Selection

Pixel and channel selection can be combined.

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 40);
image.select(piml::CHANNEL_BLUE);

image.apply_effect.brightness(10);
```

This can be interpreted as:

```text
WHERE → luma < 40%
WHAT  → blue channel
HOW   → brightness +10
```

Only the blue channel of pixels below 40% luma is modified.

This separation allows effects to remain simple while the selection system provides increasingly precise control.

---

# Effects

Effects are exposed through:

```cpp
image.apply_effect
```

For example:

```cpp
image.apply_effect.brightness(10);
```

The `Effect` object is associated with the `Image` and operates on that image's current selection.

Users therefore do not need to manually construct an `Effect` object.

The selection determines **where** an effect operates, while the effect determines **what operation** is performed.

---

# Brightness

The currently available brightness effect is:

```cpp
image.apply_effect.brightness(10);
```

The effect accepts a percentage value.

Positive values increase brightness:

```cpp
image.apply_effect.brightness(20);
```

Negative values decrease brightness:

```cpp
image.apply_effect.brightness(-20);
```

The value is normalized and constrained internally.

When no channel is specifically selected, brightness operates on the RGB channels of the selected pixels.

When a channel is selected, brightness operates only on that channel.

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::CHANNEL_RED);

image.apply_effect.brightness(20);
```

This affects only the red channel.

---

# Selection-Driven Processing

One of the main goals of PIML is to allow complex image operations to be constructed by combining simple selections and effects.

The effect itself does not need to implement separate logic for every possible selection.

For example, a simple contrast-like operation can be constructed using luma selection:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 50);

image.apply_effect.brightness(-20);

image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN, 50);

image.apply_effect.brightness(20);
```

This performs:

```text
Dark pixels  → darker
Bright pixels → brighter
```

No dedicated contrast effect is required for this particular operation.

---

# Color Selection Example

The same system can be used for basic color manipulation.

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_SMALLER_THAN, 50);
image.select(piml::CHANNEL_RED);

image.apply_effect.brightness(-20);

image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN, 50);

image.apply_effect.brightness(20);
```

The first operation modifies the red channel of darker pixels.

The second operation increases the brightness of brighter pixels.

More complex color grading can be constructed by combining different luma ranges, channel selections, and effects.

---

# Image I/O

Image input/output is provided by `pimlio.hh`.

## Reading an Image

```cpp
piml::Image image = piml::pimlio_read("image.png");
```

Image channels are stored internally as normalized `double` values.

The source image's bit depth is preserved when it can be determined.

---

## Writing an Image

```cpp
piml::pimlio_write(image, "output.png");
```

The output format is selected from the filename extension.

PIML currently supports output at:

* 8 bits per channel
* 16 bits per channel

Values are clamped to:

```text
[0.0, 1.0]
```

before being converted to the output format.

---

# Debugging

PIML includes a small logging system for debugging and development.

Debugging can be disabled:

```cpp
image.disable_debugging();
```

and enabled again:

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

Individual categories can also be accessed:

```cpp
debugger.show_messages();
debugger.show_warnings();
debugger.show_errors();
debugger.show_activities();
```

Logs can be cleared individually or all at once using the corresponding `Debugger` functions.

---

# Memory Usage

PIML maintains explicit information about selected pixels.

The image stores its actual pixels separately from the current selection:

```text
pixel_vector
    │
    └── actual Pixel objects

selected_pixels
    │
    └── pointers to selected Pixel objects
```

The selection therefore does not duplicate the actual pixel data.

Filtering the selection operates on `Pixel*` pointers.

PIML also uses a temporary pointer buffer when refining selections.

This design provides fine-grained selection while avoiding the cost of repeatedly moving elements when removing pixels from the middle of a vector.

The additional selection memory is a deliberate trade-off for the flexibility provided by the selection system.

For applications where memory usage is critical, this should be considered when choosing how large an image to process.

---

# Performance

PIML is designed to keep image operations fast while retaining the selection-based architecture.

Selection predicates currently scan the current selection linearly.

Selection filtering uses a temporary pointer buffer rather than repeatedly erasing elements from the middle of the selection vector.

Conceptually:

```text
Current Selection
       │
       ▼
    Filter
       │
       ▼
Temporary Pointer Buffer
       │
       ▼
     swap()
       │
       ▼
New Selection
```

This avoids repeated element shifting caused by middle-of-vector erases.

Performance depends on factors such as:

* Image dimensions
* Number of selected pixels
* Number of selection predicates
* Number of effects
* Number of operations performed
* Image format
* Available memory
* Compiler optimizations

PIML trades some additional memory for flexible and fine-grained selection.

---

# Design

PIML is built around a selection-driven architecture.

```text
                         Image
                           │
             ┌─────────────┴─────────────┐
             │                           │
        Pixel Data                   Selection
                                         │
                              ┌──────────┴──────────┐
                              │                     │
                            WHERE                 WHAT
                              │                     │
                       Which pixels?          Which channel?
                              │                     │
                              └──────────┬──────────┘
                                         │
                                         ▼
                                   apply_effect
                                         │
                                         ▼
                                        HOW
                                         │
                                  What operation?
```

The main responsibilities are separated:

### Image

`Image` owns the image data, current selection, image dimensions, bit depth, debugging state, and effect interface.

### Selection

Selection determines which pixels should be affected.

Selection predicates can refine the current selection based on pixel properties such as luma.

Channel selections determine which channel an effect should operate on.

### Effect

`Effect` performs the actual image operation.

Effects operate on the current selection without needing to understand how that selection was created.

This separation allows the same effect to work with:

* All pixels
* Dark pixels
* Bright pixels
* A luma range
* A specific color channel
* A specific channel within a filtered pixel selection

without requiring separate implementations.

---

# The PIML Selection Model

The selection architecture can be summarized as:

```text
WHERE
  ↓
Which pixels?

WHAT
  ↓
Which channel?

HOW
  ↓
Which effect?
```

For example:

```cpp
image.select(piml::EVERYTHING);
image.select(piml::WHERE_LUMA_GREATER_THAN, 60);
image.select(piml::CHANNEL_RED);

image.apply_effect.brightness(15);
```

This reads conceptually as:

```text
WHERE luma > 60%
WHAT  red channel
HOW   brightness +15
```

The goal is to make image operations composable instead of creating a separate effect for every possible combination of conditions.

---

# Project Structure

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

## Source Files

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

---

# Roadmap

PIML is still under development.

Planned areas of development include:

* Additional image effects
* More selection types
* More channel-based selection mechanisms
* More pixel-property predicates
* Complex selection rules
* Conditional selections
* Combining multiple selection conditions
* Additional image formats
* Performance improvements
* Memory usage improvements
* Expanded test coverage
* More advanced color manipulation
* More expressive selection operations

The selection system is expected to become considerably more expressive as the library develops.

---

# Contributing

Contributions are welcome.

If you want to work on PIML, useful areas include:

* Adding new effects
* Improving image I/O
* Implementing new selection mechanisms
* Adding new pixel predicates
* Optimizing existing operations
* Adding tests
* Improving documentation
* Improving portability

For larger changes, opening an issue first can help keep development coordinated.

---

# License

PIML is released under **CC0 1.0 Universal**.

See the license text included with the project for the complete terms.

---

# Repository

[GitHub Repository](https://github.com/darkyboys/piml)
