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
const float BASE_ROW_SPACING = 118.0f;
const float MIN_ROW_SPACING = 84.0f;
const float HORIZONTAL_PADDING = 88.0f;
const float VERTICAL_PADDING = 88.0f;
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

enum class CodePanelMode {
    Idle,
    Build,
    AddTail,
    InsertHead,
    InsertIndex,
    DeleteHead,
    DeleteTail,
    Search,
    Traversal,
    Update
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

const std::string& codePanelText(CodePanelMode mode) {
    static const std::string idle_code =
        "// Click a button\n"
        "// to load its code";

    static const std::string build_code =
        "head = tail = nullptr;\n"
        "for (int x : values) {\n"
        "    insertTail(x);\n"
        "}\n"
        "return;";

    static const std::string add_tail_code =
        "Node* newNode = new Node(x);\n"
        "if (tail == nullptr) {\n"
        "    head = tail = newNode;\n"
        "} else {\n"
        "    tail->next = newNode;\n"
        "    tail = newNode;\n"
        "}";

    static const std::string insert_head_code =
        "Node* newNode = new Node(x);\n"
        "newNode->next = head;\n"
        "head = newNode;\n"
        "if (tail == nullptr) {\n"
        "    tail = newNode;\n"
        "}";

    static const std::string insert_index_code =
        "Node* cur = head;\n"
        "for (int i = 0; i < idx - 1; ++i) {\n"
        "    cur = cur->next;\n"
        "}\n"
        "Node* newNode = new Node(x);\n"
        "newNode->next = cur->next;\n"
        "cur->next = newNode;";

    static const std::string delete_head_code =
        "if (head == nullptr) return;\n"
        "Node* removed = head;\n"
        "head = head->next;\n"
        "if (head == nullptr) {\n"
        "    tail = nullptr;\n"
        "}\n"
        "delete removed;";

    static const std::string delete_tail_code =
        "if (head == nullptr) return;\n"
        "if (head == tail) {\n"
        "    delete head; head = tail = nullptr;\n"
        "} else {\n"
        "    Node* cur = head;\n"
        "    while (cur->next != tail) cur = cur->next;\n"
        "    delete tail; tail = cur; tail->next = nullptr;\n"
        "}";

    static const std::string search_code =
        "Node* cur = head;\n"
        "int idx = 0;\n"
        "while (cur != nullptr) {\n"
        "    if (cur->value == x) return idx;\n"
        "    cur = cur->next;\n"
        "    ++idx;\n"
        "}\n"
        "return -1;";

    static const std::string traversal_code =
        "Node* cur = head;\n"
        "while (cur != nullptr) {\n"
        "    visit(cur);\n"
        "    cur = cur->next;\n"
        "}\n"
        "return;";

    static const std::string update_code =
        "Node* cur = head;\n"
        "for (int i = 0; i < idx; ++i) {\n"
        "    cur = cur->next;\n"
        "}\n"
        "cur->value = x;";

    switch (mode) {
        case CodePanelMode::Build:
            return build_code;
        case CodePanelMode::AddTail:
            return add_tail_code;
        case CodePanelMode::InsertHead:
            return insert_head_code;
        case CodePanelMode::InsertIndex:
            return insert_index_code;
        case CodePanelMode::DeleteHead:
            return delete_head_code;
        case CodePanelMode::DeleteTail:
            return delete_tail_code;
        case CodePanelMode::Search:
            return search_code;
        case CodePanelMode::Traversal:
            return traversal_code;
        case CodePanelMode::Update:
            return update_code;
        case CodePanelMode::Idle:
        default:
            return idle_code;
    }
}

int activeCodeStep(CodePanelMode mode) {
    switch (mode) {
        case CodePanelMode::Build:
            return 2;
        case CodePanelMode::AddTail:
            return 4;
        case CodePanelMode::InsertHead:
            return 2;
        case CodePanelMode::InsertIndex:
            return 5;
        case CodePanelMode::DeleteHead:
            return 2;
        case CodePanelMode::DeleteTail:
            return 5;
        case CodePanelMode::Search:
            return 2;
        case CodePanelMode::Traversal:
            return 2;
        case CodePanelMode::Update:
            return 4;
        case CodePanelMode::Idle:
        default:
            return -1;
    }
}

int doneCodeStep(CodePanelMode mode, const StatusMessage& status) {
    switch (mode) {
        case CodePanelMode::Build:
            return 4;
        case CodePanelMode::AddTail:
            return 5;
        case CodePanelMode::InsertHead:
            return 4;
        case CodePanelMode::InsertIndex:
            return 6;
        case CodePanelMode::DeleteHead:
            return 6;
        case CodePanelMode::DeleteTail:
            return 6;
        case CodePanelMode::Search:
            return status.tone == StatusTone::Success ? 3 : 7;
        case CodePanelMode::Traversal:
            return 5;
        case CodePanelMode::Update:
            return 4;
        case CodePanelMode::Idle:
        default:
            return -1;
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

void append_list_input(std::string& s, char32_t unicode, std::size_t max_length) {
    if (unicode == '\b' || unicode == 8) {
        if (!s.empty()) s.pop_back();
        if (s.empty()) s = "0";
        return;
    }

    if (unicode >= '0' && unicode <= '9') {
        if (s == "0") s.clear();

        std::size_t token_length = 0;
        for (std::size_t i = s.size(); i > 0; --i) {
            const char c = s[i - 1];
            if (c == ',' || c == ' ') break;
            ++token_length;
        }

        if (token_length < 3 && s.size() < max_length) {
            s.push_back(static_cast<char>(unicode));
        }
    } else if ((unicode == ',' || unicode == ' ') && s.size() < max_length) {
        if (!s.empty() && s.back() != ',' && s.back() != ' ') {
            s.push_back(',');
        }
    }

    if (s.empty()) s = "0";
}

bool parse_build_values(const std::string& text, std::vector<int>& values) {
    values.clear();

    std::string token;
    auto flush_token = [&]() -> bool {
        if (token.empty()) return true;

        int value = 0;
        for (char c : token) {
            if (c < '0' || c > '9') return false;
            value = value * 10 + (c - '0');
            if (value > 999) return false;
        }

        values.push_back(value);
        token.clear();
        return true;
    };

    for (char c : text) {
        if (c == ',' || c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!flush_token()) return false;
        } else {
            token.push_back(c);
        }
    }

    if (!flush_token()) return false;
    return !values.empty();
}

std::string join_values(const std::vector<int>& values) {
    std::string result;
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) result += ',';
        result += std::to_string(values[i]);
    }
    return result.empty() ? "0" : result;
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
        if (pending_delete_index >= 0 || script_mode != ScriptMode::None || build_in_progress) {
            return true;
        }

