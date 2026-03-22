#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

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
    // tọa độ, kịch thước, mày, chữ, cỡ chữ
    button(float x, float y, float width, float height, sf::Color color, const std::string labelText, unsigned int charSize);

    // lấy tọa độ của button
    sf::Vector2f getPosition();   

    // đổi màu button
    void setColor(sf::Color color);

    // đổi chữ button
    void setLabel(const std::string &newLabel);

    // vẽ button lên cửa sổ
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    // kiểm tra xem button có được click bằng chuột trái hay không
    bool isClicked(sf::Vector2i mousePos);
};

class node {
private:
    sf::CircleShape shape;
    sf::Text text;
public:
    // tọa độ, bán kính, màu nền, màu viền, độ dày viền
    node(float x, float y, float radius, sf::Color fillColor, sf::Color outlineColor, float outlineThickness);

    // lấy tọa độ của node
    sf::Vector2f getPosition() {
        return shape.getPosition();
    }

    // lấy bán kính của node
    int getRadius() {
        return shape.getRadius();
    }

    // đặt lại tọa độ theo tậm
    void setPosition(float x, float y) {
        shape.setPosition({x, y});
        text.setPosition({x, y});
    }
     
    // đặt lại bán kính
    void setRadius(float radius) {
        shape.setRadius(radius);
        shape.setOrigin({radius, radius}); // Update origin when radius changes
    }

    // đặt lại label và cỡ chữ
    void setLabel(const std::string &newLabel, unsigned int charSize);

    // đổi màu node
    void setColor(sf::Color color);

    // vẽ node lên cửa sổ
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    // kiểm tra xem chuột có nằm trong node hay không
    bool inside(sf::Vector2i mousepos);

    // kiểm tra xem node có được click bằng chuột trái hay không
    bool isClicked(sf::Vector2i mousePos);

    void printInfo() {
        sf::Vector2f pos = shape.getPosition();
        std::cout << "Node at (" << pos.x << ", " << pos.y << ")" << std::endl;
    }
};

class edge {
private:
    sf::RectangleShape shape;
    float thickness;
public:
    // tọa độ 2 đầu, màu, độ dày
    edge(float x1, float y1, float x2, float y2, sf::Color color, float thickness_ = 4.0f) : thickness(thickness_) {
        setPoints(x1, y1, x2, y2);
        shape.setFillColor(color);
        shape.setOutlineThickness(0);
    }

    // đặt lại tọa độ 2 đầu
    void setPoints(float x1, float y1, float x2, float y2);

    // đổi màu cạnh
    void setColor(sf::Color color);

    // đổi độ dày cạnh
    void setThickness(float th);

    // vẽ cạnh lên cửa sổ
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

class box {
private:
    std::string label;
    sf::RectangleShape shape;
    sf::Text text;
public:

    // tọa độ, kịch thước, mày, chữ, cỡ chữ
    box(float x, float y, float width, float height, sf::Color color, const std::string labelText, unsigned int charSize);

    // lấy tọa độ của box
    sf::Vector2f getPosition();

    // đổi màu box
    void setColor(sf::Color color);

    // đổi chữ box
    void setLabel(const std::string &newLabel);

    // chuyển sự kiện bàn phím thành ký tự
    // '\0' nếu không có gì
    // ' ' nếu là space, '\b' nếu là backspace
    // còn lại là kí tư
    char KeyboardToChar();

    // vẽ box lên cửa sổ
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    // kiểm tra xem box có được click bằng chuột trái hay không
    bool isClicked(sf::Vector2i mousePos);
};



class Visual_graph {
private:
    float node_radius = 30.0f;
    float outline_thickness = 4.0f;
    float edge_thickness = 8.0f;
    std::vector<int> nodes_state;
    std::vector<int> edges_state;
    std::vector<node> nodes;
    std::vector<std::pair<int, int>> edges;
    std::vector<int> edge_weights;

    std::vector<std::pair<int,Step>> step_history;

    sf::Color color_of_state[4] = {sf::Color::White, sf::Color::Green, sf::Color::Red, sf::Color::Yellow};
public:
    // lấy bán kính của node/
    int getRadius() {
        return node_radius;
    }

    float getEdgeThinkness() {
        return edge_thickness;
    }

    // lấy tọa độ của node thứ index
    sf::Vector2f getPosition(size_t index) {
        if (index < nodes.size()) {
            return nodes[index].getPosition();
        }
        return sf::Vector2f(0, 0);
    }

    std::vector<std::pair<int,int>>& getEdgeList() {
        return edges;
    }

    std::vector<int>& getEdgeWeightsList () {
        return edge_weights;
    }

    // thêm node vào đồ thị
    void add_node(float x, float y);

    // thêm cạnh giữa 2 node u, v
    void add_edge(int u, int v, int w);

    // add history
    void add_history(int i, Step step);

    // đặt lại tọa độ của node thứ index
    void setPosition(size_t index, float x, float y);

    void setNodeState(int i, int st);

    void setEdgeState(int i, int st);

    // tìm xem có node nào chứa (x, y) khộng
    int find_node_stored(float x, float y);

    // vẽ đồ thị lên cửa sổ
    void draw(sf::RenderWindow& window);

    // vẽ đồ thị có trọng số lên cửa sổ
    void draw(sf::RenderWindow& window, bool draw_weights);
};