Docs.add({
    title: "Introduction to PIML",
    category: "General",
    description: `
        <p><strong>PIML</strong> (Precision Image Manipulation Library) is a lightweight, open-source and Public Domain C++ image manipulation library built around precise, selection-driven image processing.</p>

        <p>PIML is designed around one core idea:</p>

        <pre>WHERE → WHAT → HOW</pre>

        <p><strong>WHERE</strong> determines which pixels should be processed.</p>
        <p><strong>WHAT</strong> determines which channel of those pixels should be affected.</p>
        <p><strong>HOW</strong> determines which effect should be applied.</p>

        <p>This separation allows selections and effects to remain independent. An effect does not need to know why a pixel was selected. It simply operates on the current selection.</p>

        <p>PIML provides APIs for:</p>

        <ul>
            <li>Reading images</li>
            <li>Writing images</li>
            <li>Selecting pixels</li>
            <li>Refining selections</li>
            <li>Selecting individual channels</li>
            <li>Selecting pixels using luma</li>
            <li>Selecting pixels using RGB-average intensity</li>
            <li>Applying brightness and tonal effects</li>
            <li>Manipulating intensity and contrast</li>
            <li>Debugging image operations</li>
        </ul>

        <p>The library is intentionally small and portable. Its core functionality is designed to require only a standard C++ STL implementation and a compatible C++ compiler.</p>
    `,
    tags: ["introduction", "piml", "cpp", "image-processing"],
    example: `
        #include "piml.hh"
        #include "pimlio.hh"

        int main()
        {
            using namespace piml;

            auto img = pimlio_read("img/test.png");

            img.select(EVERYTHING);

            img.apply_effect.brightness(10);

            pimlio_write(img, "output.png");
        }
    `
});


Docs.add({
    title: "Core Philosophy",
    category: "General",
    description: `
        <p>PIML is built around the separation of <strong>selection</strong> and <strong>effects</strong>.</p>

        <p>Traditional image manipulation code can easily become tightly coupled: an effect may contain its own logic for deciding which pixels should be modified. PIML deliberately avoids this approach.</p>

        <p>Instead, PIML treats selection as an independent operation.</p>

        <pre>
WHERE
  ↓
Which pixels?

WHAT
  ↓
Which channel?

HOW
  ↓
Which effect?
        </pre>

        <p>This means the same effect can operate on the entire image, shadows, highlights, a specific intensity range, a specific channel, or any future selection type without changing the effect itself.</p>

        <p>The architecture is therefore composable. Complex image operations can be constructed by combining simple selection rules with simple effects.</p>
    `,
    tags: ["architecture", "design", "selection", "effects"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            40
        );

        image.select(
            piml::CHANNEL_BLUE
        );

        image.apply_effect.brightness(10);
    `
});


Docs.add({
    title: "Requirements",
    category: "General",
    description: `
        <p>PIML is designed to have minimal external requirements.</p>

        <p>The core library requires:</p>

        <ul>
            <li>A C++ STL implementation</li>
            <li>An STL-compatible C++ compiler</li>
        </ul>

        <p>The core library does not require platform-specific APIs.</p>

        <p>This makes PIML suitable for a wide range of environments, including conventional Linux systems and lightweight environments such as Termux.</p>

        <p>The exact image formats supported by the I/O implementation depend on the capabilities included by the PIML image I/O layer.</p>
    `,
    tags: ["requirements", "cpp", "portable", "linux", "termux"]
});


Docs.add({
    title: "Building PIML",
    category: "Getting Started",
    description: `
        <p>PIML can be built directly from its repository using the included shell scripts.</p>

        <p>Clone the repository and enter its directory:</p>
    `,
    tags: ["build", "installation", "getting-started"],
    example: `
        git clone https://github.com/darkyboys/piml.git
        cd piml
    `
});


Docs.add({
    title: "Build Script",
    category: "Getting Started",
    description: `
        <p>The repository provides a <code>build.sh</code> script for building PIML.</p>

        <p>The shell script uses <code>build.cc</code> as the actual build implementation.</p>

        <p>Run:</p>
    `,
    tags: ["build", "build.sh", "compiler"],
    example: `
        ./build.sh
    `
});


Docs.add({
    title: "Testing PIML",
    category: "Getting Started",
    description: `
        <p>PIML includes a test program under the <code>test/</code> directory.</p>

        <p>The repository provides <code>test.sh</code> to build and execute the test program.</p>

        <p>Running the test script is useful after modifying the library or adding new functionality.</p>
    `,
    tags: ["testing", "test", "development"],
    example: `
        ./test.sh
    `
});


Docs.add({
    title: "Cleaning Build Files",
    category: "Getting Started",
    description: `
        <p>PIML provides a <code>clean.sh</code> script for removing generated build files.</p>

        <p>This can be useful when performing a clean rebuild or when generated files need to be removed from the working directory.</p>
    `,
    tags: ["clean", "build", "development"],
    example: `
        ./clean.sh
    `
});


Docs.add({
    title: "Basic Usage",
    category: "Getting Started",
    description: `
        <p>A typical PIML program follows a simple processing pipeline:</p>

        <pre>
Read image
    ↓
Create selection
    ↓
Optionally select channel
    ↓
Apply effect
    ↓
Write image
        </pre>

        <p>The public API is primarily exposed through <code>piml.hh</code>, while image input/output is provided by <code>pimlio.hh</code>.</p>

        <p>An <code>Image</code> object owns the image data and provides the selection and effect interfaces used during processing.</p>
    `,
    tags: ["basic", "usage", "workflow", "cpp"],
    example: `
        #include "piml.hh"
        #include "pimlio.hh"

        int main()
        {
            piml::Image image =
                piml::pimlio_read("image.png");

            image.select(piml::EVERYTHING);

            image.apply_effect.brightness(10);

            piml::pimlio_write(
                image,
                "output.png"
            );
        }
    `
});


Docs.add({
    title: "Pixel Representation",
    category: "Image",
    description: `
        <p>PIML internally represents each pixel using four <code>double</code> values.</p>

        <pre>
struct Pixel
{
    double r;
    double g;
    double b;
    double a;
};
        </pre>

        <p>The channels are:</p>

        <ul>
            <li><code>r</code> — Red</li>
            <li><code>g</code> — Green</li>
            <li><code>b</code> — Blue</li>
            <li><code>a</code> — Alpha</li>
        </ul>

        <p>RGB and alpha values are represented internally using normalized floating-point values from <code>0.0</code> to <code>1.0</code>.</p>

        <pre>
0.0 = 0%
0.5 = 50%
1.0 = 100%
        </pre>

        <p>This representation prevents image operations from being tied directly to an 8-bit or 16-bit integer representation.</p>
    `,
    tags: ["pixel", "Pixel", "double", "rgb", "alpha"]
});


Docs.add({
    title: "Reading Images",
    category: "Image I/O",
    description: `
        <p>Image input is provided through <code>pimlio.hh</code>.</p>

        <p><code>pimlio_read()</code> loads an image and returns a PIML <code>Image</code>.</p>

        <p>The image is converted into PIML's internal pixel representation so that processing can operate using normalized floating-point channel values.</p>
    `,
    tags: ["io", "read", "image", "input"],
    example: `
        #include "pimlio.hh"

        piml::Image image =
            piml::pimlio_read("image.png");
    `
});


Docs.add({
    title: "Writing Images",
    category: "Image I/O",
    description: `
        <p>Processed images can be written using <code>pimlio_write()</code>.</p>

        <p>The output format is selected from the output filename extension.</p>

        <p>PIML supports 8-bit and 16-bit output representations. Before conversion, floating-point channel values are constrained to the normalized range:</p>

        <pre>[0.0, 1.0]</pre>

        <p>This prevents values outside the valid output range from producing invalid channel values.</p>
    `,
    tags: ["io", "write", "image", "output"],
    example: `
        piml::pimlio_write(
            image,
            "output.png"
        );
    `
});


Docs.add({
    title: "Selection",
    category: "Selection",
    description: `
        <p>Selection is the central mechanism of PIML.</p>

        <p>An <code>Image</code> maintains a current collection of selected pixels. Effects operate on that current selection.</p>

        <p>Selections are not effects. They only determine <strong>where</strong> processing should occur.</p>

        <p>PIML selection can be understood as:</p>

        <pre>
WHERE → Which pixels?
WHAT  → Which channel?
HOW   → Which effect?
        </pre>

        <p>A selection can begin with the entire image and then be progressively refined using selection predicates.</p>
    `,
    tags: ["selection", "pixels", "where", "architecture"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_BETWEEN,
            30,
            70
        );

        image.apply_effect.brightness(10);
    `
});


