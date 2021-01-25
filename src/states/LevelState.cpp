#include <cstring>

#include "states/LevelState.hpp"
#include "ent/Adventurer.hpp"
#include "Game.hpp"
#include "Utils.hpp"

LevelState::~LevelState() {
    delete(m_tile_texture_rect);
    delete(m_framebuffer);
    delete(m_player);
}

void LevelState::init() {
    game->getWindow().setMouseCursorVisible(false);

    auto& appview_size = game->getWindow().getAppView().getSize();

    m_level.load("assets/level1.tmx");
    auto& tile_map = m_level.getTileMap();

    auto& tileset = tile_map.allTilesets()[0];
    m_tileset_image = tileset->getTexture().copyToImage();
    m_tileset_pixels = m_tileset_image.getPixelsPtr();
    m_tileset_size = tileset->getTexture().getSize();
    m_tile_texture_rect = new ns::FloatRect[tileset->tilecount];
    for (unsigned i = 0; i < tileset->tilecount; ++i) {
        m_tile_texture_rect[i] = tileset->getTileTextureRect(i);
    }

    m_framebuffer = new sf::Uint8[VIEW_WIDTH * VIEW_HEIGHT * 4];
    memset(m_framebuffer, 0, VIEW_WIDTH * VIEW_HEIGHT * 4 * sizeof(sf::Uint8));
    m_framebuffer_texture.create(VIEW_WIDTH, VIEW_HEIGHT);

    m_tile_size = static_cast<float>(tile_map.getTileSize().x);
    m_level_size.x = static_cast<int>(tile_map.getDimension().x);
    m_level_size.y = static_cast<int>(tile_map.getDimension().y);
    // max depth is the length of map's diagonal
    m_max_depth = float(std::hypot(m_level_size.x, m_level_size.y)) * 1.5f;

    ns_LOG("AppView Size :", appview_size);
    ns_LOG("Level size :", m_level_size, " Tile size :", m_tile_size);
    ns_LOG("Ray cast max depth :", m_max_depth);

    m_player = new Player();
    m_player->transform()->setPosition(6.5f, 48.5f);
    auto* pistol = WeaponFactory::createFromName("Pistol");
    auto* rifle = WeaponFactory::createFromName("Rifle");
    auto* sniper = WeaponFactory::createFromName("Sniper");
    auto* melee = WeaponFactory::createFromName("Melee");
    m_player->addWeapon(pistol);
    m_player->addWeapon(rifle);
    m_player->addWeapon(sniper);
    m_player->addWeapon(melee);
    m_level_objects.push_back(m_player);

    m_horizon = VIEW_HEIGHT;

    // add level enemies to level objects list
    for (auto* enemy : Level::getEnemies())
        m_level_objects.push_back(enemy);


    // add level items to level objects list
    for (auto* item : Level::getItems())
        m_level_objects.push_back(item);

    // resize the sprite hit buffer used by the ray caster
    m_sprite_hits_buffer.resize(m_level_objects.size());

    ///////////////////////////////////////////////////////
    // Main scene drawables
    m_billboards = new ns::SpriteBatch("billboard");
    m_billboards->start(sf::VertexBuffer::Stream);

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

    m_minimap_bg.setSize({203, 203});
    m_minimap_bg.setFillColor(sf::Color(155, 155, 155));
    m_minimap_bg.setPosition(appview_size.x - 203, 0);
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
        m_minimap_rays.append({ {0, 0}, sf::Color(0, 255, 0, 30) });
    for (unsigned i = m_minimap_rays.getVertexCount()/2-50; i < m_minimap_rays.getVertexCount()/2+50; ++i)
        m_minimap_rays[i].color = sf::Color(255, 0, 0, 30);

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
    // hide bg scene
    game->getCamera("bg")->setVisible(false);
    // main scene
    auto* main_scene = game->getScene("main");
    main_scene->getDefaultLayer()->clear();

    main_scene->getDefaultLayer()->addRaw(&m_background);
    main_scene->getDefaultLayer()->addRaw(&m_framebuffer_sprite);
    main_scene->getDefaultLayer()->add(m_billboards);

    // HUD scene
    auto* hud_scene = game->getScene("hud");
    hud_scene->getDefaultLayer()->clear();

    hud_scene->getDefaultLayer()->addRaw(&m_minimap_bg);
    hud_scene->getDefaultLayer()->add(help_text);
    hud_scene->getDefaultLayer()->add(pistol);
    hud_scene->getDefaultLayer()->add(sniper);
    hud_scene->getDefaultLayer()->add(rifle);
    hud_scene->getDefaultLayer()->add(melee);

    // minimap scene
    auto* minimap_scene = game->getScene("minimap");
    minimap_scene->getDefaultLayer()->clear();

    minimap_scene->getDefaultLayer()->add(tile_map.getTileLayer("ground"));
    minimap_scene->getDefaultLayer()->add(tile_map.getTileLayer("walls"));
    minimap_scene->getDefaultLayer()->addRaw(&m_minimap_rays);
    minimap_scene->getDefaultLayer()->addRaw(&m_minimap_grid);
    minimap_scene->getDefaultLayer()->addRaw(&m_minimap_entities);
    minimap_scene->getDefaultLayer()->addRaw(&m_minimap_bg);
    ///////////////////////////////////////////////////////////
}

