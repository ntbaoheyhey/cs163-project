#include "../headers/singly_linked_list.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <optional>
#include <random>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#endif

namespace {

const sf::Color NODE_FILL_COLOR = sf::Color(218, 168, 74);
const sf::Color NODE_OUTLINE_COLOR = sf::Color(202, 148, 95);
const sf::Color NODE_ACTIVE_COLOR = sf::Color(62, 188, 167);
const sf::Color NODE_FOUND_COLOR = sf::Color(156, 229, 147);
const sf::Color NODE_DELETE_COLOR = sf::Color(246, 88, 88);

const sf::Color PANEL_OUTLINE_COLOR = sf::Color(217, 211, 209);
const sf::Color PANEL_FILL_COLOR = sf::Color(243, 243, 251, 120);
const sf::Color STATUS_FILL_COLOR = sf::Color(243, 243, 251, 135);
const sf::Color MUTED_TEXT_COLOR = sf::Color(70, 70, 70);

const float BASE_NODE_RADIUS = 28.0f;
const float MIN_NODE_RADIUS = 18.0f;
const float BASE_SPACING = 118.0f;
const float MIN_SPACING = 70.0f;
const float HORIZONTAL_PADDING = 88.0f;
const float LIST_ANIMATION_SECONDS = 0.35f;
const float COLOR_ANIMATION_SECONDS = 0.20f;
const float SCRIPT_STEP_SECONDS = 0.32f;
const float DELETE_DELAY_SECONDS = 0.20f;

enum class StatusTone {
    Info,
    Success,
    Warning,
    Error
};

struct StatusMessage {
    StatusTone tone{StatusTone::Info};
    std::string text{"Ready"};
};

sf::Color statusTextColor(StatusTone tone) {
    switch (tone) {
        case StatusTone::Success:
            return sf::Color(39, 120, 54);
        case StatusTone::Warning:
            return sf::Color(156, 96, 0);
        case StatusTone::Error:
            return sf::Color(180, 40, 40);
        case StatusTone::Info:
        default:
            return sf::Color::Black;
    }
}

int parse_non_negative_int(const std::string& s, int fallback = 0) {
    if (s.empty()) return fallback;

    int value = 0;
    for (char c : s) {
        if (c < '0' || c > '9') return fallback;

        if (value > 1000000) {
            return value;
        }
        value = value * 10 + (c - '0');
    }

    return value;
}

void append_digit(std::string& s, char32_t unicode, std::size_t max_length) {
    if (unicode == '\b' || unicode == 8) {
        if (!s.empty()) s.pop_back();
        if (s.empty()) s = "0";
        return;
    }

    if (unicode >= '0' && unicode <= '9') {
        if (s == "0") s.clear();
        if (s.size() < max_length) s.push_back(static_cast<char>(unicode));
    }

    if (s.empty()) s = "0";
}

std::string openTextFileDialog() {
#ifdef _WIN32
    OPENFILENAMEA ofn{};
    char file_path[MAX_PATH] = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = file_path;
    ofn.nMaxFile = sizeof(file_path);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
#endif

    return "";
}

std::vector<int> readListValuesFromFile(const std::string& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return {};
    }

    std::vector<int> numbers;
    int value = 0;
    while (input >> value) {
        numbers.push_back(value);
    }

    if (numbers.empty()) {
        return {};
    }

    const auto values_are_valid = [](const std::vector<int>& values) {
        return std::all_of(values.begin(), values.end(), [](int item) {
            return 0 <= item && item <= 999;
        });
    };

    if (numbers.size() >= 2 && numbers.front() >= 0 &&
        static_cast<std::size_t>(numbers.front()) == numbers.size() - 1) {
        std::vector<int> values(numbers.begin() + 1, numbers.end());
        if (values_are_valid(values)) {
            return values;
        }
    }

    if (values_are_valid(numbers)) {
        return numbers;
    }

    return {};
}

std::string insertRangeLabel(int size) {
    return "Valid insert index: 0.." + std::to_string(size);
}

std::string accessRangeLabel(int size) {
    if (size <= 0) return "List is empty.";
    return "Valid index: 0.." + std::to_string(size - 1);
}

void setOutlineImmediate(node& visual_node, sf::Color color) {
    visual_node.isColoring = false;
    visual_node.currentColor = color;
    visual_node.startColor = color;
    visual_node.targetColor = color;
    visual_node.colorElapsedTime = 0.0f;
    visual_node.colorDuration = 0.0f;
    visual_node.setOutlineColor(color);
}

