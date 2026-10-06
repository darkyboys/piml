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

# Brightness Effects

PIML provides several effects that operate on the **brightness of selected pixels**.

These effects use the average of the pixel's RGB channels as its brightness value:

```text
brightness = (r + g + b) / 3.0
```

The alpha channel does not participate in these calculations.

All of the following effects operate on the image's **current selection**. They can therefore be combined with the selection system described earlier:

```text
WHERE → Which pixels?
WHAT  → Which channel?
HOW   → Which brightness operation?
```

The available brightness-related effects are:

```cpp
image.apply_effect.linear_contrast(value, point);

image.apply_effect.linear_gain(value, point);

image.apply_effect.linear_lift(value, point);

image.apply_effect.smoothing_brightness(value);

image.apply_effect.smoothing_brightness_opposite(value);
```

These effects are designed to modify pixel brightness according to the pixel's position relative to a specified brightness point or according to its existing brightness.

---

## Linear Contrast

```cpp
image.apply_effect.linear_contrast(value, point);
```

`linear_contrast()` applies a brightness adjustment relative to a specified brightness point.

The `point` argument represents the reference brightness around which the operation is performed.

The calculation is based on the normalized distance between the pixel brightness and the specified point:

```text
calculation = (brightness - point) / point
```

The resulting value describes how far the pixel is from the reference point.

Conceptually:

```text
             point
               │
               ▼
0.0 ───────────┼─────────── 1.0
      darker   │   brighter
```

Pixels below the point produce a negative calculation, while pixels above the point produce a positive calculation.

The calculated value is then scaled according to `value`.

### Example

```cpp
image.select(piml::EVERYTHING);

image.apply_effect.linear_contrast(20, 0.5);
```

This applies a linear contrast adjustment around a brightness point of `50%`.

The `value` argument controls the amount of the adjustment.

Because PIML internally uses normalized floating-point pixel values, the effect operates on the normalized brightness representation rather than directly on 8-bit or 16-bit integer values.

---

## Linear Gain

```cpp
image.apply_effect.linear_gain(value, point);
```

`linear_gain()` increases the brightness of pixels that are above the specified brightness point.

The effect calculates the normalized distance from the point:

```text
calculation = (brightness - point) / point
```

Only positive results are used:

```text
calculation > 0.0
```

This means pixels below the specified point receive no additional brightness from the gain calculation, while pixels above the point receive an adjustment.

Conceptually:

```text
0.0 ───────────┼─────────── 1.0
               │
             point

       no gain │ positive gain
```

### Example

```cpp
image.select(piml::EVERYTHING);

image.apply_effect.linear_gain(20, 0.5);
```

This applies a gain of `20%` to the brighter pixels relative to a `50%` brightness point.

`linear_gain()` is therefore useful when the goal is to emphasize brighter areas without applying the same adjustment to darker areas.

---

## Linear Lift

```cpp
image.apply_effect.linear_lift(value, point);
```

`linear_lift()` performs the complementary operation to `linear_gain()`.

Instead of applying the adjustment to pixels above the reference point, it applies the adjustment to pixels below the point.

The normalized distance is calculated as:

```text
calculation = (brightness - point) / point
```

Only negative values are used. The negative value is converted into a positive magnitude before applying the adjustment.

Conceptually:

```text
0.0 ───────────┼─────────── 1.0
               │
             point

 positive lift │ no lift
```

### Example

```cpp
image.select(piml::EVERYTHING);

image.apply_effect.linear_lift(20, 0.5);
```

This applies a `20%` lift to pixels below the `50%` brightness point.

This makes `linear_lift()` useful for selectively increasing the brightness of darker pixels.

---

## Smoothing Brightness

```cpp
image.apply_effect.smoothing_brightness(value);
```

`smoothing_brightness()` applies an adjustment based on the existing brightness of each pixel.

The amount of adjustment is calculated using:

```text
(1.0 - brightness) * change
```

where:

```text
change = normalize_percentage(value)
```

This causes darker pixels to receive a larger adjustment and brighter pixels to receive a smaller adjustment.

For example:

```text
brightness = 0.0

(1.0 - 0.0) = 1.0
```

The full change is applied.

At the other extreme:

```text
brightness = 1.0

(1.0 - 1.0) = 0.0
```

No change is applied.

Conceptually:

```text
Dark pixels                         Bright pixels

more adjustment  ────────────────►  less adjustment

0.0 brightness                     1.0 brightness
```

### Example

```cpp
image.select(piml::EVERYTHING);

image.apply_effect.smoothing_brightness(20);
```

This applies a `20%` brightness adjustment with stronger changes on darker pixels and progressively smaller changes as pixel brightness increases.

