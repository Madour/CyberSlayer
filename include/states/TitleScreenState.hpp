#pragma once

#include <NasNas.h>

#include "GameState.hpp"

class TitleScreenState : public GameState {
public:
    void init() override;

    void onEvent(const sf::Event& event) override;

    void update() override;

private:
    sf::Sprite m_bg_sprite1;
    sf::Sprite m_bg_sprite2;

    sf::Text m_title_text;
    sf::Sprite m_title_sprite;
    sf::Text m_play_text;
};