void stopMovementImmediate(node& visual_node, sf::Vector2f position) {
    visual_node.isMoving = false;
    visual_node.currentPos = position;
    visual_node.startPos = position;
    visual_node.targetPos = position;
    visual_node.movementElapsedTime = 0.0f;
    visual_node.movementDuration = 0.0f;
    visual_node.setPosition(position.x, position.y);
}

struct ListVisualItem {
    int value;
    node visual;

    ListVisualItem(int item_value, sf::Vector2f spawn_pos, float radius, unsigned int char_size)
        : value(item_value),
          visual(spawn_pos.x, spawn_pos.y, radius, NODE_FILL_COLOR, NODE_OUTLINE_COLOR, 4.0f) {
        visual.setLabel(std::to_string(value), char_size);
        setOutlineImmediate(visual, NODE_OUTLINE_COLOR);
    }
};

class SinglyLinkedListVisualizer {
public:
    enum class ScriptMode {
        None,
        Traversal,
        Search
    };

    explicit SinglyLinkedListVisualizer(sf::FloatRect bounds)
        : canvas_bounds(bounds) {
        refreshLayoutMetrics();
    }

    int size() const {
        return static_cast<int>(items.size());
    }

    bool empty() const {
        return items.empty();
    }

    bool atCapacity() const {
        return size() >= max_visible_nodes;
    }

    int maxNodes() const {
        return max_visible_nodes;
    }

    bool layoutIsCompact() const {
        return compact_layout;
    }

    bool isBusy() const {
        if (pending_delete_index >= 0 || script_mode != ScriptMode::None) {
            return true;
        }

        for (const auto& item : items) {
            if (item.visual.isMoving || item.visual.isColoring) {
                return true;
            }
        }

        return false;
    }

    std::string insertRangeHint() const {
        return insertRangeLabel(size());
    }

    std::string accessRangeHint() const {
        return accessRangeLabel(size());
    }

    std::optional<StatusMessage> consumePendingStatus() {
        auto message = pending_status;
        pending_status.reset();
        return message;
    }

    void clear_highlight() {
        clearVisualState(false);
        pending_status = StatusMessage{StatusTone::Info, empty() ? "List is already clear." : "Highlights cleared."};
    }

    bool create_node(int value) {
        if (atCapacity()) return false;

        clearVisualState(true);
        refreshLayoutMetrics();

        items.emplace_back(value, spawnPositionForIndex(size()), current_radius, labelCharSize());
        applyLayout(true);
        highlightIndex(size() - 1, NODE_FOUND_COLOR, 0.25f);
        return true;
    }

    bool rebuild_from_values(const std::vector<int>& values) {
        refreshLayoutMetrics();
        if (values.size() > static_cast<std::size_t>(max_visible_nodes)) return false;

        clearVisualState(true);
        pending_status.reset();
        items.clear();
        refreshLayoutMetrics();

        const float spawn_y = canvas_bounds.position.y + canvas_bounds.size.y * 0.52f - 92.0f;
        const float spawn_left = canvas_bounds.position.x + 44.0f;
        const float spawn_right = canvas_bounds.position.x + canvas_bounds.size.x - 44.0f;

        for (std::size_t i = 0; i < values.size(); ++i) {
            float t = 0.5f;
            if (values.size() > 1) {
                t = static_cast<float>(i) / static_cast<float>(values.size() - 1);
            }
            const float spawn_x = spawn_left + (spawn_right - spawn_left) * t;
            items.emplace_back(values[i], sf::Vector2f(spawn_x, spawn_y), current_radius, labelCharSize());
        }

        applyLayout(!items.empty());
        if (!items.empty()) {
            highlightIndex(0, NODE_ACTIVE_COLOR, 0.20f);
        }
        return true;
    }

    bool insert_head(int value) {
        return insert_index(0, value);
    }

    bool insert_tail(int value) {
        return insert_index(size(), value);
    }

    bool insert_index(int idx, int value) {
        if (idx < 0 || idx > size() || atCapacity()) return false;

        clearVisualState(true);
        refreshLayoutMetrics();

        items.insert(items.begin() + idx, ListVisualItem(value, spawnPositionForIndex(idx), current_radius, labelCharSize()));
        applyLayout(true);
        highlightIndex(idx, NODE_ACTIVE_COLOR, 0.25f);
        return true;
    }

    bool delete_head() {
        if (empty()) return false;
        return delete_index(0);
    }

    bool delete_tail() {
        if (empty()) return false;
        return delete_index(size() - 1);
    }

