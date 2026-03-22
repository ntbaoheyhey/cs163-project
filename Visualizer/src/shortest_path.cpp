#include "../headers/shortest_path.h"
#include <iostream>
#include <vector>
#include <queue>
#include <string>

dijsktra::dijsktra(int n) {
    dist.resize(n+1, 1e9);
    adj.resize(n+1);
}

void dijsktra::clear() {
    dist.clear();
    adj.clear();
}

void dijsktra::add_edge(int u, int v, int weight) {
    adj[u].emplace_back(v, weight);
    adj[v].emplace_back(u, weight);
}

void dijsktra::find_shortest_path(int start) {
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> heap;
    dist[start] = 0;
    heap.emplace(0, start);

    while (!heap.empty()) {
        auto [d, u] = heap.top();
        heap.pop();

        if (d > dist[u]) continue;

        for (auto& [v, weight] : adj[u]) {
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                heap.emplace(dist[v], v);
            }
        }
    }
}

void case_move_node(Visual_graph& Visual_graph, sf::RectangleShape& visual_region) {
    int idx = Visual_graph.find_node_stored(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    
    int mouse_left_pressed = 0;
    if(idx != -1) {
        do {
            int x = sf::Mouse::getPosition(window).x;
            int y = sf::Mouse::getPosition(window).y;
            int radius = Visual_graph.getRadius();
            if(x - radius < visual_region.getPosition().x) x = visual_region.getPosition().x + radius;
            if(y - radius < visual_region.getPosition().y) y = visual_region.getPosition().y + radius;
            if(x + radius > visual_region.getPosition().x + visual_region.getSize().x) x = visual_region.getPosition().x + visual_region.getSize().x - radius;
            if(y + radius > visual_region.getPosition().y + visual_region.getSize().y) y = visual_region.getPosition().y + visual_region.getSize().y - radius;

            Visual_graph.setPosition(idx, x, y);

            window.draw(visual_region);
            Visual_graph.draw(window);
            window.display();

            mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        } while(mouse_left_pressed);
    }
}

void case_add_edge_by_click(int iidx, Visual_graph& Visual_graph, sf::RectangleShape& visual_region, int weight) {
    int mouse_pressed = 0;
    while(!mouse_pressed) {
        mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        window.draw(visual_region);
        Visual_graph.draw(window, 1);
        window.display();
    }
    while(mouse_pressed) {
        mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        window.draw(visual_region);
        Visual_graph.draw(window, 1);
        window.display();
    }
    int idx = Visual_graph.find_node_stored(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    if(idx != -1 and idx != iidx) {
        Visual_graph.add_edge(iidx, idx, weight);
    }
}


void shortest_path_page() {

    Visual_graph Visual_graph;

    const float visual_region_width = WINDOW_WIDTH - 100;
    const float visual_region_height = WINDOW_HEIGHT - 480;

    const float button_width = 200;
    const float button_height = 80;

    sf::RectangleShape visual_region;

    visual_region.setFillColor(sf::Color(50, 50, 50));
    visual_region.setSize({visual_region_width, visual_region_height});
    visual_region.setPosition({50, 40});


    std::vector<button> buttons;
    bool state_buttons[4] = {0, 0, 0, 0};
    for(int i = 0; i < 4; i++) {
        buttons.emplace_back(20, WINDOW_HEIGHT - (button_height+20) * (i + 1), button_width, button_height, sf::Color::White, "Button " + std::to_string(i+1), 24);
    }

    buttons[0].setLabel("Find Path");
    buttons[1].setLabel("Read Graph");
    buttons[2].setLabel("Add Edge");
    buttons[3].setLabel("Add Node");

    box input_weight_box(buttons[2].getPosition().x + 20 + button_width, buttons[2].getPosition().y, button_width, button_height, sf::Color::White, "5", 24);
    std::string current_input_weight = "5";

    bool mouse_left_pressed = 0;
    bool mouse_left_pressed_last = 1;
    bool input_weight_box_active = 0;


    while (window.isOpen()) {
        mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if(mouse_left_pressed and !mouse_left_pressed_last) {
            for(int i = 0; i < 4; i++) {
                if(buttons[i].isClicked(sf::Mouse::getPosition(window))) {
                    state_buttons[i] = !state_buttons[i];
                    if(state_buttons[i]) {
                        for(int j = 0; j < 4; j++) {
                            if(j != i) state_buttons[j] = 0;
                        }
                    }

                    break;
                }
            }
        }

        // case: add node 
        if(state_buttons[3] and mouse_left_pressed and !mouse_left_pressed_last) {
            if(in_rect(sf::Mouse::getPosition(window), visual_region.getGlobalBounds())) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                Visual_graph.add_node(mousePos.x, mousePos.y);
            }
        }

        // case: add edge 
        if(state_buttons[2] and !mouse_left_pressed and mouse_left_pressed_last) {
            int idx = Visual_graph.find_node_stored(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            if(idx != -1) {
                case_add_edge_by_click(idx, Visual_graph, visual_region, std::stoi(current_input_weight));
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

        // case: not one of buttons is active | move node
        bool no_button_active = 1;
        for(int i=0; i<4; i++) {
            no_button_active &= !state_buttons[i];
        }

        if(no_button_active and mouse_left_pressed and !mouse_left_pressed_last) {
            case_move_node(Visual_graph, visual_region);
        }
        // 

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);


        // draw region 
        window.draw(visual_region);

        // draw stuff
        for(int i = 0; i < 4; i++) {
            if(state_buttons[i]) {
                buttons[i].setColor(sf::Color::Green);
            }
            else {
                buttons[i].setColor(sf::Color::White);
            }
            buttons[i].draw(window);
        }

        if(state_buttons[2]) {
            input_weight_box.setLabel(current_input_weight);
            input_weight_box.draw(window);
        }
        
        Visual_graph.draw(window, 1);

        window.display();
        mouse_left_pressed_last = mouse_left_pressed;
    }
}