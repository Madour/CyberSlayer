#pragma once

#include <NasNas.h>

#include "GameState.hpp"

class IntroState : public GameState {
public:
    void init() override;

    void onEvent(const sf::Event& event) override;

    void update() override;

private:
    sf::Sprite m_bg_sprite;

    sf::Clock m_clk;
    sf::Time m_cooldown;
};
