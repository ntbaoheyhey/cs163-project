#include "../headers/singly_linked_list.h"
#include <sstream>

namespace {

const sf::Color NODE_FILL_COLOR = sf::Color(218, 168, 74);
const sf::Color NODE_OUTLINE_COLOR = sf::Color(202, 148, 95);
const sf::Color NODE_ACTIVE_COLOR = sf::Color(62, 188, 167);
const sf::Color NODE_FOUND_COLOR = sf::Color(156, 229, 147);
const sf::Color NODE_DELETE_COLOR = sf::Color(246, 88, 88);

int parse_non_negative_int(const std::string& s, int fallback = 0) {
    if (s.empty()) return fallback;
    int value = 0;
    for (char c : s) {
        if (c < '0' || c > '9') return fallback;
        value = value * 10 + (c - '0');
    }
    return value;
}

struct SinglyLinkedListVisualizer {
    std::vector<int> values;
    std::vector<node> nodes;

    float node_radius = 28.0f;
    float start_x = 120.0f;
    float start_y = 260.0f;
    float spacing = 120.0f;

    sf::Vector2f target_pos(int idx) const {
        return {start_x + spacing * idx, start_y};
    }

    void update_labels() {
        for (int i = 0; i < (int)nodes.size(); ++i) {
            nodes[i].setLabel(std::to_string(values[i]), 20);
        }
    }

    void animate_layout() {
        for (int i = 0; i < (int)nodes.size(); ++i) {
            startNodeMovement(nodes[i], target_pos(i), 0.35f);
        }
    }

    void clear_highlight() {
        for (auto &n : nodes) {
            startNodeColor(n, NODE_OUTLINE_COLOR, 0.2f);
        }
    }

    void create_node(int val) {
        values.push_back(val);
        sf::Vector2f spawn = target_pos((int)nodes.size());
        spawn.y -= 100.0f;
        node new_node(spawn.x, spawn.y, node_radius, NODE_FILL_COLOR, NODE_OUTLINE_COLOR, 4);
        new_node.setLabel(std::to_string(val), 20);
        nodes.push_back(new_node);
        animate_layout();
    }

    bool insert_head(int val) {
        values.insert(values.begin(), val);
        sf::Vector2f spawn = target_pos(0);
        spawn.x -= 100.f;
        spawn.y -= 80.f;
        node new_node(spawn.x, spawn.y, node_radius, NODE_FILL_COLOR, NODE_OUTLINE_COLOR, 4);
        new_node.setLabel(std::to_string(val), 20);
        nodes.insert(nodes.begin(), new_node);
        animate_layout();
        startNodeColor(nodes[0], NODE_ACTIVE_COLOR, 0.25f);
        return true;
    }

    bool insert_tail(int val) {
        create_node(val);
        if (!nodes.empty()) startNodeColor(nodes.back(), NODE_ACTIVE_COLOR, 0.25f);
        return true;
    }

    bool insert_index(int idx, int val) {
        if (idx < 0 || idx > (int)values.size()) return false;
        if (idx == 0) return insert_head(val);
        if (idx == (int)values.size()) return insert_tail(val);

        values.insert(values.begin() + idx, val);
        sf::Vector2f spawn = target_pos(idx);
        spawn.y -= 90.f;
        node new_node(spawn.x, spawn.y, node_radius, NODE_FILL_COLOR, NODE_OUTLINE_COLOR, 4);
        new_node.setLabel(std::to_string(val), 20);
        nodes.insert(nodes.begin() + idx, new_node);
        animate_layout();
        startNodeColor(nodes[idx], NODE_ACTIVE_COLOR, 0.25f);
        return true;
    }

    bool delete_index(int idx) {
        if (idx < 0 || idx >= (int)values.size()) return false;

        startNodeColor(nodes[idx], NODE_DELETE_COLOR, 0.15f);
        values.erase(values.begin() + idx);
        nodes.erase(nodes.begin() + idx);
        animate_layout();
        return true;
    }

    bool delete_head() {
        if (values.empty()) return false;
        return delete_index(0);
    }

    bool delete_tail() {
        if (values.empty()) return false;
        return delete_index((int)values.size() - 1);
    }

    int search_value(int val) {
        for (int i = 0; i < (int)values.size(); ++i) {
            startNodeColor(nodes[i], NODE_ACTIVE_COLOR, 0.2f);
            if (values[i] == val) {
                startNodeColor(nodes[i], NODE_FOUND_COLOR, 0.25f);
                return i;
            }
        }
        return -1;
    }

