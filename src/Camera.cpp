#include "Camera.hpp"
#include "Constants.hpp"

Camera::Camera() :
m_sensibility(0.5f),
m_weapon_recoil(0.0f),
m_base_fov(FOV),
m_fov(FOV),
m_projection_distance((static_cast<float>(VIEW_WIDTH)/2.f) / std::tan(FOV/2.f))
{}

void Camera::setSensibility(float sensibility) {
    m_sensibility = sensibility;
}

void Camera::setFovDeg(float degrees) {
    setFovRad(ns::to_radian(degrees));
}

void Camera::setFovRad(float radians) {
    m_fov = radians;
    m_projection_distance = (static_cast<float>(VIEW_WIDTH)/2.f) / std::tan(m_fov/2.f);
}

auto Camera::getFovRad() const -> float {
    return m_fov;
}

void Camera::setBaseFovRad(float radians) {
    m_base_fov = radians;
}


auto Camera::getBaseFovRad() const -> float {
    return m_base_fov;
}

auto Camera::getProjectionPlaneDistance() const -> float {
    return m_projection_distance;
}

auto Camera::setPosition3D(float x, float y, float z) {
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    m_position2d.x = x;
    m_position2d.y = y;
}

auto Camera::setPosition2D(float x, float y) {
    m_position.x = x;
    m_position.y = y;
    m_position2d.x = x;
    m_position2d.y = y;
}

auto Camera::getPosition3D() const -> const sf::Vector3f& {
    return m_position;
}

auto Camera::getPosition2D() const -> const sf::Vector2f& {
    return m_position2d;
}

void Camera::setRecoil(float recoil) {
    m_weapon_recoil = recoil;
}

auto Camera::getRotationDeg() const -> sf::Vector3f {
    return {ns::to_degree(m_rotation.x), ns::to_degree(m_rotation.y), ns::to_degree(m_rotation.z)};
}

auto Camera::getRotationRad() const -> const sf::Vector3f& {
    return m_rotation;
}

auto Camera::getRoll() const -> float {
    return m_rotation.x;
}

auto Camera::getPitch() const -> float {
    return m_rotation.y;
}

auto Camera::getYaw() const -> float {
    return m_rotation.z;
}

void Camera::onEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        int dx = event.mouseMove.x - VIEW_WIDTH/2;
        int dy = event.mouseMove.y - VIEW_HEIGHT/2;

        // Yaw between 0 and 2*PI
        m_rotation.z += ns::to_radian(static_cast<float>(dx)) * m_sensibility + (std::rand()%3-1)*0.5f*m_weapon_recoil;
        if (m_rotation.z > TWO_PI) m_rotation.z -= TWO_PI;
        if (m_rotation.z < 0.f) m_rotation.z += TWO_PI;

        // Pitch between -PI/2 and +PI/2
        m_rotation.y += ns::to_radian(static_cast<float>(dy)) * m_sensibility - m_weapon_recoil;
        m_rotation.y = std::max(-HALF_PI, std::min(HALF_PI, m_rotation.y));

        // 3rd person camera
        //if (std::abs(m_rotation.y) < HALF_PI)
        //    m_camera_pos.z += ns::to_radian(static_cast<float>(dy))*0.01f;
        m_weapon_recoil = 0.0f;
    }
}


void Camera::update(Player* player) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
        setBaseFovRad(m_base_fov + PI/180.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
        setBaseFovRad(m_base_fov - PI/180.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        setBaseFovRad(FOV);

    player->physics()->setDirectionAngle(ns::to_degree(getYaw()));

    setPosition3D(
        player->getPosition().x + 0.01f*std::cos(m_rotation.z),
        player->getPosition().y + 0.01f*std::sin(m_rotation.z),
        player->getEyePos()
    );
}
