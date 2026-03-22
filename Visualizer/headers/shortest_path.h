#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

#include "global.h"
#include "../objects/canvas.h"


class dijsktra {
public: 
    std::vector<int> dist;
    std::vector<std::vector<std::pair<int, int>>> adj; 
    
    dijsktra(int n);
    void clear();
    void add_edge(int u, int v, int weight);
    void find_shortest_path(int start);
};

void shortest_path_page();