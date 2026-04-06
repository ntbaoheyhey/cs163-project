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

class shortest_path_algorithm {
public: 
    enum AlgorithmType {
        ALGO_DIJKSTRA = 0,
        ALGO_BELLMAN_FORD = 1
    };

    std::vector<int> dist;
    std::vector<std::vector<std::pair<int, int>>> adj; 
    // int what section step is
    std::vector<std::pair<int, Step>> step_history;
    std::vector<std::vector<int>> dist_history;
    std::vector<int> weights;
    std::vector<std::string> labels;
    std::vector<std::string> code {
        "while(!heap.empty()) {             ",
        "   u = heap.top();                 ",
        "   heap.pop();                     ",
        "   for( [v, w] : edges)            ",
        "       if(dist[u] + w > dist[v]) { ",
        "           dist[v] = w + dist[u];  ",
        "           heap.push(v);           ",
        "       }                           ",
        "}                                  "
    };

    inline static const std::vector<std::string> algorithm_names = {
        "Dijkstra",
        "Bellman-Ford"
    };

    inline static const std::vector<std::vector<std::string>> algorithm_code = {
        {
            "while(!heap.empty()) {             ",
            "   u = heap.top();                 ",
            "   heap.pop();                     ",
            "   for( [v, w] : edges)            ",
            "       if(dist[u] + w > dist[v]) { ",
            "           dist[v] = w + dist[u];  ",
            "           heap.push(v);           ",
            "       }                           ",
            "}                                  "
        },
        {
            "for(int it=1; it<n; ++it) {        ",
            "   for(auto& [u,v,id]:edges) {     ",
            "      if(dist[u] != inf and        ",
            "       dist[u] + w[id] < dist[v]) {",
            "         dist[v] = dist[u] + w[id];",
            "      }                            ",
            "   }                               ",
            "}                                  "
        }
    };
    
    shortest_path_algorithm(int n);
    void resize(int n);
    void clear();
    void init(std::vector<std::pair<int,int>> &edges, std::vector<int> &edge_weights, bool directed_flag = false, const std::vector<std::string> &label_map = {});
    int label_to_index(const std::string &label) const;
    void add_edge(int u, int v, int weight, bool directed_flag = false);
    void find_shortest_path(int start);
    void find_shortest_path(int start, int algorithm);
    void find_shortest_path_bellman_ford(int start);
    bool has_negative_weight() const;

    void _visual(Visual_graph &graph, RoundedRectangleShape &visual_code_region, sf::Text &text_for_code, int cur_step);       
};

void shortest_path_page();