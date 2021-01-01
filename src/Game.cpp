#include "Game.hpp"

bool lineIntersect(const sf::Vector2f p1, const sf::Vector2f p2, const sf::Vector2f& q1, const sf::Vector2f& q2, sf::Vector2f& intersect) {
    static sf::Vector2f p; p.x = p2.x - p1.x; p.y = p2.y - p1.y;
    static sf::Vector2f q; q.x = q2.x - q1.x; q.y = q2.y - q1.y;
    static sf::Vector2f q1p1; q1p1.x = q1.x - p1.x; q1p1.y = q1.y - p1.y;
    static sf::Vector2f q1p1_inv; q1p1_inv.x = -q1p1.x; q1p1_inv.y = -q1p1.y;

    float d = ns::cross_product(p, q);

    if (d == 0.f)
        return false;

    auto t = ns::cross_product(q1p1, q) / d;
    auto u = ns::cross_product(q1p1_inv, p) / -d;

    intersect.x = p1.x + t * p.x;
    intersect.y = p1.y + t * p.y;
    return (0 <= t && t <= 1 && 0 <= u && u <= 1);
}

Game::Game() {
    ns::Config::debug = false;
    auto& appview_size = getWindow().getAppView().getSize();

    m_level_map.loadFromFile("assets/level_test.tmx");
    m_tile_size = m_level_map.getTileSize().x;
    m_level_size = m_level_map.getDimension();
    // max depth is the length of map's diagonal
    m_max_depth = float(std::hypot(m_level_size.x, m_level_size.y)) * 1.5f;

    ns_LOG("AppView Size :", appview_size);
    ns_LOG("Level size :", m_level_size, " Tile size :", m_tile_size);
    ns_LOG("Ray cast max depth :", m_max_depth);

    m_player = new Player();
    m_player->transform()->setPosition(6.0f*METER, 6.0f*METER);
    m_level_objects.emplace_back(m_player);

    // camera pos
    m_camera_pos.x = 6.0f*METER;
    m_camera_pos.y = 6.0f*METER;
    m_camera_pos.z = 1.4f*METER;

    m_fov = FOV;
    m_horizon = VIEW_HEIGHT;
    m_projection_plane_distance = 0;

    // create some Entities
    for (int i = 0; i < 10; ++i) {
        auto* ent = new Adventurer();
        ent->transform()->setPosition(1.5f+std::rand()%18, 1.5f+std::rand()%18);
        m_level_objects.emplace_back(ent);
    }
    // resize the sprite hit buffer used by the ray caster
    m_sprite_hits_buffer.resize(m_level_objects.size());

    ///////////////////////////////////////////////////////
    // Main scene drawables
    // setup walls vertices
    m_walls_quads.setPrimitiveType(sf::PrimitiveType::Quads);
    for (int i = 0; i < VIEW_WIDTH; ++i) {
        m_walls_quads.append({{float(i), 0}});
        m_walls_quads.append({{float(i+1), 0}});
        m_walls_quads.append({{float(i+1), appview_size.y}});
        m_walls_quads.append({{float(i), appview_size.y}});
    }
    m_walls_quads.setTexture(m_level_map.allTilesets()[0]->getTexture());

    // adjust sprites vertex array size
    m_sprites_quads.setPrimitiveType(sf::PrimitiveType::Quads);
    m_sprites_quads.setTexture(ns::Res::getTexture("adventurer.png"));
    m_sprites_quads.resize(m_level_objects.size()*4);

    m_floor_casting.setPrimitiveType(sf::PrimitiveType::Points);
    m_floor_casting.resize(5);

    // setup background
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

    auto* help_text = new sf::Text("Press B or N to decrease or increase FOV\nPress M to reset", ns::Arial::getFont());
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
    m_minimap_entities.setPrimitiveType(sf::PrimitiveType::Lines);
    for (const auto& ent : m_level_objects) {
        m_minimap_entities.append({{ent->getPosition() * m_tile_size}, sf::Color::Blue});
        m_minimap_entities.append({{ent->getPosition() * m_tile_size}, sf::Color::Blue});
    }

    m_minimap_rays.setPrimitiveType(sf::PrimitiveType::Lines);
    for (int i = 0; i < VIEW_WIDTH*2; ++i)
        m_minimap_rays.append({ {0, 0}, sf::Color::Green });
    for (unsigned i = m_minimap_rays.getVertexCount()/2-50; i < m_minimap_rays.getVertexCount()/2+50; ++i)
        m_minimap_rays[i].color = sf::Color::Red;

    // create minimap grid
    m_minimap_grid.resize(m_level_size.x * m_level_size.y * 2);
    m_minimap_grid.setPrimitiveType(sf::PrimitiveType::Lines);
    for (int y = 0; y < m_level_size.y; ++y) {
        m_minimap_grid[y*2].position = {0, y * m_tile_size};
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
    minimap->getDefaultLayer()->addRaw(&m_minimap_grid);
    minimap->getDefaultLayer()->addRaw(&m_minimap_entities);

    // create the Minimap camera
    auto* minimap_cam = createCamera("minimap", 2, {0, 0, 25*10, 25*10}, {appview_size.x-250, 0, 250, 250});
    minimap_cam->lookAt(minimap);
    minimap_cam->zoom(0.8f);
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    // Add debug texts (F1 to activate debug mode)
    ns::DebugTextInterface::font_size = 15;
    ns::DebugTextInterface::outline_thickness = 1;
    ns::DebugTextInterface::outline_color = sf::Color::Black;
    addDebugText<sf::Vector3f>([&]{return m_camera_pos/METER;}, "camera_pos (m):", {0, 0});
    addDebugText<sf::Vector3f>(&m_camera_rot, "camera_rot :", {0, 20});
    addDebugText<sf::Vector2f>([&]{auto&& camera_angle_x = ns::to_radian(m_camera_rot.x); return sf::Vector2f(std::cos(camera_angle_x), std::sin(camera_angle_x));}, "camera_dir :", {0, 40});
    addDebugText<float>([&]{return m_wall_hits_buffer[m_wall_hits_buffer.size() / 2].distance/METER;}, "distance mid view (m):", {0, 60});
    addDebugText<float>(&m_fov, "FOV :", {0, 80});
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
            m_camera_rot.x += float(dx) * 0.5f;
            if (m_camera_rot.x > 360) m_camera_rot.x -= 360.f;
            if (m_camera_rot.x < 0) m_camera_rot.x += 360.f;
            // angle y between -90 and +90
            m_camera_rot.z += float(dy) * 0.5f * 0.5f;

            // 3rd person camera
            //if (std::abs(m_camera_rot.z) < 90)
            //    m_camera_pos.z += float(dy)*0.01f;

            m_camera_rot.z = std::max(-90.f, std::min(90.f, m_camera_rot.z));
        }
}

