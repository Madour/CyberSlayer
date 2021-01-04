#pragma once

#include <NasNas.h>

constexpr int VIEW_WIDTH = 960;//1200;
constexpr int VIEW_HEIGHT = 540;//670;
constexpr float VIEW_RATIO = static_cast<float>(VIEW_WIDTH)/static_cast<float>(VIEW_HEIGHT);
constexpr float METER = 1.f / 5.f;
constexpr float WALL_HEIGHT = 5*METER;

constexpr float PI = 3.14159f;
constexpr float TWO_PI = 2.f*PI;
constexpr float HALF_PI = PI/2.f;

constexpr float FOV = PI / 2.f;

constexpr float GRAVITY = 1.5;

static const float UPS = static_cast<float>(ns::Config::Window::update_rate);