This creates a smoothing-style brightness adjustment rather than applying the same brightness change uniformly to every pixel.

---

## Smoothing Brightness Opposite

```cpp
image.apply_effect.smoothing_brightness_opposite(value);
```

`smoothing_brightness_opposite()` applies the opposite brightness weighting.

The adjustment is based directly on the pixel's brightness:

```text
brightness * change
```

where:

```text
change = normalize_percentage(value)
```

This means brighter pixels receive a larger adjustment while darker pixels receive a smaller adjustment.

For example:

```text
brightness = 0.0

0.0 * change = 0.0
```

while:

```text
brightness = 1.0

1.0 * change = change
```

Conceptually:

```text
Dark pixels                         Bright pixels

less adjustment  ────────────────►  more adjustment

0.0 brightness                     1.0 brightness
```

### Example

```cpp
image.select(piml::EVERYTHING);

image.apply_effect.smoothing_brightness_opposite(20);
```

This applies a `20%` brightness adjustment with a stronger effect on brighter pixels.

---

## Comparing the Brightness Effects

The brightness effects can be understood by looking at how they distribute the adjustment across the brightness range.

| Effect                            | Primary behavior                                           |
| --------------------------------- | ---------------------------------------------------------- |
| `linear_contrast()`               | Adjusts brightness relative to a reference point           |
| `linear_gain()`                   | Applies the adjustment to pixels above the reference point |
| `linear_lift()`                   | Applies the adjustment to pixels below the reference point |
| `smoothing_brightness()`          | Applies more adjustment to darker pixels                   |
| `smoothing_brightness_opposite()` | Applies more adjustment to brighter pixels                 |

The smoothing effects can be visualized as:

```text
smoothing_brightness

Adjustment
    ▲
    │\
    │ \
    │  \
    │   \
    │    \
    │     \
    └──────────────► Brightness
    dark          bright
```

and:

```text
smoothing_brightness_opposite

Adjustment
    ▲
    │     /
    │    /
    │   /
    │  /
    │ /
    │/
    └──────────────► Brightness
    dark          bright
```

The linear gain and lift operations instead use a configurable reference point:

```text
                 point
                   │
                   ▼
0.0 ───────────────┼─────────────── 1.0
       lift        │       gain
```

This makes the `point` parameter useful for deciding which part of the brightness range should receive the adjustment.

---

## Combining Brightness Effects With Selection

Because all brightness effects operate on the current selection, they can be combined with PIML's selection predicates.

For example, darker pixels can first be selected:

```cpp
image.select(piml::EVERYTHING);

image.select(
    piml::WHERE_LUMA_SMALLER_THAN,
    40
);

image.apply_effect.linear_lift(20, 0.5);
```

This means:

```text
WHERE → luma < 40%

HOW   → linear lift +20
```

Only pixels with a luma below `40%` are processed.

A channel can also be selected:

```cpp
image.select(piml::EVERYTHING);

image.select(
    piml::WHERE_LUMA_SMALLER_THAN,
    40
);

image.select(piml::CHANNEL_BLUE);

image.apply_effect.smoothing_brightness(20);
```

This means:

```text
WHERE → luma < 40%

WHAT  → blue channel

HOW   → smoothing brightness +20
```

The effect therefore does not need to know whether the pixels were selected using a luma predicate, a future color predicate, or another selection mechanism.

---

## Brightness Effects and the Selection Model

These effects follow the same design principle used throughout PIML:

```text
WHERE
  ↓
Which pixels?

WHAT
  ↓
Which channel?

HOW
  ↓
Which brightness operation?
```

For example:

```cpp
image.select(piml::EVERYTHING);

image.select(
    piml::WHERE_LUMA_GREATER_THAN,
    60
);

image.select(piml::CHANNEL_RED);

image.apply_effect.linear_gain(15, 0.5);
```

Conceptually:

```text
WHERE → luma > 60%

WHAT  → red channel

HOW   → linear gain +15
```

The selection system determines **where** the operation occurs, while the brightness effect determines **how the selected pixels are modified**.

This separation allows the same brightness effect to be reused with different pixel selections and channel selections.

### Fairly Simple Teal & Orange Effect using the brightness effect.
```cpp
    img.select(EVERYTHING);
    img.apply_effect.brightness(5);
    img.select(piml::CHANNEL_RED);
    img.apply_effect.linear_contrast(10);
    img.select(EVERYTHING);
    img.select(piml::CHANNEL_BLUE);
    img.apply_effect.linear_gain(-10);
```

This creates the hollyood's Teal & Orange effects without any curve or anything. Just a linear teal & orange. For more refinements consider using smoothing_brightness in the end.

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
