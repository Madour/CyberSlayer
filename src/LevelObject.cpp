#include "LevelObject.hpp"

void LevelObject::setTexture(const sf::Texture& texture) {
    m_texture = &texture;
}

auto LevelObject::getTexture() -> const sf::Texture& {
    if (m_texture)
        return *m_texture;
    std::cerr << "Level Object does not have a texture !" << std::endl;
    std::exit(-1);
}

void LevelObject::setPosition(float x, float y) {
    m_position.x = x; m_position.y = y;
}

void LevelObject::setPosition(const sf::Vector2f& position) {
    m_position = position;
}

auto LevelObject::getPosition() -> const sf::Vector2f& {
    return m_position;
}

void LevelObject::setSize(const sf::Vector2f& size) {
    m_size = size;
}

auto LevelObject::getSize() -> const sf::Vector2f& {
    return m_size;
}
