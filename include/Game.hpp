#pragma once

#include <NasNas.h>

#include "GameState.hpp"
#include "Constants.hpp"
#include "Weapon.hpp"


class Game : public ns::App {
public:
    Game();
    ~Game() override;
    void onEvent(const sf::Event& event) override;
    void update() override;
    void preRender() override;

    template<typename NewState>
    void setState();

    GameState* m_state;

    // Weapon
    int m_weapon_selector;
    int m_number_weapon;

    Weapon* m_current_weapon;
    Pistol m_laser_pistol;
    Rifle m_laser_rifle;
    Sniper m_sniper;
    Melee m_melee;
    sf::Sprite m_gun_sprite;
};

template <typename NewState>
void Game::setState() {
    delete(m_state);
    m_state = new NewState();
    m_state->init();
}
