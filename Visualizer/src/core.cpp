#include "../headers/core.h"
#include "../headers/heap.h"
void openWindow() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 10;
    // khủ răng cưa

    window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Data Structure Visualizer", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(60);

    if (!loadFonts()) {
        std::cerr << "Warning: Could not load font!\n";
    }
    font_impact.setSmooth(true);
}

void main_menu_page() {

    float button_width = 300.0f;
    float button_height = 80.0f;

    button option_button(WINDOW_WIDTH/2 - button_width/2, WINDOW_HEIGHT/2 - button_height/2, button_width, button_height, sf::Color::White, "Data Structures", 24);
    button setting_button(WINDOW_WIDTH/2 - button_width/2, WINDOW_HEIGHT/2 - button_height/2 + 100.0f, button_width, button_height, sf::Color(232, 183, 81), "Settings", 24);

    std::vector<button*> all_buttons = {&option_button, &setting_button};

    bool is_mouse_left_pressed = 0;
    bool is_mouse_left_pressed_last = 0;

    while(window.isOpen()) {
        is_mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Black);

        for (const auto& btn : all_buttons) {
            btn->draw(window);
        }

        for(int i = 0; i < all_buttons.size(); i++) {
            if(all_buttons[i]->isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
                switch(i) {
                    case 0: option_page(); break;
                    case 1: break; // setting page
                }
            }
        }

        window.display();

        is_mouse_left_pressed_last = is_mouse_left_pressed;
    }
}

void option_page() {

    float button_width = 200.0f;
    float button_height = 100.0f;

    float position_button_x = WINDOW_WIDTH / 2.0f - button_width / 2.0f;
    float position_button_y = WINDOW_HEIGHT / 2.0f - button_height / 2.0f - 100.0f;

    button shortest_path_button(position_button_x, position_button_y, button_width, button_height, sf::Color::White, "Shortest Path", 24);

    button ducminh(position_button_x, position_button_y + button_height + 20, button_width, button_height, sf::Color::Cyan, "Heap", 24);
    button trie(position_button_x, position_button_y + 2 * (button_height + 20), button_width, button_height, sf::Color::Green, "Trie", 24);
    button linked_list(position_button_x, position_button_y + 3 * (button_height + 20), button_width, button_height, sf::Color(232, 183, 81), "Singly List", 24);
    button back_button(10.0f, 10.0f, 100.0f, 50.0f, sf::Color(232, 183, 81), "Back", 24);

    std::vector<button*> all_buttons = {&shortest_path_button, &ducminh, &trie, &linked_list, &back_button};

    bool is_mouse_left_pressed = 0;
    bool is_mouse_left_pressed_last = 1;

    while(window.isOpen()) {
        is_mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        for(int i = 0; i < all_buttons.size(); i++) {
            if(all_buttons[i]->isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
                switch(i) {
                    case 0: shortest_path_page(); break;
                    case 1: heap_page(); break;
                    case 2: trie_page(); break;
                    case 3: singly_linked_list_page(); break;
                    case 4: return; break;
                }
            }
        }

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color(235, 235, 235)); // soft gray background
        
        for(int i = 0; i < all_buttons.size(); i++) {
            if(all_buttons[i]->contains(sf::Mouse::getPosition(window))) {
                all_buttons[i]->setOutline(sf::Color::Black, 2.0f);
                all_buttons[i]->setColor(sf::Color(140, 95, 30));
            } else {
                all_buttons[i]->setOutline(sf::Color::Transparent, 0.0f);
                all_buttons[i]->setColor(sf::Color(232, 183, 81));
            }
            all_buttons[i]->draw(window);
        }

        window.display();

        is_mouse_left_pressed_last = is_mouse_left_pressed;
    }
}

void setting_page() {
    
}