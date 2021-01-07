#include "ent/Player.hpp"

Player::Player() : LevelObject("Player") {
    setTexture(ns::Res::getTexture("adventurer.png"));
    setSize({METER, 1.8f*METER});
    m_eye_pos = -1.45f*METER;

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

    // acceleration, mass, friction
    addComponent<ns::ecs::PhysicsComponent>(sf::Vector2f(0.5f*METER/UPS, 0.5f*METER/UPS), 1.f, sf::Vector2f(0.1f, 0.1f));

    using Inputs = ns::Config::Inputs;
    addComponent<ns::ecs::InputsComponent>();

    inputs()->bind(Inputs::getButtonKey("up"), [&]{
        physics()->setDirectionMagnitude(1);
    });

    inputs()->bind(Inputs::getButtonKey("down"), [&]{
        physics()->setDirectionMagnitude(-1);
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

auto Player::getZ() const -> float {
    return m_z;
}

auto Player::getEyePos() const -> float {
    return m_z+m_eye_pos;
}

void Player::update() {
    // reset physics
    m_side_walk = 0;
    physics()->setDirectionMagnitude(0);

    // update inputs
    inputs()->update();

    if (m_side_walk) {
        if (physics()->getDirectionMagnitude() == 0) {
            physics()->setDirectionMagnitude(1);
            physics()->setDirectionAngle(physics()->getDirectionAngle() + m_side_walk*90);
        }
        else
            physics()->setDirectionAngle(physics()->getDirectionAngle() + physics()->getDirectionMagnitude()*m_side_walk*(90 - 45));
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
        }
    }
    // up down walking effect
    else if (physics()->getDirectionMagnitude() != 0){
        if (m_z_offset_sign == 1) {
            if (m_z < 0.1*METER) {
                m_z+=0.003f;
            }
            else {
                m_z_offset_sign = -1;
            }
        }
        else {
            if (m_z > -0.05*METER) {
                m_z-=0.0025f;
            }
            else {
                m_z_offset_sign = 1;
            }
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