void LevelState::onEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseWheelMoved) {
        if (event.mouseWheel.delta > 0)
            m_player->selectNextWeapon();
        else
            m_player->selectPrevWeapon();
    }
    if (game->getWindow().hasFocus())
        m_camera.onEvent(event);
}

void LevelState::update() {
    if (game->getWindow().hasFocus())
        sf::Mouse::setPosition(sf::Vector2i(game->getWindow().getAppView().getSize()/2.f), game->getWindow());

    // update all entities
    for (auto& ent : m_level_objects) {
        ent->update();
        ent->computeDistanceToCamera(m_camera.getPosition2D());
    }

    // check if player can collect an item
    for (unsigned i = 0; i < Level::getItems().size(); ++i) {
        if (ns::distance(Level::getItems()[i]->getPosition(), m_player->getPosition()) < 0.3) {
            Level::getItems()[i]->onCollect(*m_player);
            for (auto it = m_level_objects.begin(); it != m_level_objects.end(); it++) {
                if (*it == Level::getItems()[i]) {
                    m_level_objects.erase(it);
                    break;
                }
            }
            Level::getItems().erase(Level::getItems().begin()+i--);
        }
    }

    if (m_player->getActiveWeapon()->isAttacking()) {
        ns_LOG("Player shoot");
        // check hit with enemies here
    }

    //update camera
    m_camera.update(m_player);
    const auto& camera_pos2d = m_camera.getPosition2D();
    for (unsigned i = 0; i < m_level_objects.size(); ++i) {
        const auto& ent = m_level_objects[i];
        auto normal = ns::normal(camera_pos2d - ent->getPosition()) / ns::distance(camera_pos2d, ent->getPosition()) * ent->getSize().x;
        m_minimap_entities[i * 2].position = (ent->getPosition() - normal / 2.f) * m_tile_size;
        m_minimap_entities[i * 2 + 1].position = (ent->getPosition() + normal / 2.f) * m_tile_size;
    }

    game->getCamera("minimap")->setCenter(camera_pos2d*m_tile_size);
    game->getCamera("minimap")->setRotation(ns::to_degree(m_camera.getYaw())+ 90);

    m_camera.setFovRad(m_camera.getBaseFovRad()/ m_player->getActiveWeapon()->getFovZoom());

    if (m_player->isRunning()) {
        m_camera.setFovRad(m_camera.getBaseFovRad()*1.1f);
    }
}

