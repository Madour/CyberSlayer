#include <cstring>  // for memse
#include "Game.hpp"

#include "GameState.hpp"
#include "states/TitleScreenState.hpp"
#include "states/LevelState.hpp"


Game::Game() {
    ns::Config::debug = false;
    GameState::game = this;

    ///////////////////////////////////////////////////////
    //create background scene
    auto* bg_scene = createScene("bg");
    auto* bg_cam = createCamera("bg", 0);
    bg_cam->lookAt(bg_scene);

    // create the main scene
    auto* main_scene = createScene("main");
    // create the main camera
    auto* main_cam = createCamera("main", 1);
    main_cam->lookAt(main_scene);
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // create the HUD scene
    auto* hud_scene = createScene("hud");
    // create the HUD camera
    auto* hud_cam = createCamera("hud", 2);
    hud_cam->lookAt(hud_scene);
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // create the Minimap scene
    auto* minimap_scene = createScene("minimap");
    // create the Minimap camera
    auto* minimap_cam = createCamera("minimap", 3, {0, 0, 20*10, 20*10}, {VIEW_WIDTH-200, 0, 200, 200});
    minimap_cam->lookAt(minimap_scene);
    minimap_cam->zoom(1.5f);
    ///////////////////////////////////////////////////////

    m_state = new LevelState();
    m_state->init();

}

void Game::onEvent(const sf::Event& event) {
    ns::App::onEvent(event);
    m_state->onEvent(event);

    if (event.type == sf::Event::KeyPressed)
        if (event.key.code == sf::Keyboard::F)
            toggleFullscreen();
}

void Game::update() {
    m_state->update();
}

void Game::preRender() {
    m_state->preRender();
}
