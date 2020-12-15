#include "Game.hpp"


Game::Game() : ns::App("Ray Cast FPS", {800, 500}){
    ns::Config::debug = false;

    ///////////////////////////////////////////////////////
    // create the main scene
    auto* scene = createScene("main");
    scene->addLayer("bg", 0);

    // add drawables to it here

    // create the main camera
    auto* camera = createCamera("main", 0, {0, 0, 800, 500});
    camera->lookAt(scene);
    ///////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////
    // create the HUD scene
    auto* hud = createScene("hud");
    hud->getDefaultLayer()->add(new sf::CircleShape(50));

    // create the HUD camera
    auto* hud_cam = createCamera("hud_came", 1);
    hud_cam->lookAt(hud);
    ///////////////////////////////////////////////////////
}

void Game::onEvent(const sf::Event& event) {
    ns::App::onEvent(event);
    if (event.type == sf::Event::KeyPressed) {

        // create a transition when T is pressed
        if (event.key.code == sf::Keyboard::T) {
            auto* tr =  new ns::transition::CircleClose();
            tr->start();
            tr->setOnEndCallback([&](){
                getWindow().setClearColor(sf::Color::Blue);
                (new ns::transition::CircleOpen())->start();
            });
        }
    }
}

void Game::update() {
}
