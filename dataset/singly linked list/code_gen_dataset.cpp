#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

struct Config {
    fs::path output_root;
    std::uint32_t seed{123456789u};
    int build_case_count{18};
    int single_case_count{18};
    int mixed_case_count{10};
    int max_list_size{12};
    int max_value{999};
};

enum class OperationType {
    AddTail,
    InsertHead,
    InsertIndex,
    DeleteHead,
    DeleteTail,
    Search,
    Traverse,
    Update
};

struct Operation {
    OperationType type{OperationType::AddTail};
    int value{-1};
    int index{-1};
};

struct Scenario {
    std::string id;
    std::string category;
    std::vector<int> initial_values;
    std::vector<Operation> operations;
};

struct ManifestRow {
    std::string id;
    std::string category;
    std::string input_file;
    std::string spec_file;
    std::string initial_values;
    std::string operations;
    std::string expected_final;
};

int random_int(std::mt19937& rng, int low, int high) {
    std::uniform_int_distribution<int> dist(low, high);
    return dist(rng);
}

int clamped_random_size(std::mt19937& rng, int low, int high, int hard_max) {
    const int safe_high = std::max(1, std::min(high, hard_max));
    const int safe_low = std::min(std::max(1, low), safe_high);
    return random_int(rng, safe_low, safe_high);
}

fs::path default_output_root() {
    const fs::path cwd = fs::current_path();
    const fs::path repo_style = cwd / "dataset" / "singly linked list";

    if (fs::exists(repo_style)) {
        return repo_style / "generated";
    }

    if (cwd.filename() == "singly linked list" && cwd.parent_path().filename() == "dataset") {
        return cwd / "generated";
    }

    return cwd / "generated";
}

bool parse_positive_int(const std::string& text, int& value) {
    if (text.empty()) return false;

    int result = 0;
    for (char c : text) {
        if (c < '0' || c > '9') {
            return false;
        }
        result = result * 10 + (c - '0');
    }

    if (result <= 0) return false;
    value = result;
    return true;
}

bool parse_u32(const std::string& text, std::uint32_t& value) {
    if (text.empty()) return false;

    std::uint64_t result = 0;
    for (char c : text) {
        if (c < '0' || c > '9') {
            return false;
        }
        result = result * 10 + static_cast<std::uint64_t>(c - '0');
        if (result > 0xffffffffULL) {
            return false;
        }
    }

    value = static_cast<std::uint32_t>(result);
    return true;
}

void print_usage(const char* exe_name) {
    std::cout
        << "Usage: " << exe_name << " [options]\n"
        << "Options:\n"
        << "  --out <path>           Output folder (default: dataset/singly linked list/generated)\n"
        << "  --seed <number>        Random seed\n"
        << "  --build-count <n>      Number of build-only input files\n"
        << "  --single-count <n>     Number of single-operation scenarios\n"
        << "  --mixed-count <n>      Number of mixed-operation scenarios\n"
        << "  --max-size <n>         Maximum list size (default: 12)\n"
        << "  --max-value <n>        Maximum node value (default: 999)\n"
        << "  --help                 Show this message\n";
}

bool parse_args(int argc, char** argv, Config& config) {
    config.output_root = default_output_root();

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        auto read_value = [&](std::string& out) -> bool {
            if (i + 1 >= argc) return false;
            out = argv[++i];
            return true;
        };

        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return false;
        }

        std::string value_text;
        if (!read_value(value_text)) {
            std::cerr << "Missing value for option: " << arg << '\n';
            return false;
        }

        if (arg == "--out") {
            config.output_root = fs::path(value_text);
        } else if (arg == "--seed") {
            if (!parse_u32(value_text, config.seed)) {
                std::cerr << "Invalid seed: " << value_text << '\n';
                return false;
            }
        } else if (arg == "--build-count") {
            if (!parse_positive_int(value_text, config.build_case_count)) {
                std::cerr << "Invalid build count: " << value_text << '\n';
                return false;
            }
        } else if (arg == "--single-count") {
            if (!parse_positive_int(value_text, config.single_case_count)) {
                std::cerr << "Invalid single-operation count: " << value_text << '\n';
                return false;
            }
        } else if (arg == "--mixed-count") {
            if (!parse_positive_int(value_text, config.mixed_case_count)) {
                std::cerr << "Invalid mixed-operation count: " << value_text << '\n';
                return false;
            }
        } else if (arg == "--max-size") {
            if (!parse_positive_int(value_text, config.max_list_size)) {
                std::cerr << "Invalid max size: " << value_text << '\n';
                return false;
            }
        } else if (arg == "--max-value") {
            if (!parse_positive_int(value_text, config.max_value)) {
                std::cerr << "Invalid max value: " << value_text << '\n';
                return false;
            }
        } else {
            std::cerr << "Unknown option: " << arg << '\n';
            return false;
        }
    }

    return true;
}