void LevelState::preRender() {
    auto firstgid = m_level.getTileMap().allTilesets()[0]->firstgid;

    auto& app_view_size = game->getWindow().getAppView().getSize();
    m_horizon = app_view_size.y * (0.5f - ns::to_degree(m_camera.getPitch()) / 45.f);
    // adjust background horizon line
    m_background[2].position.y = m_horizon;
    m_background[3].position.y = m_horizon;
    m_background[4].position.y = m_horizon;
    m_background[5].position.y = m_horizon;

    auto cam_angle = m_camera.getYaw();
    const auto& cam_pos2d = m_camera.getPosition2D();
    auto proj_plane_dist = m_camera.getProjectionPlaneDistance();

    doRayCast();

    memset(m_framebuffer, 0, VIEW_WIDTH * VIEW_HEIGHT * 4 * sizeof(sf::Uint8));
    m_billboards->clear();
    for (unsigned i = 0; i < m_wall_hits_buffer.size(); ++i) {
        bool first = true;
        while (!m_wall_hits_buffer[i].empty()) {
            const auto& wall_hit = m_wall_hits_buffer[i].top();
            auto ratio = proj_plane_dist / wall_hit.distance; // thales
            float ceiling = m_horizon - (WALL_HEIGHT + m_camera.getPosition3D().z) * ratio - 1;
            float ground = m_horizon +  (0 - m_camera.getPosition3D().z) * ratio + 1;
            int index;
            bool pr = true;
            for (int y = (int)ceiling; y < ground; y += 1) {
                if ( y < 0 || y >= VIEW_HEIGHT)
                    continue;
                auto tex_x = static_cast<int>(wall_hit.tex_coo);
                auto tex_y = static_cast<int>(m_tile_texture_rect[wall_hit.tile_gid-1].top + (y-ceiling) / (ground-ceiling)*m_tile_size);
                auto texture_pix = m_tileset_pixels+(tex_x + tex_y*m_tileset_size.x)*4;
                index = (y*VIEW_WIDTH + i) * 4;
                if(texture_pix[3] > 0) {
                    // global light comes from top/north
                    float color_mult = 1.f;
                    if (wall_hit.side == Side::Bottom)
                        color_mult += 0.2f;
                    else if (wall_hit.side != Side::Top)
                        color_mult += 0.1f;
                    color_mult += wall_hit.distance*3/m_max_depth;
                    m_framebuffer[index + 0] = static_cast<sf::Uint8>((float)texture_pix[0] / (color_mult));
                    m_framebuffer[index + 1] = static_cast<sf::Uint8>((float)texture_pix[1] / (color_mult));
                    m_framebuffer[index + 2] = static_cast<sf::Uint8>((float)texture_pix[2] / (color_mult));
                    m_framebuffer[index + 3] = texture_pix[3];
                }
            }

            if (first) {
                first = false;
                float grnd_ceil_dist;
                sf::Vector2f pos;
                sf::Vector2i pos_i;
                int gid;
                sf::Vector2f uv;
                sf::Vector2i tex_pos;
                const sf::Uint8* texture_pix;
                float ceil_ratio = (-m_camera.getPosition3D().z - WALL_HEIGHT) * proj_plane_dist / wall_hit.fisheye_correction;
                float ground_ratio = -m_camera.getPosition3D().z * proj_plane_dist / wall_hit.fisheye_correction;
                for (int y = 0; y < VIEW_HEIGHT; y+=1) {
                    if ( y < ceiling) {
                        grnd_ceil_dist = ceil_ratio / (y - m_horizon);
                        pos.x = cam_pos2d.x + wall_hit.ray_dir.x * grnd_ceil_dist;
                        pos.y = cam_pos2d.y + wall_hit.ray_dir.y * grnd_ceil_dist;
                        if (pos.x < 0 || pos.y < 0)
                            continue;
                        pos_i.x = (int)pos.x;
                        pos_i.y = (int)pos.y;
                        gid = m_level[LevelLayer::Ceiling](pos_i.x, pos_i.y);
                        if (gid == 0)
                            continue;
                        auto& tex_rect = m_tile_texture_rect[gid - firstgid];
                        uv.x = pos.x - pos_i.x;
                        uv.y = pos.y - pos_i.y;
                        tex_pos.x = static_cast<int>(tex_rect.left + m_tile_size * uv.x);
                        tex_pos.y = static_cast<int>(tex_rect.top + m_tile_size * uv.y);
                        texture_pix = m_tileset_pixels+(tex_pos.x + tex_pos.y*m_tileset_size.x)*4;
                        index = (y*VIEW_WIDTH + i) * 4;
                        float color_mult = std::max(1.2f, grnd_ceil_dist/6);
                        m_framebuffer[index + 0] = static_cast<sf::Uint8>((float)texture_pix[0] / color_mult);
                        m_framebuffer[index + 1] = static_cast<sf::Uint8>((float)texture_pix[1] / color_mult);
                        m_framebuffer[index + 2] = static_cast<sf::Uint8>((float)texture_pix[2] / color_mult);
                        m_framebuffer[index + 3] = texture_pix[3];
                    }
                    else if (ceiling < y && y < ground) {
                        y = static_cast<int>(ground);
                    }
                    else if (y >= ground) {
                        grnd_ceil_dist = ground_ratio / (y - m_horizon);
                        pos.x = cam_pos2d.x + wall_hit.ray_dir.x * grnd_ceil_dist;
                        pos.y = cam_pos2d.y + wall_hit.ray_dir.y * grnd_ceil_dist;
                        if (pos.x < 0 || pos.y < 0)
                            continue;
                        pos_i.x = (int)pos.x;
                        pos_i.y = (int)pos.y;
                        gid = m_level[LevelLayer::Ground](pos_i.x, pos_i.y);
                        if (gid == 0)
                            continue;
                        auto& tex_rect = m_tile_texture_rect[gid - firstgid];
                        uv.x = pos.x - pos_i.x;
                        uv.y = pos.y - pos_i.y;
                        tex_pos.x = static_cast<int>(tex_rect.left + m_tile_size * uv.x);
                        tex_pos.y = static_cast<int>(tex_rect.top + m_tile_size * uv.y);
                        texture_pix = m_tileset_pixels+(tex_pos.x + tex_pos.y*m_tileset_size.x)*4;
                        index = (y*VIEW_WIDTH + i) * 4;
                        float color_mult = std::max(1.2f, grnd_ceil_dist/6);
                        m_framebuffer[index + 0] = static_cast<sf::Uint8>((float)texture_pix[0] / color_mult);
                        m_framebuffer[index + 1] = static_cast<sf::Uint8>((float)texture_pix[1] / color_mult);
                        m_framebuffer[index + 2] = static_cast<sf::Uint8>((float)texture_pix[2] / color_mult);
                        m_framebuffer[index + 3] = texture_pix[3];
                    }
                }
                // update minimap rays
                m_minimap_rays[2*i].position = m_camera.getPosition2D()*m_tile_size;
                m_minimap_rays[2*i + 1].position = wall_hit.point * m_tile_size;
            }
            m_wall_hits_buffer[i].pop();
        }
    }

    sf::Transformable tr;
    sf::IntRect tex_rect;
    for (auto& sprite_hit : m_sprite_hits_buffer) {
        auto ent = sprite_hit.sprite;
        if (sprite_hit.visible) {
            auto ratio = proj_plane_dist / sprite_hit.distance; // thales
            auto& ent_size = ent->getSize();
            const auto& ent_tex_rect = ent->getTextureRect();

            tex_rect.left = (int)(ent_tex_rect.left + sprite_hit.t_min*ent_tex_rect.width);
            tex_rect.top = ent_tex_rect.top;
            tex_rect.width = (int)((sprite_hit.t_max - sprite_hit.t_min)*ent_tex_rect.width);
            tex_rect.height = ent_tex_rect.height;

            float scalex = float(sprite_hit.ray_max - sprite_hit.ray_min)/tex_rect.width;
            float scaley = float(ent_size.y*ratio)/tex_rect.height;
            tr.setScale(scalex, scaley);
            tr.setPosition((float)sprite_hit.ray_min, m_horizon - (ent_size.y + ent->getZ() + m_camera.getPosition3D().z)*ratio);
            m_billboards->draw(&ent->getTexture(), tex_rect, tr);
        }
        sprite_hit.visible = false;
        sprite_hit.t_min = 1.f;
        sprite_hit.t_max = 0.f;
        sprite_hit.ray_min = VIEW_WIDTH;
        sprite_hit.ray_max = 0;
    }
    // end billboards spritebatch
    m_billboards->end();
    // push the framebuffer to the texture
    m_framebuffer_texture.update(m_framebuffer);
    m_framebuffer_sprite.setTexture(m_framebuffer_texture);
}


