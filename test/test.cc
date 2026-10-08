#include "../piml.hh"
#include "../pimlio.hh"

int main(){
    using namespace piml;
    auto img = pimlio_read("img/test.png");
    // Teal and orange shade
    // img.select(EVERYTHING);
    // img.apply_effect.brightness(5);
    // img.select(piml::CHANNEL_RED);
    // img.apply_effect.linear_contrast(10);
    // img.select(EVERYTHING);
    // img.select(piml::CHANNEL_BLUE);
    // img.apply_effect.linear_gain(-10);

    // Selection with the luma vs intensity
    img.select(EVERYTHING);
    // img.select(WHERE_LUMA_GREATER_THAN, 50);
    img.select(WHERE_INTENSITY_GREATER_THAN_OR_EQUALS, 50);
    img.apply_effect.brightness(100);
    img.clear_selection();

    img.select(EVERYTHING);
    // img.select(WHERE_LUMA_SMALLER_THAN_OR_EQUALS, 50);
    img.select(WHERE_INTENSITY_SMALLER_THAN_OR_EQUALS, 50);
    img.apply_effect.brightness(-100);
    img.clear_selection();
    img.select(EVERYTHING);
    img.apply_effect.linear_contrast(-25);
    // img.apply_effect.linear_corners(50);


    // img.apply_effect.linear_contrast(-100);
    // img.apply_effect.linear_lift(100);
    // img.apply_effect.linear_gain(-100);
    // img.apply_effect.smoothing_brightness_opposite(-100);
    // img.apply_effect.smoothing_brightness(100);
    // img.select(EVERYTHING);
    // img.select(WHERE_LUMA_SMALLER_THAN, 50);
    // img.select(CHANNEL_RED);
    // img.apply_effect.brightness(-20);

    // img.select(EVERYTHING);
    // img.select(WHERE_LUMA_GREATER_THAN, 50);
    // img.apply_effect.brightness(20);

    pimlio_write(img, "test_output.png");
}