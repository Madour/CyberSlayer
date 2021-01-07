#pragma once

#include <NasNas.h>

enum LevelLayer {
    Ground,
    Walls,
    Ceiling
};

class Level {

    struct Layer {
        Layer() = default;
        ~Layer();
        void create(int grid_size_x, int grid_size_y);
        auto operator()(int x, int y) const -> int;
    private:
        friend Level;
        int* m_tiles = nullptr;
        sf::Vector2i m_grid_size;
    };

public:
    Level();

    void load(const std::string& file_name);
    auto operator[](const LevelLayer& layer_name) const -> const Level::Layer&;

    auto getTileMap() -> ns::tm::TiledMap&;

private:
    ns::tm::TiledMap m_tiledmap;
    Level::Layer m_layers[3];
};