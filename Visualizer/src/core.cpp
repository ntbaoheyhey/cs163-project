#include "../headers/core.h"
#include "../headers/heap.h"

#include <algorithm>
#include <cctype>

namespace {

bool isSupportedMusicFile(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    return extension == ".mp3" || extension == ".ogg" || extension == ".wav";
}

} // namespace

void loadMusic() {
    playlist.clear();
    current_music_index = -1; // No music loaded yet
    background_music.setVolume(50);
    background_music.setLooping(false);

    const std::filesystem::path playlist_dir = resolveAssetPath("playlist");
    if (playlist_dir.empty() || !std::filesystem::is_directory(playlist_dir)) {
        background_music.pause();
        return;
    }

    try {
        for (const auto& entry : std::filesystem::directory_iterator(playlist_dir)) {
            if (entry.is_regular_file() && isSupportedMusicFile(entry.path())) {
                playlist.push_back(entry.path());
            }
        }
    } catch (const std::exception&) {
        playlist.clear();
    }

    std::sort(playlist.begin(), playlist.end());
    if (playlist.empty()) {
        background_music.pause();
        return;
    }

    current_music_index = 0; // Start with the first track
    if (background_music.openFromFile(playlist[0])) {
        background_music.play();
    } else {
        current_music_index = -1; // reset to no music
        background_music.pause();
    }
}

void openWindow() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 10;
    // khủ răng cưa

    window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Data Structure Visualizer", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(60);

    if (!loadFonts()) {
    }
    font_impact.setSmooth(true);

    loadMusic();

    if (!loadTextureFromAsset(background_texture,"bg_toty.png")) {
        if(!background_texture.loadFromFile("cs163-project/Visualizer/assets/bg_toty.png")) {
        }
    } 
    background_sprite.setTexture(background_texture, 1);
}

void main_menu_page() {

    float button_width = 300.0f;
    float button_height = 80.0f;

    button option_button(WINDOW_WIDTH/2 - button_width/2, WINDOW_HEIGHT/2 - button_height/2, button_width, button_height, sf::Color::White, "Data Structures", 24);
    button setting_button(WINDOW_WIDTH/2 - button_width/2, WINDOW_HEIGHT/2 - button_height/2 + 100.0f, button_width, button_height, sf::Color(232, 183, 81), "Settings", 24);

    std::vector<button*> all_buttons = {&option_button, &setting_button};

    bool is_mouse_left_pressed = 0;
    bool is_mouse_left_pressed_last = 0;

    while(window.isOpen()) {
        is_mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.draw(background_sprite);

        for (const auto& btn : all_buttons) {
            btn->draw(window);
        }

        for(int i = 0; i < all_buttons.size(); i++) {
            if(all_buttons[i]->isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
                switch(i) {
                    case 0: option_page(); break;
                    case 1: setting_page(); break; // setting page
                }
            }
        }

        for(int i = 0; i < all_buttons.size(); i++) {
            if(all_buttons[i]->contains(sf::Mouse::getPosition(window))) {
                all_buttons[i]->setColor(sf::Color(140, 95, 30));
            } else {
                all_buttons[i]->setColor(sf::Color(232, 183, 81));
            }
            all_buttons[i]->draw(window);
        }

        window.display();

        is_mouse_left_pressed_last = is_mouse_left_pressed;
    }
}

