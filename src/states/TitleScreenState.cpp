#include "states/TitleScreenState.hpp"
#include "states/LevelState.hpp"
#include "states/IntroState.hpp"
#include "transitions/FadeTransitions.hpp"
#include "transitions/DoorTransition.hpp"
#include "Game.hpp"

void TitleScreenState::init() {
    game->getWindow().setMouseCursorVisible(true);

    (new FadeIn())->start();

    auto& bg_texture = ns::Res::getTexture("titlescreen_bg.png");
    float scale_ratio = static_cast<float>(VIEW_HEIGHT)/bg_texture.getSize().y;
    m_bg_sprite1.setTexture(bg_texture);
    m_bg_sprite1.setScale(scale_ratio, scale_ratio);
    m_bg_sprite1.setColor({200, 200, 200});

    m_bg_sprite2.setTexture(bg_texture);
    m_bg_sprite2.setScale(-scale_ratio, scale_ratio);
    m_bg_sprite2.setPosition(2*VIEW_WIDTH, 0);
    m_bg_sprite2.setColor({200, 200, 200});

    m_title_text.setString("Cyber Slayer");
    m_title_text.setFont(ns::Res::in("fonts").getFont("True_Lies.ttf"));
    m_title_text.setCharacterSize(100);
    m_title_text.setStyle(sf::Text::Style::Bold | sf::Text::Style::Underlined);
    m_title_text.setOutlineThickness(2.f);
    m_title_text.setOutlineColor(sf::Color(100, 20, 255, 0));
    m_title_text.setFillColor({255, 255, 255, 0});
    sf::Vector2f text_size{m_title_text.getGlobalBounds().width, m_title_text.getGlobalBounds().height};
    m_title_text.setPosition((VIEW_WIDTH - text_size.x) / 2, (VIEW_HEIGHT - text_size.y) / 2);

    scale_ratio = static_cast<float>(VIEW_WIDTH-100)/ns::Res::getTexture("title.png").getSize().x;
    m_title_sprite.setTexture(ns::Res::getTexture("title.png"));
    m_title_sprite.setColor({255, 255, 255, 0});
    m_title_sprite.setScale(scale_ratio, scale_ratio);
    text_size = {m_title_sprite.getGlobalBounds().width, m_title_sprite.getGlobalBounds().height};
    m_title_sprite.setPosition((VIEW_WIDTH - text_size.x) / 2, (VIEW_HEIGHT - text_size.y) / 2);

    m_play_text.setString("PLAY");
    m_play_text.setFont(ns::Res::in("fonts").getFont("Cyberpunk.ttf"));
    m_play_text.setCharacterSize(50);
    m_play_text.setStyle(sf::Text::Style::Bold);
    m_play_text.setOutlineThickness(3.f);
    m_play_text.setOutlineColor(sf::Color(100, 50, 100, 0));
    m_play_text.setFillColor({255, 255, 255, 0});
    text_size = {m_play_text.getGlobalBounds().width, m_play_text.getGlobalBounds().height};
    m_play_text.setPosition((VIEW_WIDTH - text_size.x)/2, (VIEW_HEIGHT - text_size.y)/2 + 50);

    m_alpha_text.setString("ALPHA  VERSION - doesnt  represent  the  final  look  of  the  game");
    m_alpha_text.setFont(ns::Res::in("fonts").getFont("True_Lies.ttf"));
    m_alpha_text.setCharacterSize(20);
    m_alpha_text.setStyle(sf::Text::Style::Bold);
    m_alpha_text.setOutlineThickness(3.f);
    m_alpha_text.setOutlineColor(sf::Color(100, 50, 100, 0));
    m_alpha_text.setFillColor({255, 255, 255, 0});
    text_size = {m_alpha_text.getGlobalBounds().width, m_alpha_text.getGlobalBounds().height};
    m_alpha_text.setPosition((VIEW_WIDTH - text_size.x)/2, (VIEW_HEIGHT - text_size.y)/2 + 200);

    auto* bg_scene = game->getScene("bg");
    bg_scene->getDefaultLayer()->clear();

    bg_scene->getDefaultLayer()->addRaw(&m_bg_sprite1);
    bg_scene->getDefaultLayer()->addRaw(&m_bg_sprite2);
    bg_scene->getDefaultLayer()->addRaw(&m_title_sprite);
    bg_scene->getDefaultLayer()->addRaw(&m_play_text);
    bg_scene->getDefaultLayer()->addRaw(&m_alpha_text);


    game->menu_music.play();
}

void TitleScreenState::onEvent(const sf::Event& event) {
    auto mouse_pos = game->getWindow().mapPixelToCoords(sf::Mouse::getPosition(game->getWindow()), game->getWindow().getAppView());
    if (event.type == sf::Event::MouseButtonPressed) {
        if (m_play_text.getFillColor().a == 255) {
            if (m_play_text.getGlobalBounds().contains(mouse_pos)) {
                if (ns::Transition::list.empty()) {
                    auto* tr_out = new FadeOut();
                    tr_out->setOnEndCallback([&]{
                        game->menu_music.stop();
                        game->setState<IntroState>();
                    });
                    tr_out->start();
                }
            }
        }
    }
}

void TitleScreenState::update() {
    m_bg_sprite1.move(-1, 0);
    m_bg_sprite2.move(-1, 0);

    if(m_bg_sprite1.getPosition().x <= -VIEW_WIDTH)
        m_bg_sprite1.setPosition(VIEW_WIDTH, 0);
    if (m_bg_sprite2.getPosition().x <= 0)
        m_bg_sprite2.setPosition(2*VIEW_WIDTH, 0);

    if (ns::Transition::list.empty()) {
        if (m_title_sprite.getColor().a != 255) {
            m_title_sprite.setColor(m_title_sprite.getColor() + sf::Color(0, 0, 0, 2));
        }
        else {
            if (m_title_sprite.getPosition().y > 0.05f) {
                m_title_sprite.move(0, - m_title_sprite.getPosition().y / 20);
            }
            else {
                if (m_play_text.getFillColor().a != 255) {
                    m_play_text.setFillColor(m_play_text.getFillColor() + sf::Color(0, 0, 0, 10));
                    m_play_text.setOutlineColor(m_play_text.getOutlineColor() + sf::Color(0, 0, 0, 5));
                    m_alpha_text.setFillColor(m_play_text.getFillColor() + sf::Color(0, 0, 0, 10));
                    m_alpha_text.setOutlineColor(m_play_text.getOutlineColor() + sf::Color(0, 0, 0, 5));
                }
            }
        }
    }
}
