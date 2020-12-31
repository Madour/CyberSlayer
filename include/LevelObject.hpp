#pragma once

#include <NasNas.h>

class LevelObject {
public:
    LevelObject() = default;
    virtual ~LevelObject() = default;

    void setTexture(const sf::Texture& texture);
    auto getTexture() -> const sf::Texture&;

    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f& position);
    auto getPosition() -> const sf::Vector2f&;

    void setSize(const sf::Vector2f& size);
    auto getSize() -> const sf::Vector2f&;

    virtual auto getTextureRect() -> const ns::IntRect& = 0;
    virtual void update() = 0;

protected:
    const sf::Texture* m_texture = nullptr;
    sf::Vector2f m_size;
    sf::Vector2f m_position;

};


