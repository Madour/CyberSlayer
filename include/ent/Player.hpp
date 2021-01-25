#pragma once

#include <SFML/Audio.hpp>
#include "LevelObject.hpp"
#include "Constants.hpp"

// forward declaration to avoid include cycle
class Weapon;

class Player : public LevelObject {
public:
    Player();
    auto getTextureRect() -> const ns::IntRect& override;

    auto allWeapons() const -> const std::vector<Weapon*>&;
    void setZ(float z);

    void setHP(int hp);
    auto getHP() const -> int;
    auto getMaxHP() const -> int;

    auto getActiveWeapon() const -> Weapon*;
    void selectNextWeapon();
    void selectPrevWeapon();
    void addWeapon(Weapon* new_weapon);

    auto getEyePos() const -> float;
    bool isRunning() const;

    void update() override;

private:
    int m_hp;
    int m_max_hp;
    float m_eye_pos;
    bool m_jumping = false;
    bool m_running = true;

    float m_z = 0.f;
    float m_z_vel = 0.f;
    int m_z_offset_sign = 1;

    int m_side_walk = 0;

    std::vector<Weapon*> m_weapons;
    Weapon* m_active_weapon = nullptr;
    unsigned m_active_weapon_index = -1;

    std::unique_ptr<ns::Spritesheet> m_spritesheet;
    ns::AnimPlayer m_anim_player;

    sf::SoundBuffer m_sound_buffer_jump_landing;
    sf::Sound m_sound_jump_landing;
};
