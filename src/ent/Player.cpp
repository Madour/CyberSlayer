#include "ent/Player.hpp"
#include "Level.hpp"
#include "Utils.hpp"
#include "Weapon.hpp"

using Inputs = ns::Config::Inputs;

Player::Player() : LevelObject("Player") {
    setTexture(ns::Res::getTexture("adventurer.png"));
    setSize({METER, 1.8f*METER});
    m_eye_pos = -1.45f*METER;

    m_max_hp = 100;
    m_hp = m_max_hp/1.5f;

    m_spritesheet = std::make_unique<ns::Spritesheet>("adventurer", getTexture());
    auto* idle_anim = new ns::Anim("idle", {});
    idle_anim->add({{14, 6, 20, 30}, 150});
    idle_anim->add({{64, 6, 20, 30}, 150});
    idle_anim->add({{114, 6, 20, 30}, 150});
    idle_anim->add({{164, 6, 20, 30}, 150});
    idle_anim->loop = true;
    auto* run_anim = new ns::Anim("run", {});
    run_anim->add({{67, 43, 20, 30}, 150});
    run_anim->add({{116, 43, 20, 30}, 150});
    run_anim->add({{166, 43, 20, 30}, 150});
    run_anim->add({{218, 43, 20, 30}, 150});
    run_anim->add({{266, 43, 20, 30}, 150});
    run_anim->add({{316, 43, 20, 30}, 150});
    run_anim->loop = true;

    m_spritesheet->addAnim(idle_anim);
    m_spritesheet->addAnim(run_anim);

    m_anim_player.play(m_spritesheet->getAnim("idle"));

    m_sound_buffer_jump_landing.loadFromFile("assets/sfx/jump_landing.ogg");
    m_sound_jump_landing.setBuffer(m_sound_buffer_jump_landing);

    // acceleration, mass, friction
    addComponent<ns::ecs::PhysicsComponent>(sf::Vector2f(0.5f*METER/UPS, 0.5f*METER/UPS), 1.f, sf::Vector2f(0.1f, 0.1f));

    addComponent<ns::ecs::InputsComponent>();

    inputs()->bind(Inputs::getButtonKey("up"), [&]{
        physics()->setDirectionMagnitude(m_running ? 2.f:1.f);
    });

    inputs()->bind(Inputs::getButtonKey("down"), [&]{
        physics()->setDirectionMagnitude(m_running ? -2.f:-1.f);
    });

    inputs()->bind(Inputs::getButtonKey("left"), [&]{
        m_side_walk = -1;
    });

    inputs()->bind(Inputs::getButtonKey("right"), [&]{
        m_side_walk = 1;
    });

    inputs()->bind(Inputs::getButtonKey("jump"), [&]{
        if (!m_jumping)
            m_z_vel = -20*METER/UPS;
        m_jumping = true;
    });
}

void Player::setZ(float z) {
    m_z = z;
}

auto Player::getEyePos() const -> float {
    return m_z+m_eye_pos;
}

void Player::setHP(int hp) {
    m_hp = std::min(hp, m_max_hp);
}

auto Player::getHP() const -> int {
    return m_hp;
}

auto Player::getMaxHP() const -> int {
    return m_max_hp;
}

bool Player::isRunning() const {
    return m_running;
}

auto Player::allWeapons() const -> const std::vector<Weapon*>& {
    return m_weapons;
}

auto Player::getActiveWeapon() const -> Weapon* {
    return m_active_weapon;
}

void Player::selectNextWeapon() {
    if (m_weapons.size() < 2 )
        return;
    m_active_weapon->hide();
    // select next
    m_active_weapon_index = (m_active_weapon_index+1u) % m_weapons.size();
    m_active_weapon = m_weapons[m_active_weapon_index];
}

void Player::selectPrevWeapon() {
    if (m_weapons.size() < 2 )
        return;
    m_active_weapon->hide();
    // select prev
    m_active_weapon_index = m_active_weapon_index == 0 ? m_weapons.size()-1u : m_active_weapon_index-1u;
    m_active_weapon = m_weapons[m_active_weapon_index];
}

void Player::addWeapon(Weapon* new_weapon) {
    m_weapons.push_back(new_weapon);
    if (m_active_weapon == nullptr) {
        m_active_weapon = new_weapon;
        m_active_weapon_index = 0;
        new_weapon->show();
    }
}

void Player::update() {
    // slide up and down when changing weapon anim
    bool all_hidden = true;
    for (unsigned i = 0; i < m_weapons.size(); ++i) {
        if (i != m_active_weapon_index) {
            m_weapons[i]->update();
            if (!m_weapons[i]->isHidden()) {
                all_hidden = false;
            }
        }
    }

    // update active weapon
    m_active_weapon->update();
    if (m_active_weapon->isShown()) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !m_running)
            m_active_weapon->aim(true);
        else
            m_active_weapon->aim(false);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            m_active_weapon->attack();
        }
    }
    else if (all_hidden) {
        m_active_weapon->show();
    }

    m_running = false;
    if (sf::Keyboard::isKeyPressed(Inputs::getButtonKey("run"))) {
        m_running = true;
    }
    // reset physics
    m_side_walk = 0;
    physics()->setDirectionMagnitude(0);

    // update inputs
    inputs()->update();

    // player side walking
    if (m_side_walk) {
        if (physics()->getDirectionMagnitude() == 0) {
            if (physics()->getDirectionMagnitude() == 0)
                physics()->setDirectionMagnitude(1);
            physics()->setDirectionAngle(physics()->getDirectionAngle() + m_side_walk*90);
        }
        else {
            float sign = std::signbit(physics()->getDirectionMagnitude()) ? -1.f : 1.f;
            physics()->setDirectionAngle(physics()->getDirectionAngle() + sign*m_side_walk*(90 - 45));
        }
    }

    // collisions check
    sf::Vector2f overlap;
    for (const auto& coll : Level::getCollisions()) {
        if (circleRectIntersection(getPosition() + 2.f*physics()->getVelocity(), getSize().x, coll, overlap)) {
            transform()->move(-overlap);
        }
    }

    physics()->update();

    // update anim state
    if (physics()->getDirectionMagnitude() != 0.f) {
        if (m_anim_player.getAnim()->getName() != "run")
            m_anim_player.play(m_spritesheet->getAnim("run"));
    }
    else if (m_anim_player.getAnim()->getName() != "idle")
        m_anim_player.play(m_spritesheet->getAnim("idle")) ;

    // jumping
    if (m_jumping) {
        m_z_vel += physics()->getMass() * GRAVITY * METER/UPS;
        m_z += m_z_vel;
        if (m_z > 0 && m_jumping) {
            m_z = 0;
            m_z_vel = 0.f;
            m_jumping = false;
            m_sound_jump_landing.play();
        }
    }
    // up down walking effect
    else if (physics()->getDirectionMagnitude() != 0){
        float magnitude = std::abs(physics()->getDirectionMagnitude());
        if (m_z_offset_sign == 1) {
            if (m_z < 0.05*METER)
                m_z+=0.0125f*METER*magnitude;
            else
                m_z_offset_sign = -1;
        }
        else {
            if (m_z > -0.1*METER*magnitude)
                m_z-=0.015f*METER*magnitude;
            else
                m_z_offset_sign = 1;
        }
    }
    // player is not moving
    else {
        m_z = 0;
    }

    m_anim_player.update();
}

auto Player::getTextureRect() -> const ns::IntRect& {
    return m_anim_player.getActiveFrame().rectangle;
}
