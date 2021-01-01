#include "LevelObject.hpp"

LevelObject::LevelObject(const std::string& name) : ns::BaseEntity(name)
{}

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
