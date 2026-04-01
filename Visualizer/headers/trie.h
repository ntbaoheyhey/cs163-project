#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string>
#include <iostream>
#include <vector>

#include "global.h"
#include "../objects/canvas.h"

const int LOWERCASE_CHAR = 26;
struct NodeTrie{
    NodeTrie *pnext[LOWERCASE_CHAR];
    int num_word;

    NodeTrie();
};

struct Trie{
    NodeTrie* root;

    Trie();
    
    void add(std::string s);
    bool find(std::string s);
    void remove(std::string s);

    void draw(sf::RenderWindow& window);
};

void trie_page();