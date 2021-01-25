#pragma once

#include "LevelObject.hpp"

class Adventurer : public LevelObject {
public:
    Adventurer();
    ~Adventurer() override = default;

    auto getTextureRect() -> const ns::IntRect& override;
    void update() override;
private:
    std::unique_ptr<ns::Spritesheet> m_spritesheet;
    ns::AnimPlayer m_anim_player;
};
