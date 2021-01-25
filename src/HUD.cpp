#include "HUD.hpp"
#include "Weapon.hpp"

HUD::Jauge::Jauge() {
    bar.setPointCount(8);
    bg.setPointCount(8);
}

void HUD::Jauge::setSize(const sf::Vector2f& size) {
    m_size = size;

    bar.setPoint(0, {10, m_size.y});
    bar.setPoint(1, {0,  2*m_size.y/3});
    bar.setPoint(2, {0,  m_size.y/3});
    bar.setPoint(3, {10, 0});
    bar.setPoint(4, {m_size.x-10, 0});
    bar.setPoint(5, {m_size.x,    m_size.y/3});
    bar.setPoint(6, {m_size.x,    2*m_size.y/3});
    bar.setPoint(7, {m_size.x-10, m_size.y});

    sf::Vector2f d{15, 6};
    m_size_bg = size+d;
    bg.setPoint(0, {10, m_size_bg.y});
    bg.setPoint(1, {0,  2*m_size_bg.y/3});
    bg.setPoint(2, {0,  m_size_bg.y/3});
    bg.setPoint(3, {10, 0});
    bg.setPoint(4, {m_size_bg.x-10, 0});
    bg.setPoint(5, {m_size_bg.x,    m_size_bg.y/3});
    bg.setPoint(6, {m_size_bg.x,    2*m_size_bg.y/3});
    bg.setPoint(7, {m_size_bg.x-10, m_size_bg.y});
    bg.setFillColor(sf::Color(25, 25, 25));

    bar.setOrigin(0, m_size.y/2);
    bg.setOrigin(d.x/2, m_size_bg.y/2);
}

void HUD::Jauge::setColor(const sf::Color& color) {
    bar.setFillColor(color);
}

void HUD::Jauge::setFill(float percent) {
    float sizex = percent * m_size.x;

    bar.setPoint(4, {sizex-10, 0});
    bar.setPoint(5, {sizex, m_size.y/3});
    bar.setPoint(6, {sizex, 2*m_size.y/3});
    bar.setPoint(7, {sizex-10, m_size.y});
}

void HUD::Jauge::setPosition(float x, float y) {
    bar.setPosition(x, y);
    bg.setPosition(x, y);
}

HUD::HUD(Player* player) : m_player(player) {

    m_hp_jauge.setSize({300, 30});
    m_hp_jauge.setPosition(VIEW_WIDTH/2 - 150, 25);
    m_hp_jauge.setColor(sf::Color::Red);
    m_hp_jauge.setFill(0.8f);

    m_pistol_jauge.setSize({500, 30});
    m_pistol_jauge.setPosition(25, VIEW_HEIGHT-20);
    m_pistol_jauge.setColor(sf::Color(20, 200, 255));
    m_pistol_jauge.setFill(0.5f);
    m_pistol_jauge.bar.rotate(-90);
    m_pistol_jauge.bg.rotate(-90);

    m_rifle_jauge.setSize({500, 30});
    m_rifle_jauge.setPosition(25, VIEW_HEIGHT-20);
    m_rifle_jauge.setColor(sf::Color(200, 255, 20));
    m_rifle_jauge.setFill(0.5f);
    m_rifle_jauge.bar.rotate(-90);
    m_rifle_jauge.bg.rotate(-90);

    ns::Res::getTexture("ammo.png").setRepeated(true);
    m_sniper_ammo.setTexture(ns::Res::getTexture("ammo.png"));
    m_sniper_ammo.setTextureRect({0, 0, 0, 105});
    m_sniper_ammo.setOrigin(0, 105);
    m_sniper_ammo.rotate(90.f);
    m_sniper_ammo.move(15, 15);
    m_sniper_ammo.scale(0.8, 0.8);

}

void HUD::update() {
    m_hp_jauge.setFill((float)m_player->getHP()/(float)m_player->getMaxHP());

    for (const auto* weapon : m_player->allWeapons()) {

        switch(weapon->getType()) {
            case Weapon::Type::Pistol:
                m_pistol_jauge.setFill(std::max(0.05f, (float)weapon->getAmmo()/weapon->getMaxAmmo()));
                break;
            case Weapon::Type::Rifle:
                m_rifle_jauge.setFill(std::max(0.05f, (float)weapon->getAmmo()/weapon->getMaxAmmo()));
                break;
            case Weapon::Type::Sniper:
                m_sniper_ammo.setTextureRect({0, 0, 25*weapon->getAmmo(), 105});
                break;
            case Weapon::Type::Melee:
                break;
        }

    }
}

auto HUD::getPosition() const -> sf::Vector2f {
    return {0, 0};
}

auto HUD::getGlobalBounds() const -> ns::FloatRect {
    return {0, 0, VIEW_WIDTH, VIEW_HEIGHT};
}

void HUD::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_hp_jauge.bg, states);
    target.draw(m_hp_jauge.bar, states);
    switch (m_player->getActiveWeapon()->getType()) {

        case Weapon::Type::Pistol:
            target.draw(m_pistol_jauge.bg, states);
            target.draw(m_pistol_jauge.bar, states);
            break;
        case Weapon::Type::Rifle:
            target.draw(m_rifle_jauge.bg, states);
            target.draw(m_rifle_jauge.bar, states);
            break;
        case Weapon::Type::Sniper:
            target.draw(m_sniper_ammo, states);
            break;
        case Weapon::Type::Melee:
            break;
    }
}
