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
    shape.setRadius(8.0f);  // less rounded corners
    shape.setCornerPointCount(20);
    shape.setFillColor(color);
    shape.setOutlineColor(sf::Color(200, 200, 200));
    shape.setOutlineThickness(2.0f);

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

sf::FloatRect button::getShape() {
    return shape.getGlobalBounds();
}

void button::setColor(sf::Color color) {
    shape.setFillColor(color);
}

void button::setOutline(sf::Color color, float thickness) {
    shape.setOutlineColor(color);
    shape.setOutlineThickness(thickness);
}

bool button::contains(sf::Vector2i mousePos) {
    return shape.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));
}

void button::setLabel(const std::string& newLabel) {
    label = newLabel;
    text.setString(label);
    text.setFillColor(sf::Color::Black);

    // 1. Cập nhật lại Origin của text (cộng thêm offset của bounding box)
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.f, 
        textBounds.position.y + textBounds.size.y / 2.f
    });

    // 2. Đặt vị trí text vào chính giữa tâm của Shape
    sf::FloatRect shapeBounds = shape.getGlobalBounds();
    text.setPosition({
        shapeBounds.position.x + shapeBounds.size.x / 2.f, 
        shapeBounds.position.y + shapeBounds.size.y / 2.f
    });
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

void button::setRadius(float t){
    shape.setRadius(t);
}

bool button::update(sf::Vector2i mousePos) {
    if (contains(mousePos)) {
        setColor(sf::Color(172, 123, 42));
    } else {
        setColor(sf::Color(232, 183, 81));
    }
    isPress = isClicked(mousePos);
    bool flag = false;
    if (isPress && !onPress) {
        flag = true;
    }
    onPress = isPress;
    return flag;
}

// End of class Button

// Class Node

