#include "../headers/heap.h"
#include <vector>
void heap_page(){

    button insert_button(0, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "insert", 24);
    button next_button(200, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "next", 24);
    button undo_button(400, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "undo", 24);
    
    node a(500, 500, 20, sf::Color::Cyan, sf::Color::Yellow, 5);
    node b(200, 200, 20, sf::Color::Cyan, sf::Color::Yellow, 5);
    a.setLabel("10", 10);
    b.setLabel("12", 10);

    a.draw(window);
    b.draw(window);
    sf::Clock clock;
    
    startNodeMovement(a, b.getPosition(), 1.f);
    startNodeMovement(b, a.getPosition(), 1.f);
    while(window.isOpen()){

        float dt = clock.getElapsedTime().asSeconds();
        if (dt>=1.f/60.f){
            clock.restart();
            while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

            window.clear(sf::Color::Black);
            
            insert_button.draw(window);
            next_button.draw(window);
            undo_button.draw(window);
            
            if (a.isMoving)
            a.updatePosition(dt);
            if (b.isMoving)
            b.updatePosition(dt);
            a.draw(window);
            b.draw(window);
            window.display();
        }
        
    }
}