#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "../headers/core.h"
#include "../headers/global.h"

sf::Text setCenterText(sf::Text text);

void setCenterText(sf::Text& text);

bool in_rect(sf::Vector2i mousePos, sf::FloatRect rect);

class button {
private:
    std::string label;
    sf::RectangleShape shape;
    sf::Text text;
public:
    button(float x, float y, float width, float height, sf::Color color, const std::string labelText, unsigned int charSize);

    void setColor(sf::Color color);

    void setLabel(const std::string &newLabel);

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    bool isClicked(sf::Vector2i mousePos);
};

class node {
private:
    sf::CircleShape shape;
    sf::Text text;
public:
    node(float x, float y, float radius, sf::Color fillColor, sf::Color outlineColor, float outlineThickness);

    void setPosition(float x, float y) {
        shape.setPosition({x, y});
    }
     
    void setRadius(float radius) {
        shape.setRadius(radius);
        shape.setOrigin({radius, radius}); // Update origin when radius changes
    }

    void setLabel(const std::string &newLabel, unsigned int charSize);

    void setColor(sf::Color color);

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    bool isClicked(sf::Vector2i mousePos);

    void printInfo() {
        sf::Vector2f pos = shape.getPosition();
        std::cout << "Node at (" << pos.x << ", " << pos.y << ")" << std::endl;
    }
};

class edge {
private:
    sf::Vertex line[2];
public:
    edge(float x1, float y1, float x2, float y2, sf::Color color, float thickness = 2.0f) {
        line[0] = sf::Vertex({{x1, y1}, color});
        line[1] = sf::Vertex({{x2, y2}, color});
    }

    void setPoints(float x1, float y1, float x2, float y2);
    void setColor(sf::Color color);
    
    void draw(sf::RenderWindow& window) {
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
};

class Visual_node {
private:
    float node_radius = 30.0f;
    float outline_thickness = 4.0f;
    std::vector<int> state;
    std::vector<node> nodes;

    sf::Color color_of_state[3] = {sf::Color::Cyan, sf::Color::Green, sf::Color::Red};
public:
    void add(float x, float y);

    void draw(sf::RenderWindow& window);
};