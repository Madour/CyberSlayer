#include "Constants.hpp"
#include "ent/Robot1.hpp"

Robot1::Robot1() : LevelObject("Robot1") {
    setTexture(ns::Res::in("sprites").getTexture("robot_1.png"));
    setSize({2.f*METER, 2.8f*METER});

    m_spritesheet = std::make_unique<ns::Spritesheet>("adventurer", getTexture());
    m_spritesheet->setGrid({100, 110}, 5);

    m_spritesheet->addAnim("idle", 5, 1, 500);
    m_spritesheet->addAnim("walk", 0, 4, 300);
    m_spritesheet->addAnim("shoot", 5, 2, 100);

    m_anim_player.play(m_spritesheet->getAnim("idle"));

    addComponent<ns::ecs::PhysicsComponent>(sf::Vector2f(0.3f*METER/UPS, 0.3f*METER/UPS), 2.f, sf::Vector2f(0.1f, 0.1f));

}

void Robot1::update() {

    sf::Vector2f delta_pos{m_target->getPosition() - getPosition()};
    auto dx = (int)delta_pos.x;
    auto dy = (int)delta_pos.y;

    auto distance = ns::distance(getPosition(), m_target->getPosition());
    if (distance > m_attack_range && distance < m_vision_range) {
        if (dx != 0) physics()->setDirection(dx / std::abs(dx), physics()->getDirection().y);
        if (dy != 0) physics()->setDirection(physics()->getDirection().x, dy / std::abs(dy));
    }
    else {
        physics()->setDirection(0, 0);
        if (distance <= m_attack_range )
            if (m_anim_player.getAnim()->getName() != "shoot")
                m_anim_player.play(m_spritesheet->getAnim("shoot"));
    }

    if (physics()->getDirection().x != 0 || physics()->getDirection().y != 0) {
        if (distance > m_attack_range) {
            if (m_anim_player.getAnim()->getName() != "walk")
                m_anim_player.play(m_spritesheet->getAnim("walk"));
        }
    }
    else {
        if (m_anim_player.getAnim()->getName() != "idle" && m_anim_player.getAnim()->getName() != "shoot")
            m_anim_player.play(m_spritesheet->getAnim("idle"));
    }


    m_anim_player.update();

    ns::BaseEntity::update();
}

auto Robot1::getTextureRect() -> const ns::IntRect& {
    return m_anim_player.getActiveFrame().rectangle;
}

void Robot1::setTarget(LevelObject* target) {
    m_target = target;
}
