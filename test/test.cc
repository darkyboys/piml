#include "../piml.hh"
#include "../pimlio.hh"

int main(){

    piml::Image image = piml::pimlio_read("img/test.png");

    image.select(piml::EVERYTHING);
    
    piml::Effect effect = image;
    effect.brightness(10);
    effect.brightness(-90);

    piml::pimlio_write(image, "test_output.png");
    
}