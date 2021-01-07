#include "Constants.hpp"
#include "ent/Adventurer.hpp"

Adventurer::Adventurer() : LevelObject("Adventurer") {
    setTexture(ns::Res::getTexture("adventurer.png"));
    setSize({METER, 1.8f*METER});

    m_spritesheet = std::make_unique<ns::Spritesheet>("adventurer", getTexture());
    auto* idle_anim = new ns::Anim("idle", {});
    idle_anim->add({{14, 6, 20, 30}, 150+std::rand()%3});
    idle_anim->add({{64, 6, 20, 30}, 150+std::rand()%3});
    idle_anim->add({{114, 6, 20, 30}, 150+std::rand()%3});
    idle_anim->add({{164, 6, 20, 30}, 150+std::rand()%3});
    idle_anim->loop = true;
    m_spritesheet->addAnim(idle_anim);

    m_anim_player.play(m_spritesheet->getAnim("idle"));
}

void Adventurer::update() {
    m_anim_player.update();
    ns::BaseEntity::update();
}

auto Adventurer::getTextureRect() -> const ns::IntRect& {
    return m_anim_player.getActiveFrame().rectangle;
}