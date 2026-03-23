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

class RoundedRectangleShape : public sf::Shape {
private:
    sf::Vector2f size;
    float radius;
    std::size_t pointCount;

public:
    RoundedRectangleShape(const sf::Vector2f& size = sf::Vector2f(0, 0), float radius = 0, std::size_t pointCount = 30);

    void setSize(const sf::Vector2f& size);
    const sf::Vector2f& getSize() const;

    void setRadius(float radius);
    float getRadius() const;

    void setCornerPointCount(std::size_t count);

    virtual std::size_t getPointCount() const override;
    virtual sf::Vector2f getPoint(std::size_t index) const override;
};

class button {
private:
    std::string label;
    RoundedRectangleShape shape;
    sf::Text text;
public:
    // tọa độ, kịch thước, màu, chữ, cỡ chữ
    button(float x, float y, float width, float height, sf::Color color, const std::string labelText, unsigned int charSize);

    // lấy tọa độ của button
    sf::Vector2f getPosition();   

    // đổi màu button (fill)
    void setColor(sf::Color color);

    // đổi viền button
    void setOutline(sf::Color color, float thickness);

    // kiểm tra con trỏ nằm trên button
    bool contains(sf::Vector2i mousePos);

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
    // Quản lý vị trí
    sf::Vector2f currentPos; // Vị trí đang vẽ ra màn hình
    sf::Vector2f startPos;   // Vị trí lúc bắt đầu animation
    sf::Vector2f targetPos;  // Vị trí đích cần đến

    // Quản lý thời gian animation
    bool isMoving = false;
    float elapsedTime = 0.0f; // Thời gian đã trôi qua kể từ lúc bắt đầu swap
    float duration = 1.0f;    // Tổng thời gian animation (VD: 0.5 giây)

    // tọa độ, bán kính, màu nền, màu viền, độ dày viền
    node(float x, float y, float radius, sf::Color fillColor, sf::Color outlineColor, float outlineThickness);

    // lấy tọa độ của node
    sf::Vector2f getPosition();

    // lấy bán kính của node
    int getRadius();

    // đặt lại tọa độ theo tâm
    void setPosition(float x, float y);

    // đặt lại bán kính
    void setRadius(float radius);

    // đặt lại label và cỡ chữ
    void setLabel(const std::string &newLabel, unsigned int charSize);

    // đổi màu node
    void setColor(sf::Color color);

    // vẽ node lên cửa sổ
    void draw(sf::RenderWindow& window);

    // kiểm tra xem chuột có nằm trong node hay không
    bool inside(sf::Vector2i mousepos);

    // kiểm tra xem node có được click bằng chuột trái hay không
    bool isClicked(sf::Vector2i mousePos);

    // cập nhật tình hình di chuyển của node này
    void updatePosition(float deltaTime);

    void printInfo();
};

class edge {
private:
    sf::RectangleShape shape;
    sf::ConvexShape arrow;
    float thickness;
    bool has_arrow;
public:
    // tọa độ 2 đầu, màu, độ dày
    edge(float x1, float y1, float x2, float y2, sf::Color color, float thickness_ = 1.0f) : thickness(thickness_), has_arrow(false) {
        setPoints(x1, y1, x2, y2, false);
        shape.setFillColor(color);
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(3.0f);
    }

    // đặt lại tọa độ 2 đầu
    void setPoints(float x1, float y1, float x2, float y2, bool directed = false, float radius = 30.0f);

    // đổi màu cạnh
    void setColor(sf::Color color);

    // đổi độ dày cạnh
    void setThickness(float th);

    // vẽ cạnh lên cửa sổ
    void draw(sf::RenderWindow& window);
};

class box {
private:
    std::string label;
    RoundedRectangleShape shape;
    sf::Text text;
public:

    // tọa độ, kịch thước, mày, chữ, cỡ chữ
    box(float x, float y, float width, float height, sf::Color color, const std::string labelText, unsigned int charSize);

    // lấy tọa độ của box
    sf::Vector2f getPosition();

    // đổi màu box
    void setColor(sf::Color color);

    // đổi viền box
    void setOutline(sf::Color color, float thickness);

    // kiểm tra con trỏ nằm trên box
    bool contains(sf::Vector2i mousePos);

    // đổi chữ box
    void setLabel(const std::string &newLabel);

    // chuyển sự kiện bàn phím thành ký tự
    // '\0' nếu không có gì
    // ' ' nếu là space, '\b' nếu là backspace
    // còn lại là kí tư
    char KeyboardToChar();

    // vẽ box lên cửa sổ
    void draw(sf::RenderWindow& window);

    // kiểm tra xem box có được click bằng chuột trái hay không
    bool isClicked(sf::Vector2i mousePos);
};



class Visual_graph {
private:
    float node_radius = 30.0f;
    float outline_thickness = 4.0f;
    float edge_thickness = 4.0f;
    std::vector<int> nodes_state;
    std::vector<int> edges_state;
    std::vector<node> nodes;
    std::vector<std::pair<int, int>> edges;
    std::vector<int> edge_weights;

    std::vector<std::pair<int,Step>> step_history;
    bool directed = false; // 0: undirected, 1: directed

    // normal; choose. del. pass
    sf::Color color_of_node_state[4] = {sf::Color::White, sf::Color(156,229,147), sf::Color(246,88,88), sf::Color(255,253,202)};
    sf::Color color_of_edge_state[4] = {sf::Color(0,0,0), sf::Color(156,229,147), sf::Color(246,88,88), sf::Color(255,253,202)};
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
    void add_edge(int u, int v, int w, bool directed_flag = false);

    // graph directed flag
    void setDirected(bool value);
    bool isDirected() const;

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

// dùng để bắt đầu cho node di chuyển
void startNodeMovement(node& Node, sf::Vector2f newDestination, float speedSeconds);