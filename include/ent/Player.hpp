#pragma once

#include "LevelObject.hpp"
#include "Constants.hpp"

class Player : public LevelObject {
public:
    Player();
    auto getTextureRect() -> const ns::IntRect& override;

    void setZ(float z);
    auto getZ() const -> float;
    auto getEyePos() const -> float;

    void update() override;

private:
    float m_eye_pos;
    bool m_jumping = false;
    bool m_running = true;
    float m_z = 0.f;
    float m_z_vel = 0.f;
    int m_z_offset_sign = 1;

    int m_side_walk = 0;
    std::unique_ptr<ns::Spritesheet> m_spritesheet;
    ns::AnimPlayer m_anim_player;
};