Docs.add({
    title: "Selecting Everything",
    category: "Selection",
    description: `
        <p><code>EVERYTHING</code> initializes the current selection with every pixel in the image.</p>

        <p>It is commonly used as the starting point before applying selection predicates.</p>

        <p>Calling <code>EVERYTHING</code> again resets the working selection to the complete image. This is useful when multiple independent operations need to be performed on different portions of an image.</p>

        <p>For example, the first operation can process dark pixels and the second operation can independently process bright pixels.</p>
    `,
    tags: ["EVERYTHING", "selection", "reset"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            50
        );

        image.apply_effect.brightness(-20);

        // Reset selection.
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_GREATER_THAN,
            50
        );

        image.apply_effect.brightness(20);
    `
});


Docs.add({
    title: "Selection Refinement",
    category: "Selection",
    description: `
        <p>Selection predicates refine the <strong>current selection</strong>.</p>

        <p>They do not automatically start from the entire image.</p>

        <p>This allows multiple predicates to behave like a filtering pipeline.</p>

        <pre>
EVERYTHING
    ↓
First condition
    ↓
Second condition
    ↓
Third condition
    ↓
Selected pixels
        </pre>

        <p>For example, selecting pixels with intensity greater than 30 and then smaller than 70 produces a range selection.</p>
    `,
    tags: ["selection", "filtering", "refinement", "predicates"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_GREATER_THAN,
            30
        );

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            70
        );
    `
});


Docs.add({
    title: "Channel Selection",
    category: "Selection",
    description: `
        <p>PIML separates <strong>pixel selection</strong> from <strong>channel selection</strong>.</p>

        <p>Pixel selection determines which pixels are affected. Channel selection determines which channel an effect operates on.</p>

        <p>Available channels include:</p>

        <pre>
piml::CHANNEL_RED
piml::CHANNEL_GREEN
piml::CHANNEL_BLUE
piml::CHANNEL_ALPHA
        </pre>

        <p>When no specific channel is selected, RGB-based effects generally operate on the RGB channels of the selected pixels according to the effect's behavior.</p>
    `,
    tags: ["channels", "rgb", "alpha", "selection"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::CHANNEL_GREEN
        );

        image.apply_effect.brightness(-20);
    `
});


Docs.add({
    title: "Combining Pixel and Channel Selection",
    category: "Selection",
    description: `
        <p>Pixel predicates and channel selection can be combined to create highly targeted operations.</p>

        <p>The pixel predicate determines <strong>where</strong> the operation happens, while the channel determines <strong>what part of those pixels</strong> is modified.</p>

        <p>This allows operations such as modifying only the blue channel of dark pixels or only the red channel of bright pixels.</p>
    `,
    tags: ["selection", "channels", "rgb", "targeting"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            40
        );

        image.select(
            piml::CHANNEL_BLUE
        );

        image.apply_effect.brightness(10);
    `
});


