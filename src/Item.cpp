#include "Item.hpp"
#include "Weapon.hpp"


Item::Item(const ItemData& item_data) :
LevelObject(item_data.name),
m_data(item_data) {
    setSize({METER, METER});
    setTexture(ns::Res::getTexture("items.png"));
}

auto Item::getTextureRect() -> const ns::IntRect& {
    return m_data.texture_rect;
}

void Item::onCollect(Player& player) {
    m_data.on_collect(player);
}
void Item::onUse(Player& player) {
    m_data.on_use(player);
}

void Item::update() {
    if (m_z > 0.15*METER || m_z < 0)
        m_sign *= -1;
    m_z += m_sign;
}

std::map<std::string, ItemData> ItemFactory::m_data = {
    {"Heal", {
        "Heal",
        {251, 315, 51, 34},
        [](Player& player) {player.setHP(player.getHP()+20);},
        [](Player& player) {}
    }},
    {"Ammo", {
         "Ammo",
         {21, 231, 66, 55},
         [](Player& player) {
             for (auto* weapon : player.allWeapons())
                 if (weapon->getType() == Weapon::Type::Sniper) {
                     weapon->setAmmo(weapon->getAmmo() + 4);
                 }
         },
        [](Player& player) {},
    }},
};

auto ItemFactory::createFromName(const std::string& name) -> Item*{
    if (m_data.count(name)>0) {
        return new Item(m_data.at(name));
    }
    return nullptr;
}