    void traversal() {
        for (int i = 0; i < (int)nodes.size(); ++i) {
            startNodeColor(nodes[i], NODE_ACTIVE_COLOR, 0.25f);
        }
    }

    bool update_value(int idx, int val) {
        if (idx < 0 || idx >= (int)values.size()) return false;
        values[idx] = val;
        nodes[idx].setLabel(std::to_string(val), 20);
        startNodeColor(nodes[idx], NODE_FOUND_COLOR, 0.25f);
        return true;
    }

    void render(sf::RenderWindow &window, float dt) {
        for (auto &n : nodes) {
            n.updatePosition(dt);
            n.updateColor(dt);
        }

        for (int i = 0; i + 1 < (int)nodes.size(); ++i) {
            edge e(nodes[i].currentPos.x, nodes[i].currentPos.y,
                   nodes[i + 1].currentPos.x, nodes[i + 1].currentPos.y,
                   sf::Color(203, 203, 201), 3);
            e.setPoints(nodes[i].currentPos.x, nodes[i].currentPos.y,
                        nodes[i + 1].currentPos.x, nodes[i + 1].currentPos.y,
                        true, node_radius);
            e.draw(window);
        }

        for (auto &n : nodes) n.draw(window);

        if (!nodes.empty()) {
            sf::Text head_text(font_impact, "HEAD", 22);
            head_text.setFillColor(sf::Color::Black);
            head_text.setPosition({nodes.front().currentPos.x - 35.f, nodes.front().currentPos.y - 85.f});
            window.draw(head_text);

            sf::Text tail_text(font_impact, "TAIL", 22);
            tail_text.setFillColor(sf::Color::Black);
            tail_text.setPosition({nodes.back().currentPos.x - 30.f, nodes.back().currentPos.y + 45.f});
            window.draw(tail_text);
        }
    }
};

void append_digit(std::string &s, char32_t unicode) {
    if (unicode == '\b' || unicode == 8) {
        if (!s.empty()) s.pop_back();
        if (s.empty()) s = "0";
        return;
    }

    if (unicode >= '0' && unicode <= '9') {
        if (s == "0") s.clear();
        if (s.size() < 4) s.push_back((char)unicode);
    }

    if (s.empty()) s = "0";
}

} // namespace

