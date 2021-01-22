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

    void update() override;

private:
    float m_sign=0.005f*METER;
    ItemData m_data;
};

class ItemFactory {
public:
    static auto createFromName(const std::string& name) -> Item*;

private:
    static std::unordered_map<std::string, ItemData> m_items_data;
};
