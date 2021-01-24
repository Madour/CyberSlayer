#include "transitions/FadeTransitions.hpp"

FadeOut::FadeOut() {
    // create the transition background
    m_bg.setSize(app->getWindow().getAppView().getSize());
    m_bg.setFillColor(sf::Color::Transparent);
    addShape(m_bg);
}
void FadeOut::onUpdate() {
    // update background opacity
    m_bg.setFillColor(m_bg.getFillColor()+sf::Color(0, 0, 0, 1));
    // when the background is opaque, end the transition
    if (m_bg.getFillColor().a >= 255)
        end();
}

FadeIn::FadeIn() {
    // create the transition background
    m_bg.setSize(app->getWindow().getAppView().getSize());
    m_bg.setFillColor(sf::Color::Black);
    addShape(m_bg);
}
void FadeIn::onUpdate() {
    // update background opacity
    m_bg.setFillColor(m_bg.getFillColor()-sf::Color(0, 0, 0, 1));
    // when the background is transparent, end the transition
    if (m_bg.getFillColor().a <= 0)
        end();
}