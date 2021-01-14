#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

class ConfigWindow {
public:
    ConfigWindow();
    void open();
    auto getConfig(const std::string& config_name) const -> int;
private:
    static void selectOption(sf::RectangleShape& rect, const sf::Text& text);
    std::unordered_map<std::string, int> m_configs;
};
