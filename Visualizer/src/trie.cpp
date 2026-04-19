#include "../headers/trie.h"

// Global variable definitions
const int max_charater = 7;

RoundedRectangleShape bgvisual({800.f, 600.f});
const float block_unit = 40.f;
const int block_vertical = 2;
const int block_horizontal = 1;
int block_width = 0;
int block_height = 0;

const float node_radius = block_unit / 2;
const float node_outline_thickness = -3.f;
const sf::Color BUTTON_FILL_COLOR = sf::Color(232, 183, 81);
const sf::Color BOX_FILL_COLOR = sf::Color(138, 155, 192);
const sf::Color NODE_FILL_COLOR = sf::Color(218, 168, 74);
const sf::Color NODE_OUTLINE_COLOR = sf::Color(202, 148, 95);
const sf::Color NODE_ACTIVE_COLOR = sf::Color(62, 188, 167);
const sf::Color NODE_FOUND_COLOR = sf::Color(156, 229, 147);
const sf::Color NODE_DELETE_COLOR = sf::Color(246, 88, 88);
const sf::Color EDGE_COLOR = sf::Color(203, 203, 201);

const float scale = 7.f;
const float button_area_up = 0;
const float button_area_mid = float(WINDOW_HEIGHT) / scale;
const float button_area_bot = float(WINDOW_HEIGHT) * ((scale - 1) / 2 + 1) / scale;
const float button_area_horizon = float(WINDOW_WIDTH) / 4;
const float section_tilte_word_height = 70.f;
const float space_button = 25.f;
const float button_width = (float(WINDOW_WIDTH) / 4 - space_button * 3) / 2;
const float button_height = (float(WINDOW_HEIGHT) * ((scale - 1) / 2) / scale - section_tilte_word_height - space_button * 3) / 3;
const float button_small_width = (button_width - space_button) / 2;
const float slider_width = button_area_horizon - space_button * 2; 
const float slider_height = button_height;

