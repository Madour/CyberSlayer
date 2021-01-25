#pragma once

#include <NasNas.h>

class Cooldown {
public:
    Cooldown() = default;
    explicit Cooldown(int duration_ms);

    auto getDuration() const -> int;
    void setDuration(int duration_ms);

    auto getRemainingTime() const -> int;
    void reset();

    auto isReady() const -> bool;
    void setReady(bool);

private:
    sf::Clock m_clock;
    int m_duration_ms = 0;
    bool m_ready = false;
};
