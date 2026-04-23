#include "../headers/trie.h"
#include <windows.h>
#include <fstream>

static std::string openFileDialog() {
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
    return "";
}

// Global variable definitions
const int max_charater = 7;
const int max_character_show = 20;

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
const sf::Color NODE_END_COLOR = sf::Color(53, 125, 186);
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

const float eps = 0.0001f;
const float speed_jump = 0.2f;
const float speed_min = 0.4f;
const float speed_max = 2.f;
const int base_pace = 100;

const int min_random_words = 2;
const int max_random_words = 7;
const std::string words[] = {"apple", 
                            "avocado", 
                            "banana", 
                            "grape", 
                            "mango", 
                            "kiwi", 
                            "durian", 
                            "lemon", 
                            "lime", 
                            "papaya", 
                            "peach", 
                            "pear", 
                            "melon", 
                            "longan", 
                            "berry", 
                            "guava", 
                            "cherry", 
                            "plum"};

const std::string CODE_ADD = 
    "Node* p = root;\n"                     // 0
    "for (char c : word) {\n"               // 1
    "    if (p->next[c] == null)\n"         // 2
    "        p->next[c] = new Node();\n"    // 3
    "    p = p->next[c];\n"                 // 4
    "}\n"                                   // 5
    "p->is_end = true;";                    // 6

const std::string CODE_FIND = 
    "Node* p = root;\n"                     // 0
    "for (char c : word) {\n"               // 1
    "    if (p->next[c] == null)\n"         // 2
    "        return false;\n"               // 3
    "    p = p->next[c];\n"                 // 4
    "}\n"                                   // 5
    "return p->is_end;";                    // 6

const std::string CODE_DELETE = 
    "Node* p = find(word);\n"               // 0
    "if (!p || !p->is_end) return;\n"       // 1
    "p->is_end = false;\n"                  // 2
    "while (p != root && p->is_empty()) {\n"// 3
    "    Node* par = p->parent;\n"          // 4
    "    delete p;\n"                       // 5
    "    p = par;\n"                        // 6
    "}";                                    // 7

Trie data;

