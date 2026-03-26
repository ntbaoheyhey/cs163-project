#include "../headers/global.h"
#include <filesystem>

const unsigned int WINDOW_WIDTH = 1600;
const unsigned int WINDOW_HEIGHT = 800;

sf::RenderWindow window;

sf::Font font_impact;

bool loadFonts() {
    // Thử từng đường dẫn có thể
    std::vector<std::string> paths = {
        "../assets/impact.ttf",
        "../../assets/impact.ttf",
        "Visualizer/assets/impact.ttf",
        "assets/impact.ttf",
        "C:/Windows/Fonts/arial.ttf"  // Fallback system font
    };
    
    for (const auto& path : paths) {
        if (font_impact.openFromFile(path)) {
            std::cout << "Font loaded successfully from: " << path << std::endl;
            return true;
        }
    }
    
    std::cerr << "Failed to load font from any path!" << std::endl;
    return false;
}

