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
    int damage;
    int range;

    bool attacking;

    sf::Clock clk;
    sf::Time cooldown;

    sf::SoundBuffer sound_buffer;
    sf::Sound sound;

    sf::Sprite currentSprite;
};

class Pistol : public Weapon {
public :
    Pistol();
    int getAmo();
    void setAmo(int amo_amount);
    void attack() override;
    void update() override;
    void aim();
    void noAim();

private :
    int amo;
    int dispersion;
    bool aiming;

    sf::Sprite spritesheet[4];
};
