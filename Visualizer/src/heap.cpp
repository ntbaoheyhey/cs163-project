#include "../headers/heap.h"

void heap_page(){
    while(window.isOpen()){
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);
        window.display();
    }
}