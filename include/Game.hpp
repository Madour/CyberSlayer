#pragma once

#include <array>
#include <NasNas.h>

#include "LevelObject.hpp"
#include "Entity.hpp"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 675

class Game : public ns::App {
    enum class Side { Top, Right, Bottom, Left };
    struct WallHit {
        float distance{};
        sf::Vector2f point;
        Side side{};
        float tex_coo{};
        unsigned tile_gid{};
    };
    struct SpriteHit{
        LevelObject* sprite;
        float distance{};
        float t_min{};
        float t_max{};
        unsigned ray_min{};
        unsigned ray_max{};
        bool visible;
    };
public:
    Game();
    void onEvent(const sf::Event& event) override;
    void update() override;
    void preRender() override;

    void doRayCast();

    ns::tm::TiledMap m_level_map;
    float m_tile_size;
    sf::Vector2u m_level_size;
    float m_max_depth;

    SpriteHit m_enemy;

    float m_fov;
    sf::Vector2f m_player_pos;
    float m_player_pos_z;
    float m_z_vel;
    sf::Vector2f m_player_angle;

    std::array<WallHit, WINDOW_WIDTH> m_wall_hits_buffer;
    std::unordered_map<LevelObject*, SpriteHit> m_sprite_hits_buffer;

    std::vector<std::unique_ptr<LevelObject>> m_level_objects;

    sf::RenderTexture m_wall_texture;
    ns::VertexArray m_walls_quads;
    ns::VertexArray m_sprites_quads;
    ns::VertexArray m_background;

    sf::RectangleShape m_hp_bar;
    sf::RectangleShape m_minimap_bg;

    sf::CircleShape m_minimap_player;
    ns::VertexArray m_minimap_enemies;
    ns::VertexArray m_minimap_rays;
    ns::VertexArray m_minimap_grid;
};