    bool delete_index(int idx) {
        if (idx < 0 || idx >= size()) return false;

        clearVisualState(true);
        pending_delete_index = idx;
        pending_delete_timer = 0.0f;
        highlightIndex(idx, NODE_DELETE_COLOR, 0.15f);
        return true;
    }

    int search_value(int value) {
        if (empty()) return -1;

        clearVisualState(false);
        scripted_search_value = value;
        scripted_found_index = -1;
        for (int i = 0; i < size(); ++i) {
            if (items[i].value == value) {
                scripted_found_index = i;
                break;
            }
        }

        scripted_indices.clear();
        int visit_count = (scripted_found_index >= 0) ? (scripted_found_index + 1) : size();
        for (int i = 0; i < visit_count; ++i) {
            scripted_indices.push_back(i);
        }

        script_mode = ScriptMode::Search;
        scripted_cursor = 0;
        scripted_timer = 0.0f;
        beginScriptStep();
        return scripted_found_index;
    }

    bool traversal() {
        if (empty()) return false;

        clearVisualState(false);
        scripted_indices.clear();
        for (int i = 0; i < size(); ++i) {
            scripted_indices.push_back(i);
        }

        scripted_search_value = -1;
        scripted_found_index = -1;
        script_mode = ScriptMode::Traversal;
        scripted_cursor = 0;
        scripted_timer = 0.0f;
        beginScriptStep();
        return true;
    }

    bool update_value(int idx, int value) {
        if (idx < 0 || idx >= size()) return false;

        clearVisualState(true);
        items[idx].value = value;
        items[idx].visual.setLabel(std::to_string(value), labelCharSize());
        highlightIndex(idx, NODE_FOUND_COLOR, 0.25f);
        return true;
    }

    void update(float dt) {
        for (auto& item : items) {
            item.visual.updatePosition(dt);
            item.visual.updateColor(dt);
        }

        updatePendingDelete(dt);
        updateScript(dt);
    }

    void render(sf::RenderWindow& window) const {
        if (items.empty()) {
            sf::Text empty_title(font_impact, "List is empty", 28);
            empty_title.setFillColor(sf::Color::Black);
            setCenterText(empty_title);
            empty_title.setPosition({
                canvas_bounds.position.x + canvas_bounds.size.x / 2.0f,
                canvas_bounds.position.y + canvas_bounds.size.y / 2.0f - 18.0f
            });
            window.draw(empty_title);

            sf::Text empty_hint(font_impact, "Use Create / Add, Insert Head, or Insert Tail to add the first node.", 18);
            empty_hint.setFillColor(MUTED_TEXT_COLOR);
            setCenterText(empty_hint);
            empty_hint.setPosition({
                canvas_bounds.position.x + canvas_bounds.size.x / 2.0f,
                canvas_bounds.position.y + canvas_bounds.size.y / 2.0f + 24.0f
            });
            window.draw(empty_hint);

            sf::Text null_text(font_impact, "HEAD -> NULL", 22);
            null_text.setFillColor(sf::Color::Black);
            setCenterText(null_text);
            null_text.setPosition({
                canvas_bounds.position.x + canvas_bounds.size.x / 2.0f,
                canvas_bounds.position.y + canvas_bounds.size.y / 2.0f + 74.0f
            });
            window.draw(null_text);
            return;
        }

        const float edge_thickness = compact_layout ? 2.0f : 3.0f;
        for (int i = 0; i + 1 < size(); ++i) {
            edge link(items[i].visual.currentPos.x, items[i].visual.currentPos.y,
                      items[i + 1].visual.currentPos.x, items[i + 1].visual.currentPos.y,
                      sf::Color(203, 203, 201), edge_thickness);
            link.setPoints(items[i].visual.currentPos.x, items[i].visual.currentPos.y,
                           items[i + 1].visual.currentPos.x, items[i + 1].visual.currentPos.y,
                           true, current_radius);
            link.draw(window);
        }

        for (const auto& item : items) {
            item.visual.draw(window);
        }

        const unsigned int marker_size = compact_layout ? 18U : 22U;
        sf::Text head_text(font_impact, "HEAD", marker_size);
        head_text.setFillColor(sf::Color::Black);
        head_text.setPosition({
            items.front().visual.currentPos.x - current_radius - 42.0f,
            items.front().visual.currentPos.y - current_radius - 28.0f
        });
        window.draw(head_text);

        sf::Text tail_text(font_impact, "TAIL", marker_size);
        tail_text.setFillColor(sf::Color::Black);
        tail_text.setPosition({
            items.back().visual.currentPos.x - current_radius,
            items.back().visual.currentPos.y + current_radius + 16.0f
        });
        window.draw(tail_text);

        sf::Text null_text(font_impact, "NULL", marker_size);
        null_text.setFillColor(sf::Color::Black);
        null_text.setPosition({
            items.back().visual.currentPos.x + current_radius + 24.0f,
            items.back().visual.currentPos.y - 12.0f
        });
        window.draw(null_text);
    }

private:
    std::vector<ListVisualItem> items;
    sf::FloatRect canvas_bounds;