Docs.add({
    title: "Luma Selection",
    category: "Selection Predicates",
    description: `
        <p>PIML provides luma-based selection predicates for selecting pixels according to their luma value.</p>

        <p>PIML's documented luma calculation is:</p>

        <pre>
luma = (r + g + b) / 3.0
        </pre>

        <p>Alpha does not participate in the calculation.</p>

        <p>Luma is normalized between <code>0.0</code> and <code>1.0</code>, while selection thresholds are expressed as percentages.</p>

        <p>Luma predicates can be chained together to progressively refine the current selection.</p>
    `,
    tags: ["luma", "selection", "brightness", "rgb"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_LUMA_GREATER_THAN,
            50
        );
    `
});


Docs.add({
    title: "Luma Predicates",
    category: "Selection Predicates",
    description: `
        <p>PIML provides the following luma selection predicates:</p>

        <pre>
piml::WHERE_LUMA_GREATER_THAN
piml::WHERE_LUMA_GREATER_THAN_OR_EQUALS
piml::WHERE_LUMA_SMALLER_THAN
piml::WHERE_LUMA_SMALLER_THAN_OR_EQUALS
piml::WHERE_LUMA_BETWEEN
piml::WHERE_LUMA_EQUALS
        </pre>

        <p>All threshold arguments are specified as percentages.</p>
    `,
    tags: ["luma", "predicates", "selection"]
});


Docs.add({
    title: "Intensity Selection",
    category: "Selection",
    description: `
        <p>PIML also provides <strong>intensity-based selection</strong>.</p>

        <p>Intensity is calculated as the direct arithmetic average of the RGB channels:</p>

        <pre>
intensity = (r + g + b) / 3.0
        </pre>

        <p>Each RGB channel therefore contributes equally to the result. Alpha is ignored.</p>

        <p>This makes intensity selection particularly useful when the desired definition of brightness is specifically the mathematical RGB average.</p>

        <p>Intensity is normalized between <code>0.0</code> and <code>1.0</code>, and predicates accept percentage thresholds.</p>

        <p>Intensity selection is independent of the effects that operate on the selected pixels.</p>
    `,
    tags: ["intensity", "selection", "rgb", "average", "brightness"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            40
        );
    `
});


Docs.add({
    title: "Intensity Predicates",
    category: "Selection Predicates",
    description: `
        <p>PIML provides six predicates for RGB-average intensity selection:</p>

        <pre>
piml::WHERE_INTENSITY_GREATER_THAN
piml::WHERE_INTENSITY_GREATER_THAN_OR_EQUALS
piml::WHERE_INTENSITY_SMALLER_THAN
piml::WHERE_INTENSITY_SMALLER_THAN_OR_EQUALS
piml::WHERE_INTENSITY_BETWEEN
piml::WHERE_INTENSITY_EQUALS
        </pre>

        <p>These predicates operate on the intensity calculated from the RGB average.</p>

        <p>They refine the current selection in exactly the same architectural manner as the other PIML selection predicates.</p>
    `,
    tags: ["intensity", "predicates", "selection", "rgb"]
});


Docs.add({
    title: "Intensity Greater Than",
    category: "Selection Predicates",
    description: `
        <p><code>WHERE_INTENSITY_GREATER_THAN</code> selects pixels whose RGB-average intensity is greater than the supplied percentage.</p>

        <p>A value of <code>50</code> means:</p>

        <pre>
intensity > 50%
        </pre>
    `,
    tags: ["intensity", "greater-than", "selection"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_GREATER_THAN,
            50
        );
    `
});


Docs.add({
    title: "Intensity Greater Than or Equals",
    category: "Selection Predicates",
    description: `
        <p><code>WHERE_INTENSITY_GREATER_THAN_OR_EQUALS</code> selects pixels whose RGB-average intensity is greater than or equal to the supplied percentage.</p>

        <p>A value of <code>50</code> means:</p>

        <pre>
intensity >= 50%
        </pre>

        <p>Pixels exactly at the specified boundary are included.</p>
    `,
    tags: ["intensity", "greater-than-or-equals", "selection"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_GREATER_THAN_OR_EQUALS,
            50
        );
    `
});


Docs.add({
    title: "Intensity Smaller Than",
    category: "Selection Predicates",
    description: `
        <p><code>WHERE_INTENSITY_SMALLER_THAN</code> selects pixels whose RGB-average intensity is smaller than the supplied percentage.</p>

        <p>A value of <code>50</code> means:</p>

        <pre>
intensity < 50%
        </pre>

        <p>This is useful for selecting darker portions of an image according to the direct RGB average.</p>
    `,
    tags: ["intensity", "smaller-than", "selection", "dark"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            50
        );
    `
});


Docs.add({
    title: "Intensity Smaller Than or Equals",
    category: "Selection Predicates",
    description: `
        <p><code>WHERE_INTENSITY_SMALLER_THAN_OR_EQUALS</code> selects pixels whose RGB-average intensity is less than or equal to the supplied percentage.</p>

        <pre>
intensity <= threshold
        </pre>

        <p>The boundary value is included.</p>
    `,
    tags: ["intensity", "smaller-than-or-equals", "selection"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN_OR_EQUALS,
            50
        );
    `
});


Docs.add({
    title: "Intensity Between",
    category: "Selection Predicates",
    description: `
        <p><code>WHERE_INTENSITY_BETWEEN</code> selects pixels whose RGB-average intensity lies inside an inclusive range.</p>

        <p>The first argument is the lower bound and the second argument is the upper bound.</p>

        <pre>
lower <= intensity <= upper
        </pre>

        <p>Both boundaries are included.</p>

        <p>The lower bound must be supplied before the upper bound.</p>
    `,
    tags: ["intensity", "between", "range", "selection"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_BETWEEN,
            30,
            70
        );
    `
});


Docs.add({
    title: "Intensity Equals",
    category: "Selection Predicates",
    description: `
        <p><code>WHERE_INTENSITY_EQUALS</code> selects pixels whose calculated RGB-average intensity equals the specified value.</p>

        <p>The threshold is specified as a percentage.</p>

        <pre>
intensity = threshold
        </pre>
    `,
    tags: ["intensity", "equals", "selection"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_EQUALS,
            50
        );
    `
});


