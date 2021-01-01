#include "ent/Player.hpp"

Player::Player() : LevelObject("Player") {
    setTexture(ns::Res::getTexture("adventurer.png"));

    m_spritesheet = std::make_unique<ns::Spritesheet>("adventurer", getTexture());
    auto* idle_anim = new ns::Anim("idle", {});
    idle_anim->add({{14, 6, 20, 30}, 150});
    idle_anim->add({{64, 6, 20, 30}, 150});
    idle_anim->add({{114, 6, 20, 30}, 150});
    idle_anim->add({{164, 6, 20, 30}, 150});
    idle_anim->loop = true;
    m_spritesheet->addAnim(idle_anim);

    addComponent<ns::ecs::SpriteComponent>(m_spritesheet.get());
}

auto Player::getTextureRect() -> const ns::IntRect& {
    return graphics<ns::ecs::SpriteComponent>(0)->getAnimPlayer().getActiveFrame().rectangle;
}
