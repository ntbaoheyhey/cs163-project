#include "box.h"

bool button::isClicked(sf::Vector2i mousePos) {
    bool clicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    sf::FloatRect bounds = shape.getGlobalBounds();

    bool inside = bounds.position.x <= mousePos.x && mousePos.x <= bounds.position.x + bounds.size.x &&
                  bounds.position.y <= mousePos.y && mousePos.y <= bounds.position.y + bounds.size.y;
    if (clicked && inside) {
        return true;
    }
    return false;
}