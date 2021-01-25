#pragma once

#include <NasNas.h>

#include "GameState.hpp"
#include "Constants.hpp"


class Game final : public ns::App {
public:
    Game();
    ~Game() override;

    void onEvent(const sf::Event& event) override;
    void update() override;
    void preRender() override;

    template<typename NewState>
    void setState();

    GameState* m_state = nullptr;

    sf::Music m_menu_music;
    sf::Music m_level_music;

};

template <typename NewState>
void Game::setState() {
    if (m_state != nullptr)
        delete(m_state);
    m_state = new NewState();
    m_state->init();
}
