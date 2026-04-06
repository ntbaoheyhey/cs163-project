#include "../headers/shortest_path.h"
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <chrono>
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <tuple>

dijsktra::dijsktra(int n) {
    dist.resize(n+1, 1e9);
    adj.resize(n+1);
    labels.resize(n+1);
    for(int i=0; i<=n; i++) labels[i] = std::to_string(i);
    step_history.clear();
}

void dijsktra::resize(int n) {
    dist.resize(n+1);
    adj.resize(n+1);
    labels.resize(n+1);
    for(auto &x:dist) x = 1e9;
    for(auto &x:adj) x.clear();
    for(int i=0; i<=n; i++) labels[i] = std::to_string(i);
    step_history.clear();
    weights.clear();
}

void dijsktra::clear() {
    dist.clear();
    adj.clear();
    labels.clear();
    step_history.clear();
    weights.clear();
}

void dijsktra::init(std::vector<std::pair<int,int>> &edges, std::vector<int> &edge_weights, bool directed_flag, const std::vector<std::string> &label_map) {
    int max_N = 0;
    for(auto edge:edges) max_N = std::max(max_N, std::max(edge.first, edge.second));

    resize(max_N);
    if(!label_map.empty() && label_map.size() == labels.size()) {
        labels = label_map;
    }
    weights = edge_weights;
    for(int i=0; i < std::min(edges.size(), edge_weights.size()); ++i) {
        add_edge(edges[i].first, edges[i].second, i, directed_flag);
        std::cout << edges[i].first << ' ' << edges[i].second << ' ' << weights[i] << '\n';
    }
}

int dijsktra::label_to_index(const std::string &label) const {
    for(int i = 0; i < labels.size(); ++i) {
        if(labels[i] == label) return i;
    }
    return -1;
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

        step_history.push_back({1, cur});
        cur.nodes_state_changed.push_back({u, 1});
        step_history.push_back({2, cur});

        if (d > dist[u]) continue;

        for (auto& [v, id] : adj[u]) {
            step_history.push_back({3, cur});
            cur.edges_state_changed.push_back({id, 2});
            // mark
            step_history.push_back({4, cur});
            if (dist[u] + weights[id]< dist[v]) {
                cur.nodes_state_changed.push_back({v, 1});

                //mark
                dist[v] = dist[u] + weights[id];
                step_history.push_back({5, cur});

                heap.emplace(dist[v], v);
                step_history.push_back({6, cur});

                cur.nodes_state_changed.pop_back();
            }
            step_history.push_back({7, cur});
            cur.edges_state_changed.pop_back();
        }

        cur.nodes_state_changed.pop_back();
        cur.nodes_state_changed.push_back({u, 3});
    }
    
    step_history.push_back({8,cur});

    for(int i=0;i<dist.size();++i) {
        std::cout << labels[i] << ' ' << dist[i] << '\n';
    }
}

void dijsktra::_visual(Visual_graph &graph, RoundedRectangleShape &visual_code_region, sf::Text &text_for_code, int cur_step) {

    sf::RectangleShape highlight;

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


        text_for_code.setPosition({visual_code_region.getGlobalBounds().position.x+4, visual_code_region.getGlobalBounds().position.y + 15 * (i+1) + text_for_code.getLocalBounds().size.y * i + 5});

        if(id == i) {
            // red
            highlight.setFillColor(sf::Color(246,88,88,100));
            highlight.setPosition({text_for_code.getGlobalBounds().position.x, text_for_code.getGlobalBounds().position.y - text_for_code.getLocalBounds().size.y / 2 });

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


bool read_graph_from_file(Visual_graph &vg, dijsktra &graph, bool directed) {
    OPENFILENAMEA ofn;
    char szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn)) {
        vg.clearAll();
        graph.clear();

        std::ifstream file(szFile);
        if (!file) return false;
        int M;
        file >> M;
        if (file.fail() || M <= 0) return false;
        std::vector<std::tuple<int, int, int>> edge_list;
        std::set<int> vertices;
        for (int i = 0; i < M; ++i) {
            int u, v, w;
            file >> u >> v >> w;
            if (file.fail() || w < 0) return false;
            edge_list.emplace_back(u, v, w);
            vertices.insert(u);
            vertices.insert(v);
        }
        std::vector<int> verts(vertices.begin(), vertices.end());
        std::map<int, int> compress;
        for (size_t i = 0; i < verts.size(); ++i) {
            compress[verts[i]] = i;
        }
        int N = verts.size();
        // add nodes in circle
        float center_x = 400, center_y = 300, radius = 150;
        for (int i = 0; i < N; ++i) {
            float angle = 2 * 3.1415926535f * i / N;
            float x = center_x + radius * cos(angle);
            float y = center_y + radius * sin(angle);
            vg.add_node(x, y);
            vg.setNodeLabel(i, std::to_string(verts[i]));
        }
        // add edges
        for (auto [u, v, w] : edge_list) {
            int nu = compress[u], nv = compress[v];
            vg.add_edge(nu, nv, w, directed);
        }
        // update graph
        std::vector<std::string> label_map;
        label_map.reserve(N);
        for (int i = 0; i < N; ++i) {
            label_map.emplace_back(std::to_string(verts[i]));
        }
        graph.init(vg.getEdgeList(), vg.getEdgeWeightsList(), directed, label_map);
        return true;
    }
    return false;
}

