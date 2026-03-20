#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <chrono>

class button {
private:
    std::string label;
    sf::RectangleShape shape;
public:
    button(float x, float y, float width, float height, sf::Color color, std::string label = "") {
        shape.setPosition({x, y});
        shape.setSize({width, height});
        shape.setFillColor(color);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

    bool isClicked(sf::Vector2i mousePos);
};

class node {
private:
    sf::CircleShape shape;
    int state; // 0: normal; 1: early add; 2: delete;
public:
    node(float x, float y, float radius, sf::Color fillColor, sf::Color outlineColor = sf::Color::Black, float outlineThickness = 2.0f) {
        shape.setPosition({x, y});
        shape.setRadius(radius);
        shape.setFillColor(fillColor);
        shape.setOutlineColor(outlineColor);
        shape.setOutlineThickness(outlineThickness);
        state = 0;
    }

    void draw(sf::RenderWindow& window) {
        if(state == 1) {
            shape.setFillColor(sf::Color::Green);
        } else if(state == 2) {
            shape.setFillColor(sf::Color::Red);
        } else {
            shape.setFillColor(sf::Color::White);
        }
        window.draw(shape);
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

    void draw(sf::RenderWindow& window) {
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
};