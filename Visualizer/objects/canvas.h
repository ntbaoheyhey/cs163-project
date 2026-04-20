#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>

#include "../headers/core.h"
#include "../headers/global.h"

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
    bool isClickedInside{0};
    bool onPress{0};

    // tọa độ, kịch thước, màu, chữ, cỡ chữ
    button(float x, float y, float width, float height, sf::Color color, const std::string labelText, unsigned int charSize);

    // lấy tọa độ của button
    sf::Vector2f getPosition();   

    sf::FloatRect getShape();
    // đổi màu button (fill)
    void setColor(sf::Color color);

    // đổi viền button
    void setOutline(sf::Color color, float thickness);

    // đổi gốc của button - Origin
    void setOrigin(float x, float y);

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

    void setRadius(float t);

    // update trạng thái của button
    bool update(sf::Vector2i mousePos);
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
    sf::Color currentColor;
    sf::Color startColor;
    sf::Color targetColor;

    // Quản lý thời gian animation
    bool isMoving = false;
    bool isColoring = false;
    float movementElapsedTime = 0.0f;
    float movementDuration = 1.0f;
    float colorElapsedTime = 0.0f;
    float colorDuration = 1.0f;

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
    
    // đặt lại màu viền
    void setOutlineColor(sf::Color color);
    
    // đổi màu node
    void setColor(sf::Color color);

    // đổi độ mờ toàn bộ (fill, outline, text)
    void setOpacity(std::uint8_t alpha);

    // vẽ node lên cửa sổ
    void draw(sf::RenderWindow& window) const;

    // kiểm tra xem chuột có nằm trong node hay không
    bool inside(sf::Vector2i mousepos);

    // kiểm tra xem node có được click bằng chuột trái hay không
    bool isClicked(sf::Vector2i mousePos);

    // cập nhật tình hình di chuyển của node này
    void updatePosition(float deltaTime);

    // cập nhật tình hình chuyển màu của node này
    void updateColor(float deltaTime);

    void printInfo();

    int getLabel();
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
        shape.setOutlineThickness(1.0f);
    }

    // đặt lại tọa độ 2 đầu
    void setPoints(float x1, float y1, float x2, float y2, bool directed = false, float radius = 30.0f);

    // đổi màu cạnh
    void setColor(sf::Color color);

    // đổi độ mờ toàn bộ (fill, outline, arrow)
    void setOpacity(std::uint8_t alpha);

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

    // chỉnh màu box khi hover hoặc khi click
    void update(bool isClick, sf::Vector2i mousePos);
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
    std::vector<std::string> node_labels;

    std::vector<std::pair<int,Step>> step_history;
    bool directed = false; // 0: undirected, 1: directed

    // normal; choose. del. pass
    sf::Color color_of_node_state[4] = {sf::Color(238, 239, 244), sf::Color(138,155,192), sf::Color(246,88,88), sf::Color(242, 202, 110)};
    sf::Color color_of_edge_state[4] = {sf::Color(0,0,0), sf::Color(138,155,192), sf::Color(246,88,88), sf::Color(242, 202, 110)};
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

    const std::vector<std::string>& getNodeLabels() const;

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

    void clearAll();

    void setNodeLabel(size_t index, const std::string& label);

    int find_node_stored(float x, float y);

    // vẽ đồ thị lên cửa sổ
    void draw(sf::RenderWindow& window);

    // vẽ đồ thị có trọng số lên cửa sổ
    void draw(sf::RenderWindow& window, bool draw_weights);

};

// dùng để bắt đầu cho node di chuyển
void startNodeMovement(node& Node, sf::Vector2f newDestination, float speedSeconds);

// dùng để bắt đầu đổi màu cho 
void startNodeColor(node& Node, sf::Color newColor, float speedSeconds);

class CodeBox : public sf::Drawable, public sf::Transformable {
private:
    sf::RectangleShape m_background;
    sf::Text m_text;
    sf::RectangleShape m_highlight;
    
    float m_padding;
    float m_lineSpacing;
    int m_activeLine;

    // SFML 3.0 standard draw override
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // Apply any transformations (position, rotation, scale) set on the CodeBox
        states.transform *= getTransform();

        // Draw components in order (bottom to top)
        target.draw(m_background, states);

        // Only draw the highlight if a valid line is selected
        if (m_activeLine >= 0) {
            target.draw(m_highlight, states);
        }

        target.draw(m_text, states);
    }

public:
    CodeBox(sf::Vector2f size, const sf::Font& font, unsigned int charSize = 20)
    : m_text(font), m_padding(10.f), m_activeLine(-1) {
        
        // Cache the line spacing for the highlight bar calculation
        m_lineSpacing = font.getLineSpacing(charSize);

        // 1. Setup Background 
        m_background.setSize(size);
        m_background.setFillColor(sf::Color(215, 161, 72, 200));     
        //m_background.setOutlineColor(sf::Color(171, 178, 191));
        m_background.setOutlineThickness(1.f);

        // 2. Setup Text
        m_text.setCharacterSize(charSize);
        m_text.setFillColor(sf::Color::Black);
        m_text.setPosition({m_padding, m_padding}); // Offset by padding

        // 3. Setup Highlight Bar
        m_highlight.setFillColor(sf::Color(76, 86, 106, 120)); // Semi-transparent
        m_highlight.setSize({size.x, m_lineSpacing});
    }

    // Load the raw code string into the box
    void setCode(const std::string& code) {
        m_text.setString(code);
    }

    // Highlight a specific line (0-indexed). Pass -1 to hide the highlight.
    void setStep(int line) {
        m_activeLine = line;
        if (line >= 0) {
            // Calculate the Y position based on line number and spacing
            m_highlight.setPosition({0.f, m_padding + (line * m_lineSpacing)});
        }
    }
};
