#include "transitions/DoorTransition.hpp"

DoorOpenTransition::DoorOpenTransition() {
    m_left.setSize({app->getWindow().getAppView().getSize().x/2, app->getWindow().getAppView().getSize().y});
    m_left.setFillColor(sf::Color::Black);

    m_right.setSize({app->getWindow().getAppView().getSize().x/2, app->getWindow().getAppView().getSize().y});
    m_right.setPosition(m_right.getSize().x, 0);
    m_right.setFillColor(sf::Color::Black);

    addShape(m_left);
    addShape(m_right);
}

void DoorOpenTransition::onUpdate() {
    m_left.move(-4, 0);
    m_right.move(4, 0);
    if (m_left.getPosition().x < -m_left.getSize().x)
        end();
}
