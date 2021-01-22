#pragma once

#include <array>
#include <stack>
#include <NasNas.h>

#include "Constants.hpp"
#include "Camera.hpp"
#include "Level.hpp"
#include "LevelObject.hpp"
#include "ent/Adventurer.hpp"
#include "ent/Player.hpp"
#include "Weapon.hpp"


class Game : public ns::App {
private:
    // private structs used by the ray caster
    enum class Side { Top, Right, Bottom, Left };
    struct WallHit {
        sf::Vector2f ray_dir;
        float fisheye_correction=1.f;
        float distance=0.f;
        sf::Vector2f point;
        Side side=Side::Top;
        float tex_coo=0.f;
        unsigned tile_gid=0;
    };
    struct SpriteHit{
        LevelObject* sprite=nullptr;
        float distance=0.f;
        float t_min=1.f;
        float t_max=0.f;
        unsigned ray_min=VIEW_WIDTH;
        unsigned ray_max=0;
        bool visible=false;
    };
public:
    Game();
    void onEvent(const sf::Event& event) override;
    void update() override;
    void preRender() override;

    void doRayCast();

    // level data
    Level m_level;
    sf::Vector2i m_level_size;

    // tileset data
    float m_tile_size;
    sf::Image m_tileset_image;
    const sf::Uint8* m_tileset_pixels;
    sf::Vector2u m_tileset_size;
    std::vector<ns::FloatRect> m_tile_texture_rect;

    Player* m_player;
    std::vector<std::unique_ptr<LevelObject>> m_level_objects;

    // camera data
    Camera m_camera;
    float m_horizon;

    // ray caster data
    float m_max_depth;
    std::array<float, VIEW_WIDTH> m_depth_buffer{};
    std::array<std::stack<WallHit>, VIEW_WIDTH> m_wall_hits_buffer;
    std::vector<SpriteHit> m_sprite_hits_buffer;

    // ray caster drawables
    ns::VertexArray m_background;
    ns::VertexArray m_sprites_quads;
    sf::Uint8* m_transparency_mask;
    sf::Texture m_transparency_mask_texture;
    sf::Sprite m_transparency_mask_sprite;
    sf::Uint8* m_framebuffer;
    sf::Texture m_framebuffer_texture;
    sf::Sprite m_framebuffer_sprite;

    // HUD drawables
    sf::RectangleShape m_hp_bar;
    sf::RectangleShape m_minimap_bg;

    // minimap drawables
    ns::VertexArray m_minimap_entities;
    ns::VertexArray m_minimap_rays;
    ns::VertexArray m_minimap_grid;

    // Weapon
    int m_weapon_selector;
    int m_number_weapon;

    Weapon* m_current_weapon;
    Pistol m_laser_pistol;
    Rifle m_laser_rifle;
    sf::Sprite m_gun_sprite;
};
