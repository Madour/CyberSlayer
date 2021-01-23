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
    minimap_cam->zoom(0.9f);
    ///////////////////////////////////////////////////////

    m_state = new TitleScreenState();
    m_state->init();

    ///////////////////////////////////////////////////////
    // Add debug texts (F1 to activate debug mode)
    /*ns::DebugTextInterface::font_size = 15;
    ns::DebugTextInterface::outline_thickness = 1;
    ns::DebugTextInterface::outline_color = sf::Color::Black;
    addDebugText<sf::Vector3f>([&]{return m_camera.getPosition3D()/METER;}, "camera_pos (m):", {0, 0});
    addDebugText<sf::Vector3f>([&]{return m_camera.getRotationDeg();}, "camera_rot :", {0, 20});
    addDebugText<sf::Vector2f>([&]{return sf::Vector2f(std::cos(m_camera.getYaw()), std::sin(m_camera.getYaw()));}, "camera_dir :", {0, 40});
    addDebugText<float>([&]{return m_depth_buffer[m_depth_buffer.size() / 2]/METER;}, "distance mid view (m):", {0, 60});
    addDebugText<float>([&]{return ns::to_degree(m_camera.getFovRad()); }, "FOV :", {0, 80});
    */
    ///////////////////////////////////////////////////////

}

void Game::onEvent(const sf::Event& event) {
    ns::App::onEvent(event);

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::F) {
            toggleFullscreen();
        }
    }

    m_state->onEvent(event);
}

void Game::update() {
    m_state->update();
}

void Game::preRender() {
    m_state->preRender();
}
