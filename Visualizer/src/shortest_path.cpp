#include "../headers/shortest_path.h"
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <chrono>

dijsktra::dijsktra(int n) {
    dist.resize(n+1, 1e9);
    adj.resize(n+1);
    step_history.clear();
}

void dijsktra::resize(int n) {
    dist.resize(n+1);
    adj.resize(n+1);
    for(auto &x:dist) x = 1e9;
    for(auto &x:adj) x.clear();
    step_history.clear();
    weights.clear();
}

void dijsktra::clear() {
    dist.clear();
    adj.clear();
    step_history.clear();
    weights.clear();
}

void dijsktra::init(std::vector<std::pair<int,int>> &edges, std::vector<int> &edge_weights, bool directed_flag) {
    int max_N = 0;
    for(auto edge:edges) max_N = std::max(max_N, std::max(edge.first, edge.second));

    resize(max_N);
    weights = edge_weights;
    for(int i=0; i < std::min(edges.size(), edge_weights.size()); ++i) {
        add_edge(edges[i].first, edges[i].second, i, directed_flag);
        std::cout << edges[i].first << ' ' << edges[i].second << ' ' << weights[i] << '\n';
    }
}

void dijsktra::add_edge(int u, int v, int id, bool directed_flag) {
    if(u < 0 || u >= adj.size() || v < 0 || v >= adj.size()) return;

    adj[u].emplace_back(v, id);
    if(!directed_flag) {
        adj[v].emplace_back(u, id);
    }
}

void dijsktra::find_shortest_path(int start) {
    if(start < 0 or start >= dist.size()) {
        return;
    }
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> heap;
    dist[start] = 0;
    heap.emplace(0, start);

    Step cur;

    while (!heap.empty()) {
        auto [d, u] = heap.top();
        heap.pop();

        cur.nodes_state_changed.push_back({u, 1});
        //mark
        step_history.push_back({1, cur});

        if (d > dist[u]) continue;

        for (auto& [v, id] : adj[u]) {
            cur.edges_state_changed.push_back({id, 2});
            // mark
            step_history.push_back({2, cur});
            if (dist[u] + weights[id]< dist[v]) {
                cur.nodes_state_changed.push_back({v, 1});

                //mark
                step_history.push_back({3, cur});

                dist[v] = dist[u] + weights[id];
                heap.emplace(dist[v], v);
                // mark
                step_history.push_back({4, cur});

                cur.nodes_state_changed.pop_back();
            }
            cur.edges_state_changed.pop_back();
        }

        cur.nodes_state_changed.pop_back();
        cur.nodes_state_changed.push_back({u, 3});
    }
    
    step_history.push_back({5,cur});

    for(int i=0;i<dist.size();++i) {
        std::cout << i << ' ' << dist[i] << '\n';
    }
}

void dijsktra::_visual(Visual_graph &graph, RoundedRectangleShape &visual_code_region, sf::Text &text_for_code, int cur_step) {

    RoundedRectangleShape highlight;
    highlight.setRadius(18.0f);
    highlight.setCornerPointCount(40);

    float max_right = visual_code_region.getGlobalBounds().position.x + visual_code_region.getGlobalBounds().size.x;

    int id = step_history[cur_step].first;
    Step state = step_history[cur_step].second;

    for(auto [v, st]:state.nodes_state_changed) {
         graph.setNodeState(v, st);
    }

    for(auto [e, st]:state.edges_state_changed) {
        graph.setEdgeState(e,st);
    }

    graph.draw(window, 1);

    // draw code

    window.draw(visual_code_region);
    for(int i = 0; i < code.size(); ++i) {
        text_for_code.setString(code[i]);
        text_for_code.setFillColor(sf::Color::Black);
        text_for_code.setOrigin({0 , text_for_code.getLocalBounds().size.y / 2.0f});


        text_for_code.setPosition({visual_code_region.getGlobalBounds().position.x+10, visual_code_region.getGlobalBounds().position.y + 15 * (i+1) + text_for_code.getLocalBounds().size.y * i + 25});

        if(id == i) {
            // red
            highlight.setFillColor(sf::Color(246,88,88,100));
            highlight.setPosition({text_for_code.getGlobalBounds().position.x, text_for_code.getGlobalBounds().position.y - text_for_code.getLocalBounds().size.y / 2 + 10});

            highlight.setSize({max_right - highlight.getGlobalBounds().position.x - 20, text_for_code.getLocalBounds().size.y + 20});

            window.draw(highlight);
        }

        window.draw(text_for_code);
    }

    for(auto [v, st]:state.nodes_state_changed) {
         graph.setNodeState(v, 0);
    }

    for(auto [e, st]:state.edges_state_changed) {
        graph.setEdgeState(e, 0);
    }
}