std::string join_values(const std::vector<int>& values, const std::string& separator = ",") {
    if (values.empty()) return "EMPTY";

    std::ostringstream out;
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) out << separator;
        out << values[i];
    }
    return out.str();
}

std::string csv_escape(const std::string& value) {
    bool needs_quotes = false;
    std::string escaped;
    escaped.reserve(value.size() + 8);

    for (char c : value) {
        if (c == '"' || c == ',' || c == '\n' || c == '\r') {
            needs_quotes = true;
        }

        if (c == '"') {
            escaped += "\"\"";
        } else {
            escaped += c;
        }
    }

    if (!needs_quotes) {
        return escaped;
    }

    return "\"" + escaped + "\"";
}

void ensure_parent_directory(const fs::path& file_path) {
    const fs::path parent = file_path.parent_path();
    if (!parent.empty()) {
        fs::create_directories(parent);
    }
}

void write_text_file(const fs::path& file_path, const std::vector<std::string>& lines) {
    ensure_parent_directory(file_path);
    std::ofstream out(file_path);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot write file: " + file_path.string());
    }

    for (const std::string& line : lines) {
        out << line << '\n';
    }
}

void write_visualizer_input(const fs::path& file_path, const std::vector<int>& values) {
    ensure_parent_directory(file_path);
    std::ofstream out(file_path);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot write file: " + file_path.string());
    }

    out << values.size() << '\n';
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) out << ' ';
        out << values[i];
    }
    out << '\n';
}

std::string operation_name(OperationType type) {
    switch (type) {
        case OperationType::AddTail: return "add_tail";
        case OperationType::InsertHead: return "insert_head";
        case OperationType::InsertIndex: return "insert_index";
        case OperationType::DeleteHead: return "delete_head";
        case OperationType::DeleteTail: return "delete_tail";
        case OperationType::Search: return "search";
        case OperationType::Traverse: return "traverse";
        case OperationType::Update: return "update";
        default: return "unknown";
    }
}

std::string format_operation(const Operation& operation) {
    std::ostringstream out;
    out << operation_name(operation.type);

    switch (operation.type) {
        case OperationType::AddTail:
        case OperationType::InsertHead:
        case OperationType::Search:
            out << " value=" << operation.value;
            break;
        case OperationType::InsertIndex:
            out << " value=" << operation.value << " index=" << operation.index;
            break;
        case OperationType::Update:
            out << " index=" << operation.index << " value=" << operation.value;
            break;
        case OperationType::DeleteHead:
        case OperationType::DeleteTail:
        case OperationType::Traverse:
        default:
            break;
    }

    return out.str();
}

std::vector<int> random_values(std::mt19937& rng, int size, int max_value) {
    std::vector<int> values;
    values.reserve(static_cast<std::size_t>(std::max(0, size)));

    for (int i = 0; i < size; ++i) {
        values.push_back(random_int(rng, 0, max_value));
    }
    return values;
}

int pick_missing_value(const std::vector<int>& values, std::mt19937& rng, int max_value) {
    for (int attempt = 0; attempt < 32; ++attempt) {
        const int candidate = random_int(rng, 0, max_value);
        if (std::find(values.begin(), values.end(), candidate) == values.end()) {
            return candidate;
        }
    }

    for (int candidate = 0; candidate <= max_value; ++candidate) {
        if (std::find(values.begin(), values.end(), candidate) == values.end()) {
            return candidate;
        }
    }

    return max_value + 1;
}