void Game::update() {
    auto&& player_angle_rad = ns::to_radian(m_camera_rot.x);
    sf::Vector2f player_dir{ std::cos(player_angle_rad), std::sin(player_angle_rad) };
    if (getWindow().hasFocus())
        sf::Mouse::setPosition(sf::Vector2i(getWindow().getAppView().getSize()/2.f), getWindow());

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
        m_fov ++;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
        m_fov --;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        m_fov = FOV;

    // update all entities
    for (auto& ent : m_level_objects) {
        if (ent.get() != m_player)
            ent->update();
    }
    m_player->update(m_camera_rot);

    m_camera_pos.x = m_player->getPosition().x + 0.01f*std::cos(ns::to_radian(m_camera_rot.x));
    m_camera_pos.y = m_player->getPosition().y + 0.01f*std::sin(ns::to_radian(m_camera_rot.x));
    m_camera_pos.z = m_player->getEyeHeight();

    sf::Vector2f camera_pos2d{m_camera_pos.x, m_camera_pos.y};
    for (unsigned i = 0; i < m_level_objects.size(); ++i) {
        const auto& ent = m_level_objects[i];
        auto normal = ns::normal(camera_pos2d - ent->getPosition()) / ns::distance(camera_pos2d, ent->getPosition()) * ent->getSize().x;
        m_minimap_entities[i * 2].position = (ent->getPosition() - normal / (METER*2.f)) * m_tile_size;
        m_minimap_entities[i * 2 + 1].position = (ent->getPosition() + normal / (METER*2.f)) * m_tile_size;
    }
    getCamera("minimap")->setCenter(camera_pos2d*m_tile_size);
    getCamera("minimap")->setRotation(m_camera_rot.x + 90);
}

