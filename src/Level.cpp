#include "Level.hpp"


void Level::Layer::create(int grid_size_x, int grid_size_y) {
    m_grid_size.x = grid_size_x;
    m_grid_size.y = grid_size_y;
    if (m_tiles != nullptr)
        delete[](m_tiles);
    m_tiles = new int[grid_size_x*grid_size_y];
}

Level::Layer::~Layer() {
    delete[](m_tiles);
}

auto Level::Layer::operator()(int x, int y) const -> int {
    if (x < 0 || x >= m_grid_size.x || y < 0 || y >= m_grid_size.y)
        return 0;
    return m_tiles[x + y*m_grid_size.x];
}

Level::Level() = default;

void Level::load(const std::string& file_name) {
    m_tiledmap.loadFromFile(file_name);
    auto& map_size = m_tiledmap.getDimension();

    auto& ground_tile_layer = m_tiledmap.getTileLayer("ground");
    auto& walls_tile_layer = m_tiledmap.getTileLayer("walls");
    auto& ceiling_tile_layer = m_tiledmap.getTileLayer("ceiling");

    m_layers[LevelLayer::Ground].create(map_size.x, map_size.y);
    m_layers[LevelLayer::Walls].create(map_size.x, map_size.y);
    m_layers[LevelLayer::Ceiling].create(map_size.x, map_size.y);

    for (unsigned y = 0; y < map_size.y; ++y) {
        for (unsigned x = 0; x < map_size.x; ++x) {
            m_layers[LevelLayer::Ground].m_tiles[x + y * map_size.x] = ground_tile_layer->getTile(x, y).gid;
            m_layers[LevelLayer::Walls].m_tiles[x + y * map_size.x] = walls_tile_layer->getTile(x, y).gid;
            m_layers[LevelLayer::Ceiling].m_tiles[x + y * map_size.x] = ceiling_tile_layer->getTile(x, y).gid;
        }
    }
}

auto Level::operator[](const LevelLayer& layer_name) const -> const Level::Layer& {
    return m_layers[layer_name];
}

auto Level::getTileMap() -> ns::tm::TiledMap& {
    return m_tiledmap;
}