std::string apply_operation(std::vector<int>& values, const Operation& operation) {
    std::ostringstream out;

    switch (operation.type) {
        case OperationType::AddTail:
            values.push_back(operation.value);
            out << "status=success added=" << operation.value;
            break;

        case OperationType::InsertHead:
            values.insert(values.begin(), operation.value);
            out << "status=success inserted_head=" << operation.value;
            break;

        case OperationType::InsertIndex:
            if (0 <= operation.index && operation.index <= static_cast<int>(values.size())) {
                values.insert(values.begin() + operation.index, operation.value);
                out << "status=success inserted=" << operation.value << " index=" << operation.index;
            } else {
                out << "status=invalid reason=index_out_of_range";
            }
            break;

        case OperationType::DeleteHead:
            if (!values.empty()) {
                const int removed = values.front();
                values.erase(values.begin());
                out << "status=success deleted_head=" << removed;
            } else {
                out << "status=invalid reason=list_empty";
            }
            break;

        case OperationType::DeleteTail:
            if (!values.empty()) {
                const int removed = values.back();
                values.pop_back();
                out << "status=success deleted_tail=" << removed;
            } else {
                out << "status=invalid reason=list_empty";
            }
            break;

        case OperationType::Search: {
            int found_index = -1;
            for (int i = 0; i < static_cast<int>(values.size()); ++i) {
                if (values[i] == operation.value) {
                    found_index = i;
                    break;
                }
            }
            out << "status=success found_index=" << found_index;
            break;
        }

        case OperationType::Traverse:
            out << "status=success order=" << join_values(values);
            break;

        case OperationType::Update:
            if (0 <= operation.index && operation.index < static_cast<int>(values.size())) {
                const int old_value = values[operation.index];
                values[operation.index] = operation.value;
                out << "status=success updated_index=" << operation.index
                    << " old=" << old_value
                    << " new=" << operation.value;
            } else {
                out << "status=invalid reason=index_out_of_range";
            }
            break;
    }

    return out.str();
}

std::string next_id(const std::string& prefix, int index) {
    std::ostringstream out;
    out << prefix;
    if (index < 10) out << "00";
    else if (index < 100) out << '0';
    out << index;
    return out.str();
}

Operation make_single_operation_case(int slot, const std::vector<int>& initial_values, std::mt19937& rng, int max_value) {
    const int selector = slot % 8;
    const int size = static_cast<int>(initial_values.size());

    switch (selector) {
        case 0:
            return {OperationType::AddTail, random_int(rng, 0, max_value), -1};
        case 1:
            return {OperationType::InsertHead, random_int(rng, 0, max_value), -1};
        case 2:
            return {OperationType::InsertIndex, random_int(rng, 0, max_value), random_int(rng, 0, size)};
        case 3:
            return {OperationType::DeleteHead, -1, -1};
        case 4:
            return {OperationType::DeleteTail, -1, -1};
        case 5: {
            const bool want_hit = !initial_values.empty() && random_int(rng, 0, 1) == 0;
            const int value = want_hit
                ? initial_values[random_int(rng, 0, static_cast<int>(initial_values.size()) - 1)]
                : pick_missing_value(initial_values, rng, max_value);
            return {OperationType::Search, value, -1};
        }
        case 6:
            return {OperationType::Traverse, -1, -1};
        case 7:
        default: {
            const int index = random_int(rng, 0, std::max(0, size - 1));
            int value = random_int(rng, 0, max_value);
            if (!initial_values.empty() && value == initial_values[index] && max_value > 0) {
                value = (value + 1) % (max_value + 1);
            }
            return {OperationType::Update, value, index};
        }
    }
}

Scenario make_single_operation_scenario(int index, std::mt19937& rng, const Config& config) {
    const int slot = (index - 1) % 8;
    int size = 0;

    switch (slot) {
        case 0:
        case 1:
        case 2:
            size = random_int(rng, 0, std::max(0, config.max_list_size - 1));
            break;
        default:
            size = random_int(rng, 1, config.max_list_size);
            break;
    }

    Scenario scenario;
    scenario.initial_values = random_values(rng, size, config.max_value);

    const Operation op = make_single_operation_case(slot, scenario.initial_values, rng, config.max_value);
    scenario.operations.push_back(op);
    scenario.category = "single_operation/" + operation_name(op.type);
    scenario.id = next_id("scenario_", index) + "_" + operation_name(op.type);
    return scenario;
}

