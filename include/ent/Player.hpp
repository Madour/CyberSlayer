#pragma once

#include "LevelObject.hpp"

class Player : public LevelObject {
public:
    Player();
    auto getTextureRect() -> const ns::IntRect& override;

private:
    std::unique_ptr<ns::Spritesheet> m_spritesheet;
};
