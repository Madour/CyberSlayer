#include "Cooldown.hpp"

Cooldown::Cooldown(int duration_ms) {
    setDuration(duration_ms);
}

auto Cooldown::getDuration() const -> int {
    return m_duration_ms;
}

void Cooldown::setDuration(int duration_ms) {
    m_duration_ms = duration_ms;
    reset();
}

auto Cooldown::getRemainingTime() const -> int {
    if (isReady())
        return  0;
    return m_duration_ms - m_clock.getElapsedTime().asMilliseconds();
}

void Cooldown::reset() {
    m_clock.restart();
    m_ready = false;
}

auto Cooldown::isReady() const -> bool {
    return (m_ready || m_clock.getElapsedTime().asMilliseconds() > m_duration_ms);
}

void Cooldown::setReady(bool v) {
    m_ready = v;
}