node::node(float x, float y, float radius, sf::Color fillColor, sf::Color outlineColor = sf::Color::Black, float outlineThickness = 2.0f) : text(font_impact, "", 14) {
    shape.setPosition({x, y});
    currentPos = {x,y};
    currentColor = outlineColor;
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

void node::setOutlineColor(sf::Color color) {
    shape.setOutlineColor(color);
}

void node::updatePosition(float deltaTime){
    if (!isMoving) return; // Nếu không moving thì không làm gì cả

    // 1. Cập nhật đồng hồ thời gian của riêng Node
    elapsedTime += deltaTime;

    // 2. Tính toán hệ số hoàn thành 't' [0.0 -> 1.0]
    float t = elapsedTime / duration;

    // Giới hạn t không vượt quá 1.0
    if (t > 1.0f) {
        t = 1.0f;
    }

    // 3. Áp dụng công thức LERP cho cả X và Y
    // current = start + (target - start) * t;
    currentPos.x = startPos.x + (targetPos.x - startPos.x) * t;
    currentPos.y = startPos.y + (targetPos.y - startPos.y) * t;
    setPosition(currentPos.x, currentPos.y);

    // 4. Kiểm tra xem đã đến đích chưa
    if (t >= 1.0f) {
        isMoving = false; // Tắt cờ di chuyển
        // Lúc này currentPos == targetPos hoàn toàn
    }
}

void node::updateColor(float deltaTime){
    if (!isColoring) return; // Nếu không moving thì không làm gì cả

    // 1. Cập nhật đồng hồ thời gian của riêng Node
    elapsedTime += deltaTime;

    // 2. Tính toán hệ số hoàn thành 't' [0.0 -> 1.0]
    float t = elapsedTime / duration;

    // Giới hạn t không vượt quá 1.0
    if (t > 1.0f) {
        t = 1.0f;
    }

    // 3. Áp dụng công thức LERP cho cả X và Y
    // current = start + (target - start) * t;
    currentColor.a = startColor.a + (targetColor.a - startColor.a) * t;
    currentColor.r = startColor.r + (targetColor.r - startColor.r) * t;
    currentColor.g = startColor.g + (targetColor.g - startColor.g) * t;
    currentColor.b = startColor.b + (targetColor.b - startColor.b) * t;
    
    shape.setOutlineColor(currentColor);

    // 4. Kiểm tra xem đã đến đích chưa
    if (t >= 1.0f) {
        isColoring = false; // Tắt cờ di chuyển
        // Lúc này currentPos == targetPos hoàn toàn
    }
}

int node::getLabel() {
    std::string s = text.getString();
    int res = 0;
    for (int i=s.size()-1; i>=0; --i) {
        res = res*10 + (s[i]-'0');
    }
    return res;
}
// End of class Node

// Class Edge
void edge::setPoints(float x1, float y1, float x2, float y2, bool directed, float radius) {
    float d = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

    shape.setSize({d, thickness});
    shape.setOrigin({0.f, thickness / 2.f});
    shape.rotate(-shape.getRotation()); 
    shape.rotate(sf::radians(std::atan2(y2 - y1, x2 - x1)));
    shape.setPosition({x1, y1});

    has_arrow = directed;
    if (directed) {
        // Create arrow head as a triangle
        const float arrow_size = 25.0f;
        radius -= 5;


        sf::Vector2f dir(x2 - x1, y2 - y1);
        float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (len > 0.001f) {
            dir /= len;
            // Position arrow tip at distance equal to node radius from target node
            sf::Vector2f tip = sf::Vector2f(x2, y2) - dir * radius;
            sf::Vector2f base = tip - dir * arrow_size;

            sf::Vector2f perp(-dir.y, dir.x);
            sf::Vector2f left = base + perp * (arrow_size * 0.5f);
            sf::Vector2f right = base - perp * (arrow_size * 0.5f);

            arrow.setPointCount(3);
            arrow.setPoint(0, tip);
            arrow.setPoint(1, left);
            arrow.setPoint(2, right);
            arrow.setFillColor(shape.getFillColor());
            arrow.setOutlineColor(sf::Color::Black);
            arrow.setOutlineThickness(2.0f);
        }
    }
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
    shape.setRadius(8.0f);
    shape.setCornerPointCount(20);
    shape.setFillColor(color);
    shape.setOutlineColor(sf::Color(180, 180, 180));
    shape.setOutlineThickness(2.0f);

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

void box::setOutline(sf::Color color, float thickness) {
    shape.setOutlineColor(color);
    shape.setOutlineThickness(thickness);
}

bool box::contains(sf::Vector2i mousePos) {
    return shape.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));
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

void box::update(bool isClick, sf::Vector2i mousePos) {
    if (contains(mousePos) || isClick) {
        setColor(sf::Color(214, 217, 224));
    } else {
        setColor(sf::Color(138,155,192));
    }
}

// RoundedRectangleShape definitions
RoundedRectangleShape::RoundedRectangleShape(const sf::Vector2f& size_, float radius_, std::size_t pointCount_) {
    size = size_;
    radius = radius_;
    pointCount = pointCount_;
    update();
}

void RoundedRectangleShape::setSize(const sf::Vector2f& size_) {
    size = size_;
    update();
}

const sf::Vector2f& RoundedRectangleShape::getSize() const {
    return size;
}

void RoundedRectangleShape::setRadius(float radius_) {
    radius = radius_;
    update();
}

float RoundedRectangleShape::getRadius() const {
    return radius;
}

void RoundedRectangleShape::setCornerPointCount(std::size_t count) {
    pointCount = count;
    update();
}

std::size_t RoundedRectangleShape::getPointCount() const {
    return pointCount * 4;
}

sf::Vector2f RoundedRectangleShape::getPoint(std::size_t index) const {
    if (radius == 0) {
        switch (index) {
            case 0: return sf::Vector2f(0, 0);
            case 1: return sf::Vector2f(size.x, 0);
            case 2: return sf::Vector2f(size.x, size.y);
            case 3: return sf::Vector2f(0, size.y);
            default: return sf::Vector2f(0, 0);
        }
    }

    std::size_t corner = index / pointCount;
    std::size_t localIndex = index % pointCount;
    const float PI = 3.14159265358979323846f;
    float angle = PI + corner * (PI / 2.0f) + (localIndex * (PI / 2.0f) / (pointCount - 1));

    sf::Vector2f center;
    switch (corner) {
        case 0: center = sf::Vector2f(radius, radius); break;
        case 1: center = sf::Vector2f(size.x - radius, radius); break;
        case 2: center = sf::Vector2f(size.x - radius, size.y - radius); break;
        case 3: center = sf::Vector2f(radius, size.y - radius); break;
        default: center = sf::Vector2f(0.f, 0.f); break;
    }

    return center + sf::Vector2f(std::cos(angle) * radius, std::sin(angle) * radius);
}

// Node methods moved to cpp
sf::Vector2f node::getPosition() {
    return shape.getPosition();
}

int node::getRadius() {
    return shape.getRadius();
}

void node::setPosition(float x, float y) {
    shape.setPosition({x, y});
    text.setPosition({x, y});
}

void node::setRadius(float radius) {
    shape.setRadius(radius);
    shape.setOrigin({radius, radius});
}

void node::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

void node::printInfo() {
    sf::Vector2f pos = shape.getPosition();
    std::cout << "Node at (" << pos.x << ", " << pos.y << ")" << std::endl;
}

// Edge draw
void edge::draw(sf::RenderWindow& window) {
    window.draw(shape);
    if (has_arrow) {
        window.draw(arrow);
    }
}

// Box draw
void box::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

// End of class box

// Step Struct
//


// Class Visual_graph

void Visual_graph::add_node(float x, float y) {
    std::cerr << "Adding node at (" << x << ", " << y << ")" << std::endl;

    nodes.emplace_back(x, y, node_radius, color_of_node_state[0], sf::Color::Black, outline_thickness);
    nodes_state.push_back(0);
    node_labels.push_back(std::to_string(nodes.size() - 1));
}

void Visual_graph::add_edge(int u, int v, int w, bool directed_flag) {
    if (u < nodes.size() && v < nodes.size()) {
        bool effective_directed = directed_flag ? true : directed;

        // Check and remove existing edge(s)
        for(int i = edges.size() - 1; i >= 0; --i) {
            if((edges[i].first == u && edges[i].second == v) || 
               (!effective_directed && edges[i].first == v && edges[i].second == u)) {
                edges.erase(edges.begin() + i);
                edge_weights.erase(edge_weights.begin() + i);
                edges_state.erase(edges_state.begin() + i);
            }
        }

        edges.emplace_back(u, v);
        edge_weights.push_back(w);
        edges_state.push_back(0);
    }
}

void Visual_graph::setDirected(bool value) {
    directed = value;
}

bool Visual_graph::isDirected() const {
    return directed;
}

void Visual_graph::add_history(int i, Step step) {
    step_history.push_back({i, step});
}

void Visual_graph::setPosition(size_t index, float x, float y) {
    if (index < nodes.size()) {
        nodes[index].setPosition(x, y);
    }
}

void Visual_graph::setNodeState(int i, int st) {
    if(nodes_state.size() > i) {
        nodes_state[i] = st;
    }
}

void Visual_graph::setEdgeState(int i, int st) {
    if(edges_state.size() > i) {
        edges_state[i] = st;
    }
}

const std::vector<std::string>& Visual_graph::getNodeLabels() const {
    return node_labels;
}

void Visual_graph::clearAll() {
    nodes.clear();
    edges.clear();
    edge_weights.clear();
    node_labels.clear();
    nodes_state.clear();
    edges_state.clear();
}

void Visual_graph::setNodeLabel(size_t index, const std::string& label) {
    if (index < node_labels.size()) node_labels[index] = label;
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

        temp_edge.setColor(color_of_edge_state[edges_state[i]]);
        temp_edge.setPoints(pos_u.x, pos_u.y, pos_v.x, pos_v.y, directed, getRadius());
        temp_edge.draw(window);
    }

    for(size_t i = 0; i < nodes.size(); i++) {
        nodes[i].setColor(color_of_node_state[nodes_state[i]]);
        nodes[i].draw(window);
    }
}

