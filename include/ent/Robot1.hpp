#pragma once

#include "LevelObject.hpp"

class Robot1 : public LevelObject {
public:
    Robot1();
    ~Robot1() override = default;

    void setTarget(LevelObject* target);

    auto getTextureRect() -> const ns::IntRect& override;
    void update() override;
private:
    float m_vision_range = 7.f;
    float m_attack_range = 2.f;
    LevelObject* m_target = nullptr;

    std::unique_ptr<ns::Spritesheet> m_spritesheet;
    ns::AnimPlayer m_anim_player;
};