    float current_radius{BASE_NODE_RADIUS};
    float current_spacing{BASE_SPACING};
    float list_center_y{0.0f};
    bool compact_layout{false};
    int max_visible_nodes{12};

    int pending_delete_index{-1};
    float pending_delete_timer{0.0f};

    ScriptMode script_mode{ScriptMode::None};
    std::vector<int> scripted_indices;
    int scripted_cursor{0};
    float scripted_timer{0.0f};
    int scripted_search_value{-1};
    int scripted_found_index{-1};

    std::optional<StatusMessage> pending_status;

    unsigned int labelCharSize() const {
        if (compact_layout || current_radius <= 22.0f) return 18;
        return 20;
    }

    void refreshLayoutMetrics() {
        const float usable_width = canvas_bounds.size.x - 2.0f * HORIZONTAL_PADDING;
        max_visible_nodes = std::max(1, static_cast<int>(1 + std::floor(usable_width / MIN_SPACING)));

        compact_layout = false;
        current_spacing = BASE_SPACING;
        if (items.size() > 1) {
            current_spacing = std::min(BASE_SPACING, usable_width / static_cast<float>(items.size() - 1));
            compact_layout = current_spacing < BASE_SPACING;
        }
        current_spacing = std::max(MIN_SPACING, current_spacing);

        const float compact_ratio = std::clamp(current_spacing / BASE_SPACING, MIN_NODE_RADIUS / BASE_NODE_RADIUS, 1.0f);
        current_radius = std::max(MIN_NODE_RADIUS, BASE_NODE_RADIUS * compact_ratio);
        list_center_y = canvas_bounds.position.y + canvas_bounds.size.y * 0.52f;
    }

    sf::Vector2f targetPositionForIndex(int idx) const {
        if (items.empty()) {
            return {
                canvas_bounds.position.x + canvas_bounds.size.x / 2.0f,
                canvas_bounds.position.y + canvas_bounds.size.y / 2.0f
            };
        }

        const float list_width = current_spacing * static_cast<float>(std::max(0, size() - 1));
        const float start_x = canvas_bounds.position.x + (canvas_bounds.size.x - list_width) / 2.0f;
        return {start_x + current_spacing * idx, list_center_y};
    }

    sf::Vector2f spawnPositionForIndex(int idx) const {
        const float center_y = canvas_bounds.position.y + canvas_bounds.size.y * 0.52f;

        if (idx <= 0) {
            return {canvas_bounds.position.x + 44.0f, center_y - 92.0f};
        }
        if (idx >= size()) {
            return {canvas_bounds.position.x + canvas_bounds.size.x - 44.0f, center_y - 92.0f};
        }
        return {targetPositionForIndex(idx).x, center_y - 96.0f};
    }

    void applyLayout(bool animate) {
        refreshLayoutMetrics();

        for (int i = 0; i < size(); ++i) {
            auto& item = items[i];
            item.visual.setRadius(current_radius);
            item.visual.setLabel(std::to_string(item.value), labelCharSize());
            item.visual.setColor(NODE_FILL_COLOR);

            const sf::Vector2f target = targetPositionForIndex(i);
            if (animate) {
                startNodeMovement(item.visual, target, LIST_ANIMATION_SECONDS);
            } else {
                stopMovementImmediate(item.visual, target);
            }
        }
    }

    void clearVisualState(bool animate_default_outline) {
        pending_delete_index = -1;
        pending_delete_timer = 0.0f;

        script_mode = ScriptMode::None;
        scripted_indices.clear();
        scripted_cursor = 0;
        scripted_timer = 0.0f;
        scripted_search_value = -1;
        scripted_found_index = -1;

        for (auto& item : items) {
            if (animate_default_outline) {
                startNodeColor(item.visual, NODE_OUTLINE_COLOR, COLOR_ANIMATION_SECONDS);
            } else {
                setOutlineImmediate(item.visual, NODE_OUTLINE_COLOR);
            }
        }
    }

    void highlightIndex(int idx, sf::Color color, float seconds) {
        if (idx < 0 || idx >= size()) return;
        startNodeColor(items[idx].visual, color, seconds);
    }

