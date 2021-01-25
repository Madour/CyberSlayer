#include "Weapon.hpp"

Weapon::Weapon() {
    m_attacking = false;
}

auto Weapon::getAmmo() const -> int {
    return m_ammo;
}

void Weapon::setAmmo(int amo_amount) {
    m_ammo = amo_amount;
}

auto Weapon::getDamage() const -> int {
    return m_damage;
}

auto Weapon::getRange() const -> int {
    return m_range;
}

auto Weapon::isAttacking() const -> bool {
    return m_attacking;
}

void Weapon::hide() {
    m_hide = true;
}

void Weapon::show() {
    m_hide = false;
}

auto Weapon::isHidden() -> bool {
    return m_sprite.getPosition().y > VIEW_HEIGHT + m_sprite.getGlobalBounds().height;
}

auto Weapon::isShown() -> bool {
    return m_sprite.getPosition().y == VIEW_HEIGHT;
}

auto Weapon::getPosition() const -> sf::Vector2f {
    return m_sprite.getPosition();
}

auto Weapon::getGlobalBounds() const -> ns::FloatRect {
    return ns::FloatRect(m_sprite.getGlobalBounds());
}

void Weapon::update() {
    if (m_hide && !isHidden()) {
        m_sprite.move(0, 25);
    }
    else if (!m_hide && !isShown()) {
        m_sprite.move(0, -25);
        if (m_sprite.getPosition().y < VIEW_HEIGHT)
            m_sprite.setPosition(m_sprite.getPosition().x, VIEW_HEIGHT);
    }
    m_attacking = false;
}

void Weapon::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_sprite, states);
}


Pistol::Pistol() {
    m_ammo = 50;
    m_damage = 10;
    m_range = 50;
    m_cooldown.setDuration(500);

    m_dispersion = 10;
    m_aiming = false;
    m_recoil = 0.07f;

    // create spritesheet
    m_spritesheet = std::make_unique<ns::Spritesheet>("pistol", ns::Res::in("sprites").getTexture("laser_pistol.png"));
    m_spritesheet->setGrid({125, 150}, 2);

    m_spritesheet->addAnim("idle", 0, 1, 100, {-43, 150});
    m_spritesheet->getAnim("idle").loop = false;

    m_spritesheet->addAnim("shoot", 1, 1, 100, {0, 150});
    m_spritesheet->getAnim("shoot").loop = false;

    m_spritesheet->addAnim("aim", 2, 1, 100, {23, 150});
    m_spritesheet->getAnim("aim").loop = false;

    m_spritesheet->addAnim("aimshoot", 3, 1, 100, {69, 150});
    m_spritesheet->getAnim("aimshoot").loop = false;

    // setup animation player and sprite
    m_animplayer.play(m_spritesheet->getAnim("idle"));

    m_sprite.setTexture(*m_spritesheet->texture);
    m_sprite.setTextureRect(m_animplayer.getActiveFrame().rectangle);
    m_sprite.setScale(2.3f, 2.3f);
    m_sprite.setPosition(VIEW_WIDTH/2, VIEW_HEIGHT+m_sprite.getGlobalBounds().height);

    // load weapon sounds
    m_sound_buffer.loadFromFile("assets/laser_pistol.wav");
    m_sound.setBuffer(m_sound_buffer);
}

float Pistol::getFovZoom() {
    return m_aiming ? 1.2f : 1.f;
}

void Pistol::aim(bool v) {
    m_aiming = v;
}

void Pistol::attack() {
    if (m_cooldown.isReady() && m_ammo > 0) {
        if (m_aiming)
            m_animplayer.play(m_spritesheet->getAnim("aimshoot"));
        else
            m_animplayer.play(m_spritesheet->getAnim("shoot"));

        m_attacking = true;
        m_sound.play();
        m_cooldown.reset();
        m_ammo--;
    }
}

