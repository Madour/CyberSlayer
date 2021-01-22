#pragma once

#include <string>
#include <NasNas.h>
#include <SFML/Audio.hpp>

#include "ent/Player.hpp"
#include "Camera.hpp"

class Weapon {
public :
    Weapon();

    int getDamage();
    int getRange();

    virtual float getFovZoom(){return 1.f;}
    virtual void attack(Camera* camera){}
    virtual void update(Player* player, Camera* camera){}

    sf::Sprite getSprite();

protected :
    int m_damage;
    int m_range;

    bool m_attacking;

    sf::Clock m_clk;
    sf::Time m_cooldown;

    sf::SoundBuffer m_sound_buffer;
    sf::Sound m_sound;

    sf::Sprite m_currentSprite;
};

class Pistol : public Weapon {
public :
    Pistol();
    int getAmo();
    void setAmo(int amo_amount);
    float getFovZoom() override;
    void attack(Camera* camera) override;
    void update(Player* player, Camera* camera) override;
    void aim();
    void noAim();

private :
    int m_amo;
    int m_dispersion;
    bool m_aiming;
    float m_fov_zoom;
    float m_recoil;

    sf::Time m_cooldown_sprite;

    sf::Sprite m_spritesheet[4];
};

class Rifle : public Weapon {
public :
    Rifle();
    int getAmo();
    void setAmo(int amo_amount);
    float getFovZoom() override;
    void attack(Camera* camera) override;
    void update(Player* player, Camera* camera) override;
    void aim();
    void noAim();

private :
    int m_amo;
    int m_dispersion;
    bool m_aiming;
    float m_fov_zoom;
    float m_recoil;

    sf::Time m_cooldown_sprite;

    sf::Sprite m_spritesheet[4];
};
