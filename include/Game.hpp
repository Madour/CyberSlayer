#pragma once

#include <array>
#include <NasNas.h>

#include "Constants.hpp"
#include "LevelObject.hpp"
#include "ent/Adventurer.hpp"
#include "ent/Player.hpp"


class Game : public ns::App {
private:
    // private structs used by the ray caster
    enum class Side { Top, Right, Bottom, Left };
    struct WallHit {
        float distance=0.f;
        sf::Vector2f point;
        Side side=Side::Top;
        float tex_coo=0.f;
        unsigned tile_gid=0;
    };
    struct SpriteHit{
        LevelObject* sprite=nullptr;
        float distance=0.f;
        float t_min=1.f;
        float t_max=0.f;
        unsigned ray_min=VIEW_WIDTH;
        unsigned ray_max=0;
        bool visible=false;
    };
public:
    Game();
    void onEvent(const sf::Event& event) override;
    void update() override;
    void preRender() override;

    void doRayCast();

    // level data
    ns::tm::TiledMap m_level_map;
    float m_tile_size;
    sf::Vector2u m_level_size;

    Player* m_player;
    std::vector<std::unique_ptr<LevelObject>> m_level_objects;

    // camera data
    sf::Vector3f m_camera_pos;
    sf::Vector3f m_camera_rot;
    float m_horizon;
    float m_projection_plane_distance;

    // ray caster data
    float m_fov;
    float m_max_depth;
    std::array<WallHit, VIEW_WIDTH> m_wall_hits_buffer;
    std::vector<SpriteHit> m_sprite_hits_buffer;

    // ray caster drawables
    ns::VertexArray m_background;
    ns::VertexArray m_walls_quads;
    ns::VertexArray m_sprites_quads;
    ns::VertexArray m_floor_ceil_casting;

    // HUD drawables
    sf::RectangleShape m_hp_bar;
    sf::RectangleShape m_minimap_bg;

    // minimap drawables
    ns::VertexArray m_minimap_entities;
    ns::VertexArray m_minimap_rays;
    ns::VertexArray m_minimap_grid;
};