void option_page() {

    float button_width = 200.0f;
    float button_height = 100.0f;

    float position_button_x = WINDOW_WIDTH / 2.0f - button_width / 2.0f;
    float position_button_y = WINDOW_HEIGHT / 2.0f - button_height / 2.0f - 100.0f;

    button shortest_path_button(position_button_x, position_button_y, button_width, button_height, sf::Color::White, "Shortest Path", 24);

    button ducminh(position_button_x, position_button_y + button_height + 20, button_width, button_height, sf::Color::Cyan, "Heap", 24);
    button trie(position_button_x, position_button_y + 2 * (button_height + 20), button_width, button_height, sf::Color::Green, "Trie", 24);
    button linked_list(position_button_x, position_button_y + 3 * (button_height + 20), button_width, button_height, sf::Color(232, 183, 81), "Singly List", 24);
    button back_button(10.0f, 10.0f, 100.0f, 50.0f, sf::Color(232, 183, 81), "Back", 24);

    std::vector<button*> all_buttons = {&shortest_path_button, &ducminh, &trie, &linked_list, &back_button};

    bool is_mouse_left_pressed = 0;
    bool is_mouse_left_pressed_last = 1;

    while(window.isOpen()) {
        is_mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        for(int i = 0; i < all_buttons.size(); i++) {
            if(all_buttons[i]->isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
                switch(i) {
                    case 0: shortest_path_page(); break;
                    case 1: heap_page(); break;
                    case 2: trie_page(); break;
                    case 3: singly_linked_list_page(); break;
                    case 4: return; break;
                }
            }
        }

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.draw(background_sprite);
        
        for(int i = 0; i < all_buttons.size(); i++) {
            if(all_buttons[i]->contains(sf::Mouse::getPosition(window))) {
                all_buttons[i]->setColor(sf::Color(140, 95, 30));
            } else {
                all_buttons[i]->setColor(sf::Color(232, 183, 81));
            }
            all_buttons[i]->draw(window);
        }

        window.display();

        is_mouse_left_pressed_last = is_mouse_left_pressed;
    }
}

