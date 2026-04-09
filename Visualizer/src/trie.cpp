#include "../headers/trie.h"

// Global variable definitions
RoundedRectangleShape bgmain({800.f, 600.f});
const float block_unit = 40.f;
const int block_vertical = 2;
const int block_horizontal = 1;
int block_width = 0;
int block_height = 0;

const float node_radius = block_unit / 2;
const float node_outline_thickness = -3.f;
const sf::Color NODE_FILL_COLOR = sf::Color::Blue;  // sf::Color(218, 168, 74);
const sf::Color NODE_OUTLINE_COLOR = sf::Color::White; // sf::Color(202, 148, 95);
const sf::Color NODE_ACTIVE_COLOR = sf::Color(62, 188, 167);
const sf::Color NODE_FOUND_COLOR = sf::Color(156, 229, 147);
const sf::Color NODE_DELETE_COLOR = sf::Color(246, 88, 88);
const sf::Color EDGE_COLOR = sf::Color::White;

void trie_page(){
    
    button add_button(WINDOW_WIDTH - 300, 50, 200, 50, sf::Color(232, 183, 81), "Add", 24);
    button delete_button(WINDOW_WIDTH - 300, 150, 200, 50, sf::Color(232, 183, 81), "Delete", 24);
    button find_button(WINDOW_WIDTH - 300, 250, 200, 50, sf::Color(232, 183, 81), "Find", 24);

    bgmain.setSize({800.f, 600.f});    
    bgmain.setPosition({80 , 50});
    bgmain.setFillColor(sf::Color(243, 243, 251, 100));
    bgmain.setRadius(17);
    bgmain.setOutlineThickness(5);
    bgmain.setOutlineColor(sf::Color(217, 211, 209));
    
    block_width = int(bgmain.getSize().x / block_unit);
    block_height = int(bgmain.getSize().y / block_unit);
    std::cerr << "Rectangle -> Block : " << block_width << " " << block_height << "\n";
    
    node* bennode = nullptr;
    bennode = create_node(block_width / 2, 0);

    RoundedRectangleShape bgcode({350.f, 200.f});
    bgcode.setPosition({float(WINDOW_WIDTH - 300), float(WINDOW_HEIGHT - 250)});
    bgcode.setFillColor(sf::Color(243, 243, 251, 100));
    bgcode.setRadius(17);
    bgcode.setOutlineThickness(5);
    bgcode.setOutlineColor(sf::Color(217, 211, 209));
    
    // string input box
    box input_box(WINDOW_WIDTH - 300, 350, 200, 50, sf::Color::Cyan, "Type here", 24);
    
    // Trie data
    Trie data;
    data.init();
    data.add("apple");
    data.add("bar");
    data.add("crash");
    data.add("car");
    data.add("e");
    data.add("f");
    data.add("cringe");

    data.create_visual();

    int frame_count = 0;

    while(window.isOpen()){
        while(const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>())
                window.close();
        }

        ++frame_count;
        std::cerr << "Frame :" << frame_count << "\n";
        
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (add_button.update(mousePos)) {
            std::cout << "Received Add Query\n";
        }
        if (delete_button.update(mousePos)) {
            std::cout << "Received Delete Query\n";
        }
        if (find_button.update(mousePos)) {
            std::cout << "Received Find Query\n";
        }
        
        window.clear(sf::Color(212, 188, 112, 0.71));
        data.draw(window);
        add_button.draw(window);
        delete_button.draw(window);
        find_button.draw(window);
        window.draw(bgmain);
        window.draw(bgcode);
        input_box.draw(window);
        
        window.display();
    }
}

node* create_node(int block_x, int block_y){
    float x, y;
    x = block_unit * block_x;
    y = block_unit * block_y;
    x = x + bgmain.getPosition().x;
    y = y + bgmain.getPosition().y;
    x = x + block_unit / 2;
    y = y + block_unit / 2;
    node* getnode = new node(x, y, node_radius, NODE_FILL_COLOR, NODE_OUTLINE_COLOR, node_outline_thickness);
    return getnode;
}

Trie::Trie()
{
    root = new NodeTrie();
}

void Trie::init()
{
    root = new NodeTrie();   
}