Operation random_mixed_operation(const std::vector<int>& values, std::mt19937& rng, const Config& config) {
    std::vector<OperationType> available;

    if (static_cast<int>(values.size()) < config.max_list_size) {
        available.push_back(OperationType::AddTail);
        available.push_back(OperationType::InsertHead);
        available.push_back(OperationType::InsertIndex);
    }

    if (!values.empty()) {
        available.push_back(OperationType::DeleteHead);
        available.push_back(OperationType::DeleteTail);
        available.push_back(OperationType::Search);
        available.push_back(OperationType::Traverse);
        available.push_back(OperationType::Update);
    } else {
        available.push_back(OperationType::AddTail);
        available.push_back(OperationType::InsertHead);
    }

    const OperationType type = available[random_int(rng, 0, static_cast<int>(available.size()) - 1)];
    const int size = static_cast<int>(values.size());

    switch (type) {
        case OperationType::AddTail:
            return {type, random_int(rng, 0, config.max_value), -1};
        case OperationType::InsertHead:
            return {type, random_int(rng, 0, config.max_value), -1};
        case OperationType::InsertIndex:
            return {type, random_int(rng, 0, config.max_value), random_int(rng, 0, size)};
        case OperationType::DeleteHead:
        case OperationType::DeleteTail:
        case OperationType::Traverse:
            return {type, -1, -1};
        case OperationType::Search: {
            const bool want_hit = !values.empty() && random_int(rng, 0, 1) == 0;
            const int value = want_hit
                ? values[random_int(rng, 0, static_cast<int>(values.size()) - 1)]
                : pick_missing_value(values, rng, config.max_value);
            return {type, value, -1};
        }
        case OperationType::Update: {
            const int index = random_int(rng, 0, std::max(0, size - 1));
            int value = random_int(rng, 0, config.max_value);
            if (!values.empty() && value == values[index] && config.max_value > 0) {
                value = (value + 1) % (config.max_value + 1);
            }
            return {type, value, index};
        }
        default:
            return {OperationType::Traverse, -1, -1};
    }
}

Scenario make_mixed_scenario(int index, std::mt19937& rng, const Config& config) {
    Scenario scenario;
    scenario.id = next_id("mixed_", index);
    scenario.category = "mixed_sequence";
    scenario.initial_values = random_values(rng, random_int(rng, 1, std::max(1, config.max_list_size / 2)), config.max_value);

    std::vector<int> working = scenario.initial_values;
    const int operation_count = random_int(rng, 3, 6);

    for (int step = 0; step < operation_count; ++step) {
        Operation op = random_mixed_operation(working, rng, config);
        scenario.operations.push_back(op);
        apply_operation(working, op);
    }

    return scenario;
}

void write_readme(const fs::path& file_path, const Config& config) {
    std::vector<std::string> lines = {
        "Singly linked list dataset generator output",
        "",
        "Folders:",
        "- visualizer_inputs/: build input files you can load directly in the SFML visualizer",
        "- scenario_specs/: richer scenario descriptions with operations and expected results",
        "",
        "File formats:",
        "- visualizer_inputs/*.txt uses: n on the first line, followed by n values",
        "- scenario_specs/*.txt uses key=value lines",
        "",
        "Generator settings:",
        "- seed=" + std::to_string(config.seed),
        "- build_case_count=" + std::to_string(config.build_case_count),
        "- single_case_count=" + std::to_string(config.single_case_count),
        "- mixed_case_count=" + std::to_string(config.mixed_case_count),
        "- max_list_size=" + std::to_string(config.max_list_size),
        "- max_value=" + std::to_string(config.max_value)
    };

    write_text_file(file_path, lines);
}

void write_manifest(const fs::path& file_path, const std::vector<ManifestRow>& rows) {
    ensure_parent_directory(file_path);
    std::ofstream out(file_path);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot write file: " + file_path.string());
    }

    out << "id,category,input_file,spec_file,initial_values,operations,expected_final\n";
    for (const ManifestRow& row : rows) {
        out << csv_escape(row.id) << ','
            << csv_escape(row.category) << ','
            << csv_escape(row.input_file) << ','
            << csv_escape(row.spec_file) << ','
            << csv_escape(row.initial_values) << ','
            << csv_escape(row.operations) << ','
            << csv_escape(row.expected_final) << '\n';
    }
}

std::string make_operations_summary(const std::vector<Operation>& operations) {
    std::ostringstream out;
    for (std::size_t i = 0; i < operations.size(); ++i) {
        if (i > 0) out << " | ";
        out << format_operation(operations[i]);
    }
    return out.str();
}

