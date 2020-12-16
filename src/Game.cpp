#include "Game.hpp"

float cross(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

bool lineIntersect(const sf::Vector2f p1, const sf::Vector2f p2, const sf::Vector2f& q1, const sf::Vector2f& q2, sf::Vector2f& intersect) {
    auto p = p2 - p1;
    auto q = q2 - q1;
    auto q1p1 = q1 - p1;
    auto d = cross(p, q);

    if (d == 0)
        return false;

    auto t = cross(q1p1, q) / d;
    auto u = cross(-q1p1, p) / -d;

    intersect.x = p1.x + t * p.x;
    intersect.y = p1.y + t * p.y;
    return (0 <= t && t <= 1 && 0 <= u && u <= 1);
}

Game::Game() : ns::App("Ray Cast FPS", {1200, 675}, 1){
    sf::Vector2f res;
    auto b = lineIntersect({0, 0}, {3, 1}, {1, 0}, {0, -0}, res);
    ns_LOG(b, res);

    ns::Config::debug = false;
    sf::Mouse::setPosition(sf::Vector2i(getWindow().getAppView().getSize()/2.f), getWindow());

    ns_LOG("AppView Size :", getWindow().getAppView().getSize());
    ns_LOG("View Size :", ns::Config::Window::view_size);

    m_map = {
            "###################",
            "#                 #",
            "#######           #",
            "#######           #",
            "#######           #",
            "#    #  ### ##### #",
            "#    #  ######### #",
            "#    #  ########  #",
            "#                 #",
            "###### ############",
            "###### ############",
            "###### ############",
            "###### ############",
            "###### ############",
            "#    # #          #",
            "#      #      #   #",
            "#    # #      #   #",
            "###### #### #######",
            "#                 #",
            "###################"
    };
    m_max_depth = float(std::hypot(m_map.size(), m_map[0].size()))*1.5f;
    m_fov = 60.f;

    ns_LOG("Ray cast max depth :", m_max_depth);

    m_white_texture.create(1, int(getWindow().getAppView().getSize().y));
    m_white_texture.clear(sf::Color::White);
    m_white_texture.display();

    m_quads.resize(int(getWindow().getAppView().getSize().x));

    m_player_pos.x = 1.5;
    m_player_pos.y = 1.5;

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
    auto* camera = createCamera("main", 0);
    camera->lookAt(scene);
    ///////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////
    // create the HUD scene
    auto* hud = createScene("hud");

    // add drawables to the HUD scene here
    hud->getDefaultLayer()->addRaw(&m_hp_bar);

    // create the HUD camera
    auto* hud_cam = createCamera("hud_came", 1);
    //hud_cam->lookAt(hud);
    ///////////////////////////////////////////////////////

    ns::DebugTextInterface::font_size = 15;
    ns::DebugTextInterface::outline_thickness = 1;
    ns::DebugTextInterface::outline_color = sf::Color::Black;
    addDebugText<sf::Vector2f>(&m_player_pos, "player_pos", {0, 0});
    addDebugText<sf::Vector2f>(&m_player_angle, "player_angle", {0, 20});
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
            m_player_angle.x += float(dx);
            m_player_angle.y += float(dy)*0.5f;
            m_player_angle.y = std::max(-90.f, std::min(90.f, m_player_angle.y));
            sf::Mouse::setPosition(sf::Vector2i(getWindow().getAppView().getSize()/2.f), getWindow());
        }
}

void Game::update() {
    auto&& player_angle_rad = ns::to_radian(m_player_angle.x);
    sf::Vector2f player_dir{ std::cos(player_angle_rad), std::sin(player_angle_rad) };

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
    float step = 0.02f;
    for (int i = 0; i < nb_of_rays; ++i) {

        float ray_angle = ns::to_radian(m_player_angle.x - m_fov/2 + (float(i) / float(nb_of_rays)) * m_fov);
        float distance = 0;
        bool hit = false;

        while (!hit  && distance < m_max_depth) {
            distance += step;

            sf::Vector2f test = {
                    m_player_pos.x + cos(ray_angle) * distance,
                    m_player_pos.y + sin(ray_angle) * distance,
            };

            // test if outside the map
            if (int(test.x) < 0 || unsigned(test.x) >= m_map[0].size() || int(test.y) < 0 || unsigned(test.y) >= m_map.size()) {
                hit = true;
                distance = m_max_depth;
                step = 0.05f;
            }
            else {
                if (m_map[int(test.y)][int(test.x)] == '#') {
                    hit = true;
                    if (distance <= 0.05f)
                        step = 0.001f;
                    else if (distance <= 1.f)
                        step = 0.005f;
                    else if (distance <= 3.f )
                        step = 0.01f;
                    else
                        step = 0.02f;
                }
            }
        }

        // print the distance to the wall the player is facing
        //if (i == nb_of_rays/2) ns_LOG(distance);

        // give the feel of bigger space
        distance *= 1.5f;

        float horizon = getWindow().getAppView().getSize().y / 2.f;
        float ceiling = horizon - getWindow().getAppView().getSize().y / (distance/2.f);
        float floor = horizon + getWindow().getAppView().getSize().y / distance;

        m_quads[i].setPosition(float(i), ceiling - getWindow().getAppView().getSize().y * m_player_angle.y/45.f);
        m_quads[i].setScale(1, (floor - ceiling) / getWindow().getAppView().getSize().y);
        m_quads[i].setColor({static_cast<sf::Uint8>(int(std::max(0.f, 255-distance*8))),
                             static_cast<sf::Uint8>(int(std::max(0.f, 255-distance*8))),
                             static_cast<sf::Uint8>(int(std::max(0.f, 255-distance*8))),
                             255});
    }
}