Docs.add({
    title: "Luma vs Intensity",
    category: "Selection",
    description: `
        <p>PIML supports both luma-based and intensity-based selection. They should be treated as separate selection concepts even when their current mathematical representations are similar.</p>

        <h3>Luma</h3>

        <p>Luma represents the image's luma calculation and is exposed through the <code>WHERE_LUMA_*</code> predicates.</p>

        <h3>Intensity</h3>

        <p>Intensity explicitly represents the arithmetic RGB average:</p>

        <pre>
intensity = (r + g + b) / 3.0
        </pre>

        <p>Intensity selection is useful when exact equal weighting of RGB is desired for brightness-based selection.</p>

        <p>The intensity predicates are:</p>

        <pre>
WHERE_INTENSITY_GREATER_THAN
WHERE_INTENSITY_GREATER_THAN_OR_EQUALS
WHERE_INTENSITY_SMALLER_THAN
WHERE_INTENSITY_SMALLER_THAN_OR_EQUALS
WHERE_INTENSITY_BETWEEN
WHERE_INTENSITY_EQUALS
        </pre>

        <p>Both selection systems can be combined with channel selection and effects.</p>
    `,
    tags: ["luma", "intensity", "comparison", "selection", "rgb"]
});


Docs.add({
    title: "Effects",
    category: "Effects",
    description: `
        <p>PIML exposes image effects through <code>Image::apply_effect</code>.</p>

        <p>Users do not need to manually construct an effect object.</p>

        <p>The effect interface operates on the image's current selection.</p>

        <pre>
Selection → WHERE + WHAT
Effect    → HOW
        </pre>

        <p>This keeps effect implementations independent from selection logic.</p>

        <p>An effect can therefore be reused with completely different pixel selections without requiring separate implementations for each selection type.</p>
    `,
    tags: ["effects", "apply_effect", "architecture"],
    example: `
        image.select(piml::EVERYTHING);

        image.apply_effect.brightness(10);
    `
});


Docs.add({
    title: "Brightness",
    category: "Effects",
    description: `
        <p>The <code>brightness()</code> effect changes the brightness of the currently selected pixels.</p>

        <p>Positive values increase brightness while negative values decrease brightness.</p>

        <p>The supplied value is interpreted as a percentage and normalized internally.</p>

        <p>When no channel is specifically selected, the operation works on the RGB channels of the selected pixels.</p>

        <p>When a channel has been selected, the operation is restricted to that channel.</p>
    `,
    tags: ["brightness", "effect", "rgb"],
    example: `
        image.select(piml::EVERYTHING);

        image.apply_effect.brightness(20);

        image.apply_effect.brightness(-20);
    `
});


