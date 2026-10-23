#include "../piml.hh"
#include "../pimlio.hh"
#include <iostream>

int main(){
    using namespace piml;
    // auto img = pimlio_read("img/test.png");
    // Teal and orange shade
    // img.select(EVERYTHING);
    // img.apply_effect.brightness(5);
    // img.select(piml::CHANNEL_RED);
    // img.apply_effect.linear_contrast(10);
    // img.select(EVERYTHING);
    // img.select(piml::CHANNEL_BLUE);
    // img.apply_effect.linear_gain(-10);

    // Selection with the luma vs intensity
    // img.select(EVERYTHING);
    // // img.select(WHERE_LUMA_GREATER_THAN, 50);
    // img.select(WHERE_INTENSITY_GREATER_THAN_OR_EQUALS, 50);
    // img.apply_effect.brightness(100);
    // img.clear_selection();

    // img.select(EVERYTHING);
    // // img.select(WHERE_LUMA_SMALLER_THAN_OR_EQUALS, 50);
    // img.select(WHERE_INTENSITY_SMALLER_THAN_OR_EQUALS, 50);
    // img.apply_effect.brightness(-100);
    // img.clear_selection();
    // img.select(EVERYTHING);
    // img.apply_effect.linear_intensity_contrast(-25);
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

    // img.select(EVERYTHING);
    // // img.select(WHERE_LUMA_SMALLER_THAN_OR_EQUALS, 50);
    // img.select(WHERE_INTENSITY_SMALLER_THAN_OR_EQUALS, 50);
    // // img.apply_effect().brightness(-100);
    // img.clear_selection();
    // img.select(EVERYTHING);
    // img.apply_effect().intensity_contrast(-25);
    // img.apply_effect().corners(50);


    // img.select(EVERYTHING);
    // // img.apply_effect().corners(-25);
    // // img.apply_effect().contrast(100);
    // // img.apply_effect().contrast(-50);

    // img.select(piml::WHERE_INTENSITY_SMALLER_THAN, 50);
    // // img.apply_effect().contrast(-50);
    // img.apply_effect().brightness(-100);
    // img.select(REMAINING);
    // img.apply_effect().brightness(100);
    // img.select(REMAINING);
    // img.apply_effect().brightness(50);
    // img.apply_effect().brightness(-50);
    // img.select(REMAINING);
    // img.select(CHANNEL_GREEN);
    // img.apply_effect().brightness(-50);
    // img.select(CHANNEL_BLUE);

    // Advanced Teal and Orange effect with basic effects
    // img.select(EVERYTHING);
    // img.select(CHANNEL_RED);
    // img.apply_effect().brightness(-5);
    // img.clean_selection();
    // img.select(WARM_COLORS);
    // img.select(CHANNEL_RED);

    // img.apply_effect().smoothing_brightness_opposite(5);

    // img.select(CHANNEL_BLUE);
    // img.apply_effect().smoothing_brightness_opposite(-5);

    // img.select(REMAINING);
    // img.select(CHANNEL_RED);
    // img.apply_effect().smoothing_brightness(-10);
    


    // pimlio_write(img, "test_output.png");

    // piml::Debugger db(img);
    // db.show_everything();


    // // Lut image
    // auto lut = pimlio_give_lut_image(64);

    // lut.select(EVERYTHING);
    // lut.select(CHANNEL_RED);
    // lut.apply_effect().brightness(-5);
    // lut.clean_selection();
    // lut.select(WARM_COLORS);
    // lut.select(CHANNEL_RED);

    // lut.apply_effect().smoothing_brightness_opposite(5);

    // lut.select(CHANNEL_BLUE);
    // lut.apply_effect().smoothing_brightness_opposite(-5);

    // lut.select(REMAINING);
    // lut.select(CHANNEL_RED);
    // lut.apply_effect().smoothing_brightness(-10);

    // pimlio_export_lut(lut, "PIML - Advanced Teal & Orange (DEMO).cube", 64);
    // db.show_pixels();


    // std::cout << "Diff: " <<get_distance_between_colors(
    //                          {1.0, 0.1, 0.0}
    //                        , {1.0, 0.0, 0.0}
    //                       )*100<<'\n'<<'\n';

    // std::cout << "Diff: " << get_distance_between_colors(
    //                          {1.0, 0.0, 0.0}  // Bright red
    //                        , {0.5, 0.0, 0.0}  // Dark red
    //                       ) * 100 << '\n' << '\n';

    // std::cout << "Diff: " << get_distance_between_colors(
    //                          {0.1, 0.1, 0.1}
    //                        , {1.0, 1.0, 1.0}
    //                       ) * 100 << '\n' << '\n';

    // std::cout << "Diff: " << get_distance_between_colors(
    //                          {1.0, 0.0, 0.0}
    //                        , {0.5, 0.5, 0.5}
    //                       ) * 100 << '\n' << '\n';

    // std::cout << "Diff: " <<get_distance_between_colors(
    //                          {1.0, 1.0, 0.0}
    //                        , {1.0, 0.0, 0.0}
    //                       )*100<<'\n'<<'\n';


    // Color testing
    auto img = pimlio_read("img/test.png");
    // auto img = pimlio_give_lut_image(64);
    img.select(EVERYTHING);
    img.select(CHANNEL_RED);
    img.apply_effect().brightness(-5);
    img.clean_selection();
    Color col = {0.0, 1.0, 0.0};
    img.select(piml::WHERE_COLORS_MATCHES, col, 78);
    // img.select(WARM_COLORS);
    img.select(CHANNEL_RED);
    // img.apply_effect().brightness(-100);
    img.apply_effect().color_brightness(-5, {0.0, 1.0, 0.0});
    // img.select(CHANNEL_BLUE);
    // img.apply_effect().color_brightness(-5, {1.0, 0.0, 0.0}, 60);
    img.clean_selection();

    col = {1.0, 0.0, 0.0};
    img.select(EVERYTHING);
    img.select(piml::WHERE_COLORS_MATCHES, col, 80);
    img.select(CHANNEL_RED);
    img.apply_effect().color_brightness(25, {1.0, 0.0, 0.0});

    img.select(CHANNEL_BLUE);
    img.apply_effect().color_brightness(-20, {1.0, 0.0, 0.0}, 60);
    img.clean_selection();

    img.select(EVERYTHING);
    // img.apply_effect().intensity_midtones(-100);
    // img.apply_effect().midtones(-50);

    // img.apply_effect().brightness(100);

    // img.select(REMAINING);
    // img.select(CHANNEL_RED);
    // img.apply_effect().color_brightness(100, {1.0, 0.0, 0.0}, 80);

    pimlio_write(img, "test_output.png");
    // pimlio_export_lut(img, "PIML - Advanced Teal & Orange 2 (DEMO).cube", 64);

    piml::Debugger db(img);
    db.show_everything();
}