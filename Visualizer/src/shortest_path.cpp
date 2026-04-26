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

shortest_path_algorithm::shortest_path_algorithm(int n) {
    dist.resize(n+1, 1e9);
    adj.resize(n+1);
    labels.resize(n+1);
    for(int i=0; i<=n; i++) labels[i] = std::to_string(i);
    step_history.clear();
}

void shortest_path_algorithm::resize(int n) {
    dist.resize(n+1, 1e9);
    adj.resize(n+1);
    labels.resize(n+1);
    for(auto &x:dist) x = 1e9;
    for(auto &x:adj) x.clear();
    for(int i=0; i<=n; i++) labels[i] = std::to_string(i);
    step_history.clear();
    weights.clear();
}

void shortest_path_algorithm::clear() {
    dist.clear();
    adj.clear();
    labels.clear();
    step_history.clear();
    dist_history.clear();
    weights.clear();
}

void shortest_path_algorithm::init(std::vector<std::pair<int,int>> &edges, std::vector<int> &edge_weights, bool directed_flag, const std::vector<std::string> &label_map) {
    int max_N = 0;
    for(auto edge:edges) max_N = std::max(max_N, std::max(edge.first, edge.second));
    for(auto labels: label_map) {
        int idx = label_to_index(labels);
        if(idx != -1) {
            max_N = std::max(max_N, idx);
        }
    }
    resize(max_N);
    labels = label_map;
    weights = edge_weights;
    for(int i=0; i < std::min(edges.size(), edge_weights.size()); ++i) {
        add_edge(edges[i].first, edges[i].second, i, directed_flag);
    }
}

int shortest_path_algorithm::label_to_index(const std::string &label) const {
    for(int i = 0; i < labels.size(); ++i) {
        if(labels[i] == label) return i;
    }
    return -1;
}

void shortest_path_algorithm::add_edge(int u, int v, int id, bool directed_flag) {
    if(u < 0 || u >= adj.size() || v < 0 || v >= adj.size()) return;

    adj[u].emplace_back(v, id);
    if(!directed_flag) {
        adj[v].emplace_back(u, id);
    }
}

void shortest_path_algorithm::find_shortest_path(int start) {
    bool start_exists = false;
    for(int i=0; i<labels.size(); ++i) {
        if(labels[i] == std::to_string(start)) {
            start_exists = true;
            break;
        }
    }
    if(start_exists == false) {
        return;
    }
    step_history.clear();
    dist_history.clear();
    for(int &x:dist) x = 1e9;
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> heap;
    dist[start] = 0;
    heap.emplace(0, start);

    Step cur;

    while (!heap.empty()) {
        auto [d, u] = heap.top();
        heap.pop();

        step_history.push_back({1, cur});
        dist_history.push_back(dist);
        cur.nodes_state_changed.push_back({u, 1});
        step_history.push_back({2, cur});
        dist_history.push_back(dist);

        if (d > dist[u]) {
            cur.nodes_state_changed.pop_back();
            cur.nodes_state_changed.push_back({u, 3});
            continue;
        }
        
        for (auto& [v, id] : adj[u]) {
            step_history.push_back({3, cur});
            dist_history.push_back(dist);
            cur.edges_state_changed.push_back({id, 2});
            // mark
            step_history.push_back({4, cur});
            dist_history.push_back(dist);
            if (dist[u] + weights[id]< dist[v]) {
                cur.nodes_state_changed.push_back({v, 1});

                //mark
                dist[v] = dist[u] + weights[id];
                step_history.push_back({5, cur});
                dist_history.push_back(dist);

                heap.emplace(dist[v], v);
                step_history.push_back({6, cur});
                dist_history.push_back(dist);

                cur.nodes_state_changed.pop_back();
            }
            step_history.push_back({7, cur});
            dist_history.push_back(dist);
            cur.edges_state_changed.pop_back();
        }

        cur.nodes_state_changed.pop_back();
        cur.nodes_state_changed.push_back({u, 3});
    }
    
    step_history.push_back({8,cur});
    dist_history.push_back(dist);

}

