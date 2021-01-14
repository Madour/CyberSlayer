#include <NasNas/data/Arial.hpp>
#include "ConfigWindow.hpp"

ConfigWindow::ConfigWindow() {
    m_configs["antialiasing"] = 0;
    m_configs["vertical_sync"] = 1;
}

void ConfigWindow::open() {
    sf::RenderWindow window;
    window.create({500, 300}, "CyberSlayer", sf::Style::Titlebar);

    // antialiasing options
    sf::Text antialiasing{"Antialiasing :  ", ns::Arial::getFont()};
    antialiasing.setPosition(45, 35);
    sf::Text antialiasing_options[4];
    for (int i = 0; i < 4; ++i) {
        antialiasing_options[i].setString("x"+std::to_string(i*2));
        antialiasing_options[i].setFont(ns::Arial::getFont());
        antialiasing_options[i].setPosition(antialiasing.getPosition().x+antialiasing.getGlobalBounds().width+50*i,
                                            antialiasing.getPosition().y);
    }
    sf::RectangleShape selected_antialiasing;
    selectOption(selected_antialiasing, antialiasing_options[0]);

    // vertical sync options
    sf::Text vertical_sync{"Vertical Sync : ", ns::Arial::getFont()};
    vertical_sync.setPosition(60, 110);
    sf::Text vertical_sync_options[2];
    {
        sf::Vector2f base_pos{vertical_sync.getPosition().x+vertical_sync.getGlobalBounds().width, vertical_sync.getPosition().y};
        vertical_sync_options[0].setString("ON");
        vertical_sync_options[0].setFont(ns::Arial::getFont());
        vertical_sync_options[0].setPosition(base_pos.x+20, base_pos.y);
        vertical_sync_options[1].setString("OFF");
        vertical_sync_options[1].setFont(ns::Arial::getFont());
        vertical_sync_options[1].setPosition(base_pos.x+80, base_pos.y);
    }
    sf::RectangleShape selected_vertical_sync;
    selectOption(selected_vertical_sync, vertical_sync_options[0]);

    // start button
    sf::Text start{"Start", ns::Arial::getFont(), 50};
    start.setPosition((window.getSize().x - start.getGlobalBounds().width)/2, 180);

    while (window.isOpen()) {
        sf::Vector2f mouse_pos{sf::Mouse::getPosition(window)};
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
                for (int i = 0; i < 4; ++i) {
                    auto& option = antialiasing_options[i];
                    if (option.getGlobalBounds().contains(mouse_pos)) {
                        m_configs["antialiasing"] = 2*i;
                        selectOption(selected_antialiasing, option);
                    }
                }
                for (int i = 0; i < 2; ++i) {
                    auto& option = vertical_sync_options[i];
                    if (option.getGlobalBounds().contains(mouse_pos)) {
                        m_configs["vertical_sync"] = 1-i;
                        selectOption(selected_vertical_sync, option);
                    }
                }
                if (start.getGlobalBounds().contains(mouse_pos)) {
                    window.close();
                }
            }
        }
        window.clear(sf::Color::Black);

        window.draw(antialiasing);
        for (const auto& text : antialiasing_options)
            window.draw(text);
        window.draw(selected_antialiasing);

        window.draw(vertical_sync);
        for (const auto& text : vertical_sync_options)
            window.draw(text);
        window.draw(selected_vertical_sync);

        window.draw(start);
        window.display();
    }
}

auto ConfigWindow::getConfig(const std::string& config_name) const -> int {
    return m_configs.at(config_name);
}

void ConfigWindow::selectOption(sf::RectangleShape& rect, const sf::Text& text) {
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(2.f);
    rect.setFillColor(sf::Color::Transparent);
    rect.setSize({text.getGlobalBounds().width+10,
                  text.getGlobalBounds().height+text.getLocalBounds().top*2});
    rect.setPosition(text.getPosition().x-5, text.getPosition().y);
}