        return hasVisualAnimation();
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

    bool start_build_from_values(const std::vector<int>& values) {
        refreshLayoutMetrics();
        if (values.empty() || values.size() > static_cast<std::size_t>(max_visible_nodes)) return false;

        clearVisualState(true);
        pending_status.reset();
        items.clear();
        refreshLayoutMetrics();
        build_values = values;
        build_cursor = 0;
        build_in_progress = true;
        appendBuildStep();
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
        updateBuild();
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
    float current_row_spacing{BASE_ROW_SPACING};
    float list_start_x{0.0f};
    float list_start_y{0.0f};
    float list_center_y{0.0f};
    bool compact_layout{false};
    int max_visible_nodes{12};
    int columns_per_row{1};
    int rows_used{1};

    int pending_delete_index{-1};
    float pending_delete_timer{0.0f};

    ScriptMode script_mode{ScriptMode::None};
    std::vector<int> scripted_indices;
    int scripted_cursor{0};
    float scripted_timer{0.0f};
    int scripted_search_value{-1};
    int scripted_found_index{-1};
    bool build_in_progress{false};
    std::vector<int> build_values;
    std::size_t build_cursor{0};

    std::optional<StatusMessage> pending_status;

    unsigned int labelCharSize() const {
        if (compact_layout || current_radius <= 22.0f) return 18;
        return 20;
    }

    bool hasVisualAnimation() const {
        for (const auto& item : items) {
            if (item.visual.isMoving || item.visual.isColoring) {
                return true;
            }
        }

        return false;
    }

    int computeRowsForCount(int count) const {
        if (columns_per_row <= 0) return 1;
        return std::max(1, (count + columns_per_row - 1) / columns_per_row);
    }

    sf::Vector2f layoutPositionForIndex(int idx, int total_count) const {
        const int projected_rows = computeRowsForCount(std::max(1, total_count));
        const float usable_height = canvas_bounds.size.y - 2.0f * VERTICAL_PADDING;

        float projected_row_spacing = 0.0f;
        if (projected_rows > 1) {
            projected_row_spacing = std::min(BASE_ROW_SPACING, usable_height / static_cast<float>(projected_rows - 1));
            projected_row_spacing = std::max(MIN_ROW_SPACING, projected_row_spacing);
        }

        float projected_start_y = canvas_bounds.position.y + canvas_bounds.size.y * 0.52f;
        if (projected_rows > 1) {
            const float layout_height = projected_row_spacing * static_cast<float>(projected_rows - 1);
            projected_start_y = canvas_bounds.position.y + (canvas_bounds.size.y - layout_height) / 2.0f;
        }

        const int row = idx / columns_per_row;
        const int position_in_row = idx % columns_per_row;
        const bool left_to_right = (row % 2 == 0);

        float x = canvas_bounds.position.x + canvas_bounds.size.x / 2.0f;
        if (columns_per_row > 1) {
            const int column = left_to_right ? position_in_row : (columns_per_row - 1 - position_in_row);
            x = list_start_x + current_spacing * static_cast<float>(column);
        }

        const float y = (projected_rows == 1)
            ? list_center_y
            : (projected_start_y + projected_row_spacing * static_cast<float>(row));

        return {x, y};
    }

    void refreshLayoutMetrics() {
        const float usable_width = canvas_bounds.size.x - 2.0f * HORIZONTAL_PADDING;
        const float usable_height = canvas_bounds.size.y - 2.0f * VERTICAL_PADDING;

        columns_per_row = std::max(1, static_cast<int>(1 + std::floor(usable_width / MIN_SPACING)));
        const int max_rows = std::max(1, static_cast<int>(1 + std::floor(usable_height / MIN_ROW_SPACING)));
        max_visible_nodes = std::max(1, columns_per_row * max_rows);

        rows_used = computeRowsForCount(size());
        compact_layout = (size() > columns_per_row) || (columns_per_row > 1 && usable_width / static_cast<float>(columns_per_row - 1) < BASE_SPACING);

        current_spacing = 0.0f;
        if (columns_per_row > 1) {
            current_spacing = std::min(BASE_SPACING, usable_width / static_cast<float>(columns_per_row - 1));
            current_spacing = std::max(MIN_SPACING, current_spacing);
        }

        current_row_spacing = 0.0f;
        if (rows_used > 1) {
            current_row_spacing = std::min(BASE_ROW_SPACING, usable_height / static_cast<float>(rows_used - 1));
            current_row_spacing = std::max(MIN_ROW_SPACING, current_row_spacing);
        }

        float compact_ratio = 1.0f;
        if (columns_per_row > 1) {
            compact_ratio = std::min(compact_ratio, current_spacing / BASE_SPACING);
        }
        if (rows_used > 1) {
            compact_ratio = std::min(compact_ratio, current_row_spacing / BASE_ROW_SPACING);
        }
        compact_ratio = std::clamp(compact_ratio, MIN_NODE_RADIUS / BASE_NODE_RADIUS, 1.0f);

        current_radius = std::max(MIN_NODE_RADIUS, BASE_NODE_RADIUS * compact_ratio);
        list_start_x = canvas_bounds.position.x + HORIZONTAL_PADDING;

        const float layout_height = current_row_spacing * static_cast<float>(std::max(0, rows_used - 1));
        if (rows_used == 1) {
            list_start_y = canvas_bounds.position.y + canvas_bounds.size.y * 0.52f;
        } else {
            list_start_y = canvas_bounds.position.y + (canvas_bounds.size.y - layout_height) / 2.0f;
        }
        list_center_y = canvas_bounds.position.y + canvas_bounds.size.y * 0.52f;
    }

    sf::Vector2f targetPositionForIndex(int idx) const {
        if (items.empty()) {
            return {
                canvas_bounds.position.x + canvas_bounds.size.x / 2.0f,
                canvas_bounds.position.y + canvas_bounds.size.y / 2.0f
            };
        }

        return layoutPositionForIndex(idx, size());
    }

    sf::Vector2f spawnPositionForIndex(int idx) const {
        const int projected_count = std::max(size() + 1, idx + 1);
        sf::Vector2f target = layoutPositionForIndex(std::max(0, idx), projected_count);
        target.y -= 96.0f;
        return target;
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

    void appendBuildStep() {
        if (!build_in_progress || build_cursor >= build_values.size()) {
            return;
        }

        const int value = build_values[build_cursor];
        insert_tail(value);
        ++build_cursor;

        pending_status = StatusMessage{
            StatusTone::Info,
            "Build step " + std::to_string(build_cursor) + "/" + std::to_string(build_values.size()) +
            ": added " + std::to_string(value) + "."
        };
    }

    void updateBuild() {
        if (!build_in_progress || hasVisualAnimation()) {
            return;
        }

        if (build_cursor < build_values.size()) {
            appendBuildStep();
            return;
        }

        pending_status = StatusMessage{
            StatusTone::Success,
            "Build complete. Created " + std::to_string(items.size()) + " nodes."
        };
        build_in_progress = false;
        build_values.clear();
        build_cursor = 0;
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
    if (!loadTextureFromAsset(background_texture, "bg_toty.png")) {
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

    const float control_left = 14.0f;
    const float control_gap = 10.0f;
    const float button_width = 104.0f;
    const float double_width = 161.0f;
    const float control_height = 44.0f;
    const float box_height = 48.0f;
    const float build_row_y = WINDOW_HEIGHT - 348.0f;
    const float build_box_y = build_row_y + control_height + control_gap;
    const float input_row_y = build_box_y + box_height + control_gap;
    const float action_row_1_y = input_row_y + box_height + control_gap;
    const float action_row_2_y = action_row_1_y + control_height + control_gap;
    const float action_row_3_y = action_row_2_y + control_height + control_gap;

    const float col_1_x = control_left;
    const float col_2_x = control_left + button_width + control_gap;
    const float col_3_x = control_left + 2.0f * (button_width + control_gap);
    const float wide_col_2_x = control_left + double_width + control_gap;
    button return_btn(18.f, 18.f, 104.f, 52.f, sf::Color(232, 183, 81), "RETURN", 24);
    button random_btn(col_1_x, build_row_y, button_width, control_height, sf::Color(232, 183, 81), "RANDOM", 18);
    button file_btn(col_2_x, build_row_y, button_width, control_height, sf::Color(232, 183, 81), "FILE", 18);
    button build_btn(col_3_x, build_row_y, button_width, control_height, sf::Color(232, 183, 81), "BUILD", 18);

    box build_box(control_left, build_box_y, 332.f, box_height, sf::Color(138, 155, 192), "0", 22);
    box value_box(control_left, input_row_y, double_width, box_height, sf::Color(138, 155, 192), "0", 22);
    box index_box(wide_col_2_x, input_row_y, double_width, box_height, sf::Color(138, 155, 192), "0", 22);

    button add_btn(col_1_x, action_row_1_y, button_width, control_height, sf::Color(232, 183, 81), "ADD", 20);
    button search_btn(col_2_x, action_row_1_y, button_width, control_height, sf::Color(232, 183, 81), "SEARCH", 18);
    button traversal_btn(col_3_x, action_row_1_y, button_width, control_height, sf::Color(232, 183, 81), "TRAVERSE", 15);

    button ins_head_btn(col_1_x, action_row_2_y, button_width, control_height, sf::Color(232, 183, 81), "INS HEAD", 16);
    button ins_index_btn(col_2_x, action_row_2_y, button_width, control_height, sf::Color(232, 183, 81), "INS IDX", 16);
    button update_btn(col_3_x, action_row_2_y, button_width, control_height, sf::Color(232, 183, 81), "UPDATE", 18);

    button del_head_btn(control_left, action_row_3_y, double_width, control_height, sf::Color(232, 183, 81), "DEL HEAD", 18);
    button del_tail_btn(wide_col_2_x, action_row_3_y, double_width, control_height, sf::Color(232, 183, 81), "DEL TAIL", 18);

    std::vector<button*> all_buttons = {
        &return_btn, &random_btn, &file_btn, &build_btn,
        &add_btn, &search_btn, &traversal_btn,
        &ins_head_btn, &ins_index_btn, &update_btn,
        &del_head_btn, &del_tail_btn
    };
    for (button* button_ptr : all_buttons) {
        button_ptr->setRadius(10.0f);
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
        status_panel.getPosition().x + 20.0f,
        status_panel.getPosition().y + 82.0f
    });

    std::string build_text = "0";
    std::string value_text = "0";
    std::string index_text = "0";

    bool build_active = false;
    bool value_active = true;
    bool index_active = false;

    StatusMessage page_status{StatusTone::Info, "Ready. Enter a value and choose an operation."};
    CodePanelMode code_mode = CodePanelMode::Idle;
    bool code_waiting_for_completion = false;

    SinglyLinkedListVisualizer list_core(visual_panel.getGlobalBounds());
    CodeBox code_box({435.f, 250.f}, font_impact, 19);
    code_box.setOrigin({435.f, 250.f});
    code_box.setPosition({float(WINDOW_WIDTH) - 10.0f, float(WINDOW_HEIGHT) - 10.0f});
    code_box.setCode(codePanelText(code_mode));
    code_box.setStep(-1);

    const auto set_code_panel = [&](CodePanelMode mode, int step, bool wait_for_completion) {
        code_mode = mode;
        code_waiting_for_completion = wait_for_completion;
        code_box.setCode(codePanelText(mode));
        code_box.setStep(step);
    };

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
                    const bool build_clicked = build_box.contains(mouse_pos);
                    const bool value_clicked = value_box.contains(mouse_pos);
                    const bool index_clicked = index_box.contains(mouse_pos);

                    build_active = build_clicked;
                    value_active = !build_clicked && value_clicked;
                    index_active = !build_clicked && !value_clicked && index_clicked;
                }
            }

            if (const auto* text_event = event->getIf<sf::Event::TextEntered>()) {
                if (build_active) append_list_input(build_text, text_event->unicode, 64);
                if (value_active) append_digit(value_text, text_event->unicode, 3);
                if (index_active) append_digit(index_text, text_event->unicode, 2);
            }
        }

