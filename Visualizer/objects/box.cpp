#include "box.h"
#include <vector> 

sf::Text setCenterText(sf::Text text) {
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});

    return text;
}

void setCenterText(sf::Text& text) {
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
}

bool in_rect(sf::Vector2i mousePos, sf::FloatRect rect) {
    return rect.position.x <= mousePos.x && mousePos.x <= rect.position.x + rect.size.x &&
           rect.position.y <= mousePos.y && mousePos.y <= rect.position.y + rect.size.y;
}

// Class Button
button::button(float x, float y, float width, float height, sf::Color color, const std::string labelText, unsigned int charSize)
    : text(font_impact, labelText, charSize) {
    label = labelText;
    shape.setPosition({x, y});
    shape.setSize({width, height});
    shape.setFillColor(color);

    text = sf::Text(font_impact, label, charSize);
    text.setFillColor(sf::Color::Black);

    // set center text
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
    text.setPosition({x + width / 2.f, y + height / 2.f});
}

void button::setColor(sf::Color color) {
    shape.setFillColor(color);
}

void button::setLabel(const std::string& newLabel) {
    label = newLabel;
    text.setString(label);
    text.setFillColor(sf::Color::Black);

    // căn giữa lại
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});

    sf::FloatRect shapeBounds = shape.getGlobalBounds();
    text.setPosition({shapeBounds.position.x + shapeBounds.size.x / 2.f, shapeBounds.position.y + shapeBounds.size.y / 2.f});
}

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
// End of class Button

// Class Node

node::node(float x, float y, float radius, sf::Color fillColor, sf::Color outlineColor = sf::Color::Black, float outlineThickness = 2.0f) : text(font_impact, "", 14) {
    shape.setPosition({x, y});
    shape.setRadius(radius);
    shape.setFillColor(fillColor);
    shape.setOutlineColor(outlineColor);
    shape.setOutlineThickness(outlineThickness);

    // set origin to center for easier click detection
    shape.setOrigin({radius, radius});
}

void node::setLabel(const std::string &newLabel, unsigned int charSize = 14.0f) {
    text.setFont(font_impact);
    text.setString(newLabel);
    text.setCharacterSize(charSize);
    text.setFillColor(sf::Color::Black);

    // căn giữa lại
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});

    sf::Vector2f shapePos = shape.getPosition();
    text.setPosition({shapePos.x, shapePos.y});
}

bool node::isClicked(sf::Vector2i mousePos) {
    bool clicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    sf::Vector2f center = shape.getPosition();
    float radius = shape.getRadius();

    bool inside = (mousePos.x - center.x) * (mousePos.x - center.x) + (mousePos.y - center.y) * (mousePos.y - center.y) <= radius * radius;

    if (clicked && inside) {
        return true;
    }
    return false;
}

void node::setColor(sf::Color color) {
    shape.setFillColor(color);
}

// End of class Node

// Class Edge
void edge::setPoints(float x1, float y1, float x2, float y2) {
    line[0] = sf::Vertex({{x1, y1}, line[0].color});
    line[1] = sf::Vertex({{x2, y2}, line[1].color});
}

void edge::setColor(sf::Color color) {
    line[0].color = color;
    line[1].color = color;
}
// End of class Edge

// Class Visual_node

void Visual_node::add(float x, float y) {
    std::cerr << "Adding node at (" << x << ", " << y << ")" << std::endl;
    nodes.emplace_back(x, y, node_radius, color_of_state[0], sf::Color::Black, outline_thickness);
    state.push_back(0);
    nodes.back().setLabel(std::to_string(nodes.size()), 24);
}

void Visual_node::draw(sf::RenderWindow& window) {
    for(size_t i = 0; i < nodes.size(); i++) {
        nodes[i].setColor(color_of_state[state[i]]);
        nodes[i].draw(window);
    }
}

// End of class Visual_node