void trie_page(){
    // load background
    sf::Texture backgroundTexture;
    if (!loadTextureFromAsset(backgroundTexture, "bg_trie.png")) {
        std::cerr << "cannot load background" << std::endl;
        !backgroundTexture.loadFromFile("cs163-project/Visualizer/assets/bg_trie.png");
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
    button skip_button(button_area_horizon * 3 + space_button, button_area_mid + 4 * space_button + slider_height + 2 * button_height, button_width, button_height, BUTTON_FILL_COLOR, "Skip", 24);
    button mode_button(button_area_horizon * 3 + 2 * space_button + button_width, button_area_mid + 4 * space_button + slider_height + 2 * button_height, button_width, button_height, BUTTON_FILL_COLOR, "Manual", 24);
           mode_button.setLabel("Manual");
    
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

    // Code Box
    CodeBox codeBox({350.f, 250.f}, font_impact, 18);
    codeBox.setOrigin({350.f, 250.f});
    codeBox.setPosition({float(WINDOW_WIDTH) - 10.f, float(WINDOW_HEIGHT) - 10.f});
    codeBox.setCode("// Choose an operation");
    
    // Boxs
    box input_box(space_button, button_area_bot + section_tilte_word_height + 2 * (button_height + space_button), button_width * 2 + space_button, button_height, BOX_FILL_COLOR, "   Type here", 24);
    box build_box(space_button, button_area_mid + section_tilte_word_height + 2 * (button_height + space_button), button_width * 2 + space_button, button_height, BOX_FILL_COLOR, "   Type here", 24);
    box speed_box(button_area_horizon * 3 + 2 * space_button + button_width, button_area_mid + 3 * space_button + slider_height + button_height, button_width, button_height, BOX_FILL_COLOR, "speed : 1x", 24);

    // Trie data
    data.init();
    data.create_visual(); // Must have

    int frame_count = 0;
    
    // Managing state variables
    bool left_mouse = false;
    bool left_mouse_last = false;
    bool input_box_active = false;
    std::string current_input = "";
    bool build_box_active = false;
    std::string build_input = "";
    ModeType mode = ModeType::Manual;
    float speed = 1; 
    int pace = float(base_pace) / speed;

    // Random prepare
    srand(time(NULL));

    // --- Animation state ---
    std::vector<AnimStep> anim_queue;
    int cur_step = -1;
    NodeTrie* highlighted_node = nullptr;
    OperationType current_animation_type = OperationType::None;

    auto skip_animation = [&]() {
        if (current_animation_type == OperationType::Add || current_animation_type == OperationType::Build) {
            for (auto& step : anim_queue) {
                if (step.stored_subtree) {
                    step.node->pnext[step.char_id] = step.stored_subtree;
                    step.stored_subtree = nullptr;
                }
                if(step.type == StepType::MarkEnd){
                    step.node->pnext[step.char_id]->isend = true;
                }
            }
        } else if (current_animation_type == OperationType::Delete) {
            for (int i = std::max(0, cur_step + 1); i < (int)anim_queue.size(); ++i) {
                auto& step = anim_queue[i];
                if (step.type == StepType::UnmarkEnd && step.node != nullptr) {
                    step.node->isend = false;
                } else if (step.type == StepType::DeleteLerp && step.node != nullptr) {
                    if (step.node->pnext[step.char_id] != nullptr) {
                        step.stored_subtree = step.node->pnext[step.char_id];
                        step.node->pnext[step.char_id] = nullptr;
                    }
                }
            }
            for (auto& step : anim_queue) {
                if (step.stored_subtree) {
                    data.clear_travese(step.stored_subtree);
                    step.stored_subtree = nullptr;
                }
            }
        } else if (current_animation_type == OperationType::Find) {
            // Prepare to implement animation
        }

        if (current_animation_type != OperationType::None) {
            if (highlighted_node) { data.unhighlight_node(highlighted_node); highlighted_node = nullptr; }
            data.create_visual();
            std::function<void(NodeTrie*)> reset_visual = [&](NodeTrie* p) {
                if (!p) return;
                if (p->visual_node) {
                    p->visual_node->setOpacity(255);
                    if (p->isend) p->visual_node->setColor(NODE_END_COLOR);
                    else p->visual_node->setColor(NODE_FILL_COLOR);
                    p->visual_node->currentPos = p->visual_node->targetPos;
                    p->visual_node->setPosition(p->visual_node->targetPos.x, p->visual_node->targetPos.y);
                }
                for (int i = 0; i < LOWERCASE_CHAR; ++i) {
                    if (p->visual_edge[i]) {
                        p->visual_edge[i]->setOpacity(255);
                        p->visual_edge[i]->setColor(EDGE_COLOR);
                    }
                    reset_visual(p->pnext[i]);
                }
            };
            reset_visual(data.root);
            data.update_edges(data.root);

            anim_queue.clear();
            cur_step = -1;
            current_animation_type = OperationType::None;
        }
    };

    auto next_animation = [&]() {
        int next_step = cur_step + 1;
        if (next_step < (int)anim_queue.size()) {
            cur_step = next_step;
            AnimStep& step = anim_queue[cur_step];

            if (current_animation_type == OperationType::Add || current_animation_type == OperationType::Build) {
                if (step.type == StepType::Move) {
                    if (highlighted_node) data.unhighlight_node(highlighted_node);
                    // Move lưu parent+id, giải với step.node->pnext[step.char_id]
                    highlighted_node = step.node->pnext[step.char_id];
                    data.highlight_node(highlighted_node, NODE_ACTIVE_COLOR);
                }
                else if (step.type == StepType::Lerp) {
                    // Hàn gắn lại nhánh bị tháo từ thao tác Back (nếu có)
                    if (step.stored_subtree) {
                        step.node->pnext[step.char_id] = step.stored_subtree;
                        step.stored_subtree = nullptr;
                    }
                    // Reposition toàn bộ cây + tạo visual cho node mới
                    data.create_visual_lerp(0.4f);
                    // Ẩn toàn bộ subtree mới (cả node lẫn edge)
                    std::function<void(NodeTrie*, NodeTrie*, int)> hide_sub = [&](NodeTrie* p, NodeTrie* par, int cid) {
                        if (!p) return;
                        if (p->visual_node) p->visual_node->setOpacity(0);
                        if (par && par->visual_edge[cid])
                            par->visual_edge[cid]->setOpacity(0);
                        for (int ci = 0; ci < LOWERCASE_CHAR; ++ci) hide_sub(p->pnext[ci], p, ci);
                    };
                    hide_sub(step.node->pnext[step.char_id], step.node, step.char_id);
                }
                else if (step.type == StepType::Create) {
                    if (highlighted_node) data.unhighlight_node(highlighted_node);
                    NodeTrie* new_node = step.node->pnext[step.char_id];
                    if (new_node && new_node->visual_node) {
                        new_node->visual_node->setOpacity(255);
                        new_node->visual_node->setColor(NODE_FOUND_COLOR);
                    }
                    // Hiện edge từ cha tới node mới
                    if (step.node->visual_edge[step.char_id]) {
                        step.node->visual_edge[step.char_id]->setOpacity(255);
                        step.node->visual_edge[step.char_id]->setColor(EDGE_COLOR);
                    }
                    highlighted_node = new_node;
                }
                else if (step.type == StepType::MarkEnd) {
                    if (highlighted_node) data.unhighlight_node(highlighted_node);
                    // char_id == -1: node cũ (dùng trực tiếp)
                    // char_id != -1: node mới (tra qua parent)
                    NodeTrie* target = (step.char_id == -1)
                        ? step.node
                        : step.node->pnext[step.char_id];
                    if (target) {
                        target->isend = true;
                        highlighted_node = target;
                        data.highlight_node(highlighted_node, NODE_END_COLOR);
                    }
                }
            }
            else if (current_animation_type == OperationType::Delete || current_animation_type == OperationType::Find) {
                if (step.type == StepType::Move) {
                    if (highlighted_node) data.unhighlight_node(highlighted_node);
                    highlighted_node = step.node->pnext[step.char_id];
                    data.highlight_node(highlighted_node, NODE_ACTIVE_COLOR);
                }
                else if (step.type == StepType::NotFound) {
                    if (highlighted_node) data.unhighlight_node(highlighted_node);
                    highlighted_node = nullptr; 
                }
                else if (step.type == StepType::Found) {
                    if (highlighted_node) data.unhighlight_node(highlighted_node);
                    highlighted_node = step.node;
                    data.highlight_node(highlighted_node, NODE_FOUND_COLOR);
                }
                else if (step.type == StepType::UnmarkEnd) {
                    if (highlighted_node) data.unhighlight_node(highlighted_node);
                    highlighted_node = step.node;
                    if (highlighted_node) highlighted_node->isend = false;
                    data.highlight_node(highlighted_node, NODE_FILL_COLOR);
                }
                else if (step.type == StepType::DeleteNodeMark || step.type == StepType::DeleteNodeNotMark) {
                    if (highlighted_node) data.unhighlight_node(highlighted_node);
                    
                    // Đánh dấu đỏ cho Node cha nếu là Mark
                    if (step.type == StepType::DeleteNodeMark) {
                        highlighted_node = step.node;
                        data.highlight_node(highlighted_node, NODE_DELETE_COLOR);
                    } else {
                        highlighted_node = nullptr; 
                    }
                    
                    // Ẩn đi node con bị xoá
                    NodeTrie* child = step.node->pnext[step.char_id];
                    if (child) {
                        if (child->visual_node) child->visual_node->setOpacity(0);
                        if (step.node->visual_edge[step.char_id]) step.node->visual_edge[step.char_id]->setOpacity(0);
                    }
                }
                else if (step.type == StepType::DeleteLerp) {
                    if (highlighted_node) data.unhighlight_node(highlighted_node);
                    highlighted_node = nullptr;
                    
                    // Cắt nhánh cây thực sự
                    if (step.node && step.node->pnext[step.char_id]) {
                        step.stored_subtree = step.node->pnext[step.char_id];
                        step.node->pnext[step.char_id] = nullptr;
                    }
                    
                    data.create_visual_lerp(0.4f);
                }
            }
        }
    };

    // --- Delta time clock ---
    sf::Clock frame_clock;

    while(window.isOpen()){
        float dt = frame_clock.restart().asSeconds();

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
        back_button.update(mousePos);
        next_button.update(mousePos);
        speedup_button.update(mousePos);
        slowdown_button.update(mousePos);
        skip_button.update(mousePos);
        mode_button.update(mousePos);

        // 3. Left-Mouse click handle
        if (left_mouse && !left_mouse_last) { 
            bool operation_button_pressed = false;
            bool build_button_pressed = false;

            if (return_button.contains(mousePos)){
                std::cout << "Received Return Query: Exit Trie Page" << "\n";
                skip_animation();
                data.clear();
                return;
            }

            if (mode_button.contains(mousePos)){
                if(mode == ModeType::Manual){
                    mode = ModeType::Auto;
                    mode_button.setLabel("Auto");
                    std::cout << "Received Change Mod Query, new mode : Auto" << "\n";
                } else{
                    mode = ModeType::Manual;
                    mode_button.setLabel("Manual");
                    std::cout << "Received Change Mod Query, new mode : Manual" << "\n";
                }
            }

            if (slowdown_button.contains(mousePos) && speed - speed_jump > speed_min - eps){
                speed = speed - speed_jump;
                pace = float(base_pace) / speed;
            };
            if (speedup_button.contains(mousePos) && speed + speed_jump < speed_max + eps){
                speed = speed + speed_jump;
                pace = float(base_pace) / speed;
            };
            if (slowdown_button.contains(mousePos) || speedup_button.contains(mousePos)) {
                char buf[32];
                snprintf(buf, sizeof(buf), "%.2f", speed);
                std::string s = buf;
                s.erase(s.find_last_not_of('0') + 1, std::string::npos);
                if (s.back() == '.') s.pop_back();
                speed_box.setLabel("speed : " + s + "x");
            }
            

            // ---- ADD ----
            if (add_button.contains(mousePos)) {
                std::cout << "Received Add Query: " << current_input << "\n";
                if (!current_input.empty()) {
                    skip_animation();
                    anim_queue = data.add_with_anim(current_input);
                    codeBox.setCode(CODE_ADD);
                    codeBox.setStep(0);
                    cur_step = -1;
                    current_animation_type = OperationType::Add;
                }
                operation_button_pressed = true;
            }
            // ---- DELETE ----
            if (delete_button.contains(mousePos)) {
                std::cout << "Received Delete Query: " << current_input << "\n";
                if (!current_input.empty()) {
                    skip_animation();
                    anim_queue = data.remove_with_anim(current_input);
                    codeBox.setCode(CODE_DELETE);
                    codeBox.setStep(-1);
                    cur_step = -1;
                    current_animation_type = OperationType::Delete;
                }
                operation_button_pressed = true;
            }

            if (find_button.contains(mousePos)) {
                std::cout << "Received Find Query: " << current_input << "\n";
                if (!current_input.empty()) {
                    skip_animation();
                    anim_queue = data.find_with_anim(current_input);
                    codeBox.setCode(CODE_FIND);
                    codeBox.setStep(0);
                    cur_step = -1;
                    current_animation_type = OperationType::Find;
                }
                operation_button_pressed = true;
            }

            // ---- CLEAR ----
            if (clear_button.contains(mousePos)) {
                std::cout << "Received Clear Query" << "\n";
                skip_animation();
                data.clear(); data.create_visual();
                operation_button_pressed = true;
            }

            if (file_button.contains(mousePos)) {
                std::cout << "Received File Query" << "\n";
                std::string dir = openFileDialog();
                if (!dir.empty()) {
                    std::cout << "Opened file: " << dir << '\n';
                    std::ifstream fin(dir);
                    if (fin.is_open()) {
                        build_input.clear();
                        std::string word;
                        bool first = true;
                        while (std::getline(fin, word)) { 
                            if (!word.empty() && word.back() == '\r') word.pop_back();
                            if (word.empty()) continue; // Bỏ qua dòng trống
                            
                            if (!first) build_input += ",";
                            build_input += word;
                            first = false;
                        }
                        
                        if (build_input.size() <= max_character_show)
                            build_box.setLabel(build_input + "|");
                        else
                            build_box.setLabel(build_input.substr(build_input.size() - max_character_show, max_character_show) + "|");
                        
                        fin.close();
                    } else {
                        std::cerr << "Loi: Khong the mo file!\n";
                    }
                }
                build_button_pressed = true;
            }
            if (random_button.contains(mousePos)) {
                std::cout << "Received Random Query" << "\n";
                build_input.clear();
                
                int total_vocab = sizeof(words) / sizeof(words[0]);
                int num_words = min_random_words + rand() % (max_random_words - min_random_words + 1);
                
                std::vector<int> ids(total_vocab);
                for (int i = 0; i < total_vocab; ++i) ids[i] = i;
                for (int i = total_vocab - 1; i > 0; --i) {
                    std::swap(ids[i], ids[rand() % (i + 1)]);
                }
                
                for (int i = 0; i < num_words; ++i) {
                    build_input += words[ids[i]];
                    if (i < num_words - 1) build_input += ",";
                }
                
                if (build_input.size() <= max_character_show)
                    build_box.setLabel(build_input + "|");
                else
                    build_box.setLabel(build_input.substr(build_input.size() - max_character_show, max_character_show) + "|");
                
                build_button_pressed = true;
            }
            if (build_button.contains(mousePos)) {
                std::cout << "Received build query : " << build_input << "\n";
                if (!build_input.empty()) {
                    skip_animation();
                    data.clear();

                    codeBox.setCode(CODE_ADD);
                    codeBox.setStep(0);
                    
                    std::vector<std::string> words_to_build;
                    std::string current_word = "";
                    for (char c : build_input) {
                        if (c == ',') {
                            if (!current_word.empty()) words_to_build.push_back(current_word);
                            current_word = "";
                        } else {
                            current_word += c;
                        }
                    }
                    if (!current_word.empty()) words_to_build.push_back(current_word);
                    
                    anim_queue = data.build_with_anim(words_to_build);
                    
                    // Detach ngược để bảo toàn đồ thị gốc đồng thời cập nhật đúng layout incremental cực mượt
                    for (int i = (int)anim_queue.size() - 1; i >= 0; --i) {
                        if (anim_queue[i].type == StepType::Lerp) {
                            anim_queue[i].stored_subtree = anim_queue[i].node->pnext[anim_queue[i].char_id];
                            anim_queue[i].node->pnext[anim_queue[i].char_id] = nullptr;
                        }
                    }
                    data.create_visual(); // Đặt lại view ban đầu chưa bị giãn nỡ
                    
                    cur_step = -1;
                    current_animation_type = OperationType::Build;
                }
                
                build_button_pressed = true;
            }

            // ---- NEXT ----
            if (next_button.contains(mousePos) && !anim_queue.empty() && cur_step + 1 < anim_queue.size()) {
                next_animation();
            }

            // ---- BACK ----
            if (back_button.contains(mousePos) && !anim_queue.empty() && cur_step >= 0) {
                AnimStep& step = anim_queue[cur_step];

                if (current_animation_type == OperationType::Add || current_animation_type == OperationType::Build) {
                    if (step.type == StepType::Move) {
                        if (highlighted_node) data.unhighlight_node(highlighted_node);
                        highlighted_node = nullptr;
                        if (cur_step - 1 >= 0) {
                            AnimStep& prev = anim_queue[cur_step - 1];
                            if (prev.type == StepType::Move) {
                                highlighted_node = prev.node->pnext[prev.char_id];
                                data.highlight_node(highlighted_node, NODE_ACTIVE_COLOR);
                            } else if (prev.type == StepType::Create) {
                                NodeTrie* pn = prev.node->pnext[prev.char_id];
                                if (pn) { highlighted_node = pn; data.highlight_node(highlighted_node, NODE_FOUND_COLOR); }
                            }
                        }
                    }
                    else if (step.type == StepType::MarkEnd) {
                        NodeTrie* target = (step.char_id == -1) ? step.node : step.node->pnext[step.char_id];
                        if (target) target->isend = false;
                        if (highlighted_node) data.unhighlight_node(highlighted_node);
                        highlighted_node = nullptr;
                        if (cur_step - 1 >= 0) {
                            AnimStep& prev = anim_queue[cur_step - 1];
                            if (prev.type == StepType::Create) {
                                NodeTrie* pn = prev.node->pnext[prev.char_id];
                                if (pn) { highlighted_node = pn; data.highlight_node(highlighted_node, NODE_FOUND_COLOR); }
                            } else if (prev.type == StepType::Move) {
                                highlighted_node = prev.node;
                                data.highlight_node(highlighted_node, NODE_ACTIVE_COLOR);
                            }
                        }
                    }
                    else if (step.type == StepType::Create) {
                        NodeTrie* new_node = step.node->pnext[step.char_id];
                        if (new_node && new_node->visual_node) new_node->visual_node->setOpacity(0);
                        if (step.node->visual_edge[step.char_id]) step.node->visual_edge[step.char_id]->setOpacity(0);
                        if (highlighted_node == new_node) highlighted_node = nullptr;
                        if (cur_step - 1 >= 0) {
                            AnimStep& prev = anim_queue[cur_step - 1];
                            if (prev.type == StepType::Create) {
                                NodeTrie* pn = prev.node->pnext[prev.char_id];
                                if (pn) { highlighted_node = pn; data.highlight_node(highlighted_node, NODE_FOUND_COLOR); }
                            } else if (prev.type == StepType::Move) {
                                highlighted_node = prev.node;
                                data.highlight_node(highlighted_node, NODE_ACTIVE_COLOR);
                            } else if (prev.type == StepType::Lerp) {
                                highlighted_node = nullptr;
                            }
                        }
                    }
                    else if (step.type == StepType::Lerp) {
                        step.stored_subtree = step.node->pnext[step.char_id];
                        step.node->pnext[step.char_id] = nullptr;
                        data.create_visual_lerp(0.4f);
                    }
                }
                else if (current_animation_type == OperationType::Delete || current_animation_type == OperationType::Find) {
                    if (step.type == StepType::Move) {
                        if (highlighted_node) data.unhighlight_node(highlighted_node);
                        highlighted_node = nullptr;
                        if (cur_step - 1 >= 0) {
                            AnimStep& prev = anim_queue[cur_step - 1];
                            if (prev.type == StepType::Move) {
                                highlighted_node = prev.node->pnext[prev.char_id];
                                data.highlight_node(highlighted_node, NODE_ACTIVE_COLOR);
                            }
                        }
                    }
                    else if (step.type == StepType::NotFound || step.type == StepType::Found) {
                        if (highlighted_node) data.unhighlight_node(highlighted_node);
                        highlighted_node = nullptr;
                        if (cur_step - 1 >= 0) {
                            AnimStep& prev = anim_queue[cur_step - 1];
                            // Khi lùi từ Found/NotFound thì bước trước luôn là Move
                            if (prev.type == StepType::Move) {
                                highlighted_node = prev.node->pnext[prev.char_id];
                                data.highlight_node(highlighted_node, NODE_ACTIVE_COLOR);
                            }
                        }
                    }
                    else if (step.type == StepType::UnmarkEnd) {
                        if (step.node) step.node->isend = true;
                        if (highlighted_node) data.unhighlight_node(highlighted_node);
                        highlighted_node = nullptr;
                        if (cur_step - 1 >= 0) {
                            AnimStep& prev = anim_queue[cur_step - 1];
                            if (prev.type == StepType::Move) {
                                highlighted_node = prev.node->pnext[prev.char_id];
                            }
                        }
                        if (highlighted_node) data.highlight_node(highlighted_node, NODE_END_COLOR); 
                    }
                    else if (step.type == StepType::DeleteNodeMark || step.type == StepType::DeleteNodeNotMark) {
                        // Khôi phục Node con ẩn
                        NodeTrie* child = step.node->pnext[step.char_id];
                        if (child) {
                            if (child->visual_node) child->visual_node->setOpacity(255);
                            if (step.node->visual_edge[step.char_id]) step.node->visual_edge[step.char_id]->setOpacity(255);
                        }
                        
                        if (highlighted_node) data.unhighlight_node(highlighted_node);
                        highlighted_node = nullptr;
                        
                        // Đem màu đỏ về lại Node con
                        if (child) {
                            highlighted_node = child;
                            data.highlight_node(highlighted_node, NODE_DELETE_COLOR);
                        }
                    }
                    else if (step.type == StepType::DeleteLerp) {
                        // Gắn lại nguyên cành cây
                        if (step.stored_subtree) {
                            step.node->pnext[step.char_id] = step.stored_subtree;
                            step.stored_subtree = nullptr;
                        }
                        if (highlighted_node) data.unhighlight_node(highlighted_node);
                        highlighted_node = nullptr;
                        
                        // Trả lại form đứng cũ
                        data.create_visual_lerp(0.4f);
                        
                        // Phục hồi lại màu xoá đỏ trên node cha cao nhất
                        if (cur_step - 1 >= 0 && anim_queue[cur_step - 1].type == StepType::DeleteNodeMark) {
                            highlighted_node = anim_queue[cur_step - 1].node;
                            data.highlight_node(highlighted_node, NODE_DELETE_COLOR);
                        }
                    }
                }

                cur_step--;
            }

            // -- Skip --
            if(skip_button.contains(mousePos)){
                std::cout << "Received Skip Operation" << "\n";
                skip_animation();
            }

            // Input box handling
            if(input_box.contains(mousePos)){
                std::cout << "Received Input Box Activation: On" << "\n";
                input_box_active = true;
                input_box.update(input_box_active, mousePos);
                if(current_input.empty()) input_box.setLabel("|");
            } else
            if(!operation_button_pressed && input_box_active){
                std::cout << "Received Input Box Activation: Off" << "\n";
                input_box_active = false;
                if(current_input.empty()) input_box.setLabel("   Type here");
            }

            if(build_box.contains(mousePos)){
                std::cout << "Received Build Box Activation: On" << "\n";
                build_box_active = true;
                build_box.update(build_box_active, mousePos);
                if(build_input.empty()) build_box.setLabel("|");
            } else
            if(!build_button_pressed && build_box_active){
                std::cout << "Received Build Box Activation: Off" << "\n";
                build_box_active = false;
                if(build_input.empty()) build_box.setLabel("   Type here");
            }
        }
        left_mouse_last = left_mouse;
        if(frame_count % pace == 0 && mode == ModeType::Auto && !anim_queue.empty() && cur_step + 1 < anim_queue.size()){
            next_animation();
        }

        // Receive event of present frame
        while(const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>())
                window.close();
            if (input_box_active) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    bool change = false;
                    if (textEvent->unicode == '\b' || textEvent->unicode == 8) {
                        if (!current_input.empty()) { current_input.pop_back(); change = true; }
                    }
                    else if (textEvent->unicode >= 97 && textEvent->unicode <= 122) {
                        if (current_input.size() < max_charater) {current_input += static_cast<char>(textEvent->unicode); change = true;}
                    }
                    input_box.setLabel(current_input + "|"); 
                    if(change) std::cout << "Received new input string: " << current_input << "\n";
                }
            }
            if (build_box_active) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    bool change = false;
                    if (textEvent->unicode == '\b' || textEvent->unicode == 8) {
                        if (!build_input.empty()) { build_input.pop_back(); change = true; }
                    }
                    else if (textEvent->unicode >= 97 && textEvent->unicode <= 122) {
                        build_input += static_cast<char>(textEvent->unicode); change = true;
                    }
                    else if (textEvent->unicode == ',') {
                        build_input += ','; change = true;
                    }
                    if(build_input.size() <= max_character_show)
                        build_box.setLabel(build_input + "|");
                    else
                        build_box.setLabel(build_input.substr(build_input.size() - max_character_show, max_character_show) + "|");    
                    if(change) std::cout << "Received new build input string: " << build_input << "\n";
                }
            }
        }

        // Frame count
        ++frame_count;
        if(frame_count % 20 == 0)
            std::cerr << "Frame :" << frame_count << "\n";

        // --- Cập nhật vị trí node (lerp) và edge ---
        std::function<void(NodeTrie*)> update_nodes = [&](NodeTrie* p) {
            if (!p) return;
            if (p->visual_node) p->visual_node->updatePosition(dt);
            for (int i = 0; i < LOWERCASE_CHAR; ++i) update_nodes(p->pnext[i]);
        };
        update_nodes(data.root);
        data.update_edges(data.root);
        
        // --- Cập nhật biến dạng Progress Slider ---
        if (!anim_queue.empty()) {
            float ratio = float(cur_step + 1) / float(anim_queue.size());
            if (ratio < 0) ratio = 0.f;
            if (ratio > 1) ratio = 1.f;
            slider_run.setSize({slider_fix.getSize().x * ratio, slider_fix.getSize().y});
        } else {
            slider_run.setSize({0.f, slider_fix.getSize().y});
        }

        // --- Cập nhật Line cho Highlight Code nếu có
        if(current_animation_type != OperationType::None && cur_step >= 0){
            codeBox.setStep(anim_queue[cur_step].code_line);
        }

        // --- Draw new frame --- //
        window.clear(sf::Color(212, 188, 112, 0.71));
        window.draw(backgroundSprite);
        return_button.draw(window);
        add_button.draw(window);
        delete_button.draw(window);
        find_button.draw(window);
        clear_button.draw(window);
        build_button.draw(window);
        file_button.draw(window);
        random_button.draw(window);
        window.draw(bgvisual);
        back_button.draw(window);
        next_button.draw(window);
        speedup_button.draw(window);
        slowdown_button.draw(window);
        skip_button.draw(window);
        mode_button.draw(window);
        input_box.draw(window);
        build_box.draw(window);
        speed_box.draw(window);
        window.draw(slider_fix);
        window.draw(slider_run);
        data.draw(window);

        // Code Box draw
        window.draw(codeBox);

        window.display();
    }
}