void trie_page(){
    
    // Background
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("cs163-project/Visualizer/assets/bg_toty.png")) {
        std::cerr << "cannot load background" << std::endl;
    } else{
        std::cerr << "load background successfully" << std::endl;    
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    // Line (Temporary)
    sf::RectangleShape horizontal_line_1;
    horizontal_line_1.setFillColor({0, 170, 255});
    horizontal_line_1.setSize({float(WINDOW_WIDTH) / 4, 5.f});
    horizontal_line_1.setPosition({0, float(WINDOW_HEIGHT) / scale});
    sf::RectangleShape horizontal_line_2;
    horizontal_line_2.setFillColor({0, 170, 255});
    horizontal_line_2.setSize({float(WINDOW_WIDTH) / 4, 5.f});
    horizontal_line_2.setPosition({0, float(WINDOW_HEIGHT) * ((scale - 1) / 2 + 1) / scale});
    sf::RectangleShape horizontal_line_3;
    horizontal_line_3.setFillColor({0, 170, 255});
    horizontal_line_3.setSize({float(WINDOW_WIDTH) / 4, 5.f});
    horizontal_line_3.setPosition({float(WINDOW_WIDTH) - float(WINDOW_WIDTH) / 4, float(WINDOW_HEIGHT) / scale});
    sf::RectangleShape horizontal_line_4;
    horizontal_line_4.setFillColor({0, 170, 255});
    horizontal_line_4.setSize({float(WINDOW_WIDTH) / 4, 5.f});
    horizontal_line_4.setPosition({float(WINDOW_WIDTH) - float(WINDOW_WIDTH) / 4, float(WINDOW_HEIGHT) * ((scale - 1) / 2 + 1) / scale});
    sf::RectangleShape vertical_line_1;
    vertical_line_1.setFillColor({0, 170, 255});
    vertical_line_1.setSize({5.f, float(WINDOW_HEIGHT)});
    vertical_line_1.setPosition({float(WINDOW_WIDTH) / 4, 0});
    sf::RectangleShape vertical_line_2;
    vertical_line_2.setFillColor({0, 170, 255});
    vertical_line_2.setSize({5.f, float(WINDOW_HEIGHT)});
    vertical_line_2.setPosition({float(WINDOW_WIDTH) * 3 / 4, 0});

    // Button
    button return_button(float(WINDOW_WIDTH) / 8 - button_width / 2, float(WINDOW_HEIGHT) / scale / 2 - button_height / 2, button_width, button_height, BUTTON_FILL_COLOR, "Return", 24);
    button add_button(space_button, button_area_bot + section_tilte_word_height, button_width, button_height, BUTTON_FILL_COLOR, "Add", 24);
    button delete_button(space_button * 2 + button_width, button_area_bot + section_tilte_word_height, button_width, button_height, BUTTON_FILL_COLOR, "Delete", 24);
    button find_button(space_button, button_area_bot + section_tilte_word_height + button_height + space_button, button_width, button_height, BUTTON_FILL_COLOR, "Find", 24);
    button clear_button(space_button * 2 + button_width, button_area_bot + section_tilte_word_height + button_height + space_button, button_width, button_height, BUTTON_FILL_COLOR, "Clear", 24);
    button file_button(space_button, button_area_mid + section_tilte_word_height, button_width, button_height, BUTTON_FILL_COLOR, "File", 24);
    button random_button(space_button * 2 + button_width, button_area_mid + section_tilte_word_height, button_width, button_height, BUTTON_FILL_COLOR, "Random", 24);
    button build_button(float(WINDOW_WIDTH) / 8 - button_width / 2, button_area_mid + section_tilte_word_height + button_height + space_button, button_width, button_height, BUTTON_FILL_COLOR, "Build", 24);
    button back_button(button_area_horizon * 3 + space_button, button_area_mid + 2 * space_button + slider_height, button_width, button_height, BUTTON_FILL_COLOR, "Back", 24);
    button next_button(button_area_horizon * 3 + 2 * space_button + button_width, button_area_mid + 2 * space_button + slider_height, button_width, button_height, BUTTON_FILL_COLOR, "Next", 24);
    button slowdown_button(button_area_horizon * 3 + space_button, button_area_mid + 3 * space_button + slider_height + button_height, button_small_width, button_height, BUTTON_FILL_COLOR, "-", 24);
    button speedup_button(button_area_horizon * 3 + space_button + button_small_width + space_button, button_area_mid + 3 * space_button + slider_height + button_height, button_small_width, button_height, BUTTON_FILL_COLOR, "+", 24);

    // Slider - Progess animation
    RoundedRectangleShape slider_fix({slider_width, slider_height}, 10.f);
    RoundedRectangleShape slider_run({slider_width, slider_height}, 10.f);

    slider_fix.setPosition({button_area_horizon * 3 + space_button, button_area_mid + space_button});
    slider_fix.setOutlineThickness(3);
    slider_fix.setOutlineColor(sf::Color(233, 186, 85));
    slider_fix.setFillColor(sf::Color(140, 155, 191));
    slider_run.setPosition({button_area_horizon * 3 + space_button, button_area_mid + space_button});
    slider_run.setFillColor(sf::Color(28, 41, 114));

    // Frame (Khung) for Visual & Code
    bgvisual.setSize({float(WINDOW_WIDTH) / 2 - 10.f, 600.f});
    bgvisual.setPosition({float(WINDOW_WIDTH) / 4 + 10.f , 50});
    bgvisual.setFillColor(sf::Color(251, 251, 253, 200));
    bgvisual.setRadius(17);
    bgvisual.setOutlineThickness(5);
    bgvisual.setOutlineColor(sf::Color(217, 211, 209));
    block_width = int(bgvisual.getSize().x / block_unit);
    block_height = int(bgvisual.getSize().y / block_unit);
    std::cerr << "Rectangle -> Block : " << block_width << " " << block_height << "\n";
    RoundedRectangleShape bgcode({400.f, 230.f});
    bgcode.setPosition({float(WINDOW_WIDTH - 315), float(WINDOW_HEIGHT - 280)});
    bgcode.setFillColor(sf::Color(243, 243, 251, 100));
    // bgcode.setRadius(17);
    // bgcode.setOutlineThickness(5);
    // bgcode.setOutlineColor(sf::Color(217, 211, 209));
    
    // Boxs
    box input_box(space_button, button_area_bot + section_tilte_word_height + 2 * (button_height + space_button), button_width * 2 + space_button, button_height, BOX_FILL_COLOR, "   Type here", 24);
    box build_box(space_button, button_area_mid + section_tilte_word_height + 2 * (button_height + space_button), button_width * 2 + space_button, button_height, BOX_FILL_COLOR, "   Type here", 24);
    box speed_box(button_area_horizon * 3 + 2 * space_button + button_width, button_area_mid + 3 * space_button + slider_height + button_height, button_width, button_height, BOX_FILL_COLOR, "Speed : 0x", 24);

    // Create Root node
    node* bennode = nullptr;
    bennode = create_node(block_width / 2, 0);
    
    // Trie data
    Trie data;
    data.init();
    // data.add("aaa");
    // data.add("sayy");
    // data.add("crash");
    // data.add("car");
    // data.add("e");
    // data.add("f");
    // data.add("cringe");
    data.create_visual(); // Must have

    int frame_count = 0;
    
    // Managing state variables
    bool left_mouse = false;
    bool left_mouse_last = false;
    bool input_box_active = false;
    std::string current_input = "";
    bool build_box_active = false;
    std::string build_input = "";

    while(window.isOpen()){
        // 1. Mouse information
        left_mouse = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        // 2. Mouse inside button / box region -> Color-highlight handle
        if(!input_box_active)
            input_box.update(input_box_active, mousePos);
        if(!build_box_active)
            build_box.update(build_box_active, mousePos);
        return_button.update(mousePos);
        add_button.update(mousePos);
        delete_button.update(mousePos);
        find_button.update(mousePos);
        clear_button.update(mousePos);
        file_button.update(mousePos);
        random_button.update(mousePos);
        build_button.update(mousePos);
        

        // 3. Left-Mouse click handle
        if (left_mouse && !left_mouse_last) { 
            // Button
            bool operation_button_pressed = false;
            bool build_button_pressed = false;

            if (return_button.contains(mousePos)){
                std::cout << "Received Return Query: Exit Trie Page" << "\n";
                data.clear();
                return;
            }

            if (add_button.contains(mousePos)) {
                std::cout << "Received Add Query: " << current_input << "\n";
                if (!current_input.empty()) {data.add(current_input); data.create_visual();}
                operation_button_pressed = true;
            }
            if (delete_button.contains(mousePos)) {
                std::cout << "Received Delete Query: " << current_input << "\n";
                if (!current_input.empty()) {data.remove(current_input); data.create_visual();}
                operation_button_pressed = true;
            }
            if (find_button.contains(mousePos)) {
                std::cout << "Received Find Query: " << current_input << "\n";
                operation_button_pressed = true;
            }
            if (clear_button.contains(mousePos)) {
                std::cout << "Received Clear Query" << "\n";
                {data.clear(); data.create_visual(); };
                operation_button_pressed = true;
            }

            if (file_button.contains(mousePos)) {
                std::cout << "Received File Query" << "\n";
                build_button_pressed = true;
            }
            if (random_button.contains(mousePos)) {
                std::cout << "Received Random Query" << "\n";
                build_button_pressed = true;
            }
            if (build_button.contains(mousePos)) {
                std::cout << "Received build query : " << build_input << "\n";
                build_button_pressed = true;
            }

            if(input_box.contains(mousePos)){
                std::cout << "Received Input Box Activation: On" << "\n";
                input_box_active = true;
                input_box.update(input_box_active, mousePos);
                if(current_input.empty()){
                    input_box.setLabel("|");
                }
            } else
            if(!operation_button_pressed && input_box_active){
                std::cout << "Received Input Box Activation: Off" << "\n";
                input_box_active = false;
                if(current_input.empty()){
                    input_box.setLabel("   Type here");
                }
            }

            if(build_box.contains(mousePos)){
                std::cout << "Received Build Box Activation: On" << "\n";
                build_box_active = true;
                build_box.update(build_box_active, mousePos);
                if(build_input.empty()){
                    build_box.setLabel("|");
                }
            } else
            if(!build_button_pressed && build_box_active){
                std::cout << "Received Build Box Activation: Off" << "\n";
                build_box_active = false;
                if(build_input.empty()){
                    build_box.setLabel("   Type here");
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
            if (build_box_active) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    bool change = false;
                    if (textEvent->unicode == '\b' || textEvent->unicode == 8) {
                        if (!build_input.empty()) {
                            build_input.pop_back();
                            change = true;
                        }
                    }
                    else if (textEvent->unicode >= 97 && textEvent->unicode <= 122) {
                        // Lowercase letters a-z
                        build_input += static_cast<char>(textEvent->unicode);
                        change = true;
                    }
                    else if (textEvent->unicode == ',') {
                        // Comma separator
                        build_input += ',';
                        change = true;
                    }
                    build_box.setLabel(build_input + "|");
                    if(change){
                        std::cout << "Received new build input string: " << build_input << "\n";
                    }
                }
            }
        }

        // Frame count
        ++frame_count;
        if(frame_count % 20 == 0)
            std::cerr << "Frame :" << frame_count << "\n";
        
        // --- Draw new frame --- //
        window.clear(sf::Color(212, 188, 112, 0.71));
        // Background
        window.draw(backgroundSprite);
        // Line
            // window.draw(horizontal_line_1);
            // window.draw(horizontal_line_2);
            // window.draw(horizontal_line_3);
            // window.draw(horizontal_line_4);
            // window.draw(vertical_line_1);
            // window.draw(vertical_line_2);
        // Button
        return_button.draw(window);
        add_button.draw(window);
        delete_button.draw(window);
        find_button.draw(window);
        clear_button.draw(window);
        build_button.draw(window);
        file_button.draw(window);
        random_button.draw(window);
        window.draw(bgvisual);
        window.draw(bgcode);
        back_button.draw(window);
        next_button.draw(window);
        speedup_button.draw(window);
        slowdown_button.draw(window);
        // Box
        input_box.draw(window);
        build_box.draw(window);
        speed_box.draw(window);
        // Slider
        window.draw(slider_fix);
        // Data Trie
        data.draw(window);
        
        // --- Display --- //
        window.display();
    }
}