void Trie::add(std::string s)
{
    NodeTrie* pnow = root;
    for(int i = 0; i < s.size(); ++i){
        int id = s[i] - 'a';
        if(pnow->pnext[id] == nullptr){
            pnow->pnext[id] = new NodeTrie();
        }
        pnow = pnow->pnext[id];
    }
    pnow->num_word++;
    // check current node is leaf or not
    bool ok = true;
    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnow->pnext[i] != nullptr){
        ok = false;
        break;
    }
    if(ok) pnow->isleaf = true;
    return;
}

bool Trie::find(std::string s)
{
    NodeTrie* pnow = root;
    for(int i = 0; i < s.size(); ++i){
        int id = s[i] - 'a';
        if(pnow->pnext[id] == nullptr){
            return false;
        }
        pnow = pnow->pnext[id];
    }
    return true;
}

void Trie::remove(std::string s)
{
    // Already check
    if(!find(s)) return;
    NodeTrie* pnow = root;
    for(int i = 0; i < s.size(); ++i){
        int id = s[i] - 'a';
        pnow = pnow->pnext[id];
    }
    pnow->num_word--;
    return;
}

bool Trie::cal_block(NodeTrie *pnode)
{
    bool isleaf = true;
    bool previous_leaf = false; bool previous = false;
    pnode->block_need = 0;

    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnode->pnext[i] != nullptr){
        isleaf = false;
        bool current_leaf = cal_block(pnode->pnext[i]);
        pnode->block_need += pnode->pnext[i]->block_need;
        if(previous)
            pnode->block_need += (block_horizontal + 1);
        previous = true;
    }

    if(isleaf == true){
        pnode->block_need = 1;
    }

    return isleaf;
}

void Trie::cre_node(NodeTrie *pnode, int block_x, int block_y)
{
    // Create node
    pnode->visual_node = create_node(block_x, block_y);
    std::cerr << block_x << " " << block_y << "\n";

    // Calculation
    int run_block = block_x - pnode->block_need / 2;
    bool previous_leaf = false, previous = false;
    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnode->pnext[i] != nullptr){
        bool current_leaf = pnode->pnext[i]->isleaf;
        if(previous)
            run_block = run_block + (block_horizontal + 1);
        int l = run_block;
        int r = run_block + pnode->pnext[i]->block_need - 1;
        cre_node(pnode->pnext[i], (l + r) / 2, block_y + block_vertical);
        run_block = run_block + pnode->pnext[i]->block_need;
        previous = true;
    }
}

void Trie::cre_edge(NodeTrie *pnode)
{
    if(pnode->isleaf) return;
    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnode->pnext[i] != nullptr){
        // Create edge
        std::cerr << i << "\n";
        float x_cur = pnode->visual_node->getPosition().x;
        float y_cur = pnode->visual_node->getPosition().y;
        float x_nxt = pnode->pnext[i]->visual_node->getPosition().x;
        float y_nxt = pnode->pnext[i]->visual_node->getPosition().y;
        pnode->visual_edge[i] = new edge(x_cur, y_cur, x_nxt, y_nxt, EDGE_COLOR);
        pnode->visual_edge[i]->setPoints(x_cur, y_cur, x_nxt, y_nxt, false, node_radius);
        // Traverse
        cre_edge(pnode->pnext[i]);
    }
}

void Trie::drawing(NodeTrie *pnode, sf::RenderWindow& window)
{
    // Draw edge
    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnode->pnext[i] != nullptr){
        pnode->visual_edge[i]->draw(window);
    }
    // Draw node
    pnode->visual_node->draw(window);
    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnode->pnext[i] != nullptr){
        drawing(pnode->pnext[i], window);
    }
    
}

void Trie::calculate_block()
{
    cal_block(root);
}

void Trie::create_visual_node()
{
    cre_node(root, block_width / 2, 0);
}

void Trie::create_visual_edge()
{
    cre_edge(root);
}

void Trie::create_visual()
{
    calculate_block();
    create_visual_node();
    create_visual_edge();
}

void Trie::draw(sf::RenderWindow &window)
{
    drawing(root, window);
}

NodeTrie::NodeTrie()
{
    for(int i = 0; i < LOWERCASE_CHAR; ++i){
        pnext[i] = nullptr;
    }
    num_word = 0;
    block_need = 0;
    isleaf = false;
}
