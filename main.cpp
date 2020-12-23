#include "Game.hpp"

int main() {

    // load all assets
    ns::Res::load("assets");

    ns::Config::debug.show_fps = true;
    ns::Config::debug.show_text = true;
    ns::Config::debug.show_bounds = false;

    ns::Config::Window::vertical_sync = true;
    ns::Config::Window::update_rate = 60;
    ns::Config::Window::cursor_visible = false;

    // configure some inputs buttons
    ns::Config::Inputs::setButtonKey("droite", sf::Keyboard::Right);

    //Game g;
    //g.run();

    sf::RenderWindow window;
    window.create({1200, 680}, "mode7");
    window.setFramerateLimit(60);

    float w = window.getSize().x;
    float h = window.getSize().y;

    sf::Vector3f pos = {20, 20, 0};

    float angle_deg = 0;
    float fov_deg = 80;

    float far = 0.2f;
    float near = 0.01f;

    sf::Image ground_img;
    ground_img = ns::Res::getTexture("tile.png").copyToImage();

    sf::Color ground_pixels[ground_img.getSize().x * ground_img.getSize().y];
    for (int y = 0; y < ground_img.getSize().y; ++y)
        for (int x = 0; x < ground_img.getSize().x; ++x)
            ground_pixels[x + y*ground_img.getSize().x] = ground_img.getPixel(x, y);

    sf::Image sky_img;
    sky_img = ns::Res::getTexture("sky.png").copyToImage();

    sf::Color sky_pixels[sky_img.getSize().x * sky_img.getSize().y];
    for (int y = 0; y < sky_img.getSize().y; ++y)
        for (int x = 0; x < sky_img.getSize().x; ++x)
            sky_pixels[x + y*sky_img.getSize().x] = sky_img.getPixel(x, y);

    sf::VertexArray ground;
    ground.setPrimitiveType(sf::PrimitiveType::Points);
    ground.resize(window.getSize().x*window.getSize().y/2);

    sf::VertexArray sky;
    sky.setPrimitiveType(sf::PrimitiveType::Points);
    sky.resize(window.getSize().x*window.getSize().y/2);

    for (int y = 0; y < window.getSize().y/2; y+=1)
        for (int x = 0; x < window.getSize().x; x+=1) {
            ground[x + y*window.getSize().x].position = sf::Vector2f(x, y+window.getSize().y/2);
            sky[x + y*window.getSize().x].position = sf::Vector2f(x, y);
        }

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            angle_deg -= 2;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            angle_deg += 2;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            far += 0.1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            far -= 0.1;

        auto&& angle_rad = ns::to_radian(angle_deg);
        sf::Vector2f dir{ std::cos(angle_rad)*0.05f, std::sin(angle_rad)*0.05f };

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            pos.x += dir.x;
            pos.y += dir.y;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            pos.x -= dir.x;
            pos.y -= dir.y;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            pos.y -= dir.x;
            pos.x += dir.y;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            pos.y += dir.x;
            pos.x -= dir.y;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
            fov_deg -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
            fov_deg += 1;


        sf::Clock clk;

        // computes camera frustum
        sf::Vector2f left_cossin{
                std::cos(ns::to_radian(angle_deg - fov_deg/2)),
                std::sin(ns::to_radian(angle_deg - fov_deg/2))
        };
        sf::Vector2f right_cossin{
                std::cos(ns::to_radian(angle_deg + fov_deg/2)),
                std::sin(ns::to_radian(angle_deg + fov_deg/2))
        };

        sf::Vector2f farleft, nearleft, farright, nearright;
        farleft.x = pos.x + left_cossin.x*far;
        farleft.y = pos.y + left_cossin.y*far;

        nearleft.x = pos.x + left_cossin.x*near;
        nearleft.y = pos.y + left_cossin.y*near;

        farright.x = pos.x + right_cossin.x*far;
        farright.y = pos.y + right_cossin.y*far;

        nearright.x = pos.x + right_cossin.x*near;
        nearright.y = pos.y + right_cossin.y*near;

        for (int y = 0; y < h/2; y+=2) {
            sf::Vector2f start, end;
            if (y == 0) {
                start = nearleft;
                end = nearright;
            }
            else {
                float depth = y / (h/2);
                start.x = (farleft.x - nearleft.x) / depth  + nearleft.x;
                start.y = (farleft.y - nearleft.y) / depth  + nearleft.y;

                end.x = (farright.x - nearright.x) / depth + nearright.x;
                end.y = (farright.y - nearright.y) / depth + nearright.y;
            }

            for (int x = 0; x < w; x+=2) {
                float sample_width = x / w;
                float sample_x = std::abs(std::fmod((end.x - start.x) * sample_width + start.x, 1.0f));
                float sample_y = std::abs(std::fmod((end.y - start.y) * sample_width + start.y, 1.0f));

                float u = (float)ground_img.getSize().x*sample_x;
                float v = (float)ground_img.getSize().y*sample_y;
                int iu = int(u);
                int iv = int(v);
                ground[x + y*w].color = ground_pixels[iu + iv*ground_img.getSize().x];
                ground[x + y*w +1].color = ground_pixels[iu + iv*ground_img.getSize().x];
                ground[x + (y+1)*w].color = ground_pixels[iu + iv*ground_img.getSize().x];
                ground[x + (y+1)*w +1].color = ground_pixels[iu + iv*ground_img.getSize().x];

                u = (float)sky_img.getSize().x*sample_x;
                v = (float)sky_img.getSize().y*sample_y;
                iu = int(u);
                iv = int(v);
                sky[sky.getVertexCount() - (w - x + y*w)].color = sky_pixels[iu + iv*sky_img.getSize().x];
                sky[sky.getVertexCount() - (w - x+1 + y*w)].color = sky_pixels[iu + iv*sky_img.getSize().x];
                sky[sky.getVertexCount() - (w - x+1 + (y+1)*w)].color = sky_pixels[iu + iv*sky_img.getSize().x];
                sky[sky.getVertexCount() - (w - x + (y+1)*w)].color = sky_pixels[iu + iv*sky_img.getSize().x];
            }
        }

        ns_LOG(clk.getElapsedTime().asMilliseconds(), "ms");

        window.clear();
        window.draw(sky);
        window.draw(ground);
        window.display();
    }

    // dont forget to unload all the assets
    ns::Res::dispose();
    return 0;
}