node* create_node(int block_x, int block_y, bool isend){
    float x, y;
    x = block_unit * block_x;
    y = block_unit * block_y;
    x = x + bgvisual.getPosition().x;
    y = y + bgvisual.getPosition().y;
    x = x + block_unit / 2;
    y = y + block_unit / 2;
    node* getnode;
    if(!isend)
        getnode = new node(x, y, node_radius, NODE_FILL_COLOR, NODE_OUTLINE_COLOR, node_outline_thickness);
    else
        getnode = new node(x, y, node_radius, NODE_END_COLOR, NODE_OUTLINE_COLOR, node_outline_thickness);
    return getnode;
}

Trie::Trie()
{
    root = new NodeTrie();
}

Trie::~Trie()
{
    if (root) {
        clear();
        if (root->visual_node) {
            delete root->visual_node;
            root->visual_node = nullptr;
        }
        delete root;
        root = nullptr;
    }
}

void Trie::init()
{
    if (!root) {
        root = new NodeTrie();   
    } else {
        clear();
    }
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
            if (thepath[i - 1]->visual_edge[id] != nullptr) {
                delete thepath[i - 1]->visual_edge[id];
                thepath[i - 1]->visual_edge[id] = nullptr;
            }
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
    } else {
        if (pnode->visual_node) {
            delete pnode->visual_node;
            pnode->visual_node = nullptr;
        }
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
    // --- Tính tọa độ đích ---
    float x = block_unit * block_x + bgvisual.getPosition().x + block_unit / 2;
    float y = block_unit * block_y + bgvisual.getPosition().y + block_unit / 2;

    if (pnode->visual_node == nullptr) {
        // Node chưa có visual -> tạo mới
        if(pnode->isend)
            pnode->visual_node = create_node(block_x, block_y, true);
        else
            pnode->visual_node = create_node(block_x, block_y);
        if (char_branch == -1)
            pnode->visual_node->setLabel("R", 20);
        else {
            std::string tmp = "";
            tmp.push_back(char_branch + 'a');
            pnode->visual_node->setLabel(tmp, 20);
        }
        // Đặt targetPos và currentPos về cùng vị trí (không lerp khi mới tạo)
        pnode->visual_node->targetPos  = {x, y};
        pnode->visual_node->currentPos = {x, y};
        pnode->visual_node->startPos   = {x, y};
        pnode->visual_node->setPosition(x, y);
    } else {
        // Node đã có visual -> chỉ cập nhật targetPos, giữ nguyên currentPos để lerp
        pnode->visual_node->startPos  = pnode->visual_node->currentPos;
        pnode->visual_node->targetPos = {x, y};
    }

    // Calculation
    int run_block = block_x - pnode->block_need / 2;
    bool previous = false;
    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnode->pnext[i] != nullptr){
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
        if (pnode->visual_edge[i] == nullptr) {
            pnode->visual_edge[i] = new edge(x_cur, y_cur, x_nxt, y_nxt, EDGE_COLOR, 2);
        }
        pnode->visual_edge[i]->setPoints(x_cur, y_cur, x_nxt, y_nxt, false, node_radius);
        // Traverse
        cre_edge(pnode->pnext[i]);
    }
}

