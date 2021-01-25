#pragma once

#include <NasNas.h>
#include <SFML/Audio.hpp>

using Inputs = ns::Config::Inputs;

class Game;

class GameState {
public:
    friend Game;

    static Game* game;

    virtual ~GameState() = default;

    virtual void init() = 0;

    virtual void onEvent(const sf::Event& event) = 0;

    virtual void update() = 0;

    virtual void preRender() {}

};