void LevelState::doRayCast() {
    auto& app_view_size = game->getWindow().getAppView().getSize();

    auto& ceil_tile_layer = m_level.getTileMap().getTileLayer("ceiling");
    auto& walls_tile_layer = m_level.getTileMap().getTileLayer("walls");
    auto& ground_tile_layer = m_level.getTileMap().getTileLayer("ground");

    int firstgid = m_level.getTileMap().allTilesets()[0]->firstgid;

    const auto& camera_pos2d = m_camera.getPosition2D();

    auto cam_angle = m_camera.getYaw();
    auto proj_plane_dist = m_camera.getProjectionPlaneDistance();

    // sort level objects by their distance to player, to get correct depth order
    std::sort(m_level_objects.begin(), m_level_objects.end(), [&](auto& lhs, auto& rhs){
        return lhs->getDistanceToCamera() > rhs->getDistanceToCamera();
    });

    for (int i = 0; i < VIEW_WIDTH; ++i) {

        float ray_angle_rad = std::atan((float(i)-app_view_size.x/2) / proj_plane_dist) + cam_angle;
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
                horizontal_test.y = std::floor(horizontal_test.y)-0.0001f;
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
                vertical_test.x = std::floor(vertical_test.x)-0.0001f;
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
        bool update_depth = true;
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
                if (m_level[LevelLayer::Walls](test_point_int.x, test_point_int.y) != 0) {
                    hit = true;
                    distance = ns::distance(camera_pos2d, test_point);

                    // fish eye correction
                    auto fisheye_correction = std::cos(ray_angle_rad-cam_angle);
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
                        tile_gid = m_level[LevelLayer::Walls](test_point_int.x, test_point_int.y);
                        tex_coo = m_tile_texture_rect[tile_gid-firstgid].left + m_tile_size * u;
                    }
                    else {
                        tile_gid = 0;
                        tex_coo = 0.f;
                    }
                    // fill depth buffer
                    if (update_depth)
                        m_depth_buffer[i] = distance;
                    // fill all the information
                    WallHit wall_hit;
                    wall_hit.distance = distance;
                    wall_hit.ray_dir = ray_dir;
                    wall_hit.fisheye_correction = fisheye_correction;
                    wall_hit.point = test_point;
                    wall_hit.side = side;
                    wall_hit.tex_coo = tex_coo;
                    wall_hit.tile_gid = tile_gid;
                    m_wall_hits_buffer[i].push(wall_hit);

                    // transparent tiles
                    if ( tile_gid == 28) {
                        hit = false;
                    }
                }
            }
        }
    }

    sf::Vector2f cam_dir{std::cos(cam_angle), std::sin(cam_angle)};
    int spr_hit_index = 0;
    for (auto* ent : m_level_objects) {
        if (ent == m_player)
            continue;

        // vector from camera pos to entity pos
        sf::Vector2f cam_ent_vec;
        cam_ent_vec.x = ent->getPosition().x - m_camera.getPosition2D().x;
        cam_ent_vec.y = ent->getPosition().y - m_camera.getPosition2D().y;
        auto ent_distance = ent->getDistanceToCamera();
        auto normal = ns::normal(cam_ent_vec)/ns::norm(cam_ent_vec);

        sf::Vector2f ent_left;
        ent_left.x = ent->getPosition().x - normal.x * ent->getSize().x/2.f;
        ent_left.y = ent->getPosition().y - normal.y * ent->getSize().x/2.f;
        auto ent_left_angle = ns::angle(cam_dir, ent_left-m_camera.getPosition2D());
        sf::Vector2f ent_right;
        ent_right.x = ent->getPosition().x + normal.x * ent->getSize().x/2.f;
        ent_right.y = ent->getPosition().y + normal.y * ent->getSize().x/2.f;
        auto ent_right_angle = ns::angle(cam_dir, ent_right-m_camera.getPosition2D());

        if (ent_right_angle < -m_camera.getFovRad()/2.f || m_camera.getFovRad()/2.f < ent_left_angle) {
            continue;
        }

        sf::Vector2f p_inter;

        int i_min = static_cast<int>(std::tan(ent_left_angle) * proj_plane_dist) + VIEW_WIDTH/2;
        if (i_min < 0) i_min = 0;
        if (i_min > VIEW_WIDTH-1) i_min = VIEW_WIDTH-1;
        int i_max = static_cast<int>(std::tan(ent_right_angle) * proj_plane_dist) + VIEW_WIDTH/2;
        if (i_max < 0) i_max = 0;
        if (i_max > VIEW_WIDTH-1) i_max = VIEW_WIDTH-1;

        for (int i = i_min; i < i_max; ++i) {
            float ray_angle_rad = std::atan((float(i)-app_view_size.x/2) / proj_plane_dist) + cam_angle;
            auto ray_cos = std::cos(ray_angle_rad);
            auto ray_sin = std::sin(ray_angle_rad);
            sf::Vector2f ray_dir{ray_cos, ray_sin};
            sf::Vector2f test_point{m_camera.getPosition2D() + ray_dir*m_depth_buffer[i]/std::cos(ray_angle_rad - cam_angle)};
            if (segmentIntersection(camera_pos2d, test_point, ent_left, ent_right, p_inter)) {
                auto& sprite_hit = m_sprite_hits_buffer[spr_hit_index];
                sprite_hit.visible = true;
                sprite_hit.sprite = ent;
                sprite_hit.distance = ent_distance * std::cos(ray_angle_rad - cam_angle);
                // find the intersection point in percentage of entity width (for texture coordinates)
                // and the first and last rays that see the entity (for world position)
                auto t = ns::distance(p_inter, ent_left) / ns::distance(ent_left, ent_right);
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
        spr_hit_index++;
    }
}
