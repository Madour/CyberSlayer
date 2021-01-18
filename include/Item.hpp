#pragma once

#include "LevelObject.hpp"
#include "ent/Player.hpp"

struct ItemData {
    std::string name;
    ns::IntRect texture_rect;
    std::function<void(Player&)> on_collect;
    std::function<void(Player&)> on_use;
};

class Item : public LevelObject {
public:
    explicit Item(const ItemData& item_data);
    auto getTextureRect() -> const ns::IntRect& override;

    void onCollect(Player& player);
    void onUse(Player& player);

private:
    ItemData m_data;
};