void Game::preRender() {
    auto& app_view_size = getWindow().getAppView().getSize();
    m_horizon = app_view_size.y * (0.5f - m_camera_rot.z / 45.f);
    // adjust background horizon line
    m_background[2].position.y = m_horizon;
    m_background[3].position.y = m_horizon;
    m_background[4].position.y = m_horizon;
    m_background[5].position.y = m_horizon;

    auto&& player_angle_rad = ns::to_radian(m_camera_rot.x);

    doRayCast();

    for (unsigned i = 0; i < m_wall_hits_buffer.size(); ++i) {
        const auto& wall_hit = m_wall_hits_buffer[i];

        auto ratio = m_projection_plane_distance / wall_hit.distance; // thales
        float ceiling = m_horizon - (WALL_HEIGHT - m_camera_pos.z) * ratio;
        float ground = m_horizon + (0 + m_camera_pos.z) * ratio;

        m_walls_quads[i*4+0].position.y = ceiling;
        m_walls_quads[i*4+1].position.y = ceiling;
        m_walls_quads[i*4+2].position.y = ground;
        m_walls_quads[i*4+3].position.y = ground;
        m_walls_quads[i*4+0].texCoords = {wall_hit.tex_coo, 0};
        m_walls_quads[i*4+1].texCoords = {wall_hit.tex_coo, 0};
        m_walls_quads[i*4+2].texCoords = {wall_hit.tex_coo, 16};
        m_walls_quads[i*4+3].texCoords = {wall_hit.tex_coo, 16};

        // global light comes from top/north
        float color_mult = 10;
        if (wall_hit.side == Side::Bottom)
            color_mult = 50;
        else if (wall_hit.side != Side::Top)
            color_mult = 30;
        sf::Color wall_color = {
            static_cast<sf::Uint8>(std::max(0.f, 255-(wall_hit.distance*5.f + color_mult))),
            static_cast<sf::Uint8>(std::max(0.f, 255-(wall_hit.distance*5.f + color_mult))),
            static_cast<sf::Uint8>(std::max(0.f, 255-(wall_hit.distance*5.f + color_mult))),
            255
        };
        // update color
        for (int v = 0; v < 4; ++v) {
            m_walls_quads[i*4+v].color = wall_color;
        }
        // update minimap rays
        auto camera_pos2d = sf::Vector2f(m_camera_pos.x, m_camera_pos.y);
        m_minimap_rays[2*i].position = camera_pos2d*m_tile_size;
        m_minimap_rays[2*i + 1].position = wall_hit.point * m_tile_size;
    }
    int i = 0;
    for (auto& sprite_hit : m_sprite_hits_buffer) {
        auto ent = sprite_hit.sprite;
        if (sprite_hit.visible) {
            auto ratio = m_projection_plane_distance / sprite_hit.distance; // thales
            auto& ent_size = ent->getSize();
            auto& ent_tex_rect = ent->getTextureRect();
            m_sprites_quads[i*4+0].position = {float(sprite_hit.ray_min), m_horizon - (ent_size.y  - m_camera_pos.z)*ratio};
            m_sprites_quads[i*4+1].position = {float(sprite_hit.ray_max), m_horizon - (ent_size.y  - m_camera_pos.z)*ratio};
            m_sprites_quads[i*4+2].position = {float(sprite_hit.ray_max), m_horizon + m_camera_pos.z * ratio};
            m_sprites_quads[i*4+3].position = {float(sprite_hit.ray_min), m_horizon + m_camera_pos.z * ratio};
            m_sprites_quads[i*4+0].texCoords = sf::Vector2f(ent_tex_rect.left + sprite_hit.t_min*ent_tex_rect.width, ent_tex_rect.top);
            m_sprites_quads[i*4+1].texCoords = sf::Vector2f(ent_tex_rect.left + sprite_hit.t_max*ent_tex_rect.width, ent_tex_rect.top);
            m_sprites_quads[i*4+2].texCoords = sf::Vector2f(ent_tex_rect.left + sprite_hit.t_max*ent_tex_rect.width, ent_tex_rect.top + ent_tex_rect.height);
            m_sprites_quads[i*4+3].texCoords = sf::Vector2f(ent_tex_rect.left + sprite_hit.t_min*ent_tex_rect.width, ent_tex_rect.top + ent_tex_rect.height);
        }
        else {
            m_sprites_quads[i*4+0].position = sf::Vector2f(0, 0);
            m_sprites_quads[i*4+1].position = sf::Vector2f(0, 0);
            m_sprites_quads[i*4+2].position = sf::Vector2f(0, 0);
            m_sprites_quads[i*4+3].position = sf::Vector2f(0, 0);
        }
        ++i;
        sprite_hit.visible = false;
        sprite_hit.t_min = 1.f;
        sprite_hit.t_max = 0.f;
        sprite_hit.ray_min = VIEW_WIDTH;
        sprite_hit.ray_max = 0;
    }
}

