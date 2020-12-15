#include "Game.hpp"


Game::Game() : ns::App("Ray Cast FPS", {800, 450}){
    ns::Config::debug = false;
    sf::Mouse::setPosition(sf::Vector2i(getWindow().getAppView().getSize()/2.f), getWindow());

    ns_LOG("View Size :", getWindow().getAppView().getSize());

    m_map = {
            "###################",
            "#                 #",
            "#   ###           #",
            "#   ###           #",
            "#   ###           #",
            "#    #    #       #",
            "#    #    #       #",
            "#    #  #######   #",
            "#                 #",
            "###################"
    };
    m_max_depth = std::hypot(m_map.size(), m_map[0].size());
    ns_LOG("Ray cast max depth :", m_max_depth);

    m_white_texture.create(1, int(getWindow().getAppView().getSize().y));
    m_white_texture.clear(sf::Color::White);
    m_white_texture.display();

    m_quads.resize(int(getWindow().getAppView().getSize().x));

    m_player_pos = sf::Vector2f(float(m_map[0].size())/2.f-0.5, float(m_map.size())/2.f-0.5);

    m_hp_bar.setSize({200, 10});
    m_hp_bar.setPosition(20, 20);
    m_hp_bar.setFillColor(sf::Color::Red);

    ///////////////////////////////////////////////////////
    // create the main scene
    auto* scene = createScene("main");
    scene->addLayer("bg", 0);

    // add drawables to the main scene here
    auto* batch = new ns::SpriteBatch("walls");
    for (auto& quad : m_quads) {
        quad.setTexture(m_white_texture.getTexture());
        batch->draw(&quad);
    }
    batch->end();

    scene->getDefaultLayer()->add(batch);

    // create the main camera
    auto* camera = createCamera("main", 0, {0, 0, 800, 500});
    camera->lookAt(scene);
    ///////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////
    // create the HUD scene
    auto* hud = createScene("hud");

    // add drawables to the HUD scene here
    hud->getDefaultLayer()->addRaw(&m_hp_bar);

    // create the HUD camera
    auto* hud_cam = createCamera("hud_came", 1);
    hud_cam->lookAt(hud);
    ///////////////////////////////////////////////////////

    ns::DebugTextInterface::font_size = 12;
    addDebugText<sf::Vector2f>(&m_player_pos, "player_pos", {0, 0});
    addDebugText<sf::Vector2f>(&m_player_angle, "player_angle", {0, 15});
}

void Game::onEvent(const sf::Event& event) {
    ns::App::onEvent(event);

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::F) {
            toggleFullscreen();
        }
    }

    if (getWindow().hasFocus())
        if (event.type == sf::Event::MouseMoved) {
            int dx = event.mouseMove.x - int(getWindow().getAppView().getSize().x/2);
            int dy = event.mouseMove.y - int(getWindow().getAppView().getSize().y/2);
            m_player_angle.x += float(dx)*0.005f;
            m_player_angle.y += float(dy)*1.5f;
            sf::Mouse::setPosition(sf::Vector2i(getWindow().getAppView().getSize()/2.f), getWindow());
        }
}

void Game::update() {

    sf::Vector2f player_dir = {std::cos(m_player_angle.x), std::sin(m_player_angle.x)};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        m_player_pos.x += 0.05f*player_dir.x;
        m_player_pos.y += 0.05f*player_dir.y;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        m_player_pos.x -= 0.05f*player_dir.x;
        m_player_pos.y -= 0.05f*player_dir.y;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        m_player_pos.y -= 0.05f*player_dir.x;
        m_player_pos.x += 0.05f*player_dir.y;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        m_player_pos.y += 0.05f*player_dir.x;
        m_player_pos.x -= 0.05f*player_dir.y;
    }
}

void Game::preRender() {
    int nb_of_rays = int(getWindow().getAppView().getSize().x);

    for (int i = 0; i < nb_of_rays; ++i) {
        float ray_angle = (m_player_angle.x - m_fov/2) + (float(i) / float(nb_of_rays)) * m_fov;
        float distance = 0;
        bool hit = false;

        while (!hit  && distance < m_max_depth) {
            distance += 0.025f;

            sf::Vector2f test = {
                    m_player_pos.x + cos(ray_angle) * distance,
                    m_player_pos.y + sin(ray_angle) * distance,
            };

            if (int(test.x) < 0 || unsigned(test.x) > m_map[0].size() || int(test.y) < 0 || unsigned(test.y) > m_map.size()) {
                hit = true;
                distance = m_max_depth;
            }
            else {
                if (m_map[int(test.y)][int(test.x)] == '#')
                    hit = true;
            }
        }

        float ceiling = getWindow().getAppView().getSize().y / 2.f - getWindow().getAppView().getSize().y / distance;
        float floor = getWindow().getAppView().getSize().y - ceiling;

        m_quads[i].setPosition(float(i), ceiling - m_player_angle.y);
        m_quads[i].setScale(1, (floor - ceiling) / getWindow().getAppView().getSize().y);
        m_quads[i].setColor({static_cast<sf::Uint8>(255-int(distance)*15),
                             static_cast<sf::Uint8>(255-int(distance)*15),
                             static_cast<sf::Uint8>(255-int(distance)*15),
                             255});
    }
}