void shortest_path_page() {

    dijsktra graph(0);
    Visual_graph Visual_graph;

    const float visual_region_width = 1019.0f;
    const float visual_region_height = 672.0f;

    const float button_width = 79.0f;
    const float button_height = 44.0f;

    const float visual_code_region_width = 301.0f;
    const float visual_code_region_height = 336.0f;

    RoundedRectangleShape visual_region, visual_code_region;
    sf::RectangleShape safe_region;

    sf::Text text_for_code(font_impact, "", 22);
    sf::Text error_text(font_impact, "", 16);
    error_text.setFillColor(sf::Color::Red);

    visual_region.setFillColor(sf::Color(217, 217, 217));
    visual_region.setOutlineColor(sf::Color(200, 200, 200, 255));
    visual_region.setOutlineThickness(2.0f);
    visual_region.setSize({visual_region_width, visual_region_height});
    visual_region.setRadius(18.0f);
    visual_region.setCornerPointCount(40);
    visual_region.setPosition({19.0f, 14.0f});

    visual_code_region.setFillColor(sf::Color(220, 220, 220));
    visual_code_region.setOutlineColor(sf::Color(180, 180, 180, 255));
    visual_code_region.setOutlineThickness(2.0f);
    visual_code_region.setSize({visual_code_region_width, visual_code_region_height});
    visual_code_region.setRadius(8.0f);
    visual_code_region.setCornerPointCount(20);
    visual_code_region.setPosition({1053.0f, 14.0f});

    safe_region.setSize({visual_region_width - Visual_graph.getRadius() * 2, visual_region_height - Visual_graph.getRadius() * 2});
    safe_region.setPosition({visual_region.getPosition().x + Visual_graph.getRadius(), visual_region.getPosition().y + Visual_graph.getRadius()});

    sf::RectangleShape slider_bar(sf::Vector2f(190.0f, 20.0f));
    slider_bar.setFillColor(sf::Color(210, 210, 210));
    slider_bar.setPosition({1057.0f, 609.0f});

    sf::CircleShape slider_knob(15.0f);
    slider_knob.setFillColor(sf::Color(100, 100, 100));
    slider_knob.setPosition({slider_bar.getPosition().x + slider_bar.getSize().x / 2.0f - slider_knob.getRadius(), slider_bar.getPosition().y - 5.0f});

    std::vector<button> buttons;
    bool state_buttons[5] = {0, 0, 0, 0, 0};
    float button_region_x = 1053.0f;
    float button_region_y = 364.0f;

    buttons.emplace_back(button_region_x, button_region_y + 116.0f, button_width, button_height, sf::Color(200, 200, 200), "Find Path", 16);
    buttons.emplace_back(button_region_x, button_region_y + 174.0f, button_width, button_height, sf::Color(200, 200, 200), "Read Graph", 16);
    buttons.emplace_back(button_region_x, button_region_y + 58.0f, button_width, button_height, sf::Color(200, 200, 200), "Add Edge", 16);
    buttons.emplace_back(button_region_x, button_region_y, button_width, button_height, sf::Color(200, 200, 200), "Add Node", 16);
    buttons.emplace_back(1151.0f, button_region_y, button_width, button_height, sf::Color(200, 200, 200), Visual_graph.isDirected() ? "Directed" : "Undirected", 16);
    for(int i = 0; i < 5; i++) {
        buttons[i].setOutline(sf::Color::Transparent, 0.0f);
        buttons[i].setColor(sf::Color(255,255,255,50));
    }

    box input_weight_box(1151.0f, button_region_y + 58.0f, button_width, button_height, sf::Color(200, 200, 200), "5", 16);
    input_weight_box.setOutline(sf::Color::Transparent, 0.0f);
    std::string current_input_weight = "5";

    std::string source_vertrix = "0";

    sf::FloatRect po = buttons[0].getShape();
    box input_source_box(1151.0f, button_region_y + 116.0f, button_width - 30.0f, button_height, sf::Color(200, 200, 200), "0", 16);
    input_source_box.setOutline(sf::Color::Transparent, 0.0f);

    button pre_step_button(1249.0f, button_region_y + 58.0f, button_width, button_height, sf::Color(200, 200, 200), "Prev", 16);
    pre_step_button.setOutline(sf::Color::Transparent, 0.0f);

    button start_button(1249.0f, button_region_y + 116.0f, button_width, button_height, sf::Color(200, 200, 200), "Start", 16);
    start_button.setOutline(sf::Color::Transparent, 0.0f);

    button nxt_step_button(1249.0f, button_region_y + 174.0f, button_width, button_height, sf::Color(200, 200, 200), "Next", 16);
    nxt_step_button.setOutline(sf::Color::Transparent, 0.0f);

    button back_button(1249.0f, button_region_y, button_width, button_height, sf::Color(200, 200, 200), "Back", 16);
    back_button.setOutline(sf::Color::Transparent, 0.0f);



    bool mouse_left_pressed = 0;
    bool mouse_left_pressed_last = 1;
    bool input_weight_box_active = 0;
    bool is_start_button_pressed = 0;
    bool auto_run_after_load = false;

    int cur_step = 0;

    std::string error_message = "FUK U";
    long long error_time = 0;
    int step_delay = 1000;
    bool dragging_slider = false;
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
                            graph.init(Visual_graph.getEdgeList(), Visual_graph.getEdgeWeightsList(), Visual_graph.isDirected(), graph.labels);
                            int start_index = graph.label_to_index(source_vertrix);
                            if(start_index >= 0) {
                                graph.find_shortest_path(start_index);
                            } else {
                                error_time = now;
                            }
                        }
                        else if(i == 4) {
                            bool cur_directed = Visual_graph.isDirected();
                            Visual_graph.setDirected(!cur_directed);
                            buttons[4].setLabel(Visual_graph.isDirected() ? "Directed" : "Undirected");

                            // Toggle action doesn't remain selected as a mode button
                            for(int j = 0; j < 5; j++) state_buttons[j] = 0;
                        }
                        else if(i == 1) {
                            if(!read_graph_from_file(Visual_graph, graph, Visual_graph.isDirected())) {
                                error_time = now;
                            } else {
                                const auto &node_labels = Visual_graph.getNodeLabels();
                                source_vertrix = node_labels.empty() ? "0" : node_labels[0];
                                int start_index = graph.label_to_index(source_vertrix);
                                if(start_index >= 0) {
                                    graph.find_shortest_path(start_index);
                                    state_buttons[0] = 1;
                                    for(int j = 1; j < 5; j++) state_buttons[j] = 0;
                                    cur_step = 0;
                                    step_delay = 5;
                                    is_start_button_pressed = 1;
                                    auto_run_after_load = true;
                                    last_step = now;
                                }
                            }
                            // Toggle off after action
                            state_buttons[i] = 0;
                        }
                    }

                    break;
                }
            }
        }

        // Slider update
        if (mouse_left_pressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (slider_knob.getGlobalBounds().contains({(float)mousePos.x,(float) mousePos.y}) || dragging_slider) {
                dragging_slider = true;
                float min_x = slider_bar.getPosition().x;
                float max_x = slider_bar.getPosition().x + slider_bar.getSize().x;
                float new_x = std::max(min_x, std::min(max_x, (float)mousePos.x));
                slider_knob.setPosition({new_x - slider_knob.getRadius(), slider_knob.getPosition().y});
                step_delay = 100 + (new_x - min_x) / slider_bar.getSize().x * (1000 - 5);
            }
        } else {
            dragging_slider = false;
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
                    graph.init(Visual_graph.getEdgeList(), Visual_graph.getEdgeWeightsList(), Visual_graph.isDirected(), Visual_graph.getNodeLabels());
                    int start_index = graph.label_to_index(source_vertrix);
                    if(start_index >= 0) {
                        graph.find_shortest_path(start_index);
                    }
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

            if(is_start_button_pressed and now - last_step > step_delay) {
                last_step = now;
                if(cur_step < graph.step_history.size()-1) cur_step++;
            }

            if(auto_run_after_load && graph.step_history.size() > 0 && cur_step == graph.step_history.size() - 1) {
                auto_run_after_load = false;
                is_start_button_pressed = 0;
                cur_step = 0;
                step_delay = 300;
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

            text_for_code.setPosition({visual_code_region.getGlobalBounds().position.x + 4, visual_code_region.getGlobalBounds().position.y + 15 * (i+1) + text_for_code.getLocalBounds().size.y * i + 5});

            window.draw(text_for_code);
        }

        if (error_message != "" && now - error_time < 3000) {
            error_text.setString(error_message);
            error_text.setPosition({300, 200});
            window.draw(error_text);
        }

        // Draw slider only in find_path mode
        if(state_buttons[0]) {
            window.draw(slider_bar);
            window.draw(slider_knob);
        }

        window.display();
        mouse_left_pressed_last = mouse_left_pressed;
    }
}