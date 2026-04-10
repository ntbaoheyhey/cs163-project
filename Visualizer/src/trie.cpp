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
    box input_box(WINDOW_WIDTH - 300, 350, 200, 50, sf::Color(232, 183, 81), "   Type here", 24);
    
    // Trie data
    Trie data;
    data.init();
    // data.add("say");
    // data.add("sayy");
    // data.add("crash");
    // data.add("car");
    // data.add("e");
    // data.add("f");
    // data.add("cringe");
    data.create_visual(); // Must have

    int frame_count = 0;
    
    bool left_mouse = false;
    bool left_mouse_last = false;

    std::string current_input = "";
    bool input_box_active = false;

    while(window.isOpen()){
        // 1. Mouse information
        left_mouse = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        // 2. Mouse inside button / box region
        if(!input_box_active)
            input_box.update(input_box_active, mousePos);
        add_button.update(mousePos);
        delete_button.update(mousePos);
        find_button.update(mousePos);

        // 3. Left-Mouse click handle
        if (left_mouse && !left_mouse_last) { 
            // Button
            bool button_pressed = false;
            if (add_button.contains(mousePos)) {
                std::cout << "Received Add Query: " << current_input << "\n";
                if (!current_input.empty()) {data.add(current_input); data.create_visual();}
                button_pressed = true;
            }
            if (delete_button.contains(mousePos)) {
                std::cout << "Received Delete Query: " << current_input << "\n";
                if (!current_input.empty()) {data.remove(current_input); data.create_visual();}
                button_pressed = true;
            }
            if (find_button.contains(mousePos)) {
                std::cout << "Received Find Query: " << current_input << "\n";
                button_pressed = true;
            }

            if(input_box.contains(mousePos)){
                std::cout << "Received Input Box Activation: On" << "\n";
                input_box_active = true;
                input_box.update(input_box_active, mousePos);
                if(current_input.empty()){
                    input_box.setLabel("|");
                }
            } else
            if(!button_pressed && input_box_active){
                std::cout << "Received Input Box Activation: Off" << "\n";
                input_box_active = false;
                if(current_input.empty()){
                    input_box.setLabel("   Type here");
                }
            }

        }
        // Lưu lại cờ click cho frame sau
        left_mouse_last = left_mouse;

        // Receive event of present frame
        while(const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>())
                window.close();
            if (input_box_active) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    bool change = false;
                    if (textEvent->unicode == '\b' || textEvent->unicode == 8) {
                        if (!current_input.empty()) {
                            current_input.pop_back();
                            change = true;
                        }
                    }
                    else if (textEvent->unicode >= 97 && textEvent->unicode <= 122) {
                        current_input += static_cast<char>(textEvent->unicode);
                        change = true;
                    }
                    input_box.setLabel(current_input + "|"); 
                    if(change){
                        std::cout << "Received new input string: " << current_input << "\n";
                    }
                }
            }
        }

        // Frame count
        ++frame_count;
        if(frame_count % 20 == 0)
            std::cerr << "Frame :" << frame_count << "\n";
        
        // Draw new frame
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
        pnow->isleaf = false;
        pnow = pnow->pnext[id];
    }
    std::cerr << "Yes\n";
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

        std::cerr << char(i + 'a') << " " << pnode->pnext[i]->block_need << "\n";

        pnode->block_need += pnode->pnext[i]->block_need;
        if(previous)
            pnode->block_need += (block_horizontal);
        previous = true;
    }

    if(isleaf == true){
        pnode->block_need = 1;
    }

    return isleaf;
}

void Trie::cre_node(NodeTrie *pnode, int block_x, int block_y, int char_branch)
{
    // Create node
    pnode->visual_node = create_node(block_x, block_y);
    if(char_branch == -1)
        pnode->visual_node->setLabel("R", 20);
    else{
        std::string tmp = "";
        tmp.push_back(char_branch + 'a');
        pnode->visual_node->setLabel(tmp, 20);
    }

    // Calculation
    int run_block = block_x - pnode->block_need / 2;
    bool previous_leaf = false, previous = false;
    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnode->pnext[i] != nullptr){
        bool current_leaf = pnode->pnext[i]->isleaf;
        if(previous)
            run_block = run_block + (block_horizontal);
        int l = run_block;
        int r = run_block + pnode->pnext[i]->block_need - 1;
        cre_node(pnode->pnext[i], (l + r) / 2, block_y + block_vertical, i);
        run_block = run_block + pnode->pnext[i]->block_need;
        previous = true;
    }
}

void Trie::cre_edge(NodeTrie *pnode)
{
    if(pnode->isleaf) return;
    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnode->pnext[i] != nullptr){
        // Create edge
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
        
        // Draw edge label (character)
        // char edge_char = 'a' + i;
        // sf::Text edge_text(font_impact, std::string(1, edge_char), 24);
        // edge_text.setFillColor(sf::Color::Black);
        
        // sf::FloatRect textBounds = edge_text.getLocalBounds();
        // edge_text.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, 
        //                      textBounds.position.y + textBounds.size.y / 2.f});
        
        // sf::Vector2f pos_u = pnode->visual_node->getPosition();
        // sf::Vector2f pos_v = pnode->pnext[i]->visual_node->getPosition();
        
        // float dx = pos_v.x - pos_u.x;
        // float dy = pos_v.y - pos_u.y;
        // float edge_length = std::sqrt(dx * dx + dy * dy);
        
        // if(edge_length > 0) {
        //     float perp_x = -dy / edge_length;
        //     float perp_y = dx / edge_length;
            
        //     if(perp_y > 0) {
        //         perp_x = -perp_x;
        //         perp_y = -perp_y;
        //     }
            
        //     float offset_dist = 15.0f;
        //     float weight_x = (pos_u.x + pos_v.x) / 2.f + perp_x * offset_dist;
        //     float weight_y = (pos_u.y + pos_v.y) / 2.f + perp_y * offset_dist;
            
        //     edge_text.setPosition({weight_x, weight_y});
        //     window.draw(edge_text);
        // }
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