        list_core.update(dt);
        if (auto completed_status = list_core.consumePendingStatus()) {
            page_status = *completed_status;
        }

        if (code_waiting_for_completion) {
            if (list_core.isBusy()) {
                code_box.setStep(activeCodeStep(code_mode));
            } else {
                code_box.setStep(doneCodeStep(code_mode, page_status));
                code_waiting_for_completion = false;
            }
        }

        const int input_value = parse_non_negative_int(value_text, 0);
        const int input_index = parse_non_negative_int(index_text, 0);
        const sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

        std::string build_label = build_text;
        if (build_label.empty()) build_label = "0";
        if (build_label.size() > 21) {
            build_label = build_label.substr(build_label.size() - 21);
        }
        if (build_active) build_label += '|';
        build_box.setLabel(build_label);

        std::string value_label = "val:" + value_text;
        if (value_active) value_label += '|';
        value_box.setLabel(value_label);

        std::string index_label = "idx:" + index_text;
        if (index_active) index_label += '|';
        index_box.setLabel(index_label);

        build_box.update(build_active, mouse_pos);
        build_box.setOutline(build_active ? sf::Color::Black : (build_box.contains(mouse_pos) ? sf::Color(90, 90, 90) : sf::Color::Transparent),
                             build_active || build_box.contains(mouse_pos) ? 2.0f : 0.0f);

