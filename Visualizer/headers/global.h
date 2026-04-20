#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <vector>

const extern unsigned int WINDOW_WIDTH; 
const extern unsigned int WINDOW_HEIGHT;

extern sf::RenderWindow window;

extern sf::Font font_impact;

bool loadFonts();
std::filesystem::path resolveAssetPath(const std::string& filename);
bool loadTextureFromAsset(sf::Texture& texture, const std::string& filename);

struct Step {
    std::vector<std::pair<int,int>> nodes_state_changed;
    std::vector<std::pair<int,int>> edges_state_changed;

    Step() {}
};