void singly_linked_list_page() {
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Visualizer/assets/bg_toty.png")) {
        std::cerr << "cannot load background" << std::endl;
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    RoundedRectangleShape bgmain({980.f, 520.f});
    bgmain.setPosition({40.f, 40.f});
    bgmain.setFillColor(sf::Color(243, 243, 251, 110));
    bgmain.setRadius(17);
    bgmain.setOutlineThickness(5);
    bgmain.setOutlineColor(sf::Color(217, 211, 209));

    RoundedRectangleShape side_panel({220.f, 640.f});
    side_panel.setPosition({1060.f, 30.f});
    side_panel.setFillColor(sf::Color(243, 243, 251, 120));
    side_panel.setRadius(17);
    side_panel.setOutlineThickness(4);
    side_panel.setOutlineColor(sf::Color(217, 211, 209));

    button create_btn(1080, 50, 180, 42, sf::Color(232, 183, 81), "Create", 20);
    button ins_head_btn(1080, 98, 180, 42, sf::Color(232, 183, 81), "Insert Head", 20);
    button ins_tail_btn(1080, 146, 180, 42, sf::Color(232, 183, 81), "Insert Tail", 20);
    button ins_idx_btn(1080, 194, 180, 42, sf::Color(232, 183, 81), "Insert Index", 20);
    button del_head_btn(1080, 242, 180, 42, sf::Color(232, 183, 81), "Delete Head", 20);
    button del_tail_btn(1080, 290, 180, 42, sf::Color(232, 183, 81), "Delete Tail", 20);
    button del_idx_btn(1080, 338, 180, 42, sf::Color(232, 183, 81), "Delete Index", 20);
    button search_btn(1080, 386, 180, 42, sf::Color(232, 183, 81), "Search", 20);
    button traverse_btn(1080, 434, 180, 42, sf::Color(232, 183, 81), "Traversal", 20);
    button update_btn(1080, 482, 180, 42, sf::Color(232, 183, 81), "Update Value", 20);
    button clear_hl_btn(1080, 530, 180, 42, sf::Color(138,155,192), "Clear Highlight", 20);

    for (button* b : std::vector<button*>{&create_btn, &ins_head_btn, &ins_tail_btn, &ins_idx_btn, &del_head_btn,
                                          &del_tail_btn, &del_idx_btn, &search_btn, &traverse_btn, &update_btn, &clear_hl_btn}) {
        b->setRadius(20);
    }

    box value_box(70, 600, 200, 50, sf::Color(138,155,192), "10", 24);
    box index_box(300, 600, 150, 50, sf::Color(138,155,192), "0", 24);

    sf::Text value_label(font_impact, "Value", 24);
    value_label.setFillColor(sf::Color::Black);
    value_label.setPosition({70.f, 565.f});

    sf::Text index_label(font_impact, "Index", 24);
    index_label.setFillColor(sf::Color::Black);
    index_label.setPosition({300.f, 565.f});

    sf::Text info_text(font_impact, "Ready", 24);
    info_text.setFillColor(sf::Color::Black);
    info_text.setPosition({500.f, 600.f});

    std::string value_text = "10";
    std::string index_text = "0";

    bool value_active = false;
    bool index_active = false;
    bool mouse_left_pressed = false;
    bool mouse_left_pressed_last = true;

    SinglyLinkedListVisualizer list_core;
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    value_active = value_box.isClicked(sf::Mouse::getPosition(window));
                    index_active = index_box.isClicked(sf::Mouse::getPosition(window));
                }
            }

            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (value_active) append_digit(value_text, textEvent->unicode);
                if (index_active) append_digit(index_text, textEvent->unicode);
            }
        }

        int input_value = parse_non_negative_int(value_text, 0);
        int input_index = parse_non_negative_int(index_text, 0);

        value_box.setLabel(value_text);
        index_box.setLabel(index_text);

        auto click_once = [&](button &btn) {
            return btn.isClicked(sf::Mouse::getPosition(window)) && !mouse_left_pressed_last;
        };

        if (click_once(create_btn)) {
            list_core.create_node(input_value);
            info_text.setString("Create node " + std::to_string(input_value));
        } else if (click_once(ins_head_btn)) {
            list_core.insert_head(input_value);
            info_text.setString("Insert " + std::to_string(input_value) + " at head");
        } else if (click_once(ins_tail_btn)) {
            list_core.insert_tail(input_value);
            info_text.setString("Insert " + std::to_string(input_value) + " at tail");
        } else if (click_once(ins_idx_btn)) {
            if (list_core.insert_index(input_index, input_value)) {
                info_text.setString("Insert " + std::to_string(input_value) + " at index " + std::to_string(input_index));
            } else {
                info_text.setString("Insert index out of range");
            }
        } else if (click_once(del_head_btn)) {
            if (list_core.delete_head()) info_text.setString("Delete head");
            else info_text.setString("List is empty");
        } else if (click_once(del_tail_btn)) {
            if (list_core.delete_tail()) info_text.setString("Delete tail");
            else info_text.setString("List is empty");
        } else if (click_once(del_idx_btn)) {
            if (list_core.delete_index(input_index)) info_text.setString("Delete index " + std::to_string(input_index));
            else info_text.setString("Delete index out of range");
        } else if (click_once(search_btn)) {
            int found = list_core.search_value(input_value);
            if (found >= 0) info_text.setString("Found " + std::to_string(input_value) + " at index " + std::to_string(found));
            else info_text.setString("Not found: " + std::to_string(input_value));
        } else if (click_once(traverse_btn)) {
            list_core.traversal();
            info_text.setString("Traversal highlight");
        } else if (click_once(update_btn)) {
            if (list_core.update_value(input_index, input_value)) {
                info_text.setString("Update index " + std::to_string(input_index) + " -> " + std::to_string(input_value));
            } else {
                info_text.setString("Update index out of range");
            }
        } else if (click_once(clear_hl_btn)) {
            list_core.clear_highlight();
            info_text.setString("Clear highlight");
        }

        window.clear(sf::Color(212, 188, 112, 0.71));
        window.draw(backgroundSprite);
        window.draw(bgmain);
        window.draw(side_panel);

        create_btn.draw(window);
        ins_head_btn.draw(window);
        ins_tail_btn.draw(window);
        ins_idx_btn.draw(window);
        del_head_btn.draw(window);
        del_tail_btn.draw(window);
        del_idx_btn.draw(window);
        search_btn.draw(window);
        traverse_btn.draw(window);
        update_btn.draw(window);
        clear_hl_btn.draw(window);

        value_box.draw(window);
        index_box.draw(window);
        window.draw(value_label);
        window.draw(index_label);
        window.draw(info_text);

        list_core.render(window, dt);

        window.display();
        mouse_left_pressed_last = mouse_left_pressed;
    }
}
