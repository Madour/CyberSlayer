#define _CRT_SECURE_NO_WARNINGS     // disable MSVC useless warnings when using strcpy

#include "Game.hpp"
#include <cstring>

bool lineIntersect(const sf::Vector2f p1, const sf::Vector2f p2, const sf::Vector2f& q1, const sf::Vector2f& q2, sf::Vector2f& intersect) {
    auto p = p2 - p1;
    auto q = q2 - q1;
    auto q1p1 = q1 - p1;
    auto d = ns::cross_product(p, q);

    if (d == 0)
        return false;

    auto t = ns::cross_product(q1p1, q) / d;
    auto u = ns::cross_product(-q1p1, p) / -d;

    intersect.x = p1.x + t * p.x;
    intersect.y = p1.y + t * p.y;
    return (0 <= t && t <= 1 && 0 <= u && u <= 1);
}

Game::Game() {
    ns::Config::debug = false;

    ns_LOG("AppView Size :", getWindow().getAppView().getSize());
    ns_LOG("View Size :", ns::Config::Window::view_size);

    m_level_map.loadFromFile("assets/level_test.tmx");
    m_tile_size = m_level_map.getTileSize().x;
    m_level_size = m_level_map.getDimension();

    ns_LOG("Level size :", m_level_size, " Tile size :", m_tile_size);

    // max depth is the length of map's diagonal
    m_max_depth = float(std::hypot(m_level_size.y, m_level_size.x)) * 1.5f;
    ns_LOG("Ray cast max depth :", m_max_depth);
    // field of view 60 degrees
    m_fov = 60.f;

    auto& appview_size = getWindow().getAppView().getSize();

    ///////////////////////////////////////////////////////
    // Main scene drawables
    m_wall_texture.create(1, int(appview_size.y));
    m_wall_texture.clear(sf::Color::White);
    m_wall_texture.display();

    // m_quads contains the pixels columns rendered by rays
    m_quads.resize(int(appview_size.x));
    auto* batch = new ns::SpriteBatch("walls");
    for (auto& quad : m_quads) {
        quad.setTexture(m_wall_texture.getTexture());
        batch->draw(&quad);
    }
    batch->end();

    m_player_pos.x = 1.5;
    m_player_pos.y = 1.5;

    m_background.setPrimitiveType(sf::PrimitiveType::Quads);
    // sky
    auto sky_color1 = sf::Color(145, 205, 237);
    auto sky_color2 = sf::Color(70, 124, 153);
    m_background.append({{0, 0}, sky_color1});
    m_background.append({{appview_size.x, 0}, sky_color1});
    m_background.append({{appview_size.x, appview_size.y/2}, sky_color2});
    m_background.append({{0, appview_size.y/2}, sky_color2});
    // ground
    auto ground_color1 = sf::Color(145, 110, 41);
    auto ground_color2 = sf::Color(64, 48, 17);
    m_background.append({{0, appview_size.y/2}, ground_color2});
    m_background.append({{appview_size.x, appview_size.y/2}, ground_color2});
    m_background.append({{appview_size.x, appview_size.y}, ground_color1});
    m_background.append({{0, appview_size.y}, ground_color1});
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // HUD drawables
    m_hp_bar.setSize({200, 10});
    m_hp_bar.setPosition(20, 20);
    m_hp_bar.setFillColor(sf::Color::Red);

    auto* help_text = new sf::Text("Press B or N to decrease or increase FOV\nPress M to reset FOV to 60 degree", ns::Arial::getFont());
    help_text->setCharacterSize(15);
    help_text->setOutlineColor(sf::Color::Black);
    help_text->setOutlineThickness(1.f);
    help_text->setPosition((appview_size.x - help_text->getGlobalBounds().width)/2, 10);

    m_minimap_bg.setSize({253, 253});
    m_minimap_bg.setFillColor(sf::Color(25, 25, 25));
    m_minimap_bg.setPosition(appview_size.x - 253, 0);
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // Minimap drawables
    m_minimap_player.setRadius(5.f);
    m_minimap_player.setOrigin(5.f, 5.f);
    m_minimap_player.setFillColor(sf::Color::Blue);

    m_minimap_rays.setPrimitiveType(sf::PrimitiveType::Lines);
    for (int i = 0; i < int(appview_size.x)*2; ++i)
        m_minimap_rays.append({ {0, 0}, sf::Color::Green });
    for (int i = m_minimap_rays.getVertexCount()/2-50; i < m_minimap_rays.getVertexCount()/2+50; ++i)
        m_minimap_rays[i].color = sf::Color::Red;

    // create minimap grid
    m_minimap_grid.resize(m_level_size.x * m_level_size.y * 2);
    m_minimap_grid.setPrimitiveType(sf::PrimitiveType::Lines);
    for (int y = 0; y < m_level_size.y; ++y) {
        m_minimap_grid[y*2].position = {0, y*m_tile_size};
        m_minimap_grid[y*2].color = sf::Color::Black;
        m_minimap_grid[y*2 + 1].position = {m_level_size.x * m_tile_size, y * m_tile_size};
        m_minimap_grid[y*2 + 1].color = sf::Color::Black;
    }
    for (int x = 0; x < m_level_size.x; ++x) {
        m_minimap_grid[m_level_size.y*2 + x*2].position = {x * m_tile_size, 0};
        m_minimap_grid[m_level_size.y*2 + x*2].color = sf::Color::Black;
        m_minimap_grid[m_level_size.y*2 + x*2 + 1].position = {x * m_tile_size, m_level_size.y * m_tile_size};
        m_minimap_grid[m_level_size.y*2 + x*2 + 1].color = sf::Color::Black;
    }
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // create the main scene
    auto* scene = createScene("main");
    scene->addLayer("bg", 0);

    // add drawables to the main scene here
    scene->getDefaultLayer()->addRaw(&m_background);
    scene->getDefaultLayer()->add(batch);

    // create the main camera
    auto* camera = createCamera("main", 0);
    camera->lookAt(scene);
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // create the HUD scene
    auto* hud = createScene("hud");

    // add drawables to the HUD scene here
    //hud->getDefaultLayer()->addRaw(&m_hp_bar);
    hud->getDefaultLayer()->addRaw(&m_minimap_bg);
    hud->getDefaultLayer()->add(help_text);

    // create the HUD camera
    auto* hud_cam = createCamera("hud", 1);
    hud_cam->lookAt(hud);
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // create the Minimap scene
    auto* minimap = createScene("minimap");

    // add drawables to the Minimap scene here
    //minimap->getDefaultLayer()->add(new sf::Sprite(m_minimap_texture.getTexture()));
    minimap->getDefaultLayer()->add(m_level_map.getTileLayer("walls"));
    minimap->getDefaultLayer()->addRaw(&m_minimap_rays);
    minimap->getDefaultLayer()->addRaw(&m_minimap_player);
    minimap->getDefaultLayer()->addRaw(&m_minimap_grid);

    // create the Minimap camera
    auto* minimap_cam = createCamera("minimap", 2, {0, 0, 25*10, 25*10}, {appview_size.x-250, 0, 250, 250});
    minimap_cam->lookAt(minimap);
    minimap_cam->follow(m_minimap_player);
    minimap_cam->zoom(0.8f);
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // Add debug texts (F1 to activate debug mode)
    ns::DebugTextInterface::font_size = 15;
    ns::DebugTextInterface::outline_thickness = 1;
    ns::DebugTextInterface::outline_color = sf::Color::Black;
    addDebugText<sf::Vector2f>(&m_player_pos, "player_pos :", {0, 0});
    addDebugText<sf::Vector2f>(&m_player_angle, "player_angle :", {0, 20});
    addDebugText<float>([&]{return m_ray_intersection_buffer[m_ray_intersection_buffer.size()/2].distance;}, "distance mid view :", {0, 40});
    addDebugText<float>(&m_fov, "FOV :", {0, 60});
    ///////////////////////////////////////////////////////

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
            // angle x between 0 and 359
            m_player_angle.x += float(dx)*0.5f;
            if (m_player_angle.x > 360) m_player_angle.x -= 360.f;
            if (m_player_angle.x < 0) m_player_angle.x += 360.f;
            // angle y between -90 and +90
            m_player_angle.y += float(dy)*0.5f*0.5f;
            m_player_angle.y = std::max(-90.f, std::min(90.f, m_player_angle.y));
        }
}

