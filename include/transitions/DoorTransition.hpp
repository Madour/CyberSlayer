#pragma once

#include <NasNas.h>

class DoorOpenTransition : public ns::Transition {
    sf::RectangleShape m_right;
    sf::RectangleShape m_left;
public:
    DoorOpenTransition();
    void onUpdate() override;
};