    void updatePendingDelete(float dt) {
        if (pending_delete_index < 0) return;

        pending_delete_timer += dt;
        if (pending_delete_timer < DELETE_DELAY_SECONDS) return;

        const int removed_index = pending_delete_index;
        pending_delete_index = -1;
        pending_delete_timer = 0.0f;

        items.erase(items.begin() + removed_index);
        applyLayout(true);

        if (!items.empty()) {
            const int focus_index = std::min(removed_index, size() - 1);
            highlightIndex(focus_index, NODE_ACTIVE_COLOR, 0.22f);
            pending_status = StatusMessage{StatusTone::Success, "Delete complete."};
        } else {
            pending_status = StatusMessage{StatusTone::Success, "Delete complete. The list is now empty."};
        }
    }

    void beginScriptStep() {
        if (scripted_indices.empty() || scripted_cursor < 0 || scripted_cursor >= static_cast<int>(scripted_indices.size())) {
            return;
        }

        for (auto& item : items) {
            setOutlineImmediate(item.visual, NODE_OUTLINE_COLOR);
        }

        const int index = scripted_indices[scripted_cursor];
        const sf::Color step_color = (script_mode == ScriptMode::Search && index == scripted_found_index)
            ? NODE_FOUND_COLOR
            : NODE_ACTIVE_COLOR;
        highlightIndex(index, step_color, 0.18f);
    }

    void finishScript() {
        if (script_mode == ScriptMode::Search) {
            for (auto& item : items) {
                setOutlineImmediate(item.visual, NODE_OUTLINE_COLOR);
            }

            if (scripted_found_index >= 0) {
                highlightIndex(scripted_found_index, NODE_FOUND_COLOR, 0.20f);
                pending_status = StatusMessage{
                    StatusTone::Success,
                    "Found " + std::to_string(scripted_search_value) + " at index " + std::to_string(scripted_found_index) + "."
                };
            } else {
                pending_status = StatusMessage{
                    StatusTone::Warning,
                    std::to_string(scripted_search_value) + " is not in the list."
                };
            }
        } else if (script_mode == ScriptMode::Traversal) {
            for (auto& item : items) {
                setOutlineImmediate(item.visual, NODE_OUTLINE_COLOR);
            }

            if (!items.empty()) {
                highlightIndex(size() - 1, NODE_ACTIVE_COLOR, 0.20f);
            }
            pending_status = StatusMessage{StatusTone::Success, "Traversal finished from HEAD to TAIL."};
        }

        script_mode = ScriptMode::None;
        scripted_indices.clear();
        scripted_cursor = 0;
        scripted_timer = 0.0f;
        scripted_search_value = -1;
        scripted_found_index = -1;
    }

    void updateScript(float dt) {
        if (script_mode == ScriptMode::None) return;
        if (scripted_indices.empty()) {
            finishScript();
            return;
        }

        scripted_timer += dt;
        if (scripted_timer < SCRIPT_STEP_SECONDS) return;

        scripted_timer = 0.0f;
        ++scripted_cursor;
        if (scripted_cursor >= static_cast<int>(scripted_indices.size())) {
            finishScript();
        } else {
            beginScriptStep();
        }
    }
};

} // namespace

