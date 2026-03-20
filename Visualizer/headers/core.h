#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

#include "../objects/box.h"
#include "shortest_path.h"

const unsigned int WINDOW_WIDTH = 1800;
const unsigned int WINDOW_HEIGHT = 1200;

extern sf::RenderWindow window;

void openWindow();

void main_menu_page();

void option_page();