Docs.add({
    title: "Channel-Specific Brightness",
    category: "Effects",
    description: `
        <p>Brightness can be restricted to a specific channel using channel selection.</p>

        <p>This makes it possible to perform independent RGB adjustments without implementing a separate effect for every channel.</p>
    `,
    tags: ["brightness", "channel", "rgb", "effect"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::CHANNEL_RED
        );

        image.apply_effect.brightness(20);
    `
});


Docs.add({
    title: "Linear Corners",
    category: "Effects",
    description: `
        <p><code>linear_corners()</code> is PIML's linear endpoint or corner manipulation effect.</p>

        <p>This operation was previously exposed under the name <code>linear_contrast()</code>. The name <code>linear_corners()</code> more accurately describes its behavior.</p>

        <p>The operation manipulates the relationship between the two corners of the normalized range:</p>

        <pre>
0.0 → black
1.0 → white
        </pre>

        <p>Rather than being a conventional contrast control, this operation transforms the endpoints of the intensity range.</p>

        <p>Because the two corners can be moved through the full normalized range, sufficiently negative values can reverse their relationship.</p>

        <p>At an extreme value such as <code>-100</code>, black and white can effectively exchange positions, producing an image inversion.</p>

        <p>This makes <code>linear_corners()</code> useful for linear tonal transformations, endpoint manipulation, and inversion-style effects.</p>
    `,
    tags: ["linear-corners", "corners", "endpoint", "inversion", "effect"],
    example: `
        image.select(piml::EVERYTHING);

        image.apply_effect.linear_corners(20);

        // Extreme negative transformation.
        image.apply_effect.linear_corners(-100);
    `
});


Docs.add({
    title: "Linear Contrast",
    category: "Effects",
    description: `
        <p><code>linear_contrast()</code> is the dedicated contrast adjustment effect in PIML.</p>

        <p>It is distinct from <code>linear_corners()</code>.</p>

        <p><code>linear_corners()</code> manipulates the endpoints of the normalized tonal range, while <code>linear_contrast()</code> is intended to modify the separation of pixel intensity values.</p>

        <p>A positive contrast adjustment increases the distinction between darker and brighter intensity values.</p>

        <p>A negative contrast adjustment reduces that distinction and compresses the intensity range.</p>

        <p>The effect operates on the current selection and therefore works naturally with PIML's selection architecture.</p>

        <p>For example, contrast can be restricted to an intensity range or a particular channel.</p>
    `,
    tags: ["linear-contrast", "contrast", "intensity", "effect"],
    example: `
        image.select(piml::EVERYTHING);

        image.apply_effect.linear_contrast(20);
    `
});


Docs.add({
    title: "Linear Corners vs Linear Contrast",
    category: "Effects",
    description: `
        <p>The two effects have intentionally different responsibilities.</p>

        <table>
            <thead>
                <tr>
                    <th>Effect</th>
                    <th>Purpose</th>
                </tr>
            </thead>

            <tbody>
                <tr>
                    <td><code>linear_corners()</code></td>
                    <td>Manipulates the 0.0 and 1.0 endpoints of the tonal range.</td>
                </tr>

                <tr>
                    <td><code>linear_contrast()</code></td>
                    <td>Controls separation between intensity values.</td>
                </tr>
            </tbody>
        </table>

        <p>The distinction becomes particularly important for extreme values. <code>linear_corners()</code> can reach an inversion-like transformation where black and white exchange positions.</p>

        <p><code>linear_contrast()</code> is intended for conventional contrast manipulation instead of endpoint inversion.</p>
    `,
    tags: ["linear-corners", "linear-contrast", "contrast", "comparison"]
});


Docs.add({
    title: "Linear Gain",
    category: "Effects",
    description: `
        <p><code>linear_gain()</code> applies an adjustment to pixels whose brightness is above a specified reference point.</p>

        <p>The effect uses a configurable <code>point</code> argument to determine where the gain region begins.</p>

        <p>The operation is therefore useful when brighter regions should receive an additional adjustment while darker regions receive little or no gain from this operation.</p>

        <p>The effect works on the current selection and respects channel selection.</p>
    `,
    tags: ["linear-gain", "gain", "effect", "brightness"],
    example: `
        image.select(piml::EVERYTHING);

        image.apply_effect.linear_gain(
            20,
            0.5
        );
    `
});


Docs.add({
    title: "Linear Lift",
    category: "Effects",
    description: `
        <p><code>linear_lift()</code> is the complementary operation to <code>linear_gain()</code>.</p>

        <p>Instead of targeting pixels above the reference point, it targets pixels below the specified point.</p>

        <p>This makes it useful for selectively raising darker regions while leaving brighter regions less affected.</p>

        <p>The <code>point</code> argument defines the reference brightness.</p>
    `,
    tags: ["linear-lift", "lift", "effect", "shadows"],
    example: `
        image.select(piml::EVERYTHING);

        image.apply_effect.linear_lift(
            20,
            0.5
        );
    `
});


Docs.add({
    title: "Smoothing Brightness",
    category: "Effects",
    description: `
        <p><code>smoothing_brightness()</code> applies a brightness adjustment whose strength depends on the existing brightness of each pixel.</p>

        <p>The weighting is based on:</p>

        <pre>
(1.0 - brightness) * change
        </pre>

        <p>where <code>change</code> is the normalized percentage supplied to the effect.</p>

        <p>Consequently, darker pixels receive a larger adjustment while brighter pixels receive a smaller adjustment.</p>

        <p>A completely dark pixel receives the full change, while a pixel with brightness equal to 1.0 receives no change from the brightness weighting.</p>
    `,
    tags: ["smoothing", "brightness", "effect", "tonal"],
    example: `
        image.select(piml::EVERYTHING);

        image.apply_effect.smoothing_brightness(20);
    `
});


Docs.add({
    title: "Smoothing Brightness Opposite",
    category: "Effects",
    description: `
        <p><code>smoothing_brightness_opposite()</code> uses the opposite brightness weighting from <code>smoothing_brightness()</code>.</p>

        <p>The adjustment is weighted directly by pixel brightness:</p>

        <pre>
brightness * change
        </pre>

        <p>Dark pixels therefore receive a smaller adjustment while bright pixels receive a larger adjustment.</p>

        <p>A pixel with brightness <code>0.0</code> receives no weighted adjustment, while a pixel with brightness <code>1.0</code> receives the full change.</p>
    `,
    tags: ["smoothing", "brightness", "opposite", "effect"],
    example: `
        image.select(piml::EVERYTHING);

        image.apply_effect.smoothing_brightness_opposite(20);
    `
});


Docs.add({
    title: "Brightness Effect Overview",
    category: "Effects",
    description: `
        <p>PIML currently provides several brightness and tonal effects.</p>

        <table>
            <thead>
                <tr>
                    <th>Effect</th>
                    <th>Purpose</th>
                </tr>
            </thead>

            <tbody>
                <tr>
                    <td><code>brightness()</code></td>
                    <td>Applies a direct brightness adjustment.</td>
                </tr>

                <tr>
                    <td><code>linear_corners()</code></td>
                    <td>Manipulates the endpoints of the tonal range.</td>
                </tr>

                <tr>
                    <td><code>linear_contrast()</code></td>
                    <td>Adjusts intensity contrast.</td>
                </tr>

                <tr>
                    <td><code>linear_gain()</code></td>
                    <td>Targets brightness above a reference point.</td>
                </tr>

                <tr>
                    <td><code>linear_lift()</code></td>
                    <td>Targets brightness below a reference point.</td>
                </tr>

                <tr>
                    <td><code>smoothing_brightness()</code></td>
                    <td>Weights the adjustment more strongly toward darker pixels.</td>
                </tr>

                <tr>
                    <td><code>smoothing_brightness_opposite()</code></td>
                    <td>Weights the adjustment more strongly toward brighter pixels.</td>
                </tr>
            </tbody>
        </table>
    `,
    tags: ["brightness", "effects", "contrast", "tonal"]
});


Docs.add({
    title: "Selection-Driven Processing",
    category: "Workflows",
    description: `
        <p>PIML allows complex image operations to be constructed from simple selection and effect operations.</p>

        <p>For example, a contrast-like operation can be constructed without a dedicated contrast effect by processing darker and brighter regions separately.</p>

        <p>The important concept is that the selection defines the target pixels while the effect defines the transformation.</p>

        <pre>
WHERE
  ↓
Select pixels

WHAT
  ↓
Select channel

HOW
  ↓
Apply effect
        </pre>

        <p>This architecture makes image-processing operations composable and reusable.</p>
    `,
    tags: ["workflow", "selection", "effects", "processing"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            50
        );

        image.apply_effect.brightness(-20);

        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_GREATER_THAN,
            50
        );

        image.apply_effect.brightness(20);
    `
});


