#pragma once

#include <NasNas.h>
#include <array>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 675

class Game : public ns::App {
    enum class Side { Top, Right, Bottom, Left };
    struct RayHit {
        float distance;
        sf::Vector2f point;
        Side side;
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

    float m_fov;
    sf::Vector2f m_player_pos;
    sf::Vector2f m_player_angle;
    std::array<RayHit, WINDOW_WIDTH> m_ray_intersection_buffer;

    sf::RenderTexture m_wall_texture;
    std::vector<sf::Sprite> m_quads;
    ns::VertexArray m_background;

    sf::RectangleShape m_hp_bar;
    sf::RectangleShape m_minimap_bg;

    sf::CircleShape m_minimap_player;
    ns::VertexArray m_minimap_rays;
    ns::VertexArray m_minimap_grid;
};
