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
    m_max_depth = float(std::hypot(m_level_size.x, m_level_size.y)) * 1.5f;
    ns_LOG("Ray cast max depth :", m_max_depth);
    // field of view 60 degrees
    m_fov = 60.f;

    auto& appview_size = getWindow().getAppView().getSize();

    ///////////////////////////////////////////////////////
    // Main scene drawables
    m_wall_texture.create(1, int(appview_size.y));
    m_wall_texture.clear(sf::Color::White);
    m_wall_texture.display();

    m_walls_quads.setPrimitiveType(sf::PrimitiveType::Quads);
    for (int i = 0; i < int(appview_size.x); ++i) {
        m_walls_quads.append({{float(i), 0}});
        m_walls_quads.append({{float(i+1), 0}});
        m_walls_quads.append({{float(i+1), appview_size.y}});
        m_walls_quads.append({{float(i), appview_size.y}});
    }
    m_walls_quads.setTexture(m_level_map.allTilesets()[0]->getTexture());

    m_sprites_quads.setPrimitiveType(sf::PrimitiveType::Quads);
    m_sprites_quads.setTexture(ns::Res::getTexture("adventurer.png"));

    m_player_pos.x = 1.5;
    m_player_pos.y = 1.5;
    m_player_pos_z = 0.f;

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
    m_minimap_bg.setFillColor(sf::Color(155, 155, 155));
    m_minimap_bg.setPosition(appview_size.x - 253, 0);
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // Minimap drawables
    m_minimap_player.setRadius(5.f);
    m_minimap_player.setOrigin(5.f, 5.f);
    m_minimap_player.setFillColor(sf::Color::Blue);

    /*
    m_enemy.position = {7.5, 6.5};
    m_enemy.width = 0.6f;
    m_enemy.visible_start = 1.f;
    m_enemy.visible_end = 0.f;
    */

    auto* enemy = new Entity();
    enemy->setSize({0.3, 0.4});
    enemy->setPosition(5.5, 10.5);
    m_level_objects.emplace_back(enemy);

    m_sprite_hits_buffer[enemy].sprite = enemy;
    m_sprite_hits_buffer[enemy].visible = false;
    m_sprite_hits_buffer[enemy].t_min = 1.f;
    m_sprite_hits_buffer[enemy].t_max = 0.f;
    m_sprite_hits_buffer[enemy].ray_min = WINDOW_WIDTH;
    m_sprite_hits_buffer[enemy].ray_max = 0;

    m_sprites_quads.append({{0.f, 0.f}});
    m_sprites_quads.append({{0.f, 0.f}});
    m_sprites_quads.append({{0.f, 0.f}});
    m_sprites_quads.append({{0.f, 0.f}});

    m_minimap_enemies.setPrimitiveType(sf::PrimitiveType::Lines);
    for (const auto& ent : m_level_objects) {
        m_minimap_enemies.append({{enemy->getPosition()*m_tile_size}, sf::Color::Blue});
        m_minimap_enemies.append({{enemy->getPosition()*m_tile_size}, sf::Color::Blue});
    }

    m_minimap_rays.setPrimitiveType(sf::PrimitiveType::Lines);
    for (int i = 0; i < int(appview_size.x)*2; ++i)
        m_minimap_rays.append({ {0, 0}, sf::Color::Green });
    for (unsigned i = m_minimap_rays.getVertexCount()/2-50; i < m_minimap_rays.getVertexCount()/2+50; ++i)
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
    scene->getDefaultLayer()->addRaw(&m_walls_quads);
    scene->getDefaultLayer()->addRaw(&m_sprites_quads);

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
    minimap->getDefaultLayer()->addRaw(&m_minimap_enemies);

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
    addDebugText<float>([&]{return m_wall_hits_buffer[m_wall_hits_buffer.size() / 2].distance;}, "distance mid view :", {0, 40});
    addDebugText<float>(&m_fov, "FOV :", {0, 60});
    addDebugText<sf::Vector2f>([&]{auto&& player_angle_rad = ns::to_radian(m_player_angle.x); return sf::Vector2f(std::cos(player_angle_rad), std::sin(player_angle_rad));}, "Player dir :", {0, 80});
    addDebugText<float>(&m_player_pos_z, "Player pos Z :", {0, 100});
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
    bool crouch = false;
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        m_z_vel = 80;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        if (m_z_vel == 0) {
            m_player_pos_z = 0;
            crouch = true;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
        m_fov ++;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
        m_fov --;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        m_fov = 60;

    m_z_vel -= ns::Config::Physics::gravity;
    m_player_pos_z += m_z_vel;
    if (m_player_pos_z < 400.f) {
        m_z_vel = 0.f;
        m_player_pos_z = 400.f;
    }
    if (crouch)
        m_player_pos_z = 200.f;

    for (unsigned i = 0; i < m_level_objects.size(); ++i) {
        const auto& ent = m_level_objects[i];
        auto normal = ns::normal(m_player_pos - ent->getPosition()) / ns::norm(m_player_pos - ent->getPosition()) * ent->getSize().x;
        m_minimap_enemies[i*2].position = (ent->getPosition() - normal/2.f)*m_tile_size;
        m_minimap_enemies[i*2+1].position = (ent->getPosition() + normal/2.f)*m_tile_size;
    }

    for (auto& ent : m_level_objects) {
        ent->update();
    }

    m_minimap_player.setPosition(m_player_pos*m_tile_size);
    getCamera("minimap")->setRotation(m_player_angle.x + 90);
}