void case_move_node(Visual_graph& Visual_graph, RoundedRectangleShape& visual_region, sf::RectangleShape safe_region) {
    int idx = Visual_graph.find_node_stored(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    
    int mouse_left_pressed = 0;
    if(idx != -1) {
        do {
            int x = sf::Mouse::getPosition(window).x;
            int y = sf::Mouse::getPosition(window).y;
            int radius = Visual_graph.getRadius();
            if(x - radius < safe_region.getPosition().x) x = safe_region.getPosition().x + radius;
            if(y - radius < safe_region.getPosition().y) y = safe_region.getPosition().y + radius;
            if(x + radius > safe_region.getPosition().x + safe_region.getSize().x) x = safe_region.getPosition().x + safe_region.getSize().x - radius;
            if(y + radius > safe_region.getPosition().y + safe_region.getSize().y) y = safe_region.getPosition().y + safe_region.getSize().y - radius;

            Visual_graph.setPosition(idx, x, y);

            window.draw(visual_region);
            Visual_graph.draw(window);
            window.display();

            mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        } while(mouse_left_pressed);
    }
}

void case_add_edge_by_click(int iidx, Visual_graph& Visual_graph, RoundedRectangleShape& visual_region, sf::RectangleShape &safe_region, int weight) {
    int mouse_pressed = 1;
    edge temp_edge(0, 0, 0, 0, sf::Color::Black, Visual_graph.getEdgeThinkness());
    sf::Vector2f pos_u = Visual_graph.getPosition(iidx);
    float x, y;
    
    while(mouse_pressed) {
        mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        x = sf::Mouse::getPosition(window).x;
        y = sf::Mouse::getPosition(window).y;

        if(x < safe_region.getPosition().x + 10) x = safe_region.getPosition().x + 10;
        if(y < safe_region.getPosition().y + 10) y = safe_region.getPosition().y + 10;
        if(x > safe_region.getPosition().x + safe_region.getSize().x - 10) x = safe_region.getPosition().x + safe_region.getSize().x - 10;
        if(y > safe_region.getPosition().y + safe_region.getSize().y - 10) y = safe_region.getPosition().y + safe_region.getSize().y - 10;

        temp_edge.setPoints(pos_u.x, pos_u.y, x, y, Visual_graph.isDirected(), 0);


        window.draw(visual_region);

        temp_edge.draw(window);

        Visual_graph.draw(window, 1);
        window.display();
    }
    int idx = Visual_graph.find_node_stored(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    if(idx != -1 and idx != iidx) {
        Visual_graph.add_edge(iidx, idx, weight, Visual_graph.isDirected());
    }
}


void shortest_path_page() {

    dijsktra graph(0);
    Visual_graph Visual_graph;

    const float visual_region_width = WINDOW_WIDTH - 100;
    const float visual_region_height = WINDOW_HEIGHT - 480;

    const float button_width = 150;
    const float button_height = 60;

    const float visual_code_region_width = 700;
    const float visual_code_region_height = (button_height + 20) * 4 + 80;

    RoundedRectangleShape visual_region, visual_code_region;
    sf::RectangleShape safe_region;

    sf::Text text_for_code(font_impact, "", 50);

    visual_region.setFillColor(sf::Color(235, 235, 235)); // transparent white
    visual_region.setOutlineColor(sf::Color(0, 0, 0, 200));
    visual_region.setOutlineThickness(2.0f);
    visual_region.setSize({visual_region_width, visual_region_height});
    visual_region.setRadius(18.0f);
    visual_region.setCornerPointCount(40);
    visual_region.setPosition({50, 40});

    visual_code_region.setFillColor(sf::Color(200, 200, 200));
    visual_code_region.setOutlineColor(sf::Color(0,0,0,200));
    visual_code_region.setOutlineThickness(2.0f);
    visual_code_region.setSize({visual_code_region_width, visual_code_region_height});
    visual_code_region.setRadius(8.0f);
    visual_code_region.setCornerPointCount(20);
    visual_code_region.setPosition({WINDOW_WIDTH-visual_code_region_width-50, visual_region.getGlobalBounds().position.y + visual_region.getGlobalBounds().size.y + 20});

    safe_region.setSize({visual_region_width - Visual_graph.getRadius() * 2, visual_region_height - Visual_graph.getRadius() * 2});
    safe_region.setPosition({50.0f + Visual_graph.getRadius(), 40.0f + Visual_graph.getRadius()});


    std::vector<button> buttons;
    bool state_buttons[5] = {0, 0, 0, 0, 0};
    float visual_region_y = visual_region.getGlobalBounds().position.y + visual_region.getGlobalBounds().size.y;
    float visual_region_x = visual_region.getGlobalBounds().position.x;
    buttons.emplace_back(visual_region_x, visual_region_y + (button_height + 20) * 4, button_width, button_height, sf::Color(255,255,255,50), "Find Path", 24);
    buttons.emplace_back(visual_region_x, visual_region_y + (button_height + 20) * 3, button_width, button_height, sf::Color(255,255,255,50), "Read Graph", 24);
    buttons.emplace_back(visual_region_x, visual_region_y + (button_height + 20) * 2, button_width, button_height, sf::Color(255,255,255,50), "Add Edge", 24);
    buttons.emplace_back(visual_region_x, visual_region_y + (button_height + 20) * 1, button_width, button_height, sf::Color(255,255,255,50), "Add Node", 24);
    buttons.emplace_back(visual_region_x + button_width + 10, visual_region_y + (button_height + 20) * 1, button_width, button_height, sf::Color(255,255,255,50), Visual_graph.isDirected() ? "Directed" : "Undirected", 24);
    for(int i = 0; i < 5; i++) {
        buttons[i].setOutline(sf::Color::Transparent, 0.0f);
        buttons[i].setColor(sf::Color(255,255,255,50));
    }

    box input_weight_box(visual_region_x + button_width + 10, visual_region_y + (button_height + 20) * 2, button_width, button_height, sf::Color(255,255,255,50), "5", 24);

    input_weight_box.setOutline(sf::Color::Transparent, 0.0f);
    std::string current_input_weight = "5";


    std::string source_vertrix = "0";

    sf::FloatRect po = buttons[0].getShape();
    box input_source_box(po.position.x + po.size.x + 10, po.position.y, button_width - 30, button_height, sf::Color(255,255,255,50), "0", 24);
    input_source_box.setOutline(sf::Color::Transparent, 0.0f);

    button pre_step_button(po.position.x + po.size.x + button_width + 20, po.position.y, button_width, button_height, sf::Color(255,255,255,50), "Prev", 24);
    pre_step_button.setOutline(sf::Color::Transparent, 0.0f);

    button start_button(po.position.x + po.size.x + (button_width + 10) * 2 + 10, po.position.y, button_width, button_height, sf::Color(255,255,255,50), "Start", 24);
    start_button.setOutline(sf::Color::Transparent, 0.0f);

    button nxt_step_button(po.position.x + po.size.x + (button_width+10) * 3 + 10, po.position.y, button_width, button_height, sf::Color(255,255,255,50), "Next", 24);
    nxt_step_button.setOutline(sf::Color::Transparent, 0.0f);

    button back_button(po.position.x + po.size.x + (button_width+10) + 10, buttons[1].getShape().position.y, button_width, button_height, sf::Color(255,255,255,50), "Back", 24);
    back_button.setOutline(sf::Color::Transparent, 0.0f);



    bool mouse_left_pressed = 0;
    bool mouse_left_pressed_last = 1;
    bool input_weight_box_active = 0;
    bool is_start_button_pressed = 0;

    int cur_step = 0;

    auto now_time = std::chrono::system_clock::now();
    auto duration = now_time.time_since_epoch();
    long long last_step = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();


    while (window.isOpen()) {
        now_time = std::chrono::system_clock::now();
        duration = now_time.time_since_epoch();

        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if(mouse_left_pressed and !mouse_left_pressed_last) {
            for(int i = 0; i < 5; i++) {
                if(buttons[i].isClicked(sf::Mouse::getPosition(window))) {
                    state_buttons[i] = !state_buttons[i];
                    if(state_buttons[i]) {
                            for(int j = 0; j < 5; j++) {
                            if(j != i) state_buttons[j] = 0;
                        }
                        if(i == 0) {
                            is_start_button_pressed = 0;
                            cur_step = 0;
                            graph.init(Visual_graph.getEdgeList(), Visual_graph.getEdgeWeightsList(), Visual_graph.isDirected());
                            graph.find_shortest_path(std::stoi(source_vertrix));
                        }
                        else if(i == 4) {
                            bool cur_directed = Visual_graph.isDirected();
                            Visual_graph.setDirected(!cur_directed);
                            buttons[4].setLabel(Visual_graph.isDirected() ? "Directed" : "Undirected");

                            // Toggle action doesn't remain selected as a mode button
                            for(int j = 0; j < 5; j++) state_buttons[j] = 0;
                        }
                    }

                    break;
                }
            }
        }

        // case: add node 
        if(state_buttons[3] and mouse_left_pressed and !mouse_left_pressed_last) {
            if(in_rect(sf::Mouse::getPosition(window), safe_region.getGlobalBounds())) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                Visual_graph.add_node(mousePos.x, mousePos.y);
            }
        }

        // case: add edge 
        if(state_buttons[2] and mouse_left_pressed and !mouse_left_pressed_last) {
            int idx = Visual_graph.find_node_stored(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            if(idx != -1) {
                case_add_edge_by_click(idx, Visual_graph, visual_region, safe_region, std::stoi(current_input_weight));
            }
        }

        if(state_buttons[2]) {
            if(mouse_left_pressed and !mouse_left_pressed_last and input_weight_box.isClicked(sf::Mouse::getPosition(window))) {
                input_weight_box_active = 1;
            }
        } else {
            input_weight_box_active = 0;
        }

        if(state_buttons[2] and input_weight_box_active) {
            char c = input_weight_box.KeyboardToChar();
            if(c != '\0') {
                if(current_input_weight.size() == 1 and current_input_weight[0] == '0') current_input_weight = "";
                if(c == '\b') {
                    if(current_input_weight.size() > 0) current_input_weight.pop_back();
                }
                else if('0' <= c and c <= '9') {
                    current_input_weight.push_back(c);
                }
            }
        }

        if(current_input_weight.size() == 0) current_input_weight = "0";


        // case: Find path

        if(state_buttons[0]) {
            char c = input_source_box.KeyboardToChar();
            if(c != '\0') {
                if(source_vertrix.size() == 1 and source_vertrix[0] == '0') source_vertrix = "";
                if(c == '\b') {
                    if(source_vertrix.size() > 0) source_vertrix.pop_back();
                }
                else if('0' <= c and c <= '9') {
                    source_vertrix.push_back(c);
                    cur_step = 0;
                    graph.init(Visual_graph.getEdgeList(), Visual_graph.getEdgeWeightsList(), Visual_graph.isDirected());
                    graph.find_shortest_path(std::stoi(source_vertrix));
                }
            }

            if(source_vertrix.size() == 0) source_vertrix = "0";

            if(mouse_left_pressed == 1 and !mouse_left_pressed_last and start_button.isClicked(sf::Mouse::getPosition(window))) {
                is_start_button_pressed ^= 1;
                if(is_start_button_pressed == 1) {
                    last_step = now;
                }
            }

            if(mouse_left_pressed == 1 and !mouse_left_pressed_last and pre_step_button.isClicked(sf::Mouse::getPosition(window))) {
                if(cur_step > 0) cur_step--;
                is_start_button_pressed = 0;
            }

            if(mouse_left_pressed == 1 and !mouse_left_pressed_last and nxt_step_button.isClicked(sf::Mouse::getPosition(window))) {
                if(cur_step < graph.step_history.size()-1) cur_step++;
                is_start_button_pressed = 0;
            }

            if(is_start_button_pressed and now - last_step > 1000) {
                last_step = now;
                if(cur_step < graph.step_history.size()-1) cur_step++;
            }
        }

        //

        // case: not one of buttons is active | move node
        bool no_button_active = 1;
        for(int i=0; i<4; i++) {
            no_button_active &= !state_buttons[i];
        }

        if(no_button_active and mouse_left_pressed and !mouse_left_pressed_last) {
            case_move_node(Visual_graph, visual_region, safe_region);
        }
        // 

        if(mouse_left_pressed and !mouse_left_pressed_last and back_button.isClicked(sf::Mouse::getPosition(window))) {
            return;
        }

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(235, 235, 235)); // soft gray background

        // draw region 
        window.draw(visual_region);
        window.draw(visual_code_region);

        // draw buttons with transparency + hover border
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for(int i = 0; i < 5; i++) {
            // transparent fill for all, active semi-green
            if(state_buttons[i]) {
                buttons[i].setColor(sf::Color(100, 220, 100, 100));
            } else {
                buttons[i].setColor(sf::Color(255, 255, 255, 50));
            }

            if(buttons[i].contains(mousePos)) {
                buttons[i].setOutline(sf::Color::Black, 2.0f);
            } else {
                buttons[i].setOutline(sf::Color::Transparent, 0.0f);
            }

            buttons[i].draw(window);
        }

        // input boxes hover border
        bool input_weight_hover = input_weight_box.contains(mousePos);
        if(input_weight_hover) {
            input_weight_box.setOutline(sf::Color::Black, 2.0f);
        } else {
            input_weight_box.setOutline(sf::Color::Transparent, 0.0f);
        }

        bool input_source_hover = input_source_box.contains(mousePos);
        if(input_source_hover) {
            input_source_box.setOutline(sf::Color::Black, 2.0f);
        } else {
            input_source_box.setOutline(sf::Color::Transparent, 0.0f);
        }

        if(back_button.contains(mousePos)){
            back_button.setOutline(sf::Color::Black, 2.0f);
        } else {
            back_button.setOutline(sf::Color::Transparent, 0.0f);
        }
        back_button.draw(window);


        if(state_buttons[2]) {
            input_weight_box.setLabel(current_input_weight);
            input_weight_box.draw(window);
        }

        if(!(state_buttons[0] and cur_step >= 0 and cur_step < graph.step_history.size())) Visual_graph.draw(window, 1);

        if(state_buttons[0]) {
            input_source_box.setLabel(source_vertrix);
            input_source_box.draw(window);

            if(pre_step_button.contains(mousePos)) {
                pre_step_button.setOutline(sf::Color::Black, 2.0f);
                pre_step_button.setColor(sf::Color(100, 220, 100, 100));
            } else {
                pre_step_button.setOutline(sf::Color::Transparent, 0.0f);
                pre_step_button.setColor(sf::Color(255, 255, 255, 50));
            }
            pre_step_button.draw(window);

            if(start_button.contains(mousePos) or is_start_button_pressed){
                start_button.setOutline(sf::Color::Black, 2.0f);
                start_button.setColor(sf::Color(100, 220, 100, 100));
            } else {
                start_button.setOutline(sf::Color::Transparent, 0.0f);
                start_button.setColor(sf::Color(255, 255, 255, 50));
            }
            start_button.draw(window);

            if(nxt_step_button.contains(mousePos)) {
                nxt_step_button.setOutline(sf::Color::Black, 2.0f);
                nxt_step_button.setColor(sf::Color(100, 220, 100, 100));
            } else {
                nxt_step_button.setOutline(sf::Color::Transparent, 0.0f);
                nxt_step_button.setColor(sf::Color(255, 255, 255, 50));
            }
            nxt_step_button.draw(window);

            if(cur_step >= 0 and cur_step < graph.step_history.size()) {
                graph._visual(Visual_graph, visual_code_region, text_for_code, cur_step);
            }
        }

        if(!(state_buttons[0] and cur_step >= 0 and cur_step < graph.step_history.size())) for(int i = 0; i < graph.code.size(); ++i) {
            text_for_code.setString(graph.code[i]);
            text_for_code.setFillColor(sf::Color::Black);
            text_for_code.setOrigin({0 , text_for_code.getLocalBounds().size.y / 2.0f});

            text_for_code.setPosition({visual_code_region.getGlobalBounds().position.x + 10, visual_code_region.getGlobalBounds().position.y + 15 * (i+1) + text_for_code.getLocalBounds().size.y * i + 25});

            window.draw(text_for_code);
        }
        

        window.display();
        mouse_left_pressed_last = mouse_left_pressed;
    }
}