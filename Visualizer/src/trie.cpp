#include "../headers/trie.h"

void trie_page(){
    button add_button(WINDOW_WIDTH - 300, 50, 200, 50, sf::Color::Green, "Add", 24);
    button delete_button(WINDOW_WIDTH - 300, 150, 200, 50, sf::Color::Red, "Delete", 24);
    button find_button(WINDOW_WIDTH - 300, 250, 200, 50, sf::Color::Yellow, "Find", 24);

    RoundedRectangleShape bgmain({800.f, 600.f});    
    bgmain.setPosition({80 , 50});
    bgmain.setFillColor(sf::Color(243, 243, 251, 100));
    bgmain.setRadius(17);
    bgmain.setOutlineThickness(5);
    bgmain.setOutlineColor(sf::Color(217, 211, 209));

    RoundedRectangleShape bgcode({350.f, 200.f});
    bgcode.setPosition({float(WINDOW_WIDTH - 300), float(WINDOW_HEIGHT - 250)});
    bgcode.setFillColor(sf::Color(243, 243, 251, 100));
    bgcode.setRadius(17);
    bgcode.setOutlineThickness(5);
    bgcode.setOutlineColor(sf::Color(217, 211, 209));
    
    // string input box
    box input_box(WINDOW_WIDTH - 300, 350, 200, 50, sf::Color::Cyan, "Type here", 24);
    
    while(window.isOpen()){
        while(const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>())
                window.close();
        }
        
        window.clear();
        add_button.draw(window);
        delete_button.draw(window);
        find_button.draw(window);
        window.draw(bgmain);
        window.draw(bgcode);
        input_box.draw(window);

        window.display();

    }
}