void Trie::drawing(NodeTrie *pnode, sf::RenderWindow& window)
{
    // Node chưa có visual (chưa qua Lerp step) → bỏ qua
    if (!pnode->visual_node) return;
    // Draw edge (chỉ nếu edge tồn tại)
    for(int i = 0; i < LOWERCASE_CHAR; ++i) if(pnode->pnext[i] != nullptr && pnode->visual_edge[i] != nullptr){
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

void Trie::create_visual_lerp(float speed)
{
    // Tính lại layout. cre_node sẽ chỉ cập nhật targetPos cho node đã có,
    // và tạo mới visual_node cho node chưa có (startNodeMovement sẽ lerp từ startPos).
    calculate_block();
    create_visual_node();
    create_visual_edge();
    // Kích hoạt lerp cho tất cả node đã có visual
    std::function<void(NodeTrie*)> activate = [&](NodeTrie* p) {
        if (!p) return;
        if (p->visual_node && p->visual_node->startPos != p->visual_node->targetPos) {
            startNodeMovement(*p->visual_node, p->visual_node->targetPos, speed);
        }
        for (int i = 0; i < LOWERCASE_CHAR; ++i) activate(p->pnext[i]);
    };
    activate(root);
}

void Trie::draw(sf::RenderWindow &window)
{
    drawing(root, window);
}

// ===================== ANIMATION HELPERS =====================

std::vector<AnimStep> Trie::add_with_anim(std::string s)
{

    std::vector<AnimStep> steps;

    // Bước 1: Tìm k — vị trí đầu tiên cần tạo node mới
    NodeTrie* pnow = root;
    int k = 0;
    while (k < (int)s.size() && pnow->pnext[s[k] - 'a'] != nullptr) {
        pnow = pnow->pnext[s[k] - 'a'];
        ++k;
    }
    // s[0..k-1] → Move (node cũ)
    // s[k..n-1] → 1 Lerp (tại k) + Create cho từng node mới

    // Bước 2: Thêm thật vào data (không có temp node, không có dangling pointer)
    add(s);

    // Reset isend của node cuối: MarkEnd animation sẽ set lại
    NodeTrie* last_node = root;
    for (char c : s) last_node = last_node->pnext[c - 'a'];
    last_node->isend = false;
    // Bước 3: Build queue với pointer thật
    pnow = root;
    NodeTrie* last_parent = pnow;
    int last_id = -1;
    bool had_new = (k < (int)s.size());

    for (int i = 0; i < (int)s.size(); ++i) {
        int id = s[i] - 'a';
        NodeTrie* parent = pnow;
        pnow = pnow->pnext[id];
        last_parent = parent;
        last_id = id;

        if (i < k) {
            steps.push_back({ StepType::Move, parent, id, nullptr, 4}); // lưu parent+id, nhất quán với Create/Lerp
        } else if (i == k) {
            // 1 Lerp duy nhất tại điểm rẽ nhánh
            steps.push_back({ StepType::Lerp,   parent, id, nullptr, 3});
            steps.push_back({ StepType::Create, parent, id, nullptr, 3});
        } else {
            // Các node mới tiếp theo: chỉ Create
            steps.push_back({ StepType::Create, parent, id, nullptr, 3});
        }
    }

    // MarkEnd
    if (!had_new) {
        // Từ đã tồn tại hoàn toàn → pnow là node thật
        steps.push_back({ StepType::MarkEnd, last_parent, last_id, nullptr, 6});
    } else {
        // Node cuối là node mới → dùng parent + char_id
        steps.push_back({ StepType::MarkEnd, last_parent, last_id, nullptr, 6});
    }

    return steps;
}



void Trie::highlight_node(NodeTrie* pnode, sf::Color color)
{
    if (pnode && pnode->visual_node)
        pnode->visual_node->setColor(color);
}

std::vector<AnimStep> Trie::remove_with_anim(std::string s)
{
    std::vector<AnimStep> steps;
    if (s.empty()) return steps;

    NodeTrie* pnow = root;
    std::vector<NodeTrie*> path;
    path.push_back(root);

    bool found = true;
    for (int i = 0; i < (int)s.size(); ++i) {
        int id = s[i] - 'a';
        if (pnow->pnext[id] == nullptr) {
            found = false;
            break;
        }
        steps.push_back({ StepType::Move, pnow, id, nullptr, 0});
        pnow = pnow->pnext[id];
        path.push_back(pnow);
    }

    if (!found || !pnow->isend) {
        steps.push_back({ StepType::NotFound, pnow, -1, nullptr, 1});
        return steps;
    }

    steps.push_back({ StepType::UnmarkEnd, pnow, -1, nullptr, 2});

    std::vector<int> deleted_indices;
    // Đi ngược để thu thập danh sách các node sẽ bị delete
    for(int i = path.size() - 1; i >= 1; --i){
        pnow = path[i];
        bool has_other = false;
        for(int c = 0; c < LOWERCASE_CHAR; ++c) {
            if(pnow->pnext[c] != nullptr) {
                if (i < s.size() && c == s[i] - 'a') continue; 
                has_other = true; 
            }
        }
        bool is_leaf = (i == s.size());
        if (has_other || (!is_leaf && pnow->isend)) {
            break; 
        }
        deleted_indices.push_back(i);
    }

    for (int k = 0; k < deleted_indices.size(); ++k) {
        int idx = deleted_indices[k];
        int id = s[idx - 1] - 'a';
        NodeTrie* parent = path[idx - 1];
        
        bool is_last_deleted = (k == deleted_indices.size() - 1);
        if (is_last_deleted) {
            steps.push_back({ StepType::DeleteNodeNotMark, parent, id, nullptr, 5});
        } else {
            steps.push_back({ StepType::DeleteNodeMark, parent, id, nullptr, 5});
        }
    }

    if (!deleted_indices.empty()) {
        int top_idx = deleted_indices.back();
        int top_id = s[top_idx - 1] - 'a';
        NodeTrie* top_parent = path[top_idx - 1];
        steps.push_back({ StepType::DeleteLerp, top_parent, top_id, nullptr, 7});
    }

    return steps;
}

std::vector<AnimStep> Trie::find_with_anim(std::string s)
{
    std::vector<AnimStep> steps;
    if (s.empty()) return steps;

    NodeTrie* pnow = root;
    bool found = true;
    for (int i = 0; i < (int)s.size(); ++i) {
        int id = s[i] - 'a';
        if (pnow->pnext[id] == nullptr) {
            found = false;
            break;
        }
        steps.push_back({ StepType::Move, pnow, id, nullptr, 4});
        pnow = pnow->pnext[id];
    }

    if (!found || !pnow->isend) {
        steps.push_back({ StepType::NotFound, pnow, -1, nullptr, 3});
    } else {
        steps.push_back({ StepType::Found, pnow, -1, nullptr, 6});
    }
    return steps;
}

std::vector<AnimStep> Trie::build_with_anim(std::vector<std::string> words)
{
    std::vector<AnimStep> total_steps;
    for (const auto& w : words) {
        if (w.empty()) continue;
        auto steps = add_with_anim(w);
        total_steps.insert(total_steps.end(), steps.begin(), steps.end());
    }
    return total_steps;
}

void Trie::unhighlight_node(NodeTrie* pnode)
{
    if (!pnode || !pnode->visual_node) return;
    if (pnode->isend)
        pnode->visual_node->setColor(NODE_END_COLOR);
    else
        pnode->visual_node->setColor(NODE_FILL_COLOR);
}

void Trie::update_edges(NodeTrie* pnode)
{
    if (!pnode || !pnode->visual_node) return;
    for (int i = 0; i < LOWERCASE_CHAR; ++i) {
        if (pnode->pnext[i] && pnode->pnext[i]->visual_node && pnode->visual_edge[i]) {
            float x_cur = pnode->visual_node->currentPos.x;
            float y_cur = pnode->visual_node->currentPos.y;
            float x_nxt = pnode->pnext[i]->visual_node->currentPos.x;
            float y_nxt = pnode->pnext[i]->visual_node->currentPos.y;
            pnode->visual_edge[i]->setPoints(x_cur, y_cur, x_nxt, y_nxt, false, node_radius);
        }
        update_edges(pnode->pnext[i]);
    }
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
