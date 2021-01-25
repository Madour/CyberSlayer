#include "states/TitleScreenState.hpp"
#include "states/LevelState.hpp"
#include "states/IntroState.hpp"
#include "transitions/FadeTransitions.hpp"
#include "transitions/DoorTransition.hpp"
#include "Game.hpp"

void IntroState::init() {
    game->getWindow().setMouseCursorVisible(false);

    auto& bg_texture = ns::Res::getTexture("intro_text_bg.png");
    m_bg_sprite.setTexture(bg_texture);
    m_bg_sprite.setPosition(0,0);

    auto* bg_scene = game->getScene("bg");
    bg_scene->getDefaultLayer()->clear();

    bg_scene->getDefaultLayer()->addRaw(&m_bg_sprite);
    m_cooldown = sf::seconds(19);
    m_clk.restart();

    game->m_level_music.openFromFile("assets/level_music.ogg");
    game->m_level_music.play();
}

void IntroState::onEvent(const sf::Event& event) {
    
}

void IntroState::update() {
    if (m_clk.getElapsedTime() >= m_cooldown) {
        if (ns::Transition::list.empty()) {
            auto* tr = new DoorOpenTransition();
            tr->start();
            game->setState<LevelState>();
        }
    }
}
