#include "Weapon.hpp"

Weapon::Weapon() {
    m_clk.restart();
    m_attacking = false;
}

int Weapon::getDamage() {
    return m_damage;
}

int Weapon::getRange() {
    return m_range;
}



sf::Sprite Weapon::getSprite() {
    return m_currentSprite;
}

Pistol::Pistol() {
    m_damage = 10;
    m_range = 50;
    m_cooldown = sf::milliseconds(300);

    m_amo = 50;
    m_aiming = false;
    m_dispersion = 10;
    m_fov_zoom = 1.f;
    
    for (int i=0 ; i<4 ; i++) {
        m_spritesheet[i].setTexture(ns::Res::in("sprites").getTexture("laser_pistol.png"));
        m_spritesheet[i].setPosition(400, 200);
        m_spritesheet[i].setScale(sf::Vector2f(2.3f, 2.3f));
    }

    m_spritesheet[0].setTextureRect(sf::IntRect(0, 0, 120, 150));
    m_spritesheet[1].setTextureRect(sf::IntRect(132, 0, 250, 150));
    m_spritesheet[2].setTextureRect(sf::IntRect(0, 154, 100, 300));
    m_spritesheet[3].setTextureRect(sf::IntRect(143, 154, 254, 300));

    m_currentSprite = m_spritesheet[0];

    m_sound_buffer.loadFromFile("assets/laser_pistol.wav");
    m_sound.setBuffer(m_sound_buffer);
}

int Pistol::getAmo() {
    return m_amo;
}

void Pistol::setAmo(int amo_amount) {
    m_amo = amo_amount;
}

float Pistol::getFovZoom() {
    return m_fov_zoom;
}

void Pistol::attack() {
    if (!m_attacking && m_amo > 0) {
        m_sound.play();
        m_clk.restart();
        m_amo--;
    }

}

void Pistol::update() {

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        attack();
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        aim();
    }
    else {
        noAim();
    }

    if (m_clk.getElapsedTime() <= m_cooldown) {
        m_attacking = true;
    }
    else {
        m_attacking = false;
    }

    if (m_aiming) {
        if (m_attacking) {
            m_currentSprite = m_spritesheet[3];
        }
        else {
            m_currentSprite = m_spritesheet[2];
        }
    }
    else {
        if (m_attacking) {
            m_currentSprite = m_spritesheet[1];
        }
        else {
            m_currentSprite = m_spritesheet[0];
        }
    }
}

void Pistol::aim() {
    m_aiming = true;
    m_fov_zoom = 1.2f;
    m_currentSprite = m_spritesheet[2];
    m_dispersion = 0;
}

void Pistol::noAim() {
    m_aiming = false;
    m_fov_zoom = 1.f;
    m_currentSprite = m_spritesheet[0];
    m_dispersion = 10;
}

Rifle::Rifle() {
    m_damage = 3;
    m_range = 30;
    m_cooldown = sf::milliseconds(100);

    m_amo = 500;
    m_aiming = false;
    m_dispersion = 30;
    m_fov_zoom = 1.f;
    
    for (int i=0 ; i<4 ; i++) {
        m_spritesheet[i].setTexture(ns::Res::in("sprites").getTexture("laser_rifle.png"));
    }

    m_spritesheet[0].setTextureRect(sf::IntRect(0, 0, 189, 143));
    m_spritesheet[0].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[0].setPosition(470, 255);
    m_spritesheet[1].setTextureRect(sf::IntRect(211, 0, 400, 143));
    m_spritesheet[1].setScale(sf::Vector2f(2.2f, 2.2f));
    m_spritesheet[1].setPosition(470, 255);
    m_spritesheet[2].setTextureRect(sf::IntRect(0, 144, 169, 400));
    m_spritesheet[2].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[2].setPosition(290, 70);
    m_spritesheet[3].setTextureRect(sf::IntRect(209, 144, 400, 400));
    m_spritesheet[3].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[3].setPosition(290, 70);

    m_currentSprite = m_spritesheet[0];

    m_sound_buffer.loadFromFile("assets/laser_rifle.wav");
    m_sound.setBuffer(m_sound_buffer);
}

int Rifle::getAmo() {
    return m_amo;
}

void Rifle::setAmo(int amo_amount) {
    m_amo = amo_amount;
}

float Rifle::getFovZoom() {
    return m_fov_zoom;
}

void Rifle::attack() {
    if (!m_attacking && m_amo > 0) {
        m_sound.play();
        m_clk.restart();
        m_amo--;
    }

}

void Rifle::update() {

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        attack();
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        aim();
    }
    else {
        noAim();
    }

    if (m_clk.getElapsedTime() <= m_cooldown) {
        m_attacking = true;
    }
    else {
        m_attacking = false;
    }

    if (m_aiming) {
        if (m_attacking) {
            m_currentSprite = m_spritesheet[3];
        }
        else {
            m_currentSprite = m_spritesheet[2];
        }
    }
    else {
        if (m_attacking) {
            m_currentSprite = m_spritesheet[1];
        }
        else {
            m_currentSprite = m_spritesheet[0];
        }
    }
}

void Rifle::aim() {
    if (!m_aiming) {
        m_aiming = true;
        m_fov_zoom = 1.8f;
        m_currentSprite = m_spritesheet[2];
        m_dispersion = 0;
    }
    
}

void Rifle::noAim() {
    m_aiming = false;
    m_fov_zoom = 1.f;
    m_currentSprite = m_spritesheet[0];
    m_dispersion = 10;
}
