#include "../headers/core.h"

sf::RenderWindow window;

void openWindow() {
    window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Data Structure Visualizer");
    window.setFramerateLimit(60);
}

void main_menu_page() {

    button option_button(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 50, 200, 50, sf::Color::Blue, "Options");

    while(window.isOpen()) {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Black);

        if(option_button.isClicked(sf::Mouse::getPosition(window))) {
            option_page();
        }
        option_button.draw(window);

        window.display();
    }
}

void option_page() {

    button shortest_path_button(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 50, 200, 50, sf::Color::Green, "Shortest Path");

    while(window.isOpen()) {
        
        if(shortest_path_button.isClicked(sf::Mouse::getPosition(window))) {
            shortest_path_page();
        }

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);

        shortest_path_button.draw(window);

        window.display();
    }
}