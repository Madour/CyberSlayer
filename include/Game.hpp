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
    std::array<std::string, 20> m_map;
    float m_max_depth;

    float m_fov;
    sf::Vector2f m_player_pos;
    sf::Vector2f m_player_angle;

    sf::RenderTexture m_white_texture;
    std::vector<sf::Sprite> m_quads;

    sf::RectangleShape m_hp_bar;
    sf::RectangleShape m_minimap_bg;

    sf::RenderTexture m_minimap_texture;
    ns::BaseEntity m_minimap_player;
    ns::BaseEntity m_minimap_rays;
};
