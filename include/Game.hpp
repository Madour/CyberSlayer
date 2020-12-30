#pragma once

#include <NasNas.h>
#include <array>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 675

class Game : public ns::App {
public:
    Game();
    void onEvent(const sf::Event& event) override;
    void update() override;
    void preRender() override;

    void doRayCast();

    // m_map[y][x]
    char m_map[20][33];
    sf::Vector2i m_map_size;
    float m_max_depth;

    float m_fov;
    sf::Vector2f m_player_pos;
    sf::Vector2f m_player_angle;
    std::array<float, WINDOW_WIDTH> m_depth_buffer;

    sf::RenderTexture m_wall_texture;
    std::vector<sf::Sprite> m_quads;
    ns::VertexArray m_background;

    sf::RectangleShape m_hp_bar;
    sf::RectangleShape m_minimap_bg;

    sf::RenderTexture m_minimap_texture;
    sf::CircleShape m_minimap_player;
    ns::VertexArray m_minimap_rays;
    ns::VertexArray m_minimap_grid;
};
