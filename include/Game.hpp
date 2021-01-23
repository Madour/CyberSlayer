#pragma once

#include <NasNas.h>

#include "GameState.hpp"
#include "Constants.hpp"


class Game : public ns::App {
public:
    Game();
    void onEvent(const sf::Event& event) override;
    void update() override;
    void preRender() override;

    template<typename NewState>
    void setState();

    GameState* m_state;

};

template <typename NewState>
void Game::setState() {
    delete(m_state);
    m_state = new NewState();
    m_state->init();
}
