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
    return m_current_sprite;
}

Pistol::Pistol() {
    m_damage = 10;
    m_range = 50;
    m_cooldown = sf::milliseconds(500);
    m_cooldown_sprite = sf::milliseconds(100);

    m_amo = 50;
    m_aiming = false;
    m_dispersion = 10;
    m_fov_zoom = 1.f;

    m_recoil = 0.07f;
    
    for (int i=0 ; i<4 ; i++) {
        m_spritesheet[i].setTexture(ns::Res::in("sprites").getTexture("laser_pistol.png"));
        m_spritesheet[i].setScale(sf::Vector2f(2.3f, 2.3f));
    }


    m_spritesheet[0].setTextureRect(sf::IntRect(0, 0, 120, 150));
    m_spritesheet[0].setPosition(500, 200);
    m_spritesheet[1].setTextureRect(sf::IntRect(132, 0, 250, 150));
    m_spritesheet[1].setPosition(500, 200);
    m_spritesheet[2].setTextureRect(sf::IntRect(0, 154, 100, 300));
    m_spritesheet[2].setPosition(420, 200);
    m_spritesheet[3].setTextureRect(sf::IntRect(143, 154, 254, 300));
    m_spritesheet[3].setPosition(420, 200);

    m_current_sprite = m_spritesheet[0];

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

void Pistol::attack(Camera* camera) {
    if (!m_attacking && m_amo > 0) {
        m_sound.play();
        m_clk.restart();
        m_amo--;
        camera->setRecoil(m_recoil);
    }

}

void Pistol::update(Player* player, Camera* camera) {

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        attack(camera);
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !player->isRunning()) {
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
        if (m_attacking && m_cooldown_sprite >= m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[3];
        }
        else {
            m_current_sprite = m_spritesheet[2];
        }
    }
    else {
        if (m_attacking && m_cooldown_sprite >= m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[1];
        }
        else {
            m_current_sprite = m_spritesheet[0];
        }
    }
}

void Pistol::aim() {
    m_aiming = true;
    m_fov_zoom = 1.2f;
    m_current_sprite = m_spritesheet[2];
    m_dispersion = 0;
}

void Pistol::noAim() {
    m_aiming = false;
    m_fov_zoom = 1.f;
    m_current_sprite = m_spritesheet[0];
    m_dispersion = 10;
}

Rifle::Rifle() {
    m_damage = 3;
    m_range = 30;
    m_cooldown = sf::milliseconds(100);
    m_cooldown_sprite = sf::milliseconds(50);;

    m_amo = 500;
    m_aiming = false;
    m_dispersion = 30;
    m_fov_zoom = 1.f;

    m_recoil = 0.03f;
    
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

    m_current_sprite = m_spritesheet[0];

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

void Rifle::attack(Camera* camera) {
    if (!m_attacking && m_amo > 0) {
        m_sound.play();
        m_clk.restart();
        m_amo--;
        camera->setRecoil(m_recoil);
    }

}

void Rifle::update(Player* player, Camera* camera) {

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        attack(camera);
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !player->isRunning()) {
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
        if (m_attacking && m_cooldown_sprite >= m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[3];
        }
        else {
            m_current_sprite = m_spritesheet[2];
        }
    }
    else {
        if (m_attacking && m_cooldown_sprite >= m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[1];
        }
        else {
            m_current_sprite = m_spritesheet[0];
        }
    }
}

void Rifle::aim() {
    if (!m_aiming) {
        m_aiming = true;
        m_fov_zoom = 1.8f;
        m_current_sprite = m_spritesheet[2];
        m_dispersion = 0;
    }
    
}

void Rifle::noAim() {
    m_aiming = false;
    m_fov_zoom = 1.f;
    m_current_sprite = m_spritesheet[0];
    m_dispersion = 10;
}

Sniper::Sniper() {
    m_damage = 50;
    m_range = 100;
    m_cooldown = sf::milliseconds(2000);
    

    m_amo = 10;
    m_aiming = false;
    m_dispersion = 50;
    m_fov_zoom = 6.f;

    m_recoil = 0.1f;
    
    for (int i=0 ; i<5 ; i++) {
        m_cooldown_sprite[i] = sf::milliseconds((i+1)*300);
    }
    m_cooldown_sprite[0] = sf::milliseconds(100);

    m_spritesheet[0].setTexture(ns::Res::in("sprites").getTexture("sniper_0.png"));
    m_spritesheet[0].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[0].setPosition(470, 220);

    m_spritesheet[1].setTexture(ns::Res::in("sprites").getTexture("sniper_1.png"));
    m_spritesheet[1].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[1].setPosition(470, 220);

    m_spritesheet[2].setTexture(ns::Res::in("sprites").getTexture("sniper_2.png"));
    m_spritesheet[2].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[2].setPosition(470, 220);

    m_spritesheet[3].setTexture(ns::Res::in("sprites").getTexture("sniper_3.png"));
    m_spritesheet[3].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[3].setPosition(470, 220);

    m_spritesheet[4].setTexture(ns::Res::in("sprites").getTexture("sniper_4.png"));
    m_spritesheet[4].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[4].setPosition(470, 220);


    m_spritesheet[5].setTexture(ns::Res::in("sprites").getTexture("sniper_5.png"));
    m_spritesheet[5].setScale(sf::Vector2f(0.5f, 0.5f));
    m_spritesheet[5].setPosition(220,30);

    m_current_sprite = m_spritesheet[0];

    m_sound_buffer.loadFromFile("assets/sniper.wav");
    m_sound.setBuffer(m_sound_buffer);
}

