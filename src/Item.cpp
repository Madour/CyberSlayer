#include "Item.hpp"


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

std::unordered_map<std::string, ItemData> ItemFactory::m_items_data = {
    {"Heal", {
        "Heal",
        {251, 315, 51, 34},
        [](Player& player) {ns_LOG("Health collected");},
        [](Player& player) {ns_LOG("Health used");}
    }},
    {"Ammo", {
         "Ammo",
         {240, 227, 77, 62},
         [](Player& player) {ns_LOG("Ammo collected");},
         [](Player& player) {ns_LOG("Ammo used");}
    }},
};

auto ItemFactory::createFromName(const std::string& name) -> Item*{
    auto iter = m_items_data.find(name);
    if (iter != m_items_data.end()) {
        return new Item(m_items_data.at(name));
    }
    return nullptr;
}