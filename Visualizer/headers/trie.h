#pragma once
#include <SFML/Graphics.hpp>    
#include <SFML/System.hpp>
#include <string>
#include <iostream>
#include <vector>

#include "global.h"
#include "../objects/canvas.h"

class node;
class edge;
class RoundedRectangleShape;

const int LOWERCASE_CHAR = 26;
struct NodeTrie{
    NodeTrie *pnext[LOWERCASE_CHAR];
    bool isend;
    int block_need;
    bool isleaf;
    node* visual_node;
    edge* visual_edge[LOWERCASE_CHAR];

    NodeTrie();
};

// Variable
extern RoundedRectangleShape bgmain;
extern const float block_unit;
extern const int block_vertical;
extern const int block_horizontal;
extern int block_width;
extern int block_height;

extern const float node_radius;
extern const float node_outline_thickness;
extern const sf::Color NODE_FILL_COLOR;
extern const sf::Color NODE_OUTLINE_COLOR;
extern const sf::Color NODE_ACTIVE_COLOR;
extern const sf::Color NODE_FOUND_COLOR;
extern const sf::Color NODE_DELETE_COLOR;
extern const sf::Color EDGE_COLOR;

node* create_node(int block_x, int block_y);

struct Trie{
    NodeTrie* root;

    Trie();
    
    // Logic function
    void init();
    void add(std::string s);
    bool find(std::string s);
    void remove(std::string s);

    // Recursive support function
    bool cal_block(NodeTrie *pnode);
    void cre_node(NodeTrie *pnode, int block_x, int block_y, int char_branch = -1);
    void cre_edge(NodeTrie *pnode);
    void drawing(NodeTrie *pnode, sf::RenderWindow& window);

    // Visual function
    void calculate_block();
    void create_visual_node();
    void create_visual_edge();
    void create_visual();

    void draw(sf::RenderWindow& window);
};

void trie_page();