Docs.add({
    title: "Intensity-Based Color Grading",
    category: "Workflows",
    description: `
        <p>Intensity selection can be combined with channel selection and brightness effects to create targeted color adjustments.</p>

        <p>Because intensity is based on the direct RGB average, it can be used to divide an image into darker, middle, and brighter regions.</p>

        <p>A typical workflow is:</p>

        <pre>
Select everything
       ↓
Select intensity range
       ↓
Select channel
       ↓
Apply effect
       ↓
Reset selection
       ↓
Repeat
        </pre>

        <p>This provides a simple foundation for targeted color grading without requiring curves or complex effect-specific selection logic.</p>
    `,
    tags: ["color-grading", "intensity", "rgb", "channels", "workflow"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            40
        );

        image.select(
            piml::CHANNEL_BLUE
        );

        image.apply_effect.brightness(10);

        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_GREATER_THAN,
            60
        );

        image.select(
            piml::CHANNEL_RED
        );

        image.apply_effect.brightness(10);
    `
});


Docs.add({
    title: "Teal and Orange Example",
    category: "Examples",
    description: `
        <p>PIML's simple linear effects can be combined to create a basic cinematic-style teal and orange treatment without using curves.</p>

        <p>The basic approach is to make a small global brightness adjustment, manipulate the red channel using <code>linear_contrast()</code> or the appropriate tonal operation, then manipulate the blue channel separately.</p>

        <p>Because the operation is selection-driven, individual channels can be targeted independently.</p>

        <p>Additional smoothing brightness operations can be applied afterward when a softer tonal transition is desired.</p>
    `,
    tags: ["teal", "orange", "color-grading", "example"],
    example: `
        img.select(piml::EVERYTHING);

        img.apply_effect.brightness(5);

        img.select(piml::CHANNEL_RED);

        img.apply_effect.linear_contrast(10);

        img.select(piml::EVERYTHING);

        img.select(piml::CHANNEL_BLUE);

        img.apply_effect.linear_gain(-10);

        // Optional refinement:
        // img.apply_effect.smoothing_brightness(10);
    `
});


Docs.add({
    title: "Combining Intensity and Effects",
    category: "Examples",
    description: `
        <p>The intensity predicates are particularly useful when an effect should only operate on a specific brightness region.</p>

        <p>For example, a lift can be restricted to pixels below 40% RGB-average intensity.</p>

        <p>This combines:</p>

        <pre>
WHERE → intensity < 40%
WHAT  → selected channel or RGB
HOW   → linear lift
        </pre>
    `,
    tags: ["intensity", "linear-lift", "example", "selection"],
    example: `
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            40
        );

        image.apply_effect.linear_lift(
            20,
            0.5
        );
    `
});


Docs.add({
    title: "Image I/O and Bit Depth",
    category: "Image I/O",
    description: `
        <p>PIML separates its internal representation from the representation used when an image is written to disk.</p>

        <p>Internally, pixels use normalized floating-point channel values.</p>

        <p>When writing an image, PIML converts those values into the appropriate output representation.</p>

        <p>Supported output depths include:</p>

        <ul>
            <li>8 bits per channel</li>
            <li>16 bits per channel</li>
        </ul>

        <p>Before conversion, channel values are constrained to the normalized range:</p>

        <pre>
0.0 <= value <= 1.0
        </pre>
    `,
    tags: ["io", "bit-depth", "8-bit", "16-bit", "image"]
});


Docs.add({
    title: "Debugging",
    category: "Debugging",
    description: `
        <p>PIML includes a built-in logging and debugging system intended to make image-processing operations easier to inspect during development.</p>

        <p>Debugging can be disabled when logging is not required.</p>

        <p>The logging system categorizes information into messages, warnings, errors, and activities.</p>
    `,
    tags: ["debugging", "logging", "development"],
    example: `
        image.disable_debugging();

        image.enable_debugging();
    `
});


Docs.add({
    title: "Log Categories",
    category: "Debugging",
    description: `
        <p>PIML defines several log categories:</p>

        <pre>
piml::Log::MESSAGE
piml::Log::WARNING
piml::Log::ERROR
piml::Log::ACTIVITY
        </pre>

        <p>These categories allow debugging output to be separated according to its purpose.</p>
    `,
    tags: ["logging", "debugging", "Log"]
});


Docs.add({
    title: "Debugger",
    category: "Debugging",
    description: `
        <p>The <code>Debugger</code> class provides an interface for inspecting the logs associated with an <code>Image</code>.</p>

        <p>A debugger can display all available log categories or individual categories.</p>

        <p>This is useful when diagnosing image operations, selection behavior, or other library activity.</p>
    `,
    tags: ["Debugger", "debugging", "logging"],
    example: `
        piml::Debugger debugger(image);

        debugger.show_everything();
    `
});


Docs.add({
    title: "Debugger Log Access",
    category: "Debugging",
    description: `
        <p>Individual log categories can be displayed separately.</p>

        <p>The debugger provides access to:</p>

        <pre>
show_messages()
show_warnings()
show_errors()
show_activities()
        </pre>

        <p>Logs can also be cleared individually or together using the corresponding debugger operations.</p>
    `,
    tags: ["Debugger", "logs", "messages", "warnings", "errors"],
    example: `
        piml::Debugger debugger(image);

        debugger.show_messages();
        debugger.show_warnings();
        debugger.show_errors();
        debugger.show_activities();
    `
});


Docs.add({
    title: "Memory Usage",
    category: "Architecture",
    description: `
        <p>PIML keeps the actual image pixels separate from the current selection.</p>

        <p>The image owns its actual <code>Pixel</code> objects, while the selection stores pointers to selected pixels.</p>

        <pre>
