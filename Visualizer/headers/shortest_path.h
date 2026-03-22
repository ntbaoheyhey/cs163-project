#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

#include "global.h"
#include "../objects/canvas.h"

struct Step;
class Visual_graph;

class dijsktra {
public: 
    std::vector<int> dist;
    std::vector<std::vector<std::pair<int, int>>> adj; 
    // int what section step is
    std::vector<std::pair<int, Step>> step_history;
    std::vector<int> weights;
    
    dijsktra(int n);
    void resize(int n);
    void clear();
    void init(std::vector<std::pair<int,int>> &edges, std::vector<int> &edge_weights);
    void add_edge(int u, int v, int weight);
    void find_shortest_path(int start);

    void _visual(Visual_graph &graph);       
};

void shortest_path_page();