void Game::preRender() {
    auto& app_view_size = getWindow().getAppView().getSize();
    float horizon = app_view_size.y * (0.5f - m_player_angle.y/45.f);
    // adjust background horizon line
    m_background[2].position.y = horizon;
    m_background[3].position.y = horizon;
    m_background[4].position.y = horizon;
    m_background[5].position.y = horizon;

    auto&& player_angle_rad = ns::to_radian(m_player_angle.x);
    sf::Vector2f player_dir{ std::cos(player_angle_rad), std::sin(player_angle_rad) };

    doRayCast();

    for (unsigned i = 0; i < m_wall_hits_buffer.size(); ++i) {
        const auto& intersection = m_wall_hits_buffer[i];

        auto distance = intersection.distance;
        float ceiling = horizon - (getWindow().getAppView().getSize().y - (m_player_pos_z-app_view_size.y)) / distance;
        float ground = horizon + (getWindow().getAppView().getSize().y + (m_player_pos_z-app_view_size.y)) / distance;

        m_walls_quads[i*4].position.y = ceiling;
        m_walls_quads[i*4+1].position.y = ceiling;
        m_walls_quads[i*4+2].position.y = ground;
        m_walls_quads[i*4+3].position.y = ground;
        m_walls_quads[i*4].texCoords = {intersection.tex_coo, 0};
        m_walls_quads[i*4+1].texCoords = {intersection.tex_coo, 0};
        m_walls_quads[i*4+2].texCoords = {intersection.tex_coo, 16};
        m_walls_quads[i*4+3].texCoords = {intersection.tex_coo, 16};

        // light comes from top / north
        float color_mult = 10;
        if (intersection.side == Side::Bottom)
            color_mult = 50;
        else if (intersection.side != Side::Top)
            color_mult = 30;
        sf::Color wall_color = {
            static_cast<sf::Uint8>(std::max(0.f, 255-(distance + color_mult))),
            static_cast<sf::Uint8>(std::max(0.f, 255-(distance + color_mult))),
            static_cast<sf::Uint8>(std::max(0.f, 255-(distance + color_mult))),
            255
        };
        for (int v = 0; v < 4; ++v) {
            m_walls_quads[i*4+v].color = wall_color;
        }

        // update minimap rays
        m_minimap_rays[2*i].position = m_minimap_player.getPosition();
        m_minimap_rays[2*i + 1].position = intersection.point * m_tile_size;
    }
    int i = 0;
    for (auto& [ent, sprite_hit] : m_sprite_hits_buffer) {
        if (sprite_hit.visible) {
            auto& ent_size = ent->getSize();
            auto& ent_tex_rect = ent->getTextureRect();
            m_sprites_quads[i*4+0].position = {float(sprite_hit.ray_min), horizon + (m_player_pos_z - 2*app_view_size.y*ent_size.y) / sprite_hit.distance};
            m_sprites_quads[i*4+1].position = {float(sprite_hit.ray_max), horizon + (m_player_pos_z - 2*app_view_size.y*ent_size.y) / sprite_hit.distance};
            m_sprites_quads[i*4+2].position = {float(sprite_hit.ray_max), horizon + m_player_pos_z / sprite_hit.distance};
            m_sprites_quads[i*4+3].position = {float(sprite_hit.ray_min), horizon + m_player_pos_z / sprite_hit.distance};
            m_sprites_quads[i*4+0].texCoords = sf::Vector2f(ent_tex_rect.left + sprite_hit.t_min*ent_tex_rect.width, ent_tex_rect.top);
            m_sprites_quads[i*4+1].texCoords = sf::Vector2f(ent_tex_rect.left + sprite_hit.t_max*ent_tex_rect.width, ent_tex_rect.top);
            m_sprites_quads[i*4+2].texCoords = sf::Vector2f(ent_tex_rect.left + sprite_hit.t_max*ent_tex_rect.width, ent_tex_rect.top + ent_tex_rect.height);
            m_sprites_quads[i*4+3].texCoords = sf::Vector2f(ent_tex_rect.left + sprite_hit.t_min*ent_tex_rect.width, ent_tex_rect.top + ent_tex_rect.height);
        }
        else {
            m_sprites_quads[0].position = sf::Vector2f(0, 0);
            m_sprites_quads[1].position = sf::Vector2f(0, 0);
            m_sprites_quads[2].position = sf::Vector2f(0, 0);
            m_sprites_quads[3].position = sf::Vector2f(0, 0);
        }
        ++i;
        sprite_hit.visible = false;
        sprite_hit.t_min = 1.f;
        sprite_hit.t_max = 0.f;
        sprite_hit.ray_min = WINDOW_WIDTH;
        sprite_hit.ray_max = 0;
    }
}