int Sniper::getAmo() {
    return m_amo;
}

void Sniper::setAmo(int amo_amount) {
    m_amo = amo_amount;
}

float Sniper::getFovZoom() {
    return m_fov_zoom;
}

void Sniper::attack(Camera* camera) {
    if (!m_attacking && m_amo > 0) {
        m_sound.play();
        m_clk.restart();
        m_amo--;
        camera->setRecoil(m_recoil);
    }

}

void Sniper::update(Player* player, Camera* camera) {

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        attack(camera);
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !player->isRunning() && !m_attacking) {
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
        m_current_sprite = m_spritesheet[5];
    }
    else {
        if (m_attacking && m_cooldown_sprite[0] >= m_clk.getElapsedTime() && m_cooldown_sprite[1] > m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[1];
        }
        else if (m_cooldown_sprite[1] >= m_clk.getElapsedTime() && m_cooldown_sprite[2] > m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[2];
        }
        else if (m_cooldown_sprite[2] >= m_clk.getElapsedTime() && m_cooldown_sprite[3] > m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[3];
        }
        else if (m_cooldown_sprite[3] >= m_clk.getElapsedTime() && m_cooldown_sprite[4] > m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[4];
        }
        else if (m_cooldown_sprite[4] >= m_clk.getElapsedTime() && m_cooldown_sprite[4]+m_cooldown_sprite[4] > m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[2];
        }
        else {
            m_current_sprite = m_spritesheet[0];
        }
    }
}

void Sniper::aim() {
    if (!m_aiming) {
        m_aiming = true;
        m_fov_zoom = 4.f;
        m_dispersion = 0;
        m_current_sprite = m_spritesheet[5];
    }
    
}

void Sniper::noAim() {
    m_aiming = false;
    m_fov_zoom = 1.f;
    m_dispersion = 10;
    m_current_sprite = m_spritesheet[1];
}

Melee::Melee() {
    m_damage = 10;
    m_range = 5;
    m_cooldown = sf::milliseconds(500);
    
    
    m_cooldown_sprite[0] = sf::milliseconds(50);
    m_cooldown_sprite[1] = sf::milliseconds(100);
    m_cooldown_sprite[2] = sf::milliseconds(300);

    m_spritesheet[0].setTexture(ns::Res::in("sprites").getTexture("melee_0.png"));
    m_spritesheet[0].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[0].setPosition(250, 150);

    m_spritesheet[1].setTexture(ns::Res::in("sprites").getTexture("melee_1.png"));
    m_spritesheet[1].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[1].setPosition(250, 150);

    m_spritesheet[2].setTexture(ns::Res::in("sprites").getTexture("melee_2.png"));
    m_spritesheet[2].setScale(sf::Vector2f(2.f, 2.f));
    m_spritesheet[2].setPosition(250, 150);

    m_current_sprite = m_spritesheet[0];

    m_sound_buffer.loadFromFile("assets/melee.wav");
    m_sound.setBuffer(m_sound_buffer);
}

void Melee::attack(Camera* camera) {
    if (!m_attacking) {
        m_sound.play();
        m_clk.restart();
    }
}

float Melee::getFovZoom() {
    return 1.f;
}

void Melee::update(Player* player, Camera* camera) {

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        attack(camera);
    }
    if (m_clk.getElapsedTime() <= m_cooldown) {
        m_attacking = true;
    }
    else {
        m_attacking = false;
    }

    if (m_attacking) {
        if (m_cooldown_sprite[0] >= m_clk.getElapsedTime() && m_cooldown_sprite[1] > m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[1];
        }
        else if (m_cooldown_sprite[1] >= m_clk.getElapsedTime() && m_cooldown_sprite[2] > m_clk.getElapsedTime()) {
            m_current_sprite = m_spritesheet[2];
        }
    }
    else {
        m_current_sprite = m_spritesheet[0];
    }
}