void Game::doRayCast() {
    auto& app_view_size = getWindow().getAppView().getSize();
    auto& walls_tile_layer = m_level_map.getTileLayer("walls");
    auto& tileset = m_level_map.allTilesets()[0];
    sf::Vector2f camera_pos2d{m_camera_pos.x, m_camera_pos.y};

    auto player_angle_rad = ns::to_radian(m_camera_rot.x);
    auto fov_rad = ns::to_radian(m_fov);
    m_projection_plane_distance = (app_view_size.x/2) / std::tan(fov_rad/2);

    // sort level objects by their distance to player, to get correct depth order
    std::sort(m_level_objects.begin(), m_level_objects.end(), [&](auto& lhs, auto& rhs){
        return ns::distance(camera_pos2d, lhs->getPosition()) > ns::distance(camera_pos2d, rhs->getPosition());
    });

    for (int i = 0; i < VIEW_WIDTH; ++i) {

        float ray_angle_rad = std::atan((float(i)-app_view_size.x/2) / m_projection_plane_distance) + player_angle_rad;
        // the angle must be between 0 and 2*PI
        if (ray_angle_rad < 0) ray_angle_rad += 2*ns::PI;
        if (ray_angle_rad > 2*ns::PI) ray_angle_rad -= 2*ns::PI;

        auto ray_cos = std::cos(ray_angle_rad);
        auto ray_sin = std::sin(ray_angle_rad);
        auto ray_tan = std::tan(ray_angle_rad);
        sf::Vector2f ray_dir{ray_cos, ray_sin};

        sf::Vector2f horizontal_test = camera_pos2d;
        sf::Vector2f horizontal_test_step;
        float horizontal_test_dist;
        // ray is completely horizontal, do not check intersects with x axis
        if (ray_dir.y == 0)
            horizontal_test_dist = m_max_depth;
        else {
            // looking up
            if (ray_dir.y < 0) {
                horizontal_test.y = std::floor(horizontal_test.y)-0.0001;
                horizontal_test_step.y = -1.f;
                horizontal_test_step.x = -1/ray_tan;
            }
            // looking down
            else if (ray_dir.y > 0) {
                horizontal_test.y = std::ceil(horizontal_test.y);
                horizontal_test_step.y = 1.f;
                horizontal_test_step.x = 1/ray_tan;
            }
            horizontal_test.x += (horizontal_test.y - camera_pos2d.y) / ray_tan;
            horizontal_test_dist = ns::distance(camera_pos2d, horizontal_test);
        }

        sf::Vector2f vertical_test = camera_pos2d;
        sf::Vector2f vertical_test_step;
        float vertical_test_dist;
        // ray is completely vertical, do not check intersects with vertical lines
        if (ray_dir.x == 0)
            vertical_test_dist = m_max_depth;
        else {
            // looking left
            if (ray_dir.x < 0) {
                vertical_test.x = std::floor(vertical_test.x)-0.0001;
                vertical_test_step.x = -1.f;
                vertical_test_step.y = -ray_tan;
            }
            // looking right
            else if (ray_dir.x > 0) {
                vertical_test.x = std::ceil(vertical_test.x);
                vertical_test_step.x = 1.f;
                vertical_test_step.y = ray_tan;
            }
            vertical_test.y += (vertical_test.x - camera_pos2d.x) * ray_tan;
            vertical_test_dist = ns::distance(camera_pos2d, vertical_test);
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
                vertical_test_dist = ns::distance(camera_pos2d, vertical_test);
                checking_horizontal = false;
            }
            // check intersection with horizontal line
            else {
                test_point = horizontal_test;
                horizontal_test += horizontal_test_step;
                horizontal_test_dist = ns::distance(camera_pos2d, horizontal_test);
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
                    distance += ns::distance(camera_pos2d, test_point);
                }
            }
        }
        // fish eye correction
        auto fisheye_correction = std::cos(ray_angle_rad-player_angle_rad);
        distance *= fisheye_correction;

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
        unsigned tile_gid;
        float tex_coo;
        if (distance < m_max_depth) {
            tile_gid = walls_tile_layer->getTile(test_point_int.x, test_point_int.y).gid;
            tex_coo = tileset->getTileTextureRect(tile_gid - tileset->firstgid).left + 16 * u;
        }
        else {
            tile_gid = 0;
            tex_coo = 0.f;
        }

        // fill all the information
        auto& wall_hit = m_wall_hits_buffer[i];
        wall_hit.distance = distance;
        wall_hit.point = test_point;
        wall_hit.side = side;
        wall_hit.tex_coo = tex_coo;
        wall_hit.tile_gid = tile_gid;

        /*int ground = int(m_horizon + (app_view_size.y +  (m_camera_pos.z-app_view_size.y)) / distance);
        for (int y = ground; y < VIEW_HEIGHT; ++y) {

        }*/

        int spr_hit_index = 0;
        for (auto& ent_unique : m_level_objects) {
            auto* ent = ent_unique.get();

            // vector from entity pos to player pos
            sf::Vector2f ent_play_vec;
            ent_play_vec.x = m_camera_pos.x - ent->getPosition().x;
            ent_play_vec.y = m_camera_pos.y - ent->getPosition().y;
            auto ent_distance = ns::distance(camera_pos2d, ent->getPosition());
            ent_distance *= fisheye_correction;

            auto normal = ns::normal(ent_play_vec);
            normal.x /= ent_distance; normal.y /= ent_distance;

            sf::Vector2f p1;
            p1.x = ent->getPosition().x - normal.x * ent->getSize().x/2.f;
            p1.y = ent->getPosition().y - normal.y * ent->getSize().x/2.f;
            sf::Vector2f p2;
            p2.x = ent->getPosition().x + normal.x * ent->getSize().x/2.f;
            p2.y = ent->getPosition().y + normal.y * ent->getSize().x/2.f;
            sf::Vector2f p_inter;
            if (lineIntersect(camera_pos2d, test_point, p1, p2, p_inter)) {
                auto& sprite_hit = m_sprite_hits_buffer[spr_hit_index];
                sprite_hit.visible = true;
                sprite_hit.sprite = ent;
                sprite_hit.distance = ent_distance;
                // find the intersection point in percentage of entity width (for texture coordinates)
                // and the first and last rays that see the entity (for world position)
                auto t = ns::distance(p_inter, p1) / ns::distance(p2, p1);
                if (sprite_hit.t_min > t) {
                    sprite_hit.t_min = t;
                    sprite_hit.ray_min = i;
                }
                if (t > sprite_hit.t_max) {
                    sprite_hit.t_max = t;
                    sprite_hit.ray_max = i;
                }
            }
            spr_hit_index++;
        }
    }
}