void Game::doRayCast() {
    auto& walls_tile_layer = m_level_map.getTileLayer("walls");
    auto& tileset = m_level_map.allTilesets()[0];

    auto nb_of_rays = WINDOW_WIDTH;
    auto player_angle_rad = ns::to_radian(m_player_angle.x);
    auto fov_rad = ns::to_radian(m_fov);
    auto projection_distance = (float(nb_of_rays)/2) / std::tan(fov_rad/2);

    for (int i = 0; i < nb_of_rays; ++i) {

        float ray_angle_rad = std::atan(float(i-nb_of_rays/2) / projection_distance) + player_angle_rad;
        // the angle must be between 0 and 2*PI
        if (ray_angle_rad < 0) ray_angle_rad += 2*ns::PI;
        if (ray_angle_rad > 2*ns::PI) ray_angle_rad -= 2*ns::PI;

        auto ray_cos = std::cos(ray_angle_rad);
        auto ray_sin = std::sin(ray_angle_rad);
        auto ray_tan = std::tan(ray_angle_rad);
        sf::Vector2f ray_dir{ray_cos, ray_sin};

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
        sf::Vector2i test_point_int;

        float distance = 0;
        bool hit = false;
        bool checking_horizontal;

        while ( !hit ) {
            // check intersection with vertical line
            if ( vertical_test_dist < horizontal_test_dist ) {
                test_point = vertical_test;
                vertical_test += vertical_test_step;
                vertical_test_dist = ns::norm(vertical_test - m_player_pos);
                checking_horizontal = false;
            }
            // check intersection with horizontal line
            else {
                test_point = horizontal_test;
                horizontal_test += horizontal_test_step;
                horizontal_test_dist = ns::norm(horizontal_test - m_player_pos);
                checking_horizontal = true;
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
                if (walls_tile_layer->getTile(test_point_int.x, test_point_int.y).gid != 0) {
                    hit = true;
                    distance = ns::norm(test_point - m_player_pos);
                }
            }
        }
        // fish eye correction
        distance *= std::cos(ray_angle_rad-player_angle_rad);
        // adjust distance width fov angle
        distance *= fov_rad;

        // find the wall hit side and position
        Side side;
        float u;
        if (checking_horizontal) {
            u = test_point.x - std::floor(test_point.x);
            side = ray_dir.y < 0 ? Side::Bottom : Side::Top;
        }
        else {
            u = test_point.y - std::floor(test_point.y);
            side = ray_dir.x < 0 ? Side::Right : Side::Left;
        }
        if (side == Side::Top || side == Side::Right)
            u = 1 - u;

        // find wall tile and texture pos
        auto tile_gid = walls_tile_layer->getTile(test_point_int.x, test_point_int.y).gid;
        auto tex_coo = tileset->getTileTextureRect(tile_gid - tileset->firstgid).left + 16 * u;

        // fill all the information
        auto& wall_hit = m_wall_hits_buffer[i];
        wall_hit.distance = distance;
        wall_hit.point = test_point;
        wall_hit.side = side;
        wall_hit.tex_coo = tex_coo;
        wall_hit.tile_gid = tile_gid;

        for (auto& ent_unique : m_level_objects) {
            auto* ent = ent_unique.get();
            auto ent_distance = ns::norm(m_player_pos - ent->getPosition())*fov_rad;
            if (ent_distance < distance) {
                auto normal = ns::normal(m_player_pos-ent->getPosition())/ent_distance;
                sf::Vector2f p1 = ent->getPosition() - normal * ent->getSize().x/2.f;
                sf::Vector2f p2 = ent->getPosition() + normal * ent->getSize().x/2.f;
                sf::Vector2f p_inter;
                if (lineIntersect(m_player_pos, test_point, p1, p2, p_inter)) {
                    auto& sprite_hit = m_sprite_hits_buffer[ent];
                    sprite_hit.visible = true;
                    sprite_hit.distance = ent_distance;
                    // find the intersection point in percentage of entity width (for texture coordinates)
                    // and find the first and last rays that see the entity (for world position)
                    auto t = ns::norm(p_inter - p1) / ns::norm(p2 - p1);
                    if (sprite_hit.t_min > t) {
                        sprite_hit.t_min = t;
                        sprite_hit.ray_min = i;
                    }
                    if (t > sprite_hit.t_max) {
                        sprite_hit.t_max = t;
                        sprite_hit.ray_max = i;
                    }
                }
            }
        }
    }
}
