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
