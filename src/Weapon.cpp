#include "Weapon.hpp"

Weapon::Weapon() {
    clk.restart();
    attacking = false;
}

int Weapon::getDamage() {
    return damage;
}

int Weapon::getRange() {
    return range;
}



sf::Sprite Weapon::getSprite() {
    return currentSprite;
}

Pistol::Pistol() {
    damage = 10;
    range = 50;
    cooldown = sf::milliseconds(300);

    amo = 50;
    aiming = false;
    dispersion = 10;
    
    for (int i=0 ; i<4 ; i++) {
        spritesheet[i].setTexture(ns::Res::in("sprites").getTexture("laser_pistol.png"));
        spritesheet[i].setPosition(400, 200);
        spritesheet[i].setScale(sf::Vector2f(2.3f, 2.3f));
    }
    spritesheet[0].setTextureRect(sf::IntRect(0, 0, 120, 150));
    spritesheet[1].setTextureRect(sf::IntRect(132, 0, 250, 150));
    spritesheet[2].setTextureRect(sf::IntRect(0, 154, 100, 300));
    spritesheet[3].setTextureRect(sf::IntRect(143, 154, 254, 300));

    currentSprite = spritesheet[0];

    sound_buffer.loadFromFile("assets/laser_pistol.wav");
    sound.setBuffer(sound_buffer);
}

int Pistol::getAmo() {
    return amo;
}

void Pistol::setAmo(int amo_amount) {
    amo = amo_amount;
}

void Pistol::attack() {
    if (!attacking) {
        if (amo > 0) {
            std::cout << "pan" <<std::endl;
            sound.play();
            clk.restart();
            amo--;
        }
    }

}

void Pistol::update() {
    if (clk.getElapsedTime() <= cooldown) {
        attacking = true;
    }
    else {
        attacking = false;
    }

    if (aiming) {
        if (attacking) {
            currentSprite = spritesheet[3];
        }
        else {
            currentSprite = spritesheet[2];
        }
    }
    else {
        if (attacking) {
            currentSprite = spritesheet[1];
        }
        else {
            currentSprite = spritesheet[0];
        }
    }
}

void Pistol::aim() {
    aiming = true;
    currentSprite = spritesheet[2];
    dispersion = 0;
}

void Pistol::noAim() {
    aiming = false;
    currentSprite = spritesheet[0];
    dispersion = 10;
}
