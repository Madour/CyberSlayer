#include "Game.hpp"

int main() {

    // load all assets
    ns::Res::load("assets");

    // configure some inputs buttons
    ns::Config::Inputs::setButtonKey("droite", sf::Keyboard::Right);

    Game g;
    g.run();

    // dont forget to unload all the assets
    ns::Res::dispose();
    return 0;
}