node* create_node(int block_x, int block_y){
    float x, y;
    x = block_unit * block_x;
    y = block_unit * block_y;
    x = x + bgvisual.getPosition().x;
    y = y + bgvisual.getPosition().y;
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
    pnow->isend = true;
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
    return pnow->isend;
}

void Trie::remove(std::string s)
{
    // Already check
    if(!find(s)) return;
    std::vector<NodeTrie*> thepath;
    thepath.push_back(root);
    NodeTrie* pnow = root;
    for(int i = 0; i < s.size(); ++i){
        int id = s[i] - 'a';
        pnow = pnow->pnext[id];
        thepath.push_back(pnow);
    }
    pnow->isend = false;
    for(int i = thepath.size() - 1; i >= 1; --i){
        pnow = thepath[i];
        if(pnow->isend) break;
        // Check
        bool exist = false;
        for(int c = 0; c < LOWERCASE_CHAR; ++c) if(pnow->pnext[c] != nullptr){
            exist = true;
        }
        if(!exist){
            if(pnow->visual_node != nullptr)
                delete pnow->visual_node;
            for(int c = 0; c < LOWERCASE_CHAR; ++c) if(pnow->visual_edge[c] != nullptr){
                delete pnow->visual_edge[c];
                pnow->visual_edge[c] = nullptr;
            }
            delete pnow;
            int id = s[i - 1] - int('a');
            thepath[i - 1]->pnext[id] = nullptr;
        }
    }
    return;
}

void Trie::clear(){
    clear_travese(root, true);
}

void Trie::clear_travese(NodeTrie *pnode, bool isRoot){
    for(int c = 0; c < LOWERCASE_CHAR; ++c) if(pnode->pnext[c] != nullptr){
        clear_travese(pnode->pnext[c]);
        pnode->pnext[c] = nullptr;
    }
    for(int c = 0; c < LOWERCASE_CHAR; ++c) if(pnode->visual_edge[c] != nullptr){
        delete pnode->visual_edge[c];
        pnode->visual_edge[c] = nullptr;
    }
    if(!isRoot){
        delete pnode->visual_node;
        delete pnode;
    }
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
        pnode->visual_edge[i] = new edge(x_cur, y_cur, x_nxt, y_nxt, EDGE_COLOR, 2);
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
        visual_edge[i] = nullptr;
    }
    isend = false;
    block_need = 0;
    isleaf = false;
    visual_node = nullptr;

}
