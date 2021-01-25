#pragma once

#include <string>
#include <NasNas.h>
#include <SFML/Audio.hpp>

#include "ent/Player.hpp"
#include "Camera.hpp"
#include "Cooldown.hpp"


class Weapon : public ns::Drawable {
public :
    Weapon();

    auto getAmmo() const -> int;
    void setAmmo(int amount);

    auto getDamage() const -> int;
    auto getRange() const -> int;

    void hide();
    void show();
    auto isHidden() -> bool;
    auto isShown() -> bool;

    auto isAttacking() const -> bool;

    virtual auto getFovZoom() -> float { return 1.f; }
    virtual void aim(bool) = 0;
    virtual void attack() = 0;
    virtual void update();

    auto getPosition() const -> sf::Vector2f override;
    auto getGlobalBounds() const -> ns::FloatRect override;

protected :
    int m_hide = true;
    int m_damage;
    int m_range;
    int m_ammo;
    bool m_attacking;
    bool m_aiming;

    Cooldown m_cooldown;

    std::unique_ptr<ns::Spritesheet> m_spritesheet;
    ns::AnimPlayer m_animplayer;
    sf::Sprite m_sprite;

    sf::SoundBuffer m_sound_buffer;
    sf::Sound m_sound;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class Pistol : public Weapon {
public :
    Pistol();
    float getFovZoom() override;

    void aim(bool) override;
    void attack() override;
    void update() override;

private :
    int m_dispersion;
    bool m_aiming;
    float m_recoil;
};


class Rifle : public Weapon {
public :
    Rifle();
    float getFovZoom() override;

    void aim(bool) override;
    void attack() override;
    void update() override;

private :
    int m_dispersion;
    bool m_aiming;
    float m_recoil;
};

class Sniper : public Weapon {
public :
    Sniper();
    float getFovZoom() override;

    void aim(bool) override;
    void attack() override;
    void update() override;

private :
    int m_dispersion;
    bool m_aiming;
    float m_recoil;
};

class Melee : public Weapon {
public :
    Melee();
    float getFovZoom() override;
    void aim(bool) override;
    void attack() override;
    void update() override;
};



class WeaponFactory {
public:
    WeaponFactory() = delete;
    static auto createFromName(const std::string& name) -> Weapon*;

private:
    static std::map<std::string, std::function<Weapon*(void)>> m_data;
};
