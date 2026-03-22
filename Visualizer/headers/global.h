#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

const extern unsigned int WINDOW_WIDTH; 
const extern unsigned int WINDOW_HEIGHT;

extern sf::RenderWindow window;

extern sf::Font font_impact;

bool loadFonts();

struct Step {
    std::vector<std::pair<int,int>> nodes_state_changed;
    std::vector<std::pair<int,int>> edges_state_changed;

    Step() {}
};

