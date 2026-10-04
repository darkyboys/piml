#include "../piml.hh"
#include "../pimlio.hh"

int main(){
    using namespace piml;
    auto img = pimlio_read("img/test.png");

    img.select(EVERYTHING);
    img.select(WHERE_LUMA_SMALLER_THAN, 50);
    img.select(CHANNEL_RED);
    img.apply_effect.brightness(-20);

    img.select(EVERYTHING);
    img.select(WHERE_LUMA_GREATER_THAN, 50);
    img.apply_effect.brightness(20);

    pimlio_write(img, "test_output.png");
}