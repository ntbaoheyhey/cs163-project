#include "../headers/shortest_path.h"
#include <iostream>
#include <vector>
#include <queue>

dijsktra::dijsktra(int n) {
    dist.resize(n, 1e9);
    adj.resize(n);
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


void shortest_path_page() {

    Visual_node visual_node;

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

    bool mouse_left_pressed = 0;
    bool mouse_left_pressed_last = 1;

    while (window.isOpen()) {
        mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if(mouse_left_pressed and !mouse_left_pressed_last) {
            if(buttons[3].isClicked(sf::Mouse::getPosition(window))) {
                state_buttons[3] = !state_buttons[3];
                if(state_buttons[3]) {
                    for(int i=0; i<4; i++) {
                        if(i != 3) {
                            state_buttons[i] = 0;
                        }
                    }
                }
            }  
        }

        if(state_buttons[3] and mouse_left_pressed and !mouse_left_pressed_last) {
            if(in_rect(sf::Mouse::getPosition(window), visual_region.getGlobalBounds())) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                visual_node.add(mousePos.x, mousePos.y);
            }
        }


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
        
        visual_node.draw(window);

        window.display();
        mouse_left_pressed_last = mouse_left_pressed;
    }
}