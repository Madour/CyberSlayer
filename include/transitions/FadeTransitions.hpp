#pragma once

#include <NasNas.h>

class FadeOut : public ns::Transition {
    sf::RectangleShape m_bg;
public:
    FadeOut();
    void onUpdate() override;
};

class FadeIn : public ns::Transition {
    sf::RectangleShape m_bg;
public:
    FadeIn();
    void onUpdate() override;
};
