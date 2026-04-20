#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <filesystem>

#include "global.h"
// include cả canva
#include "../objects/canvas.h"
#include "trie.h"
#include "shortest_path.h"
#include "singly_linked_list.h"

void loadMuisc();

void openWindow();

void main_menu_page();

void option_page();

void setting_page();
