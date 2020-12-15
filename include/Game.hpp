#pragma once

#include <NasNas.h>
#include <array>

class Game : public ns::App {
public:
    Game();
    void onEvent(const sf::Event& event) override;
    void update() override;
    void preRender() override;

    std::vector<std::unique_ptr<sf::Sprite>> m_sprites;

    // m_map[y][x]
    std::array<std::string, 10> m_map;
    float m_max_depth;

    float m_fov = ns::to_radian(50.f);

    sf::Vector2f m_player_pos;
    sf::Vector2f m_player_angle;

    sf::RenderTexture m_white_texture;

    std::vector<sf::Sprite> m_quads;

    sf::RectangleShape m_hp_bar;
};
