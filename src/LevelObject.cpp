#include "LevelObject.hpp"
#include "ent/Adventurer.hpp"
#include "ent/Robot1.hpp"

LevelObject::LevelObject(const std::string& name) :
ns::BaseEntity(name),
m_z(0.f)
{}

auto LevelObject::getZ() const -> float {
    return m_z;
}

void LevelObject::setTexture(const sf::Texture& texture) {
    m_texture = &texture;
}

auto LevelObject::getTexture() -> const sf::Texture& {
    if (m_texture)
        return *m_texture;
    std::cerr << "Level Object does not have a texture !" << std::endl;
    std::exit(-1);
}

void LevelObject::setSize(const sf::Vector2f& size) {
    m_size = size;
}

auto LevelObject::getSize() -> const sf::Vector2f& {
    return m_size;
}

void LevelObject::computeDistanceToCamera(const sf::Vector2f& camera_pos) {
    m_distance_to_camera = ns::distance(transform()->getPosition(), camera_pos);
}

auto LevelObject::getDistanceToCamera() const -> float {
    return m_distance_to_camera;
}

std::map<std::string, std::function<LevelObject*(void)>> EnemyFactory::m_data = {
        {"Adventurer", []{return new Adventurer();}},
        {"Robot", []{return new Robot1();}},
};

auto EnemyFactory::createFromName(const std::string& name) -> LevelObject* {
    if (m_data.count(name) > 0)
        return m_data.at(name)();
    return nullptr;
}