void singly_linked_list_page() {
    sf::Texture background_texture;
    if (!loadTextureFromAsset(background_texture, "bg.png")) {
        std::cerr << "cannot load background" << std::endl;
    }
    sf::Sprite background_sprite(background_texture);

    RoundedRectangleShape visual_panel({975.f, 525.f});
    visual_panel.setPosition({380.f, 20.f});
    visual_panel.setFillColor(PANEL_FILL_COLOR);
    visual_panel.setRadius(17.0f);
    visual_panel.setOutlineThickness(5.0f);
    visual_panel.setOutlineColor(PANEL_OUTLINE_COLOR);

    RoundedRectangleShape status_panel({975.f, 120.f});
    status_panel.setPosition({380.f, 560.f});
    status_panel.setFillColor(STATUS_FILL_COLOR);
    status_panel.setRadius(17.0f);
    status_panel.setOutlineThickness(4.0f);
    status_panel.setOutlineColor(PANEL_OUTLINE_COLOR);

    const float title_x = 18.0f;
    const float input_x = 118.0f;
    const float input_width = 236.0f;
    const float input_height = 42.0f;
    const float button_left_x = 118.0f;
    const float button_width = 112.0f;
    const float button_height = 34.0f;
    const float button_gap = 8.0f;
    const float button_right_x = button_left_x + button_width + 10.0f;

    sf::Text panel_title(font_impact, "Singly Linked List", 22);
    panel_title.setFillColor(sf::Color::White);
    panel_title.setPosition({title_x, 30.0f});

    box value_box(input_x, 154.0f, input_width, input_height, sf::Color(138, 155, 192), "10", 22);
    box index_box(input_x, 226.0f, input_width, input_height, sf::Color(138, 155, 192), "0", 22);

    sf::Text value_label(font_impact, "Value (0-999)", 17);
    value_label.setFillColor(sf::Color::White);
    value_label.setPosition({input_x, 130.0f});

    sf::Text index_label(font_impact, "Index", 17);
    index_label.setFillColor(sf::Color::White);
    index_label.setPosition({input_x, 202.0f});

    float y = 336.0f;
    button random_btn(button_left_x, y, button_width, button_height, sf::Color(232, 183, 81), "Random", 16);
    button file_btn(button_right_x, y, button_width, button_height, sf::Color(232, 183, 81), "File", 16);
    y += button_height + button_gap;
    button create_btn(button_left_x, y, button_width, button_height, sf::Color(232, 183, 81), "Create / Add", 14);
    button search_btn(button_right_x, y, button_width, button_height, sf::Color(232, 183, 81), "Search", 16);
    y += button_height + button_gap;
    button ins_head_btn(button_left_x, y, button_width, button_height, sf::Color(232, 183, 81), "Insert Head", 14);
    button ins_tail_btn(button_right_x, y, button_width, button_height, sf::Color(232, 183, 81), "Insert Tail", 14);
    y += button_height + button_gap;
    button ins_idx_btn(button_left_x, y, button_width, button_height, sf::Color(232, 183, 81), "Insert Index", 14);
    button update_btn(button_right_x, y, button_width, button_height, sf::Color(232, 183, 81), "Update Value", 14);
    y += button_height + button_gap;
    button del_head_btn(button_left_x, y, button_width, button_height, sf::Color(232, 183, 81), "Delete Head", 14);
    button del_tail_btn(button_right_x, y, button_width, button_height, sf::Color(232, 183, 81), "Delete Tail", 14);
    y += button_height + button_gap;
    button del_idx_btn(button_left_x, y, button_width, button_height, sf::Color(232, 183, 81), "Delete Index", 14);
    button return_btn(button_right_x, y, button_width, button_height, sf::Color(232, 183, 81), "Return", 16);

    std::vector<button*> all_buttons = {
        &random_btn, &file_btn,
        &create_btn, &search_btn,
        &ins_head_btn, &ins_tail_btn, &ins_idx_btn, &update_btn,
        &del_head_btn, &del_tail_btn, &del_idx_btn, &return_btn
    };
    for (button* button_ptr : all_buttons) {
        button_ptr->setRadius(20.0f);
    }

    sf::Text status_title(font_impact, "Status", 20);
    status_title.setFillColor(sf::Color::Black);
    status_title.setPosition({
        status_panel.getPosition().x + 20.0f,
        status_panel.getPosition().y + 16.0f
    });

    sf::Text status_text(font_impact, "Ready", 20);
    status_text.setFillColor(sf::Color::Black);
    status_text.setPosition({
        status_panel.getPosition().x + 20.0f,
        status_panel.getPosition().y + 50.0f
    });

    sf::Text stats_text(font_impact, "", 16);
    stats_text.setFillColor(MUTED_TEXT_COLOR);
    stats_text.setPosition({
        status_panel.getPosition().x + 560.0f,
        status_panel.getPosition().y + 18.0f
    });

    sf::Text canvas_title(font_impact, "Visualization", 22);
    canvas_title.setFillColor(sf::Color::Black);
    canvas_title.setPosition({
        visual_panel.getPosition().x + 20.0f,
        visual_panel.getPosition().y + 16.0f
    });

    sf::Text canvas_hint(font_impact, "Nodes scale to fit the panel. Tail always points to NULL.", 15);
    canvas_hint.setFillColor(MUTED_TEXT_COLOR);
    canvas_hint.setPosition({
        visual_panel.getPosition().x + 20.0f,
        visual_panel.getPosition().y + 44.0f
    });

    std::string value_text = "10";
    std::string index_text = "0";

    bool value_active = true;
    bool index_active = false;

    StatusMessage page_status{StatusTone::Info, "Ready. Enter a value and choose an operation."};

    SinglyLinkedListVisualizer list_core(visual_panel.getGlobalBounds());
    sf::Clock clock;

    while (window.isOpen()) {
        const float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mouse_event = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse_event->button == sf::Mouse::Button::Left) {
                    const sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                    const bool value_clicked = value_box.contains(mouse_pos);
                    const bool index_clicked = index_box.contains(mouse_pos);

                    value_active = value_clicked;
                    index_active = !value_clicked && index_clicked;
                }
            }

            if (const auto* text_event = event->getIf<sf::Event::TextEntered>()) {
                if (value_active) append_digit(value_text, text_event->unicode, 3);
                if (index_active) append_digit(index_text, text_event->unicode, 2);
            }
        }

        list_core.update(dt);
        if (auto completed_status = list_core.consumePendingStatus()) {
            page_status = *completed_status;
        }

        const int input_value = parse_non_negative_int(value_text, 0);
        const int input_index = parse_non_negative_int(index_text, 0);
        const sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

        value_box.setLabel(value_text);
        index_box.setLabel(index_text);

        value_box.update(value_active, mouse_pos);
        value_box.setOutline(value_active ? sf::Color::Black : (value_box.contains(mouse_pos) ? sf::Color(90, 90, 90) : sf::Color::Transparent),
                             value_active || value_box.contains(mouse_pos) ? 2.0f : 0.0f);

        index_box.update(index_active, mouse_pos);
        index_box.setOutline(index_active ? sf::Color::Black : (index_box.contains(mouse_pos) ? sf::Color(90, 90, 90) : sf::Color::Transparent),
                             index_active || index_box.contains(mouse_pos) ? 2.0f : 0.0f);

        const bool random_clicked = random_btn.update(mouse_pos);
        const bool file_clicked = file_btn.update(mouse_pos);
        const bool create_clicked = create_btn.update(mouse_pos);
        const bool ins_head_clicked = ins_head_btn.update(mouse_pos);
        const bool ins_tail_clicked = ins_tail_btn.update(mouse_pos);
        const bool ins_idx_clicked = ins_idx_btn.update(mouse_pos);
        const bool del_head_clicked = del_head_btn.update(mouse_pos);
        const bool del_tail_clicked = del_tail_btn.update(mouse_pos);
        const bool del_idx_clicked = del_idx_btn.update(mouse_pos);
        const bool search_clicked = search_btn.update(mouse_pos);
        const bool update_clicked = update_btn.update(mouse_pos);
        const bool return_clicked = return_btn.update(mouse_pos);

        if (return_clicked) {
            return;
        }

        const bool action_clicked = random_clicked || file_clicked ||
                                    create_clicked || ins_head_clicked || ins_tail_clicked || ins_idx_clicked ||
                                    del_head_clicked || del_tail_clicked || del_idx_clicked ||
                                    search_clicked || update_clicked;

        if (action_clicked && list_core.isBusy()) {
            page_status = StatusMessage{StatusTone::Warning, "Wait for the current animation to finish."};
        } else if (random_clicked) {
            std::mt19937 rng(std::random_device{}());
            const int max_nodes = std::min(8, list_core.maxNodes());
            const int min_nodes = std::min(3, max_nodes);

            if (max_nodes <= 0) {
                page_status = StatusMessage{StatusTone::Error, "Cannot generate a list in the current canvas."};
            } else {
                std::uniform_int_distribution<int> size_dist(std::max(1, min_nodes), max_nodes);
                std::uniform_int_distribution<int> value_dist(0, 999);

                std::vector<int> values;
                const int count = size_dist(rng);
                values.reserve(count);
                for (int i = 0; i < count; ++i) {
                    values.push_back(value_dist(rng));
                }

                if (list_core.rebuild_from_values(values)) {
                    page_status = StatusMessage{
                        StatusTone::Success,
                        "Built a random list with " + std::to_string(count) + " nodes."
                    };
                } else {
                    page_status = StatusMessage{
                        StatusTone::Error,
                        "Random build is too large for this view. Maximum visible nodes: " + std::to_string(list_core.maxNodes()) + "."
                    };
                }
            }
        } else if (file_clicked) {
            const std::string path = openTextFileDialog();
            if (!path.empty()) {
                const std::vector<int> values = readListValuesFromFile(path);
                if (values.empty()) {
                    page_status = StatusMessage{
                        StatusTone::Error,
                        "Could not read list data. Use either: n followed by n values, or just a list of values (0-999)."
                    };
                } else if (!list_core.rebuild_from_values(values)) {
                    page_status = StatusMessage{
                        StatusTone::Error,
                        "File has too many nodes for this view. Maximum visible nodes: " + std::to_string(list_core.maxNodes()) + "."
                    };
                } else {
                    page_status = StatusMessage{
                        StatusTone::Success,
                        "Built list from file with " + std::to_string(values.size()) + " nodes."
                    };
                }
            }
        } else if (create_clicked) {
            if (list_core.atCapacity()) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "List is full for this view. Maximum visible nodes: " + std::to_string(list_core.maxNodes()) + "."
                };
            } else {
                list_core.create_node(input_value);
                page_status = StatusMessage{
                    StatusTone::Success,
                    "Added " + std::to_string(input_value) + " to the list."
                };
            }
        } else if (ins_head_clicked) {
            if (list_core.atCapacity()) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "List is full for this view. Maximum visible nodes: " + std::to_string(list_core.maxNodes()) + "."
                };
            } else {
                list_core.insert_head(input_value);
                page_status = StatusMessage{
                    StatusTone::Success,
                    "Inserted " + std::to_string(input_value) + " at HEAD."
                };
            }
        } else if (ins_tail_clicked) {
            if (list_core.atCapacity()) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "List is full for this view. Maximum visible nodes: " + std::to_string(list_core.maxNodes()) + "."
                };
            } else {
                list_core.insert_tail(input_value);
                page_status = StatusMessage{
                    StatusTone::Success,
                    "Inserted " + std::to_string(input_value) + " at TAIL."
                };
            }
        } else if (ins_idx_clicked) {
            if (list_core.atCapacity()) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "List is full for this view. Maximum visible nodes: " + std::to_string(list_core.maxNodes()) + "."
                };
            } else if (!list_core.insert_index(input_index, input_value)) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "Insert index out of range. " + list_core.insertRangeHint()
                };
            } else {
                page_status = StatusMessage{
                    StatusTone::Success,
                    "Inserted " + std::to_string(input_value) + " at index " + std::to_string(input_index) + "."
                };
            }
        } else if (del_head_clicked) {
            if (!list_core.delete_head()) {
                page_status = StatusMessage{StatusTone::Error, "Delete Head failed. The list is empty."};
            } else {
                page_status = StatusMessage{StatusTone::Info, "Deleting the HEAD node..."};
            }
        } else if (del_tail_clicked) {
            if (!list_core.delete_tail()) {
                page_status = StatusMessage{StatusTone::Error, "Delete Tail failed. The list is empty."};
            } else {
                page_status = StatusMessage{StatusTone::Info, "Deleting the TAIL node..."};
            }
        } else if (del_idx_clicked) {
            if (!list_core.delete_index(input_index)) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "Delete index out of range. " + list_core.accessRangeHint()
                };
            } else {
                page_status = StatusMessage{
                    StatusTone::Info,
                    "Deleting node at index " + std::to_string(input_index) + "..."
                };
            }
        } else if (search_clicked) {
            if (list_core.empty()) {
                page_status = StatusMessage{StatusTone::Error, "Search failed. The list is empty."};
            } else {
                list_core.search_value(input_value);
                page_status = StatusMessage{
                    StatusTone::Info,
                    "Searching for " + std::to_string(input_value) + " from HEAD to TAIL..."
                };
            }
        } else if (update_clicked) {
            if (!list_core.update_value(input_index, input_value)) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "Update index out of range. " + list_core.accessRangeHint()
                };
            } else {
                page_status = StatusMessage{
                    StatusTone::Success,
                    "Updated index " + std::to_string(input_index) + " to " + std::to_string(input_value) + "."
                };
            }
        }

        status_text.setString(page_status.text);
        status_text.setFillColor(statusTextColor(page_status.tone));

        std::string stats_label = "Nodes: " + std::to_string(list_core.size()) + " / " + std::to_string(list_core.maxNodes());
        if (list_core.layoutIsCompact()) {
            stats_label += "\nLayout compressed to keep the list readable.";
        } else {
            stats_label += "\nLayout is in normal spacing.";
        }
        stats_text.setString(stats_label);

        window.clear(sf::Color(212, 188, 112, 180));
        window.draw(background_sprite);

        window.draw(visual_panel);
        window.draw(status_panel);
        window.draw(canvas_title);
        window.draw(canvas_hint);
        window.draw(panel_title);
        window.draw(value_label);
        window.draw(index_label);
        window.draw(status_title);
        window.draw(status_text);
        window.draw(stats_text);

        for (button* button_ptr : all_buttons) {
            button_ptr->draw(window);
        }

        value_box.draw(window);
        index_box.draw(window);
        list_core.render(window);

        window.display();
    }
}
