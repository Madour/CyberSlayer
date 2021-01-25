#pragma once

#include <NasNas.h>
#include "Item.hpp"

enum LevelLayer {
    Ground,
    Walls,
    Ceiling
};

class Level {

    class Layer {
    public:
        ~Layer();
        auto operator()(int x, int y) const -> int;
    private:
        friend Level;
        Layer() = default;
        void create(int grid_size_x, int grid_size_y);
        int* m_tiles = nullptr;
        sf::Vector2i m_grid_size;
    };

public:
    Level();

    void load(const std::string& file_name);
    auto operator[](const LevelLayer& layer_name) const -> const Level::Layer&;

    auto getTileMap() -> ns::tm::TiledMap&;
    static auto getCollisions() -> const std::vector<sf::FloatRect>&;
    static auto getItems() -> std::vector<Item*>&;
    static auto getEnemies() -> std::vector<LevelObject*>&;

private:
    static std::vector<sf::FloatRect> collisions;
    static std::vector<Item*> items;
    static std::vector<LevelObject*> enemies;
    std::vector<std::unique_ptr<LevelObject>> m_objects;
    ns::tm::TiledMap m_tiledmap;
    Level::Layer m_layers[3];
};
