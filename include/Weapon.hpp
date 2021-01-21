#pragma once

#include <string>
#include <NasNas.h>
#include <SFML/Audio.hpp>

class Weapon {
public :
    Weapon();

    int getDamage();
    int getRange();

    virtual void attack(){}
    virtual void update(){}

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
    float getFovZoom();
    void attack() override;
    void update() override;
    void aim();
    void noAim();

private :
    int m_amo;
    int m_dispersion;
    bool m_aiming;
    float m_fov_zoom;

    sf::Sprite m_spritesheet[4];
};

class Rifle : public Weapon {
public :
    Rifle();
    int getAmo();
    void setAmo(int amo_amount);
    float getFovZoom();
    void attack() override;
    void update() override;
    void aim();
    void noAim();

private :
    int m_amo;
    int m_dispersion;
    bool m_aiming;
    float m_fov_zoom;

    sf::Sprite m_spritesheet[4];
};
