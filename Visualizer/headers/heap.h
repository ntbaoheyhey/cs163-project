#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include "pseudocode.h"
#include "global.h"
#include "../objects/canvas.h"

void heap_page();

// Để làm gì đây?
sf::Vector2f getHeapNodePosition(int heapIndex, float windowWidth, float startY, float verticalSpacing);
