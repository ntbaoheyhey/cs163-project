#include "../headers/global.h"
#include <filesystem>

// const unsigned int WINDOW_WIDTH = 1600;
// const unsigned int WINDOW_HEIGHT = 900;
const unsigned int WINDOW_WIDTH = 1366;
const unsigned int WINDOW_HEIGHT = 700;
// Nhân: Máy tui 1366x768 nên tui dùng tạm để code nha anh em, nào có meet ae bàn cái này giúp tui nha

sf::RenderWindow window;

sf::Font font_impact;

bool loadFonts() {
    // Thử từng đường dẫn có thể
    std::vector<std::string> paths = {
        "Visualizer/assets/JetBrainsMono-Medium.ttf",
        "cs163-project/Visualizer/assets/JetBrainsMono-Medium.ttf",
        "../../assets/JetBrainsMono-Medium.ttf",
        "Visualizer/assets/impact.ttf",
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