ManifestRow write_scenario_files(const fs::path& output_root, const Scenario& scenario) {
    std::vector<std::string> lines;
    lines.push_back("# Singly linked list scenario");
    lines.push_back("name=" + scenario.id);
    lines.push_back("category=" + scenario.category);
    lines.push_back("initial=" + join_values(scenario.initial_values));
    lines.push_back("operation_count=" + std::to_string(scenario.operations.size()));

    std::vector<int> working = scenario.initial_values;
    for (std::size_t i = 0; i < scenario.operations.size(); ++i) {
        const std::string op_text = format_operation(scenario.operations[i]);
        const std::string result_text = apply_operation(working, scenario.operations[i]);
        const std::string step = std::to_string(i + 1);

        lines.push_back("op" + step + "=" + op_text);
        lines.push_back("expected" + step + "=" + result_text);
        lines.push_back("after" + step + "=" + join_values(working));
    }

    lines.push_back("expected_final=" + join_values(working));

    const fs::path spec_path = output_root / "scenario_specs" / (scenario.id + ".txt");
    write_text_file(spec_path, lines);

    std::string input_file_text = "N/A";
    if (!scenario.initial_values.empty()) {
        const fs::path input_path = output_root / "visualizer_inputs" / (scenario.id + "_initial.txt");
        write_visualizer_input(input_path, scenario.initial_values);
        input_file_text = input_path.lexically_relative(output_root).generic_string();
    }

    ManifestRow row;
    row.id = scenario.id;
    row.category = scenario.category;
    row.input_file = input_file_text;
    row.spec_file = spec_path.lexically_relative(output_root).generic_string();
    row.initial_values = join_values(scenario.initial_values);
    row.operations = make_operations_summary(scenario.operations);
    row.expected_final = join_values(working);
    return row;
}

void write_build_case(const fs::path& output_root, const std::string& id, const std::vector<int>& values, std::vector<ManifestRow>& manifest_rows) {
    const fs::path input_path = output_root / "visualizer_inputs" / (id + ".txt");
    write_visualizer_input(input_path, values);

    ManifestRow row;
    row.id = id;
    row.category = "build_only";
    row.input_file = input_path.lexically_relative(output_root).generic_string();
    row.spec_file = "N/A";
    row.initial_values = join_values(values);
    row.operations = "build";
    row.expected_final = join_values(values);
    manifest_rows.push_back(row);
}

void generate_edge_cases(const fs::path& output_root, int max_list_size, std::vector<ManifestRow>& manifest_rows) {
    write_build_case(output_root, "edge_single_node", {42}, manifest_rows);
    write_build_case(output_root, "edge_duplicates", {7, 7, 3, 7, 1, 1}, manifest_rows);
    write_build_case(output_root, "edge_sorted", {5, 12, 25, 36, 48, 59}, manifest_rows);
    write_build_case(output_root, "edge_reverse_sorted", {59, 48, 36, 25, 12, 5}, manifest_rows);

    std::vector<int> near_capacity;
    for (int i = 0; i < max_list_size; ++i) {
        near_capacity.push_back((i * 73) % 1000);
    }
    write_build_case(output_root, "edge_max_visible_nodes", near_capacity, manifest_rows);
}

} // namespace

int main(int argc, char** argv) {
    try {
        Config config;
        if (!parse_args(argc, argv, config)) {
            return 0;
        }

        fs::create_directories(config.output_root);
        fs::create_directories(config.output_root / "visualizer_inputs");
        fs::create_directories(config.output_root / "scenario_specs");

        std::mt19937 rng(config.seed);
        std::vector<ManifestRow> manifest_rows;

        write_readme(config.output_root / "README.txt", config);
        generate_edge_cases(config.output_root, config.max_list_size, manifest_rows);

        for (int i = 1; i <= config.build_case_count; ++i) {
            int size = 0;
            if (i <= config.build_case_count / 3) {
                size = clamped_random_size(rng, 3, 5, config.max_list_size);
            } else if (i <= (2 * config.build_case_count) / 3) {
                size = clamped_random_size(rng, 6, 8, config.max_list_size);
            } else {
                size = clamped_random_size(rng, 9, config.max_list_size, config.max_list_size);
            }

            const std::string id = next_id("build_", i);
            write_build_case(config.output_root, id, random_values(rng, size, config.max_value), manifest_rows);
        }

        for (int i = 1; i <= config.single_case_count; ++i) {
            const Scenario scenario = make_single_operation_scenario(i, rng, config);
            manifest_rows.push_back(write_scenario_files(config.output_root, scenario));
        }

        for (int i = 1; i <= config.mixed_case_count; ++i) {
            const Scenario scenario = make_mixed_scenario(i, rng, config);
            manifest_rows.push_back(write_scenario_files(config.output_root, scenario));
        }

        write_manifest(config.output_root / "manifest.csv", manifest_rows);

        std::cout
            << "Generated singly linked list dataset at: " << config.output_root << '\n'
            << "Build-only cases: " << config.build_case_count + 5 << '\n'
            << "Single-operation scenarios: " << config.single_case_count << '\n'
            << "Mixed-operation scenarios: " << config.mixed_case_count << '\n'
            << "Manifest: " << (config.output_root / "manifest.csv") << '\n';
    } catch (const std::exception& ex) {
        std::cerr << "Dataset generation failed: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
