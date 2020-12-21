#pragma once

#include <NasNas.h>
#include <array>

class Game : public ns::App {
public:
    Game();
    void onEvent(const sf::Event& event) override;
    void update() override;
    void preRender() override;

    // m_map[y][x]
    char m_map[20][33];
    sf::Vector2i m_map_size;
    float m_max_depth;

    float m_fov;
    sf::Vector2f m_player_pos;
    sf::Vector2f m_player_angle;
    float m_midview_distance;

    sf::RenderTexture m_wall_texture;
    std::vector<sf::Sprite> m_quads;
    ns::VertexArray m_background;

    sf::RectangleShape m_hp_bar;
    sf::RectangleShape m_minimap_bg;

    sf::RenderTexture m_minimap_texture;
    sf::CircleShape m_minimap_player;
    ns::VertexArray m_minimap_rays;
};
