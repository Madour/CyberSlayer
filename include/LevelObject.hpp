#pragma once

#include <NasNas.h>

class LevelObject : public ns::BaseEntity {
public:
    explicit LevelObject(const std::string& name);

    void setTexture(const sf::Texture& texture);
    auto getTexture() -> const sf::Texture&;

    void setSize(const sf::Vector2f& size);
    auto getSize() -> const sf::Vector2f&;

    virtual auto getTextureRect() -> const ns::IntRect& = 0;

protected:
    const sf::Texture* m_texture = nullptr;
    sf::Vector2f m_size;
};


