#include "../headers/core.h"

void openWindow() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    // khủ răng cưa

    window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Data Structure Visualizer", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(60);

    if (!loadFonts()) {
        std::cerr << "Warning: Could not load font!\n";
    }
    font_impact.setSmooth(true);
}

void main_menu_page() {

    button option_button(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 50, 200, 50, sf::Color::White, "Option", 24);

    bool is_mouse_left_pressed = 0;
    bool is_mouse_left_pressed_last = 1;

    while(window.isOpen()) {
        is_mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Black);

        option_button.draw(window);
        if(option_button.isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
            option_page();
        }
        window.display();

        is_mouse_left_pressed_last = is_mouse_left_pressed;
    }
}

void option_page() {

    button shortest_path_button(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 50, 200, 50, sf::Color::White, "Shortest Path", 24);

    bool is_mouse_left_pressed = 0;
    bool is_mouse_left_pressed_last = 1;

    while(window.isOpen()) {
        is_mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if(shortest_path_button.isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
            shortest_path_page();
            return;
        }

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);

        shortest_path_button.draw(window);

        window.display();

        is_mouse_left_pressed_last = is_mouse_left_pressed;
    }
}