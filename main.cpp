#include "Game.hpp"

int main() {

    // load all assets
    ns::Res::load("assets");

    ns::Config::debug.show_fps = true;
    ns::Config::debug.show_text = true;
    ns::Config::debug.show_bounds = false;

    ns::Config::Window::title = "RayCast FPS";
    ns::Config::Window::size = {VIEW_WIDTH, VIEW_HEIGHT};

    ns::Config::Window::antialiasing = 0;
    ns::Config::Window::update_rate = 60;
    ns::Config::Window::framerate_limit = 60;
    ns::Config::Window::vertical_sync = true;
    ns::Config::Window::cursor_visible = false;

    // configure some inputs buttons
    ns::Config::Inputs::setButtonKey("up", sf::Keyboard::Z);
    ns::Config::Inputs::setButtonKey("down", sf::Keyboard::S);
    ns::Config::Inputs::setButtonKey("left", sf::Keyboard::Q);
    ns::Config::Inputs::setButtonKey("right", sf::Keyboard::D);
    ns::Config::Inputs::setButtonKey("jump", sf::Keyboard::Space);

    Game g;
    g.run();

    // dont forget to unload all the assets
    ns::Res::dispose();
    return 0;
}
