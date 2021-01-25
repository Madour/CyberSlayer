#pragma once

#include <SFML/Graphics.hpp>
#include "ent/Player.hpp"

class Camera {
public:
    Camera();

    void setSensibility(float sensibility);

    void setFovDeg(float degrees);
    void setFovRad(float radians);
    auto getFovRad() const -> float;
    void setBaseFovRad(float radians);
    auto getBaseFovRad() const -> float;

    auto getProjectionPlaneDistance() const -> float;

    auto setPosition3D(float x, float y, float z);
    auto setPosition2D(float x, float y);

    void setRecoil(float recoil);

    auto getPosition3D() const -> const sf::Vector3f&;
    auto getPosition2D() const -> const sf::Vector2f&;

    auto getRotationDeg() const -> sf::Vector3f;
    auto getRotationRad() const -> const sf::Vector3f&;

    auto getRoll() const -> float;
    auto getPitch() const -> float;
    auto getYaw() const -> float;

    void onEvent(const sf::Event& event);
    void update(Player* player);

private:
    float m_sensibility;
    float m_base_fov;           // field of view raw
    float m_fov;                // field of view in radians with multiplicator
    float m_projection_distance;// distance to projection plane

    float m_weapon_recoil;

    sf::Vector3f m_position;    // position in fake 3D space
    sf::Vector2f m_position2d;  // position in 2D space
    sf::Vector3f m_rotation = {0, 0, -HALF_PI};    // rotation in radians
};

