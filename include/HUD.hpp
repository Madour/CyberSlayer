#pragma once

#include <NasNas.h>

#include "ent/Player.hpp"



class HUD : public ns::Drawable {
    struct Jauge {
        Jauge();
        void setColor(const sf::Color& color);
        void setSize(const sf::Vector2f& size);
        void setFill(float percent);
        void setPosition(float x, float y);
        sf::ConvexShape bar;
        sf::ConvexShape bg;
    private:
        sf::Vector2f m_size;
        sf::Vector2f m_size_bg;
    };

public:

    explicit HUD(Player* player);

    void update();

    auto getPosition() const -> sf::Vector2f override;
    auto getGlobalBounds() const -> ns::FloatRect override;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    Player* m_player;
    Jauge m_hp_jauge;
    Jauge m_pistol_jauge;
    Jauge m_rifle_jauge;
    sf::Sprite m_sniper_ammo;

};


