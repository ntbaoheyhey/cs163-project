#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

#include "global.h"
#include "../objects/canvas.h"

struct Step;
class Visual_graph;
class RoundedRectangleShape;

class dijsktra {
public: 
    std::vector<int> dist;
    std::vector<std::vector<std::pair<int, int>>> adj; 
    // int what section step is
    std::vector<std::pair<int, Step>> step_history;
    std::vector<int> weights;
    std::vector<std::string> code {
        "while(!heap.empty()) {             ",
        "   u = heap.top(); heap.pop();     ",
        "   for( [v, w] : edges) {          ",
        "       if(dist[u] + w > dist[v])   ",
        "           dist[v] = w + dist[u];  ",
        "}                                  "
    };
    
    dijsktra(int n);
    void resize(int n);
    void clear();
    void init(std::vector<std::pair<int,int>> &edges, std::vector<int> &edge_weights, bool directed_flag = false);
    void add_edge(int u, int v, int weight, bool directed_flag = false);
    void find_shortest_path(int start);

    void _visual(Visual_graph &graph, RoundedRectangleShape &visual_code_region, sf::Text &text_for_code, int cur_step);       
};

void shortest_path_page();