void shortest_path_algorithm::find_shortest_path(int start, int algorithm) {
    switch (algorithm) {
        case ALGO_DIJKSTRA:
            find_shortest_path(start);
            break;
        case ALGO_BELLMAN_FORD:
            find_shortest_path_bellman_ford(start);
            break;
        default:
            find_shortest_path(start);
            break;
    }
}

bool shortest_path_algorithm::has_negative_weight() const {
    for (int w : weights) {
        if (w < 0) return true;
    }
    return false;
}

void shortest_path_algorithm::find_shortest_path_bellman_ford(int start) {
    if(start < 0 or start >= dist.size()) {
        return;
    }
    for(auto &x : dist) x = 1e9;
    dist[start] = 0;
    step_history.clear();
    dist_history.clear();
    Step cur;

    for(int iter = 0; iter < static_cast<int>(adj.size()) - 1; ++iter) {
        for(int u = 0; u < static_cast<int>(adj.size()); ++u) {
            step_history.push_back({0, cur});
            dist_history.push_back(dist);
            for (auto& [v, id] : adj[u]) {
                cur.nodes_state_changed.push_back({v, 3});
                cur.nodes_state_changed.push_back({u, 3});
                cur.edges_state_changed.push_back({id, 2});
                step_history.push_back({1, cur});
                dist_history.push_back(dist);
                if (dist[u] != 1e9 && dist[u] + weights[id] < dist[v]) {
                    cur.nodes_state_changed.pop_back();
                    cur.nodes_state_changed.pop_back();
                    cur.edges_state_changed.pop_back();
                    cur.nodes_state_changed.push_back({u, 1});
                    cur.nodes_state_changed.push_back({v, 1});
                    cur.edges_state_changed.push_back({id, 1});
                    dist[v] = dist[u] + weights[id];
                    step_history.push_back({3, cur});
                    dist_history.push_back(dist);
                    step_history.push_back({4, cur});
                    dist_history.push_back(dist);
                }
                cur.edges_state_changed.pop_back();
                cur.nodes_state_changed.pop_back();
                cur.nodes_state_changed.pop_back();

                step_history.push_back({5, cur});
                dist_history.push_back(dist);
            }
        }
    }
    
    step_history.push_back({7, cur});
    dist_history.push_back(dist);
}