void Game::update() {
    auto&& player_angle_rad = ns::to_radian(m_player_angle.x);
    sf::Vector2f player_dir{ std::cos(player_angle_rad), std::sin(player_angle_rad) };

    if (getWindow().hasFocus())
        sf::Mouse::setPosition(sf::Vector2i(getWindow().getAppView().getSize()/2.f), getWindow());

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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
        m_fov ++;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
        m_fov --;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        m_fov = 60;

    m_minimap_player.setPosition(m_player_pos*m_tile_size);
    getCamera("minimap")->setRotation(m_player_angle.x + 90);
}

void Game::preRender() {
    float horizon = getWindow().getAppView().getSize().y * (0.5f - m_player_angle.y/45.f);
    // adjust background horizon line
    m_background[2].position.y = horizon;
    m_background[3].position.y = horizon;
    m_background[4].position.y = horizon;
    m_background[5].position.y = horizon;

    doRayCast();

    for (unsigned i = 0; i < m_ray_intersection_buffer.size(); ++i) {
        const auto& intersection = m_ray_intersection_buffer[i];
        // gives the feel of bigger space
        auto distance = intersection.distance * 1.5f;

        float ceiling = horizon - (getWindow().getAppView().getSize().y / distance) * 2; // walls are two units high
        float ground = horizon + getWindow().getAppView().getSize().y / distance;

        auto& slice = m_quads[i];
        slice.setPosition(float(i), ceiling);
        slice.setScale(1, (ground - ceiling) / getWindow().getAppView().getSize().y);
        slice.setColor({
            static_cast<sf::Uint8>(int(std::max(0.f, 255-distance*2))),
            static_cast<sf::Uint8>(int(std::max(0.f, 255-distance*2))),
            static_cast<sf::Uint8>(int(std::max(0.f, 255-distance*2))),
            255
        });

        // update minimap rays
        m_minimap_rays[2*i].position = m_minimap_player.getPosition();
        m_minimap_rays[2*i + 1].position = intersection.point * m_tile_size;
    }
}

