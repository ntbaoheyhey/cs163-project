#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

#include "global.h"
#include "../objects/canvas.h"

void heap_page();

sf::Vector2f getHeapNodePosition(int heapIndex, float windowWidth, float startY, float verticalSpacing);
