#pragma once

#include "LevelObject.hpp"

class Adventurer : public LevelObject {
public:
    explicit Adventurer();

    auto getTextureRect() -> const ns::IntRect& override;

protected:
    std::unique_ptr<ns::Spritesheet> m_spritesheet;
};