void Game::doRayCast() {
    auto nb_of_rays = int(getWindow().getAppView().getSize().x);
    auto player_angle_rad = ns::to_radian(m_player_angle.x);
    auto fov_rad = ns::to_radian(m_fov);
    auto projection_distance = float(nb_of_rays/2) / std::tan(fov_rad/2);
    for (int i = 0; i < nb_of_rays; ++i) {

        float ray_angle_rad = std::atan(float(i-nb_of_rays/2) / projection_distance) + player_angle_rad;
        // the angle must be between 0 and 2*PI
        if (ray_angle_rad < 0) ray_angle_rad += 2*ns::PI;
        if (ray_angle_rad > 2*ns::PI) ray_angle_rad -= 2*ns::PI;

        float ray_cos = std::cos(ray_angle_rad);
        float ray_sin = std::sin(ray_angle_rad);
        float ray_tan = std::tan(ray_angle_rad);
        sf::Vector2f ray_dir{ ray_cos, ray_sin };

        sf::Vector2f horizontal_test = m_player_pos;
        sf::Vector2f horizontal_test_step;
        float horizontal_test_dist;
        // ray is completely horizontal, do not check intersects with x axis
        if (ray_angle_rad == 0 || ray_angle_rad == ns::PI)
            horizontal_test_dist = m_max_depth;
        else {
            // looking up
            if (ray_angle_rad > ns::PI) {
                horizontal_test.y = std::floor(horizontal_test.y)-0.0001;
                horizontal_test_step.y = -1.f;
                horizontal_test_step.x = -1/ray_tan;
            }
                // looking down
            else if (ray_angle_rad < ns::PI) {
                horizontal_test.y = std::ceil(horizontal_test.y);
                horizontal_test_step.y = 1.f;
                horizontal_test_step.x = 1/ray_tan;
            }
            horizontal_test.x += (horizontal_test.y - m_player_pos.y) / ray_tan;
            horizontal_test_dist = ns::norm(horizontal_test - m_player_pos);
        }

        sf::Vector2f vertical_test = m_player_pos;
        sf::Vector2f vertical_test_step;
        float vertical_test_dist;
        // ray is completely vertical, do not check intersects with vertical lines
        if (ray_angle_rad == ns::PI/2 || ray_angle_rad == 3*ns::PI/2)
            vertical_test_dist = m_max_depth;
        else {
            // looking left
            if (ns::PI/2 < ray_angle_rad && ray_angle_rad < 3*ns::PI/2) {
                vertical_test.x = std::floor(vertical_test.x)-0.0001;
                vertical_test_step.x = -1.f;
                vertical_test_step.y = -ray_tan;
            }
                // looking right
            else if (ray_angle_rad < ns::PI/2 || ray_angle_rad > 3*ns::PI/2) {
                vertical_test.x = std::ceil(vertical_test.x);
                vertical_test_step.x = 1.f;
                vertical_test_step.y = ray_tan;
            }
            vertical_test.y += (vertical_test.x - m_player_pos.x) * ray_tan;
            vertical_test_dist = ns::norm(vertical_test - m_player_pos);
        }

        sf::Vector2f test_point;

        float distance = 0;
        bool hit = false;

        sf::Vector2i test_point_int;

        while ( !hit ) {
            // check intersection with vertical line
            if ( vertical_test_dist < horizontal_test_dist ) {
                test_point = vertical_test;
                vertical_test += vertical_test_step;
                vertical_test_dist = ns::norm(vertical_test - m_player_pos);
            }
                // check intersection with horizontal line
            else {
                test_point = horizontal_test;
                horizontal_test += horizontal_test_step;
                horizontal_test_dist = ns::norm(horizontal_test - m_player_pos);
            }

            test_point_int.x = int(test_point.x);
            test_point_int.y = int(test_point.y);

            // test if outside the map
            if (test_point_int.x < 0 || test_point_int.x >= m_level_size.x
                || test_point_int.y < 0 || test_point_int.y >= m_level_size.y) {
                hit = true;
                distance = m_max_depth;
            }
            else {
                //if (m_map[test_point_int.y][test_point_int.x] == '#') {
                if (m_level_map.getTileLayer("walls")->getTile(test_point_int.x, test_point_int.y).gid != 0) {
                    hit = true;
                    distance = ns::norm(test_point - m_player_pos);
                }
            }
        }

        // fish eye correction
        distance *= std::cos(ray_angle_rad-player_angle_rad);
        // adjust distance width fov angle
        distance *= fov_rad*fov_rad;
        // store distance in depth buffer
        m_ray_intersection_buffer[i].distance = distance;
        m_ray_intersection_buffer[i].point = test_point;
    }
}
