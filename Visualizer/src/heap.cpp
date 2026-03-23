#include "../headers/heap.h"
#include <vector>
void heap_page(){

    button insert_button(0, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "insert", 24);
    button next_button(200, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "next", 24);
    button undo_button(400, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "undo", 24);
    


    while(window.isOpen()){
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);
        
        insert_button.draw(window);
        next_button.draw(window);
        undo_button.draw(window);
        window.display();
    }
}