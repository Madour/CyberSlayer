#include "Game.hpp"

Game::Game() :ns::App("NasNas Game Template", 720, 480) {
    ns::Config::debug = true;
    addDebugText<std::string>([](){return "World !";}, "Hello", {0, 0});
}

void Game::onEvent(const sf::Event& event) {
    ns::App::onEvent(event);
}

void Game::update() {

}
