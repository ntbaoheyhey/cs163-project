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

node* create_node(int block_x, int block_y);

struct Trie{
    NodeTrie* root;

    Trie();
    
    // Logic function
    void init();
    void add(std::string s);
    bool find(std::string s);
    void remove(std::string s);
    void clear();

    // Recursive support function
    bool cal_block(NodeTrie *pnode);
    void cre_node(NodeTrie *pnode, int block_x, int block_y, int char_branch = -1);
    void cre_edge(NodeTrie *pnode);
    void drawing(NodeTrie *pnode, sf::RenderWindow& window);
    void clear_travese(NodeTrie *pnode, bool isRoot = false);

    // Visual function
    void calculate_block();
    void create_visual_node();
    void create_visual_edge();
    void create_visual();

    void draw(sf::RenderWindow& window);
};

void trie_page();