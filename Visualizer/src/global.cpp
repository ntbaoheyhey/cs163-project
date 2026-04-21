#include "../headers/global.h"

#include <algorithm>
#include <filesystem>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

// const unsigned int WINDOW_WIDTH = 1600;
// const unsigned int WINDOW_HEIGHT = 900;
const unsigned int WINDOW_WIDTH = 1366;
const unsigned int WINDOW_HEIGHT = 700;

sf::RenderWindow window;

sf::Font font_impact;

sf::Texture background_texture;
sf::Sprite background_sprite(background_texture);

int current_music_index;
std::vector<std::filesystem::path> playlist;
sf::Music background_music;

namespace {

std::filesystem::path executableDirectory() {
#ifdef _WIN32
    char buffer[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length == 0 || length == MAX_PATH) {
        return {};
    }

    return std::filesystem::path(buffer).parent_path();
#else
    return {};
#endif
}

std::vector<std::filesystem::path> assetSearchRoots() {
    std::vector<std::filesystem::path> roots;

    const auto exe_dir = executableDirectory();
    if (!exe_dir.empty()) {
        roots.push_back(exe_dir);
        roots.push_back(exe_dir / "..");
    }

    const auto cwd = std::filesystem::current_path();
    roots.push_back(cwd);
    roots.push_back(cwd / "Visualizer");
    roots.push_back(cwd / "..");
    roots.push_back(cwd / ".." / "Visualizer");

#ifdef VISUALIZER_SOURCE_DIR
    roots.emplace_back(VISUALIZER_SOURCE_DIR);
#endif

    std::vector<std::filesystem::path> unique_roots;
    for (const auto& root : roots) {
        if (root.empty()) {
            continue;
        }

        const auto normalized = root.lexically_normal();
        const bool already_seen = std::find(unique_roots.begin(), unique_roots.end(), normalized) != unique_roots.end();
        if (!already_seen) {
            unique_roots.push_back(normalized);
        }
    }

    return unique_roots;
}

} // namespace

std::filesystem::path resolveAssetPath(const std::string& filename) {
    for (const auto& root : assetSearchRoots()) {
        const auto candidate = (root / "assets" / filename).lexically_normal();
        if (std::filesystem::exists(candidate)) {
            return candidate;
        }
    }

    return {};
}

bool loadTextureFromAsset(sf::Texture& texture, const std::string& filename) {
    const auto path = resolveAssetPath(filename);
    if (path.empty()) {
        std::cerr << "Missing asset: " << filename << std::endl;
        return false;
    }

    if (!texture.loadFromFile(path.string())) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }

    return true;
}

bool loadFonts() {
    const std::vector<std::string> asset_fonts = {
        "JetBrainsMono-Medium.ttf",
        "impact.ttf"
    };

    for (const auto& font_name : asset_fonts) {
        const auto path = resolveAssetPath(font_name);
        if (!path.empty() && font_impact.openFromFile(path.string())) {
            std::cout << "Font loaded successfully from: " << path << std::endl;
            return true;
        }
    }

#ifdef _WIN32
    const std::filesystem::path system_font = "C:/Windows/Fonts/arial.ttf";
    if (std::filesystem::exists(system_font) && font_impact.openFromFile(system_font.string())) {
        std::cout << "Font loaded successfully from: " << system_font << std::endl;
        return true;
    }
#endif

    std::cerr << "Failed to load font from assets and system fallback!" << std::endl;
    return false;
}