void Pistol::update() {
    Weapon::update();

    if (!m_animplayer.isPlaying()) {
        if (m_aiming)
            m_animplayer.play(m_spritesheet->getAnim("aim"));
        else
            m_animplayer.play(m_spritesheet->getAnim("idle"));
    }

    m_animplayer.update(m_sprite);
}

Rifle::Rifle() {
    m_ammo = 500;
    m_damage = 3;
    m_range = 30;
    m_cooldown.setDuration(100);

    m_dispersion = 10;
    m_aiming = false;
    m_recoil = 0.f;

    // create spritesheet
    m_spritesheet = std::make_unique<ns::Spritesheet>("pistol", ns::Res::in("sprites").getTexture("laser_rifle.png"));
    m_spritesheet->setGrid({200, 257}, 2);

    m_spritesheet->addAnim("idle", 0, 1, 50, {20, 257});
    m_spritesheet->getAnim("idle").loop = false;

    m_spritesheet->addAnim("shoot", 1, 1, 50, {20 , 257});
    m_spritesheet->getAnim("shoot").loop = false;

    m_spritesheet->addAnim("aim", 2, 1, 50, {97, 257});
    m_spritesheet->getAnim("aim").loop = false;

    m_spritesheet->addAnim("aimshoot", 3, 1, 50, {109, 257});
    m_spritesheet->getAnim("aimshoot").loop = false;

    // setup animation player and sprite
    m_animplayer.play(m_spritesheet->getAnim("idle"));

    m_sprite.setTexture(*m_spritesheet->texture);
    m_sprite.setTextureRect(m_animplayer.getActiveFrame().rectangle);
    m_sprite.setScale(2.f, 2.f);
    m_sprite.setPosition(VIEW_WIDTH/2, VIEW_HEIGHT+m_sprite.getGlobalBounds().height);

    // load weapon sounds
    m_sound_buffer.loadFromFile("assets/laser_rifle.wav");
    m_sound.setBuffer(m_sound_buffer);
}

float Rifle::getFovZoom() {
    return m_aiming ? 3.f : 1.f;
}

void Rifle::aim(bool v) {
    m_aiming = v;
}

void Rifle::attack() {
    if (m_cooldown.isReady() && m_ammo > 0) {
        if (m_aiming)
            m_animplayer.play(m_spritesheet->getAnim("aimshoot"));
        else
            m_animplayer.play(m_spritesheet->getAnim("shoot"));

        m_attacking = true;
        m_sound.play();
        m_cooldown.reset();
        m_ammo--;
    }
}

void Rifle::update() {
    Weapon::update();

    if (!m_animplayer.isPlaying()) {
        if (m_aiming)
            m_animplayer.play(m_spritesheet->getAnim("aim"));
        else
            m_animplayer.play(m_spritesheet->getAnim("idle"));
    }

    m_animplayer.update(m_sprite);
}

Sniper::Sniper() {
    m_ammo = 500;
    m_damage = 30;
    m_range = 30;
    m_cooldown.setDuration(2100);

    m_dispersion = 10;
    m_aiming = false;
    m_recoil = 0.f;

    // create spritesheet
    m_spritesheet = std::make_unique<ns::Spritesheet>("pistol", ns::Res::in("sprites").getTexture("sniper.png"));
    m_spritesheet->setGrid({250, 250}, 6);

    m_spritesheet->addAnim("idle", 1, 1, 50, {25, 220});
    m_spritesheet->getAnim("idle").loop = false;

    m_spritesheet->addAnim("shoot", 0, 5, {250, 200, 200, 200, 200}, {{22 , 220}, {25, 220}, {25, 220}, {25, 220}, {25, 220}});
    m_spritesheet->getAnim("shoot").loop = false;

    m_spritesheet->addAnim("aim", 5, 1, 50, {125, 250});
    m_spritesheet->getAnim("aim").loop = false;

    m_spritesheet->addAnim("aimshoot", {0, 2, 3, 4, 1}, {250, 200, 200, 200, 200}, {25, 200});
    m_spritesheet->getAnim("aimshoot").loop = false;

    // setup animation player and sprite
    m_animplayer.play(m_spritesheet->getAnim("idle"));

    m_sprite.setTexture(*m_spritesheet->texture);
    m_sprite.setTextureRect(m_animplayer.getActiveFrame().rectangle);
    m_sprite.setScale(2.f, 2.f);
    m_sprite.setPosition(VIEW_WIDTH/2, VIEW_HEIGHT+m_sprite.getGlobalBounds().height);

    // load weapon sounds
    m_sound_buffer.loadFromFile("assets/sniper.wav");
    m_sound.setBuffer(m_sound_buffer);
}