void Visual_graph::draw(sf::RenderWindow& window, bool draw_weights) {
    edge temp_edge(0, 0, 0, 0, sf::Color::White, edge_thickness);
    for(size_t i = 0; i < edges.size(); i++) {
        temp_edge.setColor(color_of_edge_state[edges_state[i]]);
        int u = edges[i].first;
        int v = edges[i].second;
        sf::Vector2f pos_u = nodes[u].getPosition();
        sf::Vector2f pos_v = nodes[v].getPosition();
        temp_edge.setPoints(pos_u.x, pos_u.y, pos_v.x, pos_v.y, directed, getRadius());
        temp_edge.draw(window);

        if(draw_weights) {
            sf::Text weight_text(font_impact, std::to_string(edge_weights[i]), 20 + edge_thickness);
            weight_text.setFillColor(sf::Color::Black);
            sf::FloatRect textBounds = weight_text.getLocalBounds();
            weight_text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
            
            // Calculate perpendicular offset to place weight above the edge
            float dx = pos_v.x - pos_u.x;
            float dy = pos_v.y - pos_u.y;
            float edge_length = std::sqrt(dx * dx + dy * dy);
            
            if(edge_length > 0) {
                // Perpendicular vector (rotated 90 degrees)
                float perp_x = -dy / edge_length;
                float perp_y = dx / edge_length;
                
                // Ensure weight is always placed above the edge (smaller y = higher)
                // If perp_y > 0, reverse the direction
                if(perp_y > 0) {
                    perp_x = -perp_x;
                    perp_y = -perp_y;
                }
                
                // Position weight at edge midpoint + perpendicular offset
                float offset_dist = 20.0f;
                float weight_x = (pos_u.x + pos_v.x) / 2.f + perp_x * offset_dist;
                float weight_y = (pos_u.y + pos_v.y) / 2.f + perp_y * offset_dist;
                
                weight_text.setPosition({weight_x, weight_y});
                window.draw(weight_text);
            }
        }
    }

    for(size_t i = 0; i < nodes.size(); i++) {
        nodes[i].setColor(color_of_node_state[nodes_state[i]]);
        nodes[i].setLabel(node_labels[i], 24);
        nodes[i].draw(window);
    }
}

// End of class Visual_graph

void startNodeMovement(node& Node, sf::Vector2f newDestination, float speedSeconds) {
    Node.startPos = Node.currentPos; // Lưu lại điểm xuất phát
    Node.targetPos = newDestination;   // Đặt điểm đích
    Node.duration = speedSeconds;      // Đặt tốc độ bay
    Node.elapsedTime = 0.0f;           // Reset đồng hồ
    Node.isMoving = true;             // Bật cờ di chuyển
}

void startNodeColor(node& Node, sf::Color newColor, float speedSeconds) {
    Node.startColor = Node.currentColor; // Lưu lại màu đầu
    Node.targetColor = newColor;   // Đặt màu target
    Node.duration = speedSeconds;      // Đặt tốc độ chuyển
    Node.elapsedTime = 0.0f;           // Reset đồng hồ
    Node.isColoring = true;             // Bật cờ di chuyển
}
