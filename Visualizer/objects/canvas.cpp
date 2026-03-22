#include "canvas.h"
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

sf::Vector2f button::getPosition() {
    return shape.getGlobalBounds().position;
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

bool node::inside(sf::Vector2i mousePos) {
    sf::Vector2f center = shape.getPosition();
    float radius = shape.getRadius();

    return (mousePos.x - center.x) * (mousePos.x - center.x) + (mousePos.y - center.y) * (mousePos.y - center.y) <= radius * radius;
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
    float d = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

    shape.setSize({d, thickness});
    shape.setOrigin({0.f, thickness / 2.f});
    shape.rotate(-shape.getRotation()); 
    shape.rotate(sf::radians(std::atan2(y2 - y1, x2 - x1)));
    shape.setPosition({x1, y1});
}


void edge::setColor(sf::Color color) {
    shape.setFillColor(color);
}

void edge::setThickness(float th) {
    shape.setSize({shape.getSize().x, th});
}
// End of class Edge

// Class box 
box::box(float x, float y, float width, float height, sf::Color color, const std::string labelText, unsigned int charSize)
    : text(font_impact, labelText, charSize) {
    label = labelText;
    shape.setPosition({x, y});
    shape.setSize({width, height});
    shape.setFillColor(color);

    text = sf::Text(font_impact, label, charSize);
    text.setFillColor(sf::Color::Black);

    // set center text
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({0 , textBounds.size.y / 2.f});
    text.setPosition({x + std::min(0.1f * width, 10.0f), y + height / 2.f});
}

sf::Vector2f box::getPosition() {
    return shape.getGlobalBounds().position;
}

void box::setColor(sf::Color color) {
    shape.setFillColor(color);
}

void box::setLabel(const std::string& newLabel) {
    label = newLabel;
    text.setString(label);
    text.setFillColor(sf::Color::Black);

    // căn giữa lại
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({0 , textBounds.size.y / 2.f});

    sf::FloatRect shapeBounds = shape.getGlobalBounds();
    text.setPosition({shapeBounds.position.x + std::min(0.1f * shapeBounds.size.x, 10.0f), shapeBounds.position.y + shapeBounds.size.y / 2.f});
}

char box::KeyboardToChar() {
    if(auto *key = window.pollEvent()->getIf<sf::Event::KeyPressed>()) {
        std::string keyp = sf::Keyboard::getDescription(key->scancode).toAnsiString();
        if(keyp.size() == 1) {
            return keyp[0];
        }
        if(keyp == "Space") return ' ';
        if(keyp == "Backspace") return '\b';
    }
    return '\0';
}

bool box::isClicked(sf::Vector2i mousePos) {
    bool clicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    sf::FloatRect bounds = shape.getGlobalBounds();

    bool inside = bounds.position.x <= mousePos.x && mousePos.x <= bounds.position.x + bounds.size.x &&
                  bounds.position.y <= mousePos.y && mousePos.y <= bounds.position.y + bounds.size.y;
    if (clicked && inside) {
        return true;
    }
    return false;
}
// End of class box

// Class Visual_graph

void Visual_graph::add_node(float x, float y) {
    std::cerr << "Adding node at (" << x << ", " << y << ")" << std::endl;
    nodes.emplace_back(x, y, node_radius, color_of_state[0], sf::Color::Black, outline_thickness);
    state.push_back(0);
    nodes.back().setLabel(std::to_string(nodes.size()), 24);
}

void Visual_graph::add_edge(int u, int v, int w = 1) {
    if (u < nodes.size() && v < nodes.size()) {
        if(u > v) std::swap(u, v);
        for(int i=0;i<edges.size();i++) {
            if(edges[i].first == u and edges[i].second == v) {
                edges.erase(edges.begin() + i);
                edge_weights.erase(edge_weights.begin() + i);
            }
        }
        edges.emplace_back(u, v);
        edge_weights.push_back(w);
    }
}

void Visual_graph::setPosition(size_t index, float x, float y) {
    if (index < nodes.size()) {
        nodes[index].setPosition(x, y);
    }
}

int Visual_graph::find_node_stored(float x, float y) {
    for(size_t i = 0; i < nodes.size(); i++) {
        if(nodes[i].inside({(int)x, (int)y})) {
            return i;
        }
    }
    return -1; // not found
}

void Visual_graph::draw(sf::RenderWindow& window) {
    edge temp_edge(0, 0, 0, 0, sf::Color::White, edge_thickness);
    for(size_t i = 0; i < edges.size(); i++) {
        int u = edges[i].first;
        int v = edges[i].second;
        sf::Vector2f pos_u = nodes[u].getPosition();
        sf::Vector2f pos_v = nodes[v].getPosition();
        temp_edge.setPoints(pos_u.x, pos_u.y, pos_v.x, pos_v.y);
        temp_edge.draw(window);
    }

    for(size_t i = 0; i < nodes.size(); i++) {
        nodes[i].setColor(color_of_state[state[i]]);
        nodes[i].draw(window);
    }
}

void Visual_graph::draw(sf::RenderWindow& window, bool draw_weights) {
    edge temp_edge(0, 0, 0, 0, sf::Color::White, edge_thickness);
    for(size_t i = 0; i < edges.size(); i++) {
        int u = edges[i].first;
        int v = edges[i].second;
        sf::Vector2f pos_u = nodes[u].getPosition();
        sf::Vector2f pos_v = nodes[v].getPosition();
        temp_edge.setPoints(pos_u.x, pos_u.y, pos_v.x, pos_v.y);
        temp_edge.draw(window);

        if(draw_weights) {
            sf::Text weight_text(font_impact, std::to_string(edge_weights[i]), 14 + edge_thickness);
            weight_text.setFillColor(sf::Color::Black);
            sf::FloatRect textBounds = weight_text.getLocalBounds();
            weight_text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
            weight_text.setPosition({(pos_u.x + pos_v.x) / 2.f, std::max((pos_u.y + pos_v.y) / 2.f - 2 * edge_thickness, edge_thickness)});
            window.draw(weight_text);
        }
    }

    for(size_t i = 0; i < nodes.size(); i++) {
        nodes[i].setColor(color_of_state[state[i]]);
        nodes[i].draw(window);
    }
}

// End of class Visual_graph