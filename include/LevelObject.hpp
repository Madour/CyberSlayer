#pragma once

#include <NasNas.h>

class LevelObject : public ns::BaseEntity {
public:
    explicit LevelObject(const std::string& name);

    auto getZ() const -> float;

    void setTexture(const sf::Texture& texture);
    auto getTexture() -> const sf::Texture&;

    void setSize(const sf::Vector2f& size);
    auto getSize() -> const sf::Vector2f&;

    virtual auto getTextureRect() -> const ns::IntRect& = 0;

    void computeDistanceToCamera(const sf::Vector2f& camera_pos);
    auto getDistanceToCamera() const -> float;

protected:
    float m_z;

private:
    const sf::Texture* m_texture = nullptr;
    sf::Vector2f m_size;
    float m_distance_to_camera = 0.f;
};

class EnemyFactory {
public:
    static auto createFromName(const std::string& name) -> LevelObject*;

private:
    static std::map<std::string, std::function<LevelObject*(void)>> m_data;
};
