#pragma once

#include <NasNas.h>

constexpr int VIEW_WIDTH = 1200;
constexpr int VIEW_HEIGHT = 670;
constexpr float VIEW_RATIO = static_cast<float>(VIEW_WIDTH)/static_cast<float>(VIEW_HEIGHT);
constexpr float METER = 1.f / 5.f;
constexpr float WALL_HEIGHT = 6*METER;
constexpr float FOV = 90.f;

constexpr float GRAVITY = 2;

static const float UPS = static_cast<float>(ns::Config::Window::update_rate);