void setting_page() {
    float choose_music_button_width = 390.0f;
    float choose_music_button_height = 60.0f;
    float button_width = 200.0f;
    float button_height = 100.0f;
    float choose_button_width = 80.0f;
    float font_size = 24.0f;
    const float volume_step = 1.0f;
    float position_button_x = WINDOW_WIDTH / 2.0f - button_width / 2.0f;
    float position_button_y = WINDOW_HEIGHT / 2.0f - button_height / 2.0f - 100.0f;


    bool is_music_menu_active = false;
    box music_button(106 , 110, choose_music_button_width, choose_music_button_height, sf::Color(232, 183, 81), "Music" , font_size);
    box volume(      1215, 110, choose_button_width      , choose_button_width       , sf::Color(232, 183, 81), "Volume", font_size);

    std::vector<box> music_options;
    for(int i = 0; i < playlist.size(); i++) {
        music_options.emplace_back(106, 110 + (i+1) * choose_music_button_height, choose_music_button_width, choose_music_button_height, sf::Color(232, 183, 81, 150), playlist[i].filename().string(), font_size);
    }

    button play_button(   106 , 390, choose_button_width, choose_button_width, sf::Color(232, 183, 81), "Play", font_size);
    button loop_button(   106 , 540, choose_button_width, choose_button_width, sf::Color(232, 183, 81), "Loop", font_size);
    button inc_vol_button(1045, 110, choose_button_width, choose_button_width, sf::Color(232, 183, 81), "+"   , font_size);
    button dec_vol_button(944 , 110, choose_button_width, choose_button_width, sf::Color(232, 183, 81), "-"   , font_size);
    button back_button(   1045, 520, button_width       , button_height      , sf::Color(232, 183, 81), "Back", font_size);

    std::vector<button*> all_buttons = {&play_button, &loop_button, &inc_vol_button, &dec_vol_button, &back_button};
    std::vector<bool> state_buttons(all_buttons.size(), 0);
    state_buttons[0] = background_music.getStatus() == sf::Music::Status::Playing; // play button state
    state_buttons[1] = background_music.isLooping(); // loop button state

    bool is_mouse_left_pressed = 0;
    bool is_mouse_left_pressed_last = 1;

    while(window.isOpen()) {
        is_mouse_left_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if(state_buttons[0] && background_music.getStatus() == sf::Music::Status::Stopped) {
            state_buttons[0] = false;
        }

        //====================================
        //          PLAY BUTTON              //
        if(play_button.isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
            // id = 0 means play/pause button
            if(state_buttons[0]) {
                state_buttons[0] = 0;
                background_music.pause();
            } else  if(current_music_index != -1 and current_music_index < playlist.size()) {
                // NOTE Stopped not Paused: if music is stopped, we need to open it again before playing
                if(background_music.getStatus() == sf::Music::Status::Stopped) {
                    if(!background_music.openFromFile(playlist[current_music_index])) {
                        state_buttons[0] = 0; // reset play button state
                        continue;
                    }
                    background_music.play();
                    state_buttons[0] = 1;
                } else {
                    background_music.play();
                    state_buttons[0] = 1;
                }
            }
        }

        //====================================
        //          LOOP BUTTON              //
        if(loop_button.isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
            background_music.setLooping(!background_music.isLooping());
            // 1 means loop button
            state_buttons[1] = !state_buttons[1];

            // Need to start playing if loop is enabled and music is not currently playing

            if(state_buttons[1] and background_music.getStatus() == sf::Music::Status::Stopped and current_music_index != -1 and current_music_index < playlist.size()) {
                if(!background_music.openFromFile(playlist[current_music_index])) {
                    state_buttons[1] = 0; // reset loop button state
                    continue;
                }
                background_music.play();
            }
        }

        //====================================
        //          VOLUME BUTTON            //
        if(inc_vol_button.isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
            float current_volume = background_music.getVolume();
            background_music.setVolume(std::min(100.0f, current_volume + volume_step));
        }

        if(dec_vol_button.isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
            float current_volume = background_music.getVolume();
            background_music.setVolume(std::max(0.0f, current_volume - volume_step));
        }

        //====================================
        //         BACK BUTTON              //
        if(back_button.isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
            return;
        }
        
        //====================================
        //        MUSIC BUTTON              //
        if(music_button.isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
            is_music_menu_active = !is_music_menu_active;
        }

        //====================================
        //       MUSIC SELECTION MENU        //
        if(is_music_menu_active) {
            for(int i = 0; i < music_options.size(); i++) {
                if(music_options[i].isClicked(sf::Mouse::getPosition(window)) and !is_mouse_left_pressed_last) {
                    const bool was_playing = background_music.getStatus() == sf::Music::Status::Playing;
                    if(!background_music.openFromFile(playlist[i])) {
                        state_buttons[0] = 0;
                        continue;
                    }

                    current_music_index = i;
                    is_music_menu_active = false;
                    state_buttons[0] = was_playing;
                    if(was_playing) {
                        background_music.play();
                    }
                }
            }
        }


        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.draw(background_sprite);
        
        for(int i = 0; i < all_buttons.size(); i++) {
            if(all_buttons[i]->contains(sf::Mouse::getPosition(window)) or state_buttons[i]) {
                all_buttons[i]->setColor(sf::Color(140, 95, 30));
            } else {
                all_buttons[i]->setColor(sf::Color(232, 183, 81));
            }
            all_buttons[i]->draw(window);
        }

        //=============================
        //      draw music box
        music_button.setLabel(current_music_index != -1 and current_music_index < playlist.size() ? std::filesystem::path(playlist[current_music_index]).filename().string() : "No Music");
        music_button.draw(window);

        if(is_music_menu_active) {
            for(int i = 0; i < music_options.size(); i++) {
                if(music_options[i].contains(sf::Mouse::getPosition(window))) {
                    music_options[i].setColor(sf::Color(140, 95, 30));
                } else {
                    music_options[i].setColor(sf::Color(232, 183, 81, 150));
                }
                music_options[i].draw(window);
            }
        }


        //=============================
        //      draw volume box
        volume.setLabel(std::to_string((int)background_music.getVolume()));
        volume.draw(window);

        //=============================
        window.display();

        is_mouse_left_pressed_last = is_mouse_left_pressed;
    }
}