pixel_vector
    ↓
actual Pixel objects

selected_pixels
    ↓
Pixel* pointers
        </pre>

        <p>This means refining a selection does not require duplicating the actual pixel objects.</p>

        <p>Selection filtering works with pointers to the existing pixel data.</p>

        <p>PIML also uses a temporary pointer buffer while refining selections. This adds some memory overhead but avoids repeatedly moving pixel-selection elements when removing items from the middle of a vector.</p>

        <p>The design intentionally trades some additional memory for flexible and fine-grained selection.</p>
    `,
    tags: ["memory", "architecture", "selection", "performance"]
});


Docs.add({
    title: "Selection Performance",
    category: "Performance",
    description: `
        <p>PIML selection predicates currently process the current selection linearly.</p>

        <p>When a selection is refined, PIML uses a temporary pointer buffer and then swaps the resulting selection instead of repeatedly erasing elements from the middle of a vector.</p>

        <pre>
Current Selection
       ↓
Filter
       ↓
Temporary Pointer Buffer
       ↓
swap()
       ↓
New Selection
        </pre>

        <p>This avoids repeated element shifting that can occur with middle-of-vector erases.</p>

        <p>The cost of selection therefore depends primarily on the number of currently selected pixels and the number of predicates being applied.</p>
    `,
    tags: ["performance", "selection", "optimization", "vector"]
});


Docs.add({
    title: "Performance Considerations",
    category: "Performance",
    description: `
        <p>PIML performance depends on several factors:</p>

        <ul>
            <li>Image dimensions</li>
            <li>Number of selected pixels</li>
            <li>Number of selection predicates</li>
            <li>Number of effects</li>
            <li>Number of processing passes</li>
            <li>Image format</li>
            <li>Available memory</li>
            <li>Compiler optimizations</li>
        </ul>

        <p>The selection architecture intentionally uses additional memory to provide precise and composable pixel selection.</p>

        <p>For very large images, applications should therefore consider both processing time and selection memory usage.</p>
    `,
    tags: ["performance", "optimization", "memory", "image-processing"]
});


Docs.add({
    title: "Architecture",
    category: "Architecture",
    description: `
        <p>PIML's architecture is centered around the <code>Image</code>, selection system, and effect interface.</p>

        <pre>
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
        </pre>

        <p>This separation allows PIML to extend its selection system without requiring every effect to be rewritten.</p>
    `,
    tags: ["architecture", "design", "Image", "Selection", "Effect"]
});


Docs.add({
    title: "Image Class",
    category: "Architecture",
    description: `
        <p>The <code>Image</code> class represents the main processing object in PIML.</p>

        <p>It owns the image's pixel data and maintains the state required for selection-driven processing.</p>

        <p>The image interface provides access to selection operations, channel selection, effects, debugging state, and image-related information.</p>

        <p>The effect interface is exposed directly through <code>Image::apply_effect</code>.</p>
    `,
    tags: ["Image", "class", "architecture"]
});


Docs.add({
    title: "Selection System",
    category: "Architecture",
    description: `
        <p>The selection system determines which pixels should be affected by subsequent effects.</p>

        <p>Selections can be initialized using <code>EVERYTHING</code>, refined using predicates, and combined with channel selection.</p>

        <p>Because selection is independent from effects, new selection mechanisms can be introduced without requiring every existing effect to understand them.</p>
    `,
    tags: ["Selection", "architecture", "predicates", "channels"]
});


Docs.add({
    title: "Effect System",
    category: "Architecture",
    description: `
        <p>The effect system performs operations on the current selection.</p>

        <p>Effects are exposed through:</p>

        <pre>