float Sniper::getFovZoom() {
    if (m_animplayer.getAnim()->getName() == "aimshoot")
        return 1.f;
    else
        return m_aiming ? 6.f : 1.f;
}

void Sniper::aim(bool v) {
    m_aiming = v;
}

void Sniper::attack() {
    if (m_cooldown.isReady() && m_ammo > 0) {
        if (m_aiming)
            m_animplayer.play(m_spritesheet->getAnim("aimshoot"));
        else
            m_animplayer.play(m_spritesheet->getAnim("shoot"));

        m_attacking = true;
        m_sound.play();
        m_cooldown.reset();
        m_ammo--;
    }
}

void Sniper::update() {
    Weapon::update();

    if (!m_animplayer.isPlaying()) {
        if (m_aiming)
            m_animplayer.play(m_spritesheet->getAnim("aim"));
        else
            m_animplayer.play(m_spritesheet->getAnim("idle"));
    }

    m_animplayer.update(m_sprite);
}

Melee::Melee() {
    m_ammo = 1;
    m_damage = 7;
    m_range = 30;
    m_cooldown.setDuration(150*4);

    m_aiming = false;

    // create spritesheet
    m_spritesheet = std::make_unique<ns::Spritesheet>("pistol", ns::Res::in("sprites").getTexture("melee.png"));
    m_spritesheet->setGrid({250, 250}, 6);

    m_spritesheet->addAnim("idle", 0, 1, 50, {165, 250});
    m_spritesheet->getAnim("idle").loop = false;

    m_spritesheet->addAnim("attack", {1, 2, 1, 0}, 150, {165, 250});
    m_spritesheet->getAnim("attack").loop = false;

    // setup animation player and sprite
    m_animplayer.play(m_spritesheet->getAnim("idle"));

    m_sprite.setTexture(*m_spritesheet->texture);
    m_sprite.setTextureRect(m_animplayer.getActiveFrame().rectangle);
    m_sprite.setScale(2.f, 2.f);
    m_sprite.setPosition(VIEW_WIDTH/2, VIEW_HEIGHT+m_sprite.getGlobalBounds().height);

    // load weapon sounds
    m_sound_buffer.loadFromFile("assets/melee.wav");
    m_sound.setBuffer(m_sound_buffer);
}

float Melee::getFovZoom() {
    if (m_animplayer.getAnim()->getName() == "aimshoot")
        return 1.f;
    else
        return m_aiming ? 6.f : 1.f;
}

void Melee::aim(bool v) {
}

void Melee::attack() {
    if (m_cooldown.isReady() && m_ammo > 0) {
        m_animplayer.play(m_spritesheet->getAnim("attack"));

        m_attacking = true;
        m_sound.play();
        m_cooldown.reset();
    }
}

void Melee::update() {
    Weapon::update();

    if (!m_animplayer.isPlaying()) {
        m_animplayer.play(m_spritesheet->getAnim("idle"));
    }

    m_animplayer.update(m_sprite);
}

std::map<std::string, std::function<Weapon*(void)>> WeaponFactory::m_data = {
        {"Pistol", []{return new Pistol();}},
        {"Rifle", []{return new Rifle();}},
        {"Sniper", []{return new Sniper();}},
        {"Melee", []{return new Melee();}},
};

auto WeaponFactory::createFromName(const std::string& name) -> Weapon* {
    if (m_data.count(name) > 0)
        return m_data.at(name)();
    return nullptr;
}