        value_box.update(value_active, mouse_pos);
        value_box.setOutline(value_active ? sf::Color::Black : (value_box.contains(mouse_pos) ? sf::Color(90, 90, 90) : sf::Color::Transparent),
                             value_active || value_box.contains(mouse_pos) ? 2.0f : 0.0f);

        index_box.update(index_active, mouse_pos);
        index_box.setOutline(index_active ? sf::Color::Black : (index_box.contains(mouse_pos) ? sf::Color(90, 90, 90) : sf::Color::Transparent),
                             index_active || index_box.contains(mouse_pos) ? 2.0f : 0.0f);

        const bool random_clicked = random_btn.update(mouse_pos);
        const bool file_clicked = file_btn.update(mouse_pos);
        const bool build_clicked = build_btn.update(mouse_pos);
        const bool create_clicked = add_btn.update(mouse_pos);
        const bool ins_head_clicked = ins_head_btn.update(mouse_pos);
        const bool ins_index_clicked = ins_index_btn.update(mouse_pos);
        const bool del_head_clicked = del_head_btn.update(mouse_pos);
        const bool del_tail_clicked = del_tail_btn.update(mouse_pos);
        const bool search_clicked = search_btn.update(mouse_pos);
        const bool traversal_clicked = traversal_btn.update(mouse_pos);
        const bool update_clicked = update_btn.update(mouse_pos);
        const bool return_clicked = return_btn.update(mouse_pos);