image.apply_effect
        </pre>

        <p>An effect should not need to know whether its input selection came from the entire image, a luma predicate, an intensity predicate, a channel selection, or another future selection mechanism.</p>

        <p>This makes effects reusable and keeps selection logic separate from image transformations.</p>
    `,
    tags: ["Effect", "effects", "architecture", "apply_effect"]
});


Docs.add({
    title: "Project Structure",
    category: "Development",
    description: `
        <p>The PIML repository is organized into public headers, implementation files, effects, tests, and build utilities.</p>
    `,
    tags: ["project", "structure", "development"],
    example: `
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
    `
});


Docs.add({
    title: "Source Files",
    category: "Development",
    description: `
        <table>
            <thead>
                <tr>
                    <th>File</th>
                    <th>Purpose</th>
                </tr>
            </thead>

            <tbody>
                <tr>
                    <td><code>piml.hh</code></td>
                    <td>Main public PIML API.</td>
                </tr>

                <tr>
                    <td><code>pimlio.hh</code></td>
                    <td>Public image input/output API.</td>
                </tr>

                <tr>
                    <td><code>src/pimlio.cc</code></td>
                    <td>Image I/O implementation.</td>
                </tr>

                <tr>
                    <td><code>src/classes/Image.cc</code></td>
                    <td>Image implementation.</td>
                </tr>

                <tr>
                    <td><code>src/classes/Debugger.cc</code></td>
                    <td>Debugger implementation.</td>
                </tr>

                <tr>
                    <td><code>src/effects/brightness.cc</code></td>
                    <td>Brightness and related effect implementation.</td>
                </tr>

                <tr>
                    <td><code>test/test.cc</code></td>
                    <td>Test program.</td>
                </tr>

                <tr>
                    <td><code>build.cc</code></td>
                    <td>Build implementation.</td>
                </tr>

                <tr>
                    <td><code>build.sh</code></td>
                    <td>Build script.</td>
                </tr>

                <tr>
                    <td><code>test.sh</code></td>
                    <td>Builds and runs tests.</td>
                </tr>

                <tr>
                    <td><code>clean.sh</code></td>
                    <td>Removes generated files.</td>
                </tr>
            </tbody>
        </table>
    `,
    tags: ["source", "files", "development", "structure"]
});


Docs.add({
    title: "Creating Complex Operations",
    category: "Workflows",
    description: `
        <p>PIML is intended to make complex operations possible through composition rather than through increasingly complicated individual effects.</p>

        <p>A complex operation can be constructed from:</p>

        <ul>
            <li>A starting selection</li>
            <li>One or more selection predicates</li>
            <li>An optional channel selection</li>
            <li>An effect</li>
            <li>A new selection for the next operation</li>
        </ul>

        <p>This approach makes it possible to build color-grading and tonal-processing workflows from relatively small building blocks.</p>
    `,
    tags: ["workflow", "composition", "selection", "effects"]
});


Docs.add({
    title: "Resetting Between Operations",
    category: "Workflows",
    description: `
        <p>Because selection predicates refine the current selection, independent operations should generally begin by selecting <code>EVERYTHING</code>.</p>

        <p>This prevents the next operation from unintentionally inheriting the filtered selection produced by a previous operation.</p>
    `,
    tags: ["selection", "EVERYTHING", "workflow"],
    example: `
        // First operation.
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_SMALLER_THAN,
            40
        );

        image.apply_effect.brightness(-10);

        // Independent second operation.
        image.select(piml::EVERYTHING);

        image.select(
            piml::WHERE_INTENSITY_GREATER_THAN,
            60
        );

        image.apply_effect.brightness(10);
    `
});


Docs.add({
    title: "Extending PIML",
    category: "Development",
    description: `
        <p>PIML's architecture is intended to support future expansion.</p>

        <p>New effects can operate on the existing selection interface without needing to implement their own pixel-selection system.</p>

        <p>Likewise, new selection predicates can be added to provide new ways of determining which pixels should be processed.</p>

        <p>This separation is especially useful for future selection systems involving additional pixel properties, color conditions, masks, conditional rules, and more advanced selection combinations.</p>
    `,
    tags: ["extending", "development", "effects", "selection"]
});


Docs.add({
    title: "Roadmap",
    category: "Development",
    description: `
        <p>PIML is an actively developing image manipulation library. Areas for future development include:</p>

        <ul>
            <li>Additional image effects</li>
            <li>More selection types</li>
            <li>Additional channel-selection mechanisms</li>
            <li>More pixel-property predicates</li>
            <li>Complex selection rules</li>
            <li>Conditional selections</li>
            <li>Combining multiple selection conditions</li>
            <li>Additional image formats</li>
            <li>Performance improvements</li>
            <li>Memory usage improvements</li>
            <li>Expanded test coverage</li>
            <li>More advanced color manipulation</li>
            <li>More expressive selection operations</li>
        </ul>

        <p>The selection system is intended to become increasingly expressive while preserving the core separation between selection and effects.</p>
    `,
    tags: ["roadmap", "development", "future"]
});


Docs.add({
    title: "Contributing",
    category: "Development",
    description: `
        <p>Contributions to PIML are welcome.</p>

        <p>Useful areas for contribution include:</p>

        <ul>
            <li>Adding new effects</li>
            <li>Improving image I/O</li>
            <li>Implementing new selection mechanisms</li>
            <li>Adding new pixel predicates</li>
            <li>Optimizing existing operations</li>
            <li>Adding tests</li>
            <li>Improving documentation</li>
            <li>Improving portability</li>
        </ul>

        <p>For larger changes, opening an issue before implementation can help coordinate development and avoid conflicting approaches.</p>
    `,
    tags: ["contributing", "development", "open-source"]
});


Docs.add({
    title: "License",
    category: "General",
    description: `
        <p>PIML is released under the <strong>CC0 1.0 Universal</strong> license.</p>

        <p>This places the project in the public domain to the extent permitted by applicable law.</p>

        <p>See the license included with the repository for the complete legal terms.</p>
    `,
    tags: ["license", "cc0", "public-domain"]
});


Docs.add({
    title: "Repository",
    category: "General",
    description: `
        <p>The complete PIML source code, build scripts, tests, image resources, and documentation are maintained in the project's GitHub repository.</p>

        <p>The repository is:</p>

        <p><a href="https://github.com/darkyboys/piml">
            https://github.com/darkyboys/piml
        </a></p>
    `,
    tags: ["github", "repository", "source-code", "piml"]
});


Docs.add({
    title: "Complete Processing Model",
    category: "Reference",
    description: `
        <p>The complete PIML processing model can be summarized as a three-stage pipeline:</p>

        <pre>
┌──────────────────────┐
│        WHERE         │
│                      │
│ Which pixels?        │
│                      │
│ EVERYTHING           │
│ LUMA predicates      │
│ INTENSITY predicates │
│ Future predicates    │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│        WHAT          │
│                      │
│ Which channel?       │
│                      │
│ RED                  │
│ GREEN                │
│ BLUE                 │
│ ALPHA                │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│         HOW          │
│                      │
│ Which effect?        │
│                      │
│ brightness()         │
│ linear_corners()     │
│ linear_contrast()    │
│ linear_gain()        │
│ linear_lift()        │
│ smoothing_brightness │
└──────────────────────┘
        </pre>

        <p>This architecture is the central design principle of PIML.</p>

        <p>The selection system decides where an operation happens. Channel selection decides what portion of those pixels is affected. The effect decides how the selected data is transformed.</p>

        <p>Because these responsibilities remain separate, PIML can build increasingly complex image-processing workflows without requiring every possible combination to become a separate effect.</p>
    `,
    tags: ["reference", "architecture", "where", "what", "how", "piml"]
});