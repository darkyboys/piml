# PIML

**PIML — Pixel Image Manipulation Library**

PIML is a C++ image-processing library built around a simple idea: select the pixels you want to modify, choose the channel or image data you want to affect, and compose effects to create a custom image-processing pipeline.

Rather than relying only on predefined filters, PIML provides a selection-driven workflow for manipulating image regions according to color, intensity, luma, and channel-based criteria.

## Table of Contents

- [Overview](#overview)
- [Core Concepts](#core-concepts)
- [Getting Started](#getting-started)
- [Working with Selections](#working-with-selections)
- [Channels](#channels)
- [Effects](#effects)
- [Example: Teal and Orange Grading](#example-teal-and-orange-grading)
- [Image Analysis and Debugging](#image-analysis-and-debugging)
- [Design Philosophy](#design-philosophy)
- [Implementation Notes](#implementation-notes)

## Overview

PIML is designed for composable image manipulation. Its main components are:

- **Pixel representation:** Stores per-pixel color components and provides channel-aware operations.
- **Selection engine:** Identifies the pixels that should participate in an operation.
- **Channel selection:** Restricts operations to a particular color channel.
- **Effect system:** Provides reusable brightness, contrast, intensity, and other transformations.
- **Complementary selection:** Allows processing the pixels outside a previously selected region.
- **Debugging utilities:** Help inspect image-processing operations and selection sizes.

The central workflow is:

1. Select a pixel population.
2. Choose the channel or data to manipulate.
3. Apply an effect.
4. Change the selection and continue processing.

This makes it possible to build custom grading pipelines from smaller operations.

## Core Concepts

### Pixels

PIML's `Pixel` representation stores color components as `double` values:

```cpp
double r, g, b, a;
```

The library's pixel operations use normalized channel values in the range `[0.0, 1.0]`.

For example:

- `0.0` represents the minimum channel value.
- `1.0` represents the maximum channel value.

The pixel representation also supports channel-aware operations through an internal selection pointer. When a channel is selected, operations such as addition, subtraction, and channel access can act on the selected component rather than treating every channel identically.

### Images

The `Image` class manages the pixel collection and provides the interface for selection, effects, and debugging.

Typical image-processing operations are performed through an `Image` instance:

```cpp
img.select(EVERYTHING);
img.select(CHANNEL_RED);
img.apply_effect().brightness(-5);
```

The exact effect result depends on the effect's implementation and its parameters.

### Selection and Effects

Selection determines **where** an operation is applied. Effects determine **what** happens to the selected data.

These are separate concerns. A single effect can be reused with different selections, allowing the same transformation to produce different results depending on which pixels receive it.

## Getting Started

### Include PIML

Include the main library header:

```cpp
#include "../piml.hh"
#include "../pimlio.hh"

using namespace piml;
```

The second header is used for the image I/O functions shown in the examples.

### Load and Save an Image

```cpp
#include "../piml.hh"
#include "../pimlio.hh"

int main() {
    using namespace piml;

    auto img = pimlio_read("img/test.png");

    img.select(EVERYTHING);
    img.select(CHANNEL_RED);
    img.apply_effect().brightness(-5);
    img.clean_selection();

    pimlio_write(img, "output.png");
}
```

This example loads an image, adjusts its red channel, clears the active channel selection, and writes the result.

The example uses the project's existing header and I/O interfaces. Adapt the include paths and build command to your project layout.

## Working with Selections

PIML's selection engine is a core part of its image-processing model.

A selection determines which pixels are available to subsequent operations. Depending on the selection type, pixels may be chosen based on the entire image, a channel, intensity, luma, or a custom color-selection rule.

### Color
`Color` is a type struct in the **PIML Library** to provide the *I/O* System for the developer facing API.

It contains 3 doubles:
 - **r**: This stores the channel red value
 - **g**: This stores the channel green value
 - **b**: This stores the channel blue value

The values can be anything as long as a `double` on your system allows but `PIML` itself expects these values to be somewhere between `0.0` to `1.0`, If you go above or bellow them the library will internally clamp it anyways.

### Selecting the Entire Image

```cpp
img.select(EVERYTHING);
```

Use `EVERYTHING` when an operation should affect the full image.

### Selecting by Intensity

PIML provides intensity-based selection options, including:

```cpp
img.select(WHERE_INTENSITY_GREATER_THAN, 50);
img.select(WHERE_INTENSITY_GREATER_THAN_OR_EQUALS, 50);

img.select(WHERE_INTENSITY_SMALLER_THAN, 50);
img.select(WHERE_INTENSITY_SMALLER_THAN_OR_EQUALS, 50);
```

These examples illustrate threshold-based selection. The threshold values and comparison semantics should be interpreted according to the implementation of the corresponding selection functions.

### Selecting by Luma

Luma-based selection options include:

```cpp
img.select(WHERE_LUMA_GREATER_THAN, 50);
img.select(WHERE_LUMA_SMALLER_THAN, 50);
img.select(WHERE_LUMA_SMALLER_THAN_OR_EQUALS, 50);
```

Intensity and luma are distinct concepts. Their exact calculations and threshold units depend on PIML's implementation; do not assume that an intensity threshold and a luma threshold select identical pixels.

### Selecting Warm Colors

```cpp
img.select(WARM_COLORS);
```

`WARM_COLORS` uses a custom RGB-based selection rule to identify a subset of pixels associated with warm colors.

This selection is **not a skin-tone detector**. It may select many skin tones in a particular image, but it does not guarantee selection of every skin tone and may also select other warm-colored objects, lighting, or scenery.

The resulting selection depends on the rule and the image's color distribution.

### Selecting the Remaining Pixels

```cpp
img.select(REMAINING);
```

`REMAINING` is used to target the complementary pixel population: pixels outside the relevant selection.

This is particularly useful when an image needs different treatments in different regions. For example, one selection can receive a warm color treatment while its complement receives a cooler treatment.

The exact behaviour depends on how PIML maintains selection state between calls. For complex pipelines, ensure that the active selection is the one you intend to complement.

### Clearing a Selection

```cpp
img.clear_selection();
```

The library examples also use:

```cpp
img.clean_selection();
```

These operations are used to clear active selection state. Check the implementation for any differences between the two interfaces before treating them as interchangeable.

## Channels

PIML supports channel selection, including:

```cpp
CHANNEL_RED
CHANNEL_GREEN
CHANNEL_BLUE
CHANNEL_ALPHA
```

For example:

```cpp
img.select(EVERYTHING);
img.select(CHANNEL_BLUE);
img.apply_effect().brightness(-5);
```

This applies a brightness adjustment through the selected blue channel.

Channel selection can be combined with pixel selection to control both the region and the component being modified.

Conceptually:

- Pixel selection determines the target population.
- Channel selection determines the target component.
- The effect determines the transformation.

This combination is useful for color grading and other selective image manipulations.

## Effects

PIML provides reusable effects that can be combined with selections and channel operations.

The following names are available in the library's effect interface.

| Effect | Purpose |
|---|---|
| `brightness(value)` | Incremental Brightness adjustment. Literally increments the intensity (RGB Combined) in every selected pixel (Actually great for viewing the selection) |
| `smoothing_brightness(value)` | Smoothing-based brightness adjustment (Affects dark way more than brighter pixels) |
| `smoothing_brightness_opposite(value)` | Opposite-direction smoothing-based brightness adjustment (Affects brights way more than darker pixels) |
| `intensity_lift(value)` | Increases the intensity (RGB Combined) in dark pixels |
| `intensity_gain(value)` | Increases the intensity (RGB Combined) in dark pixels |
| `intensity_corners(value)` | Gives the access to both the corner points in the image and changes their position together (50% makes a flat line) and based on that changes intensity (RGB Values), The value 100 makes the blacks white and whites black. |
| `intensity_contrast(value)` | Decreases the contrast by giving `intensity_corners` value divided by 2 |
| `corners(value)` | Works same as `linear_corners` but works independently for each channel (The value 100 here actually inverts the entire image) |
| `contrast(value)` | Decreases the contrast by giving `corners` value divided by 2 (The standard contrast) |
| `color_brightness(value, color, distance)` | This effect applies the brightness depending on how far a color is from the given color. If the color is too far / difference like (Blue) and the input color is (Red) then the brightness won't be increased. This calculates independently of selection (Giving the correct color selection) but respects the selection when applying the brightness change. This is recommended in advanced color correction and grading. |

Always remember that you are directly working with the image on a graph so be careful while applying effects.

### Applying an Effect

```cpp
img.select(EVERYTHING);
img.select(CHANNEL_RED);
img.apply_effect().brightness(-5);
```

### Applying Multiple Effects

```cpp
img.select(EVERYTHING);
img.select(CHANNEL_RED);
img.apply_effect().brightness(-5);

img.select(CHANNEL_BLUE);
img.apply_effect().brightness(5);
```

This illustrates the composition of channel-specific adjustments. The exact output depends on the implementations of the selected effects.

### Effect Parameters

Effect parameters should be interpreted according to each function's implementation. Do not assume that every effect uses the same mathematical mapping, parameter scale, or tonal response.

For predictable results, document each effect's input range, formula, clamping behaviour, and treatment of selected channels alongside its implementation.

## Example: Teal and Orange Grading

One of PIML's strengths is the ability to combine custom pixel selection with independent channel adjustments.

The following pipeline uses a warm-color selection and its complement to create a teal-and-orange-style grade.

```cpp
#include "../piml.hh"
#include "../pimlio.hh"

int main() {
    using namespace piml;

    auto img = pimlio_read("img/test.png");
    img.select(EVERYTHING);
    img.select(CHANNEL_RED);
    img.apply_effect().brightness(-5);
    img.clean_selection();

    Color col = {0.0, 1.0, 0.0};
    img.select(piml::WHERE_COLORS_MATCHES, col, 78);
    img.select(CHANNEL_RED);
    img.apply_effect().color_brightness(-5, {0.0, 1.0, 0.0});
    img.clean_selection();

    col = {1.0, 0.0, 0.0};
    img.select(EVERYTHING);
    img.select(piml::WHERE_COLORS_MATCHES, col, 80);
    img.select(CHANNEL_RED);
    img.apply_effect().color_brightness(25, {1.0, 0.0, 0.0});

    img.select(CHANNEL_BLUE);
    img.apply_effect().color_brightness(-20, {1.0, 0.0, 0.0}, 60);
    img.clean_selection();

    pimlio_write(img, "test_output.png");

    piml::Debugger db(img);
    db.show_everything();
}
```

This gives an advanced teal and orange shade to an image while still being in the RGB Color Space.

### How the Pipeline Works

**1. Global red adjustment**

The first operation modifies the red channel across the image. This establishes a global adjustment before the more selective operations.

**2. Warm-color selection**

`WARM_COLORS` identifies pixels according to PIML's custom RGB-based rule. This selection may include skin tones, but it is not limited to them.

**3. Red and blue adjustments on the warm selection**

The selected warm-color population receives separate red- and blue-channel transformations. Their combined effect can push this population toward a warmer, more orange appearance.

**4. Complementary-region adjustment**

`REMAINING` targets the pixels outside the warm selection. The red-channel transformation gives this region a different treatment from the selected warm population.

Together, these operations can create a teal-and-orange-style grade through selective color manipulation rather than a single fixed filter.

### Why This Is Useful

The grade is built from general-purpose operations rather than a dedicated teal-and-orange effect.

The selection engine controls which pixels receive each transformation. Channel selection controls which component is affected. The effect system provides the transformations themselves.

The final visual result depends on the input image, the selection rule, and the mathematical behaviour of each effect. In particular, reducing red alone does not guarantee that every remaining pixel becomes teal; the resulting RGB balance determines its appearance.

## Image Analysis and Debugging

PIML includes a `Debugger` interface for inspecting image-processing state.

```cpp
piml::Debugger db(img);
db.show_everything();
```

The library examples also use:

```cpp
db.show_pixels();
```

These utilities can help inspect pixel data and investigate the behaviour of selections and effects.

Selection counts are especially useful when developing a custom grading pipeline. They can reveal whether a selection is too broad, too narrow, or unexpectedly overlaps with other pixel populations.

For a complementary selection, comparing the selected and remaining pixel counts can also help verify that the partition behaves as expected.

## Design Philosophy

PIML separates image processing into reusable operations:

- **Selection:** Which pixels should be affected?
- **Channel targeting:** Which component should be affected?
- **Transformation:** How should the selected data change?
- **Composition:** How should several operations work together?

This design allows a relatively small collection of effects to support many different workflows.

A brightness adjustment applied to the entire image is one operation. The same adjustment applied only to a selected intensity range, a color-defined population, or the complement of another selection becomes a different image-processing tool.

The selection engine is therefore a central part of PIML's expressive power. Its value is not limited to identifying colors; it makes targeted and composable transformations possible.

## Implementation Notes

When extending PIML or integrating it into another project, keep the following considerations in mind:

- **Channel state:** If pixel getters redirect to the currently selected channel, clear or account for that state before performing calculations that require the original RGB components.
- **Luma calculations:** Ensure luma is computed from the intended RGB values rather than repeatedly reading a redirected channel value.
- **Selection semantics:** Define whether successive selection calls replace, combine, or otherwise transform the active selection.
- **Complement semantics:** Document exactly which selection `REMAINING` complements.
- **Numeric ranges:** Specify the accepted parameter ranges and units for every effect and selection threshold.
- **Initialization:** Initialize pixel components and image dimensions before using them.
- **Copy behaviour:** Ensure copied images and pixels have well-defined dimensions, channel-selection state, and ownership of internal pointers.
- **Clamping:** Document when channel values are clamped and how out-of-range intermediate values are handled.

These details are important for making image-processing results predictable and the API safe to use.

## LUTs (Lookup Tables)

PIML supports creating and exporting **3D Lookup Tables (LUTs)** using its image-processing workflow. A LUT stores a mapping from input RGB values to output RGB values, allowing a color transformation to be reused across different images in compatible applications.

Instead of applying a grading pipeline directly to a photograph, PIML can generate a LUT image, manipulate its pixels using selections, channels, and effects, and export the resulting transformation as a `.cube` file.

This makes it possible to turn a custom PIML grading pipeline into a reusable color-grading preset.

### Generating a LUT

Use `pimlio_give_lut_image(size)` to create an image representing the LUT data.

```cpp
auto lut = pimlio_give_lut_image(64);
```

The argument specifies the LUT size. For a 3D LUT with a size of `64`, the transformation uses a \(64 \times 64 \times 64\) RGB sampling grid, subject to the layout and conventions implemented by PIML's LUT I/O functions.

The generated LUT image can be processed using the same selection and effect operations used for ordinary images.

### Applying Effects to a LUT

Because a LUT is represented as an image, PIML's selection engine and effect system can be used to modify the sampled color values.

For example, the following pipeline creates a LUT with a teal-and-orange-style grading transformation.

```cpp
#include "../piml.hh"
#include "../pimlio.hh"

int main() {
    using namespace piml;

    auto lut = pimlio_give_lut_image(64);

    // Apply a global red-channel adjustment.
    lut.select(EVERYTHING);
    lut.select(CHANNEL_RED);
    lut.apply_effect().brightness(-5);
    lut.clean_selection();

    // Adjust the warm-color population.
    lut.select(WARM_COLORS);
    lut.select(CHANNEL_RED);
    lut.apply_effect().smoothing_brightness_opposite(5);

    lut.select(CHANNEL_BLUE);
    lut.apply_effect().smoothing_brightness_opposite(-5);

    // Adjust the complementary population.
    lut.select(REMAINING);
    lut.select(CHANNEL_RED);
    lut.apply_effect().smoothing_brightness(-10);

    // Export the resulting LUT.
    pimlio_export_lut(
        lut,
        "PIML - Advanced Teal & Orange (DEMO).cube",
        64
    );
}
```

The pipeline follows the same general structure as the photographic grading example:

1. **Initialize the LUT:** Generate a LUT image with the desired sampling resolution.
2. **Apply global adjustments:** Modify the red channel across the LUT.
3. **Process warm colors:** Apply separate red- and blue-channel transformations to the selected population.
4. **Process the complementary population:** Apply a different adjustment to the remaining pixels.
5. **Export the LUT:** Write the transformed color mapping to a `.cube` file.

The resulting LUT encodes the transformation represented by the processed LUT image. Its actual appearance depends on the input LUT layout, selection rules, effect implementations, and export conventions.

### Exporting a LUT

Use `pimlio_export_lut()` to export the processed LUT.

```cpp
pimlio_export_lut(
    lut,
    "PIML - Advanced Teal & Orange (DEMO).cube",
    64
);
```

The function receives the processed LUT image, the output filename, and the LUT size.

The `.cube` extension is commonly used for LUT files supported by color-grading and video-editing software. Compatibility depends on the exported file's formatting, RGB ordering, domain, and sampling conventions.

### Choosing a LUT Size

LUT size determines how densely the RGB color space is sampled.

- **Smaller LUTs:** Use fewer samples and produce more compact mappings.
- **Larger LUTs:** Represent the color transformation with more samples, potentially preserving finer variations in complex transformations.

For example, a size of `64` represents \(64^3 = 262{,}144\) RGB sample points. Increasing the size raises the number of samples cubically, so memory usage and export cost should be considered.

The appropriate size depends on the target application's requirements and the complexity of the transformation.

### LUTs vs. Direct Image Processing

Direct image processing modifies the pixels of a particular image. LUT processing modifies a sampled color transformation that can subsequently be applied to other images.

| Workflow | Purpose |
|---|---|
| Process a photograph | Modify that image's pixels directly |
| Process a LUT image | Build a reusable RGB transformation |
| Export a `.cube` file | Make the transformation available to compatible external software |

A LUT is especially useful when the same grading style needs to be reused across multiple photographs or video clips.

### Important Considerations

When working with LUTs, keep the following points in mind:

- **LUT layout:** The generated image must use the layout expected by PIML's exporter.
- **Color selection:** `WARM_COLORS` operates on the sampled RGB values represented by the LUT image; it is not a semantic detector for skin tones or objects.
- **Resolution:** The LUT size passed to the generator and exporter must match the intended sampling resolution.
- **Effect behavior:** Every transformation changes the RGB mapping stored in the LUT, so the result depends on the mathematical implementation of each effect.
- **Export compatibility:** Verify the exported `.cube` file in the target application, especially its RGB ordering, domain, and interpolation behavior.

PIML's LUT workflow extends its selection-driven design beyond individual images: the same pixel operations can be used to construct a reusable color transformation.

## License

CC0 Universal V1.0 (Public Domain)

---

**PIML — Select the pixels. Choose the channels. Compose the effects.**