        if (return_clicked) {
            return;
        }

        const bool action_clicked = random_clicked || file_clicked || build_clicked ||
                                    create_clicked || ins_head_clicked || ins_index_clicked ||
                                    del_head_clicked || del_tail_clicked ||
                                    search_clicked || traversal_clicked || update_clicked;

        if (action_clicked && list_core.isBusy() && !random_clicked && !file_clicked) {
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

                build_text = join_values(values);
                build_active = true;
                value_active = false;
                index_active = false;
                page_status = StatusMessage{
                    StatusTone::Info,
                    "Random data generated. Press BUILD to create a list with " + std::to_string(count) + " nodes."
                };
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
                } else {
                    build_text = join_values(values);
                    build_active = true;
                    value_active = false;
                    index_active = false;
                    page_status = StatusMessage{
                        StatusTone::Info,
                        "Loaded " + std::to_string(values.size()) + " values from file. Press BUILD to create the list."
                    };
                }
            }
        } else if (build_clicked) {
            set_code_panel(CodePanelMode::Build, 0, false);
            std::vector<int> values;
            if (!parse_build_values(build_text, values)) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "Build input is invalid. Use comma-separated values between 0 and 999."
                };
            } else if (!list_core.start_build_from_values(values)) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "Build input has too many nodes for this view. Maximum visible nodes: " + std::to_string(list_core.maxNodes()) + "."
                };
            } else {
                build_text = join_values(values);
                set_code_panel(CodePanelMode::Build, activeCodeStep(CodePanelMode::Build), true);
                page_status = StatusMessage{
                    StatusTone::Info,
                    "Build step 1/" + std::to_string(values.size()) + ": added " + std::to_string(values.front()) + "."
                };
            }
        } else if (create_clicked) {
            set_code_panel(CodePanelMode::AddTail, 0, false);
            if (list_core.atCapacity()) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "List is full for this view. Maximum visible nodes: " + std::to_string(list_core.maxNodes()) + "."
                };
            } else {
                list_core.create_node(input_value);
                set_code_panel(CodePanelMode::AddTail, activeCodeStep(CodePanelMode::AddTail), true);
                page_status = StatusMessage{
                    StatusTone::Success,
                    "Added " + std::to_string(input_value) + " to the list."
                };
            }
        } else if (ins_head_clicked) {
            set_code_panel(CodePanelMode::InsertHead, 0, false);
            if (list_core.atCapacity()) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "List is full for this view. Maximum visible nodes: " + std::to_string(list_core.maxNodes()) + "."
                };
            } else {
                list_core.insert_head(input_value);
                set_code_panel(CodePanelMode::InsertHead, activeCodeStep(CodePanelMode::InsertHead), true);
                page_status = StatusMessage{
                    StatusTone::Success,
                    "Inserted " + std::to_string(input_value) + " at HEAD."
                };
            }
        } else if (ins_index_clicked) {
            set_code_panel(CodePanelMode::InsertIndex, 0, false);
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
                set_code_panel(CodePanelMode::InsertIndex, activeCodeStep(CodePanelMode::InsertIndex), true);
                page_status = StatusMessage{
                    StatusTone::Success,
                    "Inserted " + std::to_string(input_value) + " at index " + std::to_string(input_index) + "."
                };
            }
        } else if (del_head_clicked) {
            set_code_panel(CodePanelMode::DeleteHead, 0, false);
            if (!list_core.delete_head()) {
                page_status = StatusMessage{StatusTone::Error, "Delete Head failed. The list is empty."};
            } else {
                set_code_panel(CodePanelMode::DeleteHead, activeCodeStep(CodePanelMode::DeleteHead), true);
                page_status = StatusMessage{StatusTone::Info, "Deleting the HEAD node..."};
            }
        } else if (del_tail_clicked) {
            set_code_panel(CodePanelMode::DeleteTail, 0, false);
            if (!list_core.delete_tail()) {
                page_status = StatusMessage{StatusTone::Error, "Delete Tail failed. The list is empty."};
            } else {
                set_code_panel(CodePanelMode::DeleteTail, activeCodeStep(CodePanelMode::DeleteTail), true);
                page_status = StatusMessage{StatusTone::Info, "Deleting the TAIL node..."};
            }
        } else if (search_clicked) {
            set_code_panel(CodePanelMode::Search, 0, false);
            if (list_core.empty()) {
                page_status = StatusMessage{StatusTone::Error, "Search failed. The list is empty."};
            } else {
                list_core.search_value(input_value);
                set_code_panel(CodePanelMode::Search, activeCodeStep(CodePanelMode::Search), true);
                page_status = StatusMessage{
                    StatusTone::Info,
                    "Searching for " + std::to_string(input_value) + " from HEAD to TAIL..."
                };
            }
        } else if (traversal_clicked) {
            set_code_panel(CodePanelMode::Traversal, 0, false);
            if (!list_core.traversal()) {
                page_status = StatusMessage{StatusTone::Error, "Traversal failed. The list is empty."};
            } else {
                set_code_panel(CodePanelMode::Traversal, activeCodeStep(CodePanelMode::Traversal), true);
                page_status = StatusMessage{StatusTone::Info, "Traversing from HEAD to TAIL..."};
            }
        } else if (update_clicked) {
            set_code_panel(CodePanelMode::Update, 0, false);
            if (!list_core.update_value(input_index, input_value)) {
                page_status = StatusMessage{
                    StatusTone::Error,
                    "Update index out of range. " + list_core.accessRangeHint()
                };
            } else {
                set_code_panel(CodePanelMode::Update, activeCodeStep(CodePanelMode::Update), true);
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
        window.draw(status_title);
        window.draw(status_text);
        window.draw(stats_text);

        for (button* button_ptr : all_buttons) {
            button_ptr->draw(window);
        }

        build_box.draw(window);
        value_box.draw(window);
        index_box.draw(window);
        list_core.render(window);
        window.draw(code_box);

        window.display();
    }
}