void shortest_path_algorithm::_visual(Visual_graph &graph, RoundedRectangleShape &visual_code_region, sf::Text &text_for_code, int cur_step) {

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

    // Draw distances on nodes
    if(cur_step < dist_history.size()) {
        const auto& current_dist = dist_history[cur_step];
        sf::Text dist_text(font_impact, "", 22);
        dist_text.setFillColor(sf::Color(255, 0, 0));
        
        for(int i = 0; i < current_dist.size(); ++i) {
            std::string dist_str;
            if(current_dist[i] >= 1e9) {
                dist_str = "inf";
            } else {
                dist_str = std::to_string(static_cast<int>(current_dist[i]));
            }
            
            dist_text.setString(dist_str);
            sf::Vector2f node_pos = graph.getPosition(i);
            dist_text.setOrigin({dist_text.getLocalBounds().size.x / 2.0f, 0.0f});
            dist_text.setPosition({node_pos.x, node_pos.y + graph.getRadius() + 5.0f});
            window.draw(dist_text);
        }
    }

    // draw code
    highlight.setFillColor(sf::Color(76, 86, 106, 120));
    highlight.setSize({max_right - highlight.getGlobalBounds().position.x - 20, text_for_code.getLocalBounds().size.y + 5});
    highlight.setOrigin({0, highlight.getSize().y / 2.0f});
    window.draw(visual_code_region);
    for(int i = 0; i < code.size(); ++i) {
        text_for_code.setString(code[i]);
        text_for_code.setFillColor(sf::Color::Black);
        text_for_code.setOrigin({0 , text_for_code.getLocalBounds().size.y / 2.0f});


        text_for_code.setPosition({visual_code_region.getGlobalBounds().position.x+4, visual_code_region.getGlobalBounds().position.y + 8 * (i+1) + 16 * i + 5});

        if(id == i) {
            highlight.setPosition({text_for_code.getGlobalBounds().position.x, text_for_code.getGlobalBounds().position.y + 8});

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


bool read_graph_from_file(Visual_graph &vg, shortest_path_algorithm &graph, bool directed) {
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
            if (file.fail()) return false;
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
        float center_x = 900, center_y = 325, radius = 150;
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

void random_graph(Visual_graph &vg, shortest_path_algorithm &graph, bool directed) {
    vg.clearAll();
    graph.clear();

    int N = 5 + rand() % 6; // 5 to 10 nodes
    int M = N + rand() % (N * (N - 1) / 2 - N + 1); // at least a tree, at most complete graph

    std::vector<std::pair<int, int>> edges;
    std::set<std::pair<int, int>> existing_edges;

    // add nodes in circle
    float center_x = 900, center_y = 325, radius = 150;
    for (int i = 0; i < N; ++i) {
        float angle = 2 * 3.1415926535f * i / N;
        float x = center_x + radius * cos(angle);
        float y = center_y + radius * sin(angle);
        vg.add_node(x, y);
        vg.setNodeLabel(i, std::to_string(i));
    }

    while (edges.size() < M) {
        int u = rand() % N;
        int v = rand() % N;
        if (u != v && existing_edges.count({std::min(u,v), std::max(u,v)}) == 0) {
            int w = 1 + rand() % 20; // weight between 1 and 20
            edges.emplace_back(u, v);
            graph.add_edge(u, v, graph.weights.size(), directed);
            graph.weights.push_back(w);
            existing_edges.insert({std::min(u,v), std::max(u,v)});
            vg.add_edge(u, v, w, directed);
        }
    }
}

void shortest_path_page() {
    // load background
    sf::Texture backgroundTexture;
    if (!loadTextureFromAsset(backgroundTexture, "bg.png")) {
        !backgroundTexture.loadFromFile("cs163-project/Visualizer/assets/bg.png");
    } 
    sf::Sprite backgroundSprite(backgroundTexture);

    shortest_path_algorithm graph(0);
    Visual_graph Visual_graph;

    const float visual_region_width = 975.0f;
    const float visual_region_height = 525.0f;

    const float button_width = 110.0f;
    const float button_height = 60.0f;

    const float visual_code_region_width = 450.0f;
    const float visual_code_region_height = 250.0f;

    RoundedRectangleShape visual_region, visual_code_region;
    sf::RectangleShape safe_region;

    // load font
    sf::Text text_for_code(font_impact, "", 20);
    sf::Text error_text(font_impact, "", 30);
    error_text.setFillColor(sf::Color::Red);

    visual_region.setFillColor(sf::Color(251, 251, 253, 200));
    visual_region.setOutlineColor(sf::Color(217, 211, 209));
    visual_region.setOutlineThickness(2.0f);
    visual_region.setSize({visual_region_width, visual_region_height});
    visual_region.setRadius(18.0f);
    visual_region.setCornerPointCount(40);
    visual_region.setPosition({380.0f, 20.0f});

    visual_code_region.setFillColor(sf::Color(215, 161, 72, 200));
    visual_code_region.setOutlineColor(sf::Color(217, 211, 209));
    visual_code_region.setOutlineThickness(1.0f);
    visual_code_region.setSize({visual_code_region_width, visual_code_region_height});
    visual_code_region.setRadius(0.0f);
    visual_code_region.setCornerPointCount(20);
    visual_code_region.setPosition({WINDOW_WIDTH - visual_code_region_width, WINDOW_HEIGHT - visual_code_region_height - 10.0f});

    safe_region.setSize({visual_region_width - Visual_graph.getRadius() * 2, visual_region_height - Visual_graph.getRadius() * 2});
    safe_region.setPosition({visual_region.getPosition().x + Visual_graph.getRadius(), visual_region.getPosition().y + Visual_graph.getRadius()});

    // slider for speed
    sf::RectangleShape slider_bar(sf::Vector2f(500.0f, 20.0f));
    slider_bar.setFillColor(sf::Color(138,155,192));
    slider_bar.setPosition({400.0f, WINDOW_HEIGHT - 120.0f});

    sf::CircleShape slider_knob(15.0f);
    slider_knob.setFillColor(sf::Color(28, 41, 114));
    slider_knob.setPosition({slider_bar.getPosition().x + slider_bar.getSize().x / 2.0f - slider_knob.getRadius(), slider_bar.getPosition().y - 5.0f});

    // buttons
    std::vector<button> buttons;
    bool state_buttons[7] = {0, 0, 0, 0, 0, 0, 0};
    float button_region_x = 1053.0f;
    float button_region_y = 364.0f;

    buttons.emplace_back(10, 325, button_width, button_height, sf::Color(232, 183, 81), "Find Path", 20);
    buttons.emplace_back(260, 625, button_width, button_height, sf::Color(232, 183, 81), "File", 20);
    buttons.emplace_back(10, 550, button_width, button_height, sf::Color(232, 183, 81), "Add Edge", 20);
    buttons.emplace_back(10, 475, button_width, button_height, sf::Color(232, 183, 81), "Add Node", 20);
    buttons.emplace_back(135, 475, button_width, button_height, sf::Color(232, 183, 81), Visual_graph.isDirected() ? "Directed" : "Undirected", 18);
    for(int i = 0; i < 5; i++) {
        buttons[i].setColor(sf::Color(232, 183, 81));
    }
    buttons.emplace_back(10, 625, button_width, button_height, sf::Color(232, 183, 81), "Clear", 24);
    buttons.emplace_back(135, 625, button_width, button_height, sf::Color(232, 183, 81), "Random", 20);

    box input_weight_box(135, 550, button_width + 10.0f, button_height, sf::Color(138,155,192), "5", 18);
    std::string current_input_weight = "5";

    // input for source vertex
    std::string source_vertrix = "0";
    int selected_algorithm = shortest_path_algorithm::ALGO_DIJKSTRA;
    bool show_algorithm_options = false;

    sf::FloatRect po = buttons[0].getShape();
    box input_source_box(135.0f, 325.0f, button_width - 30.0f, button_height, sf::Color(138,155,192), "0", 18);

    button pre_step_button(10.0f, 400.0f, button_width, button_height - 5, sf::Color(232, 183, 81), "Prev", 24);

    button start_button(135.0f, 400.0f, button_width, button_height - 5, sf::Color(232, 183, 81), "Start", 24);

    button nxt_step_button(260.0f, 400.0f, button_width, button_height - 5, sf::Color(232, 183, 81), "Next", 24);
    button skip_button(260.0f, 325.0f, button_width, button_height - 5, sf::Color(232, 183, 81), "Skip", 24);

    button back_button(15.0f, 15.0f, button_width, button_height, sf::Color(232, 183, 81), "Return", 24);

    button speed_inc(700, WINDOW_HEIGHT - 75, 50, 50, sf::Color(232, 183, 81), "+", 24);
    button speed_dec(760, WINDOW_HEIGHT - 75, 50, 50, sf::Color(232, 183, 81), "-", 24);
    box speed_box(820, WINDOW_HEIGHT - 75, 60, 50, sf::Color(138,155,192), "0", 24);

    // algorithm selection
    button algorithm_button(10.0f, 200.0f, button_width, button_height, sf::Color(232, 183, 81), shortest_path_algorithm::algorithm_names[shortest_path_algorithm::ALGO_DIJKSTRA], 16);
    float algorithm_option_x = 1249.0f;
    button algorithm_option_0(135, 200.0f, button_width, button_height, sf::Color(232, 183, 81), shortest_path_algorithm::algorithm_names[shortest_path_algorithm::ALGO_DIJKSTRA], 16);
    button algorithm_option_1(260, 200.0f, button_width, button_height, sf::Color(232, 183, 81), shortest_path_algorithm::algorithm_names[shortest_path_algorithm::ALGO_BELLMAN_FORD], 16);

    bool mouse_left_pressed = 0;
    bool mouse_left_pressed_last = 1;
    bool input_weight_box_active = 0;
    bool is_start_button_pressed = 0;
    bool auto_run_after_load = false;

    int cur_step = 0;

    std::string error_message = "ERROR: Invalid input or algorithm cannot be applied!";
    long long error_time = -1000000;
    int step_delay = 500;
    bool dragging_slider = false;
    auto now_time = std::chrono::system_clock::now();
    auto duration = now_time.time_since_epoch();
    long long last_step = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    auto updateIntegerInput = [](std::string& value, char32_t unicode, bool allow_negative) {
        if (unicode == '\b' || unicode == 8 || unicode == 127) {
            if (!value.empty()) {
                value.pop_back();
                return true;
            }
            return false;
        }

        if (value == "0") {
            value.clear();
        }

        if (allow_negative && unicode == '-' && value.empty()) {
            value.push_back('-');
            return true;
        }

        if ('0' <= unicode && unicode <= '9') {
            value.push_back(static_cast<char>(unicode));
            return true;
        }

        return false;
    };

    auto rebuildShortestPath = [&](long long current_time) {
        cur_step = 0;
        graph.init(Visual_graph.getEdgeList(), Visual_graph.getEdgeWeightsList(), Visual_graph.isDirected(), Visual_graph.getNodeLabels());
        graph.code = shortest_path_algorithm::algorithm_code[selected_algorithm];
        int start_index = graph.label_to_index(source_vertrix);
        if(start_index >= 0) {
            if(selected_algorithm == shortest_path_algorithm::ALGO_DIJKSTRA && graph.has_negative_weight()) {
                error_time = current_time;
            } else {
                graph.find_shortest_path(start_index, selected_algorithm);
            }
        }
    };

    auto parseInputNumber = [](const std::string& value, int fallback) {
        if (value.empty() || value == "-") {
            return fallback;
        }
        return std::stoi(value);
    };


    while (window.isOpen()) {
        now_time = std::chrono::system_clock::now();
        duration = now_time.time_since_epoch();

        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::vector<char32_t> text_entered;
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* text_event = event->getIf<sf::Event::TextEntered>()) {
                text_entered.push_back(text_event->unicode);
            }
        }
        if (!window.isOpen()) {
            break;
        }

        mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if(mouse_left_pressed and !mouse_left_pressed_last) {
            for(int i = 0; i < buttons.size(); i++) {
                if(buttons[i].isClicked(sf::Mouse::getPosition(window))) {
                    state_buttons[i] = !state_buttons[i];
                    if(state_buttons[i]) {
                            for(int j = 0; j < buttons.size(); j++) {
                            if(j != i) state_buttons[j] = 0;
                        }
                        // case: find path
                        if(i == 0) {
                            is_start_button_pressed = 0;
                            cur_step = 0;
                            graph.init(Visual_graph.getEdgeList(), Visual_graph.getEdgeWeightsList(), Visual_graph.isDirected(), Visual_graph.getNodeLabels());
                            graph.code = shortest_path_algorithm::algorithm_code[selected_algorithm];
                            int start_index = graph.label_to_index(source_vertrix);
                            if(start_index >= 0) {
                                if(selected_algorithm == shortest_path_algorithm::ALGO_DIJKSTRA && graph.has_negative_weight()) {
                                    error_time = now;
                                } else {
                                    graph.find_shortest_path(start_index, selected_algorithm);
                                }
                            } else {
                                error_time = now;
                            }
                        }
                        else if(i == 4) { //case toggle directed/undirected
                            bool cur_directed = Visual_graph.isDirected();
                            Visual_graph.setDirected(!cur_directed);
                            buttons[4].setLabel(Visual_graph.isDirected() ? "Directed" : "Undirected");

                            // Toggle action doesn't remain selected as a mode button
                            for(int j = 0; j < buttons.size(); j++) state_buttons[j] = 0;
                        }
                        else if(i == 1) { // case load graph from file
                            if(!read_graph_from_file(Visual_graph, graph, Visual_graph.isDirected())) {
                                error_time = now;
                            } else {
                                const auto &node_labels = Visual_graph.getNodeLabels();
                                source_vertrix = node_labels.empty() ? "0" : node_labels[0];
                                int start_index = graph.label_to_index(source_vertrix);
                                if(start_index >= 0) {
                                    if(selected_algorithm == shortest_path_algorithm::ALGO_DIJKSTRA && graph.has_negative_weight()) {
                                        error_time = now;
                                    } else {
                                        graph.code = shortest_path_algorithm::algorithm_code[selected_algorithm];
                                        graph.find_shortest_path(start_index, selected_algorithm);
                                        state_buttons[0] = 1;
                                        for(int j = 1; j < 5; j++) state_buttons[j] = 0;
                                        cur_step = 0;
                                        step_delay = 5;
                                        is_start_button_pressed = 1;
                                        auto_run_after_load = true;
                                        last_step = now;
                                    }
                                }
                            }
                            // Toggle off after action
                            state_buttons[i] = 0;
                        }
                    }

                    break;
                }
            }

            // Algorithm selection
            if (algorithm_button.isClicked(sf::Mouse::getPosition(window))) {
                show_algorithm_options = !show_algorithm_options;
                for(int i = 0; i < buttons.size(); i++) state_buttons[i] = 0;
                is_start_button_pressed = 0;
            }

            // Algorithm option selection
            if (show_algorithm_options) {
                if (algorithm_option_0.isClicked(sf::Mouse::getPosition(window))) {
                    selected_algorithm = shortest_path_algorithm::ALGO_DIJKSTRA;
                    algorithm_button.setLabel(shortest_path_algorithm::algorithm_names[selected_algorithm]);
                    graph.code = shortest_path_algorithm::algorithm_code[selected_algorithm];
                    for(int i = 0; i < buttons.size(); i++) state_buttons[i] = 0;
                    is_start_button_pressed = 0;
                    show_algorithm_options = false;
                } else if (algorithm_option_1.isClicked(sf::Mouse::getPosition(window))) {
                    selected_algorithm = shortest_path_algorithm::ALGO_BELLMAN_FORD;
                    algorithm_button.setLabel(shortest_path_algorithm::algorithm_names[selected_algorithm]);
                    graph.code = shortest_path_algorithm::algorithm_code[selected_algorithm];
                    for(int i = 0; i < buttons.size(); i++) state_buttons[i] = 0;
                    is_start_button_pressed = 0;
                    show_algorithm_options = false;
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
                step_delay = 100.0f +  (max_x - new_x) / slider_bar.getSize().x * (1000.0f - 10.0f);
            }
        } else {
            dragging_slider = false;
        }

        //inc dec speed

        if(speed_inc.isClicked(sf::Mouse::getPosition(window)) and mouse_left_pressed and !mouse_left_pressed_last) {
            step_delay = std::max(10, step_delay - 100);
            slider_knob.setPosition({slider_bar.getPosition().x + slider_bar.getSize().x * (1000 - step_delay) / (1000 - 10) - slider_knob.getRadius(), slider_knob.getPosition().y});
        }

        if(speed_dec.isClicked(sf::Mouse::getPosition(window)) and mouse_left_pressed and !mouse_left_pressed_last) {
            step_delay = std::min(1000, step_delay + 100);
            slider_knob.setPosition({slider_bar.getPosition().x + slider_bar.getSize().x * (1000 - step_delay) / (1000 - 10) - slider_knob.getRadius(), slider_knob.getPosition().y});
        }

        //

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
                case_add_edge_by_click(idx, Visual_graph, visual_region, safe_region, parseInputNumber(current_input_weight, 0));
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
            for (char32_t unicode : text_entered) {
                updateIntegerInput(current_input_weight, unicode, true);
            }
        }

        if(current_input_weight.size() == 0) current_input_weight = "0";


        // case: Find path

        if(state_buttons[0]) {
            bool source_changed = false;
            for (char32_t unicode : text_entered) {
                source_changed = updateIntegerInput(source_vertrix, unicode, true) || source_changed;
            }

            if(source_vertrix.size() == 0) source_vertrix = "0";
            if(source_changed) rebuildShortestPath(now);

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

            if(mouse_left_pressed == 1 and !mouse_left_pressed_last and skip_button.isClicked(sf::Mouse::getPosition(window))) {
                cur_step = graph.step_history.size() - 1;
                is_start_button_pressed = 0;
            }

            if(auto_run_after_load && graph.step_history.size() > 0 && cur_step == graph.step_history.size() - 1) {
                auto_run_after_load = false;
                is_start_button_pressed = 0;
                cur_step = 0;
                step_delay = 300;
            }
        }

        //


        //case: clear

        if(state_buttons[5] and mouse_left_pressed and !mouse_left_pressed_last) {
            Visual_graph.clearAll();
            graph.clear();
            source_vertrix = "0";
            current_input_weight = "5";
            is_start_button_pressed = 0;
            for(int i = 0; i < buttons.size(); i++) state_buttons[i] = 0;
        }

        //

        // case: random graph
        if(state_buttons[6] and mouse_left_pressed and !mouse_left_pressed_last) {
            random_graph(Visual_graph, graph, Visual_graph.isDirected());
            source_vertrix = "0";
            current_input_weight = "5";
            is_start_button_pressed = 0;
            for(int i = 0; i < buttons.size(); i++) state_buttons[i] = 0;
        }
        //

        // case: not one of buttons is active | move node
        bool no_button_active = 1;
        for(int i=1; i<buttons.size(); i++) {
            no_button_active &= !state_buttons[i];
        }

        if(no_button_active and mouse_left_pressed and !mouse_left_pressed_last) {
            case_move_node(Visual_graph, visual_region, safe_region);
        }
        // 

        if(!mouse_left_pressed and mouse_left_pressed_last and back_button.contains(sf::Mouse::getPosition(window))) {
            return;
        }

        window.clear(sf::Color(235, 235, 235)); // soft gray background
        window.draw(backgroundSprite);

        // draw region 
        window.draw(visual_region);
        window.draw(visual_code_region);

        // draw buttons with transparency + hover border
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for(int i = 0; i < buttons.size(); i++) {
            // transparent fill for all, active semi-green
            if(state_buttons[i] or buttons[i].contains(mousePos)) {
                buttons[i].setColor(sf::Color(140, 95, 30));
            } else {
                buttons[i].setColor(sf::Color(232, 183, 81));
            }

            buttons[i].draw(window);
        }

        if (algorithm_button.contains(mousePos) || show_algorithm_options) {
            algorithm_button.setColor(sf::Color(140, 95, 30));
        } else {
            algorithm_button.setColor(sf::Color(232, 183, 81));
        }
        algorithm_button.draw(window);

        if (show_algorithm_options) {
            button* algo_options[] = {&algorithm_option_0, &algorithm_option_1};
            for (int i = 0; i < 2; ++i) {
                if (algo_options[i]->contains(mousePos)) {
                    algo_options[i]->setColor(sf::Color(140, 95, 30));
                } else {
                    algo_options[i]->setColor(sf::Color(232, 183, 81));
                }
                algo_options[i]->draw(window);
            }
        }

        // input boxes hover border
        bool input_weight_hover = input_weight_box.contains(mousePos);

        bool input_source_hover = input_source_box.contains(mousePos);

        if(back_button.contains(mousePos)){
            back_button.setColor(sf::Color(140, 95, 30));
        } else {
            back_button.setColor(sf::Color(232, 183, 81));
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
                pre_step_button.setColor(sf::Color(140, 95, 30));
            } else {
                pre_step_button.setColor(sf::Color(232, 183, 81));
            }
            pre_step_button.draw(window);

            if(start_button.contains(mousePos) or is_start_button_pressed){
                start_button.setColor(sf::Color(140, 95, 30));
            } else {
                start_button.setColor(sf::Color(232, 183, 81));
            }
            start_button.draw(window);

            if(nxt_step_button.contains(mousePos)) {
                nxt_step_button.setColor(sf::Color(140, 95, 30));
            } else {
                nxt_step_button.setColor(sf::Color(232, 183, 81));
            }
            nxt_step_button.draw(window);

            if(skip_button.contains(mousePos)) {
                skip_button.setColor(sf::Color(140, 95, 30));
            } else {
                skip_button.setColor(sf::Color(232, 183, 81));
            }
            skip_button.draw(window);

            if(cur_step >= 0 and cur_step < graph.step_history.size()) {
                graph._visual(Visual_graph, visual_code_region, text_for_code, cur_step);
            }
        }

        if(!(state_buttons[0] and cur_step >= 0 and cur_step < graph.step_history.size())) for(int i = 0; i < graph.code.size(); ++i) {
            text_for_code.setString(graph.code[i]);
            text_for_code.setFillColor(sf::Color::Black);
            text_for_code.setOrigin({0 , text_for_code.getLocalBounds().size.y / 2.0f});

            text_for_code.setPosition({visual_code_region.getGlobalBounds().position.x + 4, visual_code_region.getGlobalBounds().position.y + 8 * (i+1) + 16 * i + 5});

            window.draw(text_for_code);
        }

        if (now - error_time < 2000) {
            error_text.setString(error_message);
            error_text.setPosition({425, 300});
            window.draw(error_text);
        }

        // Draw distance list
        if(state_buttons[0] && cur_step >= 0 && cur_step < graph.step_history.size() && cur_step < graph.dist_history.size() && graph.labels.size() > 0) {
            const auto& current_dist = graph.dist_history[cur_step];
            sf::Text dist_list_text(font_impact, "", 14);
            dist_list_text.setFillColor(sf::Color::Black);
            
            float list_x = 1370.0f;
            float list_y = 350.0f;
            
            for(int i = 0; i < current_dist.size() && i < graph.labels.size(); ++i) {
                std::string dist_str;
                if(current_dist[i] == 1e9) {
                    dist_str = graph.labels[i] + ": inf";
                } else {
                    dist_str = graph.labels[i] + ": " + std::to_string(static_cast<int>(current_dist[i]));
                }
                
                dist_list_text.setString(dist_str);
                dist_list_text.setPosition({list_x, list_y + i * 25});
                window.draw(dist_list_text);
            }
        }

        // Draw slider only in find_path mode
        if(state_buttons[0]) {
            window.draw(slider_bar);
            window.draw(slider_knob);
            speed_inc.draw(window);
            speed_dec.draw(window);
            std::string speed_label = std::to_string(step_delay / 1000.0f);
            while(speed_label.size() > 3) speed_label.pop_back();
            speed_box.setLabel(speed_label + "s");
            speed_box.draw(window);
        }

        if(speed_inc.contains(mousePos)) {
            speed_inc.setColor(sf::Color(140, 95, 30));
        } else {
            speed_inc.setColor(sf::Color(232, 183, 81));
        }

        if(speed_dec.contains(mousePos)) {
            speed_dec.setColor(sf::Color(140, 95, 30));
        } else {
            speed_dec.setColor(sf::Color(232, 183, 81));
        }

        window.display();
        mouse_left_pressed_last = mouse_left_pressed;
    }
}
