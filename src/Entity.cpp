#include "Entity.hpp"

Entity::Entity() {
    setTexture(ns::Res::getTexture("adventurer.png"));
    m_spritesheet = std::make_unique<ns::Spritesheet>("adventurer", getTexture());
    auto* idle_anim = new ns::Anim("idle", {});
    idle_anim->add({{14, 7, 19, 29}, 150});
    idle_anim->add({{66, 6, 17, 30}, 150});
    idle_anim->add({{115, 6, 19, 30}, 150});
    idle_anim->add({{167, 7, 20, 29}, 150});
    idle_anim->loop = true;
    m_spritesheet->addAnim(idle_anim);

    m_anim_player.play(m_spritesheet->getAnim("idle"));
}

void Entity::update() {
    m_anim_player.update();
}

auto Entity::getTextureRect() -> const ns::IntRect& {
    return m_anim_player.getActiveFrame().rectangle;
}
