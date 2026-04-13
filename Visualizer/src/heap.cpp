#include "../headers/heap.h"
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <windows.h>
#include <pseudocode.h>
#define WIN32_LEAN_AND_MEAN
// Hàm mở file
    std::string openFileDialog() {
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"; // Chỉ hiện file txt
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Hiển thị hộp thoại, nếu người dùng chọn "Open" thì trả về TRUE
    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
    return ""; // Trả về rỗng nếu người dùng bấm "Cancel"
}
// Hàm tính tọa độ cho node. Lưu ý: 'heapIndex' phải bắt đầu từ 1 (1-based index)
// Nếu mảng của bạn bắt đầu từ 0, hãy truyền vào (index + 1)
sf::Vector2f getHeapNodePosition(int heapIndex, float windowWidth, float startY, float startX, float verticalSpacing) {
    // 1. Tính độ sâu của node (level 0 là root)
    int level = std::log2(heapIndex);

    // 2. Tính tọa độ Y
    float y = startY + level * verticalSpacing;

    // 3. Tính tọa độ X
    int maxNodesInLevel = 1 << level; 
    int posInLevel = heapIndex - maxNodesInLevel;

    // --- PHẦN CHỈNH SỬA ĐỂ CANH GIỮA ---
    
    // Đặt tỷ lệ chiều rộng cây muốn hiển thị (VD: 80% chiều rộng màn hình)
    float treeWidth = windowWidth * 0.92f; 
    
    // Tính khoảng trống bên trái để đẩy cây ra giữa
    float offsetX = (windowWidth - treeWidth) / 2.0f;

    // Chiều rộng của mỗi segment bây giờ dựa trên treeWidth
    float segmentWidth = treeWidth / maxNodesInLevel;

    // Cộng thêm offsetX vào tọa độ X cuối cùng
    float x = startX + offsetX + (posInLevel + 0.5f) * segmentWidth;

    return sf::Vector2f(x, y);
}

// Hàm xử lý nhập số cơ bản (cho Insert, Search, Update)
void handleNumericInput(std::string& targetStr, char32_t unicode, size_t maxLength = 2) {
    // Xử lý Backspace
    if (unicode == '\b' || unicode == 8) {
        if (!targetStr.empty()) targetStr.pop_back();
    }
    // Xử lý nhập số
    else if (unicode >= '0' && unicode <= '9') {
        // Bỏ số '0' vô nghĩa ở đầu
        if (targetStr == "0") targetStr.clear();
        
        if (targetStr.size() < maxLength) {
            targetStr.push_back(static_cast<char>(unicode));
        }
    }
}

// Hàm xử lý nhập mảng (cho Build)
void handleArrayInput(std::string& targetStr, char32_t unicode, size_t maxLength = 17) {
    if (unicode == '\b' || unicode == 8) {
        if (!targetStr.empty()) targetStr.pop_back();
    } 
    else if (unicode >= '0' && unicode <= '9') {
        // Bỏ số '0' ở đầu nếu nó đứng ngay đầu chuỗi
        if (unicode != ',' && targetStr.size() == 1 && targetStr[0] == '0') {
            targetStr.clear();
        }
        // Giới hạn 2 chữ số cho mỗi phần tử cách nhau bởi dấu phẩy
        if (targetStr.size() >= 2) {
            int z = targetStr.size();
            if (targetStr[z-1] != ',' && targetStr[z-2] != ',') return; // Bỏ qua không thêm
        }
        if (targetStr.size() < maxLength) {
            targetStr.push_back(static_cast<char>(unicode));
        }
    } 
    else if (unicode == ',') {
        // Tránh dấu phẩy ở đầu hoặc 2 dấu phẩy liên tiếp
        if (!targetStr.empty() && targetStr.back() != ',' && targetStr.size() < maxLength) {
            targetStr.push_back(static_cast<char>(unicode));
        }
    }
}

int convert_str(std::string &s){
    int res = 0;
    for (char c: s){
        res = res*10 + (c-'0');
    }
    return res;
}

std::vector<int> str_to_vec(std::string s){
    std::vector<int> ans;
    if (s.empty()) return ans;
    int last = 0;
    if (s.back() != ',') s.push_back(',');
    for (auto &c: s) {
        if (c == ',') {
            ans.push_back(last);
            last = 0;
        } else {
            last = last * 10 + (c-'0');
        }
    }
    return ans;
}
enum FocusedBox {
    NONE,
    INSERT_VAL,
    BUILD,
    UPDATE_VAL,
    UPDATE_KEY,
};

enum ActionType {INSERT, POP, SWAPUI, HIGHLIGHT, UNHIGHLIGHT, SNAPSHOT};

    struct AnimationStep {
        ActionType type;
        int index1{-1};
        int index2{-1};
    };

    struct Heap {
        bool isMinHeap{1};
        bool isSkip{0};
        std::string val{"0"};
        std::string build{"0"};
        std::string changeval{"0"};
        std::string changekey{"0"};
        std::vector<int> v;
        std::vector<node> nodelist;
        std::vector<edge> edgelist;
        std::vector<sf::Text> indexlist;
        std::vector<node> ss_nodelist;
        std::vector<edge> ss_edgelist;
        std::vector<sf::Text> ss_indexlist;
        std::vector<AnimationStep> animation_queue;
        std::vector<int> codebox_queue;
        
        bool hasAnimation{0};
        bool isAnimate{0};
        int cur_step{0};
        double animation_speed{1};

        // Hàm so sánh dùng chung cho cả Heapify và Push
        // Trả về true nếu child "ưu tiên" hơn parent (cần đưa child lên trên)
        bool compare(int child_val, int parent_val) {
            if (isMinHeap) {
                return child_val < parent_val; // Min Heap: số nhỏ hơn sẽ nổi lên
            } else {
                return child_val > parent_val; // Max Heap: số lớn hơn sẽ nổi lên
            }
        }

        void upheap(int i) {
            animation_queue.push_back({HIGHLIGHT, i, -1});
            codebox_queue.push_back(3);
            while(i) {
                int par = (i-1)/2;
                animation_queue.push_back({HIGHLIGHT, par, -1});
                codebox_queue.push_back(4);
                if (compare(v[i], v[par])) {
                    std::swap(v[i], v[par]);
                    animation_queue.push_back({SWAPUI, i, par});
                    codebox_queue.push_back(5);
                    animation_queue.push_back({UNHIGHLIGHT, i, -1});
                    codebox_queue.push_back(5);
                    i = par;
                } else {
                    animation_queue.push_back({UNHIGHLIGHT, par, i});
                    codebox_queue.push_back(6);
                    break;
                }
            }
            if (!i) {
                animation_queue.push_back({UNHIGHLIGHT, i, -1});
                codebox_queue.push_back(3);
            }
        }

        void downheap(int i) {
            if (v.size() == 0) {
                animation_queue.push_back({SNAPSHOT, -1, -1});
                codebox_queue.push_back(8);
                return;
            }
            int l = 2*i + 1;
            int r = 2*i +2;
            animation_queue.push_back({HIGHLIGHT, i, -1});
            codebox_queue.push_back(3);
            int next = i;
            if (l<v.size()) {
                if (compare(v[l], v[next])) next = l;
                animation_queue.push_back({HIGHLIGHT, l, -1});
                codebox_queue.push_back(4);
                animation_queue.push_back({UNHIGHLIGHT, l, -1});
                codebox_queue.push_back(4);
            }
            if (r<v.size()) {
                if (compare(v[r], v[next])) next = r;
                animation_queue.push_back({HIGHLIGHT, r, -1});
                codebox_queue.push_back(4);
                animation_queue.push_back({UNHIGHLIGHT, r, -1});
                codebox_queue.push_back(4);
            }
            if (next == i) {
                animation_queue.push_back({UNHIGHLIGHT, i, -1});
                codebox_queue.push_back(6);
                animation_queue.push_back({SNAPSHOT, -1, -1});
                codebox_queue.push_back(8);
                return;
            }
            std::swap(v[next], v[i]);
            animation_queue.push_back({HIGHLIGHT, next, -1});
            codebox_queue.push_back(5);
            animation_queue.push_back({SWAPUI, i, next});
            codebox_queue.push_back(5);
            animation_queue.push_back({UNHIGHLIGHT, i, -1});
            codebox_queue.push_back(5);
            downheap(next);
        }

        void push(int x){

            v.push_back(x);
            
            int i = v.size()-1;
            animation_queue.push_back({INSERT, i, x});
            codebox_queue.push_back(0);

            upheap(i);
            animation_queue.push_back({SNAPSHOT, -1, -1});
            codebox_queue.push_back(8);
        }

        void pop(){
            if (!v.size()) return;
            std::swap(v.back(), v[0]);
            animation_queue.push_back({ActionType::HIGHLIGHT, int(0), int(v.size()-1)});
            codebox_queue.push_back(0);
            animation_queue.push_back({SWAPUI, int(0), int(v.size()-1)});
            codebox_queue.push_back(0);
            animation_queue.push_back({UNHIGHLIGHT, int(0), int(v.size()-1)});
            codebox_queue.push_back(0);
            animation_queue.push_back({POP, int(v.size()-1), v.back()});
            codebox_queue.push_back(1);
            v.pop_back();
            downheap(0);
        }

        int update(int key, int val) {
            int ans;
            int old = v[key];
            v[key] = val;
            nodelist[key].setLabel(std::to_string(val), 20);
            if (isMinHeap) {
                if (val >= old) {
                    downheap(key);
                    ans = 4;
                } else {
                    upheap(key);
                    ans = 3;
                }
            } else {
                if (val >= old) {
                    upheap(key);
                    ans = 3;
                } else {
                    downheap(key);
                    ans = 4;
                }
            }
            animation_queue.push_back({SNAPSHOT, -1, -1});
            codebox_queue.push_back(8);
            return ans;
        }

        
        void UISWAPUI(int i, int j){
            startNodeMovement(nodelist[i], nodelist[j].currentPos, 2);
            startNodeMovement(nodelist[j], nodelist[i].currentPos, 2);
            std::swap(nodelist[i], nodelist[j]);
        }

        void UIHIGHLIGHT(int i, int j){
            if (i>=0) {
                startNodeColor(nodelist[i], sf::Color(188, 11, 11), 1);
            }
            if (j>=0 && j!=i) {
                startNodeColor(nodelist[j], sf::Color(188, 11, 11), 1);
            }
        }

        void UIUNHIGHLIGHT(int i, int j){
            if (i>=0) {
                startNodeColor(nodelist[i], sf::Color(202, 148, 95), 1);
            }
            if (j>=0 && j!=i) {
                startNodeColor(nodelist[j], sf::Color(202, 148, 95), 1);
            }
        }

        void UIPOP(){
            nodelist.pop_back();
            edgelist.pop_back();
            indexlist.pop_back();
        }

        void UIINSERT(int i, int x){
            // i = v.size() - 1 

            sf::Vector2f position = getHeapNodePosition(i+1, 975, 50, 380 , 75);
            // INSERT INDEX
            sf::Text newidx(font_impact, std::to_string(i), 18);
            newidx.setPosition({position.x, position.y + 20});
            newidx.setFillColor(sf::Color(54, 173, 36));
            indexlist.push_back(newidx);

            // INSERTNODE
            node mem(position.x, position.y, 20, sf::Color(218, 168, 74), sf::Color(202, 148, 95), 4);
            mem.setLabel(std::to_string(x), 20);
            nodelist.push_back(mem);
            
            // INSERTEDGE
            edge canh(position.x, position.y, nodelist[(i-1)/2].currentPos.x, nodelist[(i-1)/2].currentPos.y, 
            sf::Color(203, 203, 201), 3);
            edgelist.push_back(canh);
        }

        void UISNAPSHOT(){
            ss_edgelist = edgelist;
            ss_nodelist = nodelist;
            ss_indexlist = indexlist;
        }

        void RESET() {
            edgelist = ss_edgelist;
            nodelist = ss_nodelist;
            indexlist = ss_indexlist;
            animation_queue.clear();
            codebox_queue.clear();
            cur_step = 0;
        }

        void REBUILD(std::vector<int> &query) {
            v.clear();
            nodelist.clear();
            edgelist.clear();
            indexlist.clear();
            ss_nodelist.clear();
            ss_edgelist.clear();
            animation_queue.clear();
            codebox_queue.clear();
            hasAnimation = 0;
            isAnimate = 0;
            cur_step = 0;
            v = query;
            int n = query.size();
            for (int i=0; i<n; ++i) {
                UIINSERT(i, query[i]);
            }
            for (int i = (n / 2) - 1; i >= 0; --i) {
                downheap(i);
            }
            animation_queue.push_back({SNAPSHOT, -1, -1});
            codebox_queue.push_back(8);
        }
        void CLEAR() {
            v.clear();
            nodelist.clear();
            edgelist.clear();
            indexlist.clear();
            ss_nodelist.clear();
            ss_edgelist.clear();
            animation_queue.clear();
            codebox_queue.clear();
            
            hasAnimation = 0;
            isAnimate = 0;
            cur_step = 0;
        }
    };

void heap_page(){
    // 2. Khai báo Texture và tải ảnh từ thư mục
    sf::Texture backgroundTexture;
    // Lưu ý: Đảm bảo file "background.png" nằm cùng thư mục với file thực thi (executable) 
    // hoặc bạn phải truyền đường dẫn tuyệt đối/tương đối chính xác.
    if (!backgroundTexture.loadFromFile("Visualizer/assets/bg.png")) {
        std::cerr << "cannot load background" << std::endl;
    }

    // 3. Khai báo Sprite và gán Texture cho nó
    sf::Sprite backgroundSprite(backgroundTexture);
    button min_button(10, WINDOW_HEIGHT - 500, 100, 50, sf::Color(232, 183, 81), "MIN", 24);
    button max_button(135, WINDOW_HEIGHT - 500, 100, 50, sf::Color(232, 183, 81), "MAX", 24);
    button insert_button(10, WINDOW_HEIGHT - 225, 100, 50, sf::Color(232, 183, 81), "PUSH", 24);
    button pop_button(10, WINDOW_HEIGHT - 150, 100, 50, sf::Color(232, 183, 81), "POP", 24);
    button clear_button(135, WINDOW_HEIGHT - 150, 100, 50, sf::Color(232, 183, 81), "CLEAR", 24);
    button next_button(400, WINDOW_HEIGHT - 75, 75, 50, sf::Color(232, 183, 81), "NEXT", 24);
    button previous_button(500, WINDOW_HEIGHT - 75, 75, 50, sf::Color(232, 183, 81), "BACK", 24);
    button getout_button(15, 15, 100, 50, sf::Color(232, 183, 81), "RETURN", 24);
    button speed_inc(700, WINDOW_HEIGHT - 75, 50, 50, sf::Color(232, 183, 81), "+", 24);
    button speed_dec(760, WINDOW_HEIGHT - 75, 50, 50, sf::Color(232, 183, 81), "-", 24);
    button skip_button(600, WINDOW_HEIGHT - 75, 75, 50, sf::Color(232, 183, 81), "SKIP", 24);
    button build_button(240, WINDOW_HEIGHT - 375, 100, 50, sf::Color(232, 183, 81), "BUILD", 24);
    button update_button(10, WINDOW_HEIGHT - 75, 100, 50, sf::Color(232, 183, 81), "UPDATE", 24);
    button random_button(10, WINDOW_HEIGHT - 375, 100, 50, sf::Color(232, 183, 81), "RANDOM", 24);
    button txtfile_button(125, WINDOW_HEIGHT - 375, 100, 50, sf::Color(232, 183, 81), "FILE", 24);
    RoundedRectangleShape slider({500, 30});
    slider.setPosition({400.f, float(WINDOW_HEIGHT) - 120.f});
    RoundedRectangleShape slider_bg({500, 30});
    slider_bg.setPosition({400.f, float(WINDOW_HEIGHT) - 120.f});
    slider_bg.setOutlineThickness(3);
    slider_bg.setOutlineColor(sf::Color(233, 186, 85));
    slider_bg.setFillColor(sf::Color(140, 155, 191));
    slider.setFillColor(sf::Color(28, 41, 114));
    box valIn_box(135, WINDOW_HEIGHT - 225, 100, 50, sf::Color(138,155,192), "0", 24);
    box upIdx_box(135, WINDOW_HEIGHT - 75, 100, 50, sf::Color(138,155,192), "0", 24);
    box upVal_box(250, WINDOW_HEIGHT - 75, 100, 50, sf::Color(138,155,192), "0", 24);
    box build_box(10, WINDOW_HEIGHT - 300, 325, 50, sf::Color(138,155,192), "0", 24);
    box speed_box(820, WINDOW_HEIGHT - 75, 50, 50, sf::Color(138,155,192), "0", 24);

    RoundedRectangleShape bgmain({975.f, 525.f});    
    bgmain.setFillColor(sf::Color(251, 251, 253, 200));
    bgmain.setPosition({380, 20});
    bgmain.setRadius(17);
    bgmain.setOutlineThickness(5);
    bgmain.setOutlineColor(sf::Color(217, 211, 209));
    
    std::vector<button*> all_buttons = {
    &insert_button, &pop_button, &next_button, &previous_button, 
    &getout_button, &speed_inc, &speed_dec, &build_button, &update_button,
    &skip_button, &random_button, &txtfile_button, &clear_button,
    &min_button, &max_button
    };

    std::vector<box*> all_boxes = {
    &valIn_box, &build_box, &speed_box, &upIdx_box, &upVal_box
    };
    

    sf::Clock clock;
    Heap core_heap;

    // Create the CodeBox (width: 350, height: 400)
    CodeBox codeBox({450, 250}, font_impact, 20);
    codeBox.setOrigin({450, 250});
    codeBox.setPosition({float(WINDOW_WIDTH), float(WINDOW_HEIGHT-10)});

    FocusedBox currentFocus = FocusedBox::NONE;

    while(window.isOpen()){
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        float dt = clock.getElapsedTime().asSeconds();
        if (dt>=1.f/60.f){
            clock.restart();
            while (const std::optional event = window.pollEvent())
                {
                if (event->is<sf::Event::Closed>())
                    window.close();
                
                // 1. Xử lý CLICK CHUỘT -> Chuyển đổi trạng thái Focus
                if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        // Mặc định click ra ngoài là mất focus
                        currentFocus = FocusedBox::NONE; 
                        
                        // Dùng else-if vì 1 điểm click chỉ thuộc về 1 box
                        if (valIn_box.isClicked(mousePos)) {
                            currentFocus = FocusedBox::INSERT_VAL;
                        } else if (build_box.isClicked(mousePos)) {
                            currentFocus = FocusedBox::BUILD;
                        } else if (upIdx_box.isClicked(mousePos)) {
                            currentFocus = FocusedBox::UPDATE_KEY;
                        } else if (upVal_box.isClicked(mousePos)) {
                            currentFocus = FocusedBox::UPDATE_VAL;
                        }

                    }
                }

                // 2. Xử lý GÕ PHÍM dựa trên trạng thái Focus hiện tại
                if (currentFocus != FocusedBox::NONE) {
                    if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                        char32_t unicode = textEvent->unicode;
                        
                        // Điều hướng input vào đúng string của struct core_heap
                        switch (currentFocus) {
                            case FocusedBox::INSERT_VAL:
                                handleNumericInput(core_heap.val, unicode, 2);
                                break;
                            case FocusedBox::BUILD:
                                handleArrayInput(core_heap.build, unicode, 64);
                                break;
                            case FocusedBox::UPDATE_KEY:
                                handleArrayInput(core_heap.changekey, unicode, 2);
                                break;
                            case FocusedBox::UPDATE_VAL:
                                handleArrayInput(core_heap.changeval, unicode, 2);
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            // Trạng thái min-heap / max-heap
            if (core_heap.isMinHeap) {
                min_button.setColor(sf::Color(140, 95, 30));
                max_button.setColor(sf::Color(232, 183, 81));
            } else {
                max_button.setColor(sf::Color(140, 95, 30));
                min_button.setColor(sf::Color(232, 183, 81));
            }

            // Cập nhật nhãn cho Box
            std::string sp = std::to_string(core_heap.animation_speed);
            sp = sp.substr(0, 3);
            speed_box.setLabel(sp);
            if (core_heap.val.size() == 0) core_heap.val = "0";
            valIn_box.setLabel(core_heap.val+'|');
            if (core_heap.changekey.size() == 0) core_heap.changekey = "0";
            upIdx_box.setLabel("idx:"+core_heap.changekey+'|');
            if (core_heap.changeval.size() == 0) core_heap.changeval = "0";
            upVal_box.setLabel("val:"+core_heap.changeval+'|');

            int strsize = core_heap.build.size();
            if (strsize == 0) core_heap.build = "0";
            if (strsize < 21) {
                build_box.setLabel(core_heap.build+'|');
            }
            else build_box.setLabel(core_heap.build.substr(strsize-21) + '|');


            window.clear(sf::Color(212, 188, 112, 0.71));
            window.draw(backgroundSprite);

            window.draw(bgmain);
            window.draw(codeBox);
            window.draw(slider_bg);
            window.draw(slider);
            for (auto* btn : all_buttons) btn->draw(window);
            for (auto* b : all_boxes) b->draw(window);
            
            // Update states of buttons
            bool insert_active = insert_button.update(mousePos);
            bool pop_active    = pop_button.update(mousePos);
            bool next_active   = next_button.update(mousePos);
            bool back_active   = previous_button.update(mousePos);
            bool out_active    = getout_button.update(mousePos);
            bool inc_active    = speed_inc.update(mousePos);
            bool dec_active    = speed_dec.update(mousePos);
            bool build_active  = build_button.update(mousePos);
            bool update_active = update_button.update(mousePos);
            bool skip_active   = skip_button.update(mousePos);
            bool min_active    = min_button.update(mousePos);
            bool max_active    = max_button.update(mousePos);
            bool random_active = random_button.update(mousePos);
            bool file_active   = txtfile_button.update(mousePos);
            bool clear_active  = clear_button.update(mousePos);

            for (auto* b : all_boxes) {
                b->update(0, mousePos); // Mặc định tắt hết
            }

            // Bật lại box đang được focus
            switch (currentFocus) {
                case FocusedBox::BUILD: build_box.update(1, mousePos); break;
                case FocusedBox::INSERT_VAL: valIn_box.update(1, mousePos); break;
                case FocusedBox::UPDATE_KEY: upIdx_box.update(1, mousePos); break;
                case FocusedBox::UPDATE_VAL: upVal_box.update(1, mousePos); break;
                default: break;
            }
            
            // BUTTONS

            if (out_active) {
                return;
            }
            
            else if (skip_active) {
                core_heap.isSkip = true;
            }

            else if (inc_active) {
                core_heap.animation_speed = std::min(2.0, core_heap.animation_speed + 0.1);
                std::cout << core_heap.animation_speed << '\n';
            }
            
            else if (dec_active) {
                core_heap.animation_speed = std::max(0.5, core_heap.animation_speed - 0.1);
                std::cout << core_heap.animation_speed << '\n';
            }

            else if (file_active) {
                std::string dir = openFileDialog();
                if (!dir.empty()) {
                    std::cout << dir << '\n';
                    std::ifstream fin(dir);
                    if (fin.is_open()) {
                        std::cout << "Ghi nhan: File da mo thanh cong!\n";
                    } else {
                        std::cout << "Loi: Khong the mo file!\n";
                    }
                    int n; 
                    if (fin >> n) {
                        std::cout << "Gia tri n doc duoc: " << n << '\n';
                    } else {
                        std::cout << "Loi: Khong doc duoc so! (Vui long check lai format hoac Encoding cua file txt)\n";
                    }
                    if (n) {
                        std::vector<int> inp(n);
                        for (int i=0; i<n; ++i) {
                            fin >> inp[i];
                        }
                        std::string s;
                        for (int i=0; i<n; ++i) {
                            s+=std::to_string(inp[i])+',';
                        }
                        core_heap.build = s;
                    }
                    fin.close();

                }
            }

            else if (random_active) {
                std::mt19937 rng(time(0));
                int n = rng()%32+1;
                std::string s;
                for (int i=0; i<n; ++i) {
                    int x = rng()%100;
                    s+=std::to_string(x)+',';
                }
                core_heap.build = s;
            }

            else if (clear_active) {
                core_heap.CLEAR();
            }

            else if (min_active) {
                if (!core_heap.isMinHeap) {
                    core_heap.CLEAR();
                    core_heap.isMinHeap = 1;
                }
            }

            else if (max_active) {
                if (core_heap.isMinHeap) {
                    core_heap.CLEAR();
                    core_heap.isMinHeap = 0;
                }
            }


            if (!core_heap.hasAnimation && !core_heap.isAnimate){
    
            if (build_active) {
                std::vector<int> query = str_to_vec(core_heap.build);
                codeBox.setCode(PseudoCode::heapCode[core_heap.isMinHeap][2]);
                core_heap.REBUILD(query);
                core_heap.hasAnimation = true;
            }
            
            else if (insert_active && core_heap.v.size()<63) {
                codeBox.setCode(PseudoCode::heapCode[core_heap.isMinHeap][0]);
                core_heap.RESET();
                core_heap.push(convert_str(core_heap.val));
                core_heap.hasAnimation = true;
            }     
            
            else if (pop_active){
                codeBox.setCode(PseudoCode::heapCode[core_heap.isMinHeap][1]);
                core_heap.RESET();
                core_heap.pop();
                core_heap.hasAnimation = true;
            }

            else if (update_active) {
                int key = convert_str(core_heap.changekey);
                int val = convert_str(core_heap.changeval);
                if (key < core_heap.v.size()) {
                    core_heap.RESET();
                    int code = core_heap.update(key, val);
                    codeBox.setCode(PseudoCode::heapCode[core_heap.isMinHeap][code]);
                    core_heap.hasAnimation = true;
                }
            }

            else if (back_active && core_heap.cur_step > 0) {
                
                core_heap.hasAnimation = false;
                core_heap.cur_step--; // Lùi lại 1 step để lấy hành động vừa xảy ra

                codeBox.setStep(core_heap.codebox_queue[core_heap.cur_step]);   
                auto animation = core_heap.animation_queue[core_heap.cur_step];

                // Đã xóa bỏ đoạn if () gây lỗi nhảy cóc ở đây

                switch (animation.type)
                {
                case ActionType::INSERT:
                    core_heap.UIPOP(); // Undo của Insert là Pop UI
                    break;
                case ActionType::POP:
                    core_heap.UIINSERT(animation.index1, animation.index2); // Undo của Pop là Insert lại
                    break;
                case ActionType::SWAPUI:
                    core_heap.UISWAPUI(animation.index1, animation.index2);
                    break;
                case ActionType::SNAPSHOT:
                    // Bỏ qua vì case SWAPNODE đi lùi đã xử lý gộp, hoặc đây là mốc snapshot không cần undo hình ảnh
                    break;
                case ActionType::UNHIGHLIGHT:
                    core_heap.UIHIGHLIGHT(animation.index1, animation.index2); // Undo của Unhighlight là Highlight lại
                    break;
                case ActionType::HIGHLIGHT:
                    core_heap.UIUNHIGHLIGHT(animation.index1, animation.index2); // Undo của Highlight là tắt Highlight
                    break;    
                default:
                    break;
                }
            }
            
            else if (next_active && core_heap.cur_step < core_heap.animation_queue.size()) { 

                core_heap.hasAnimation = false;
                codeBox.setStep(core_heap.codebox_queue[core_heap.cur_step]);
                auto animation = core_heap.animation_queue[core_heap.cur_step];

                switch (animation.type)
                {
                case ActionType::INSERT:
                    core_heap.UIINSERT(animation.index1, animation.index2);
                    break;
                case ActionType::POP:
                    core_heap.UIPOP();
                    break;
                case ActionType::SWAPUI:
                    core_heap.UISWAPUI(animation.index1, animation.index2);
                    break;
                case ActionType::SNAPSHOT:
                    // Các hidden steps này sẽ bị bỏ qua nếu lỡ lọt vào
                    break;
                case ActionType::HIGHLIGHT:
                    core_heap.UIHIGHLIGHT(animation.index1, animation.index2);
                    break;
                case ActionType::UNHIGHLIGHT:
                    core_heap.UIUNHIGHLIGHT(animation.index1, animation.index2);
                    break;
                default:
                    break;
                }
                
                ++core_heap.cur_step; 
            }
        }

            // ANIMATION
            if (!core_heap.isAnimate && core_heap.hasAnimation) {
                if (core_heap.cur_step < core_heap.animation_queue.size()) {
                    codeBox.setStep(core_heap.codebox_queue[core_heap.cur_step]);
                    core_heap.hasAnimation = true;
                    auto animation = core_heap.animation_queue[core_heap.cur_step];
                    switch (animation.type)
                    {
                    case ActionType::SWAPUI:
                        core_heap.UISWAPUI(animation.index1, animation.index2);
                        break;
                    case ActionType::HIGHLIGHT:
                        core_heap.UIHIGHLIGHT(animation.index1, animation.index2);
                        break;
                    case ActionType::UNHIGHLIGHT:
                        core_heap.UIUNHIGHLIGHT(animation.index1, animation.index2);
                        break;
                    case ActionType::POP:
                        core_heap.UIPOP();
                        break;
                    case ActionType::INSERT:
                        core_heap.UIINSERT(animation.index1, animation.index2);
                        break;
                    case ActionType::SNAPSHOT:
                        core_heap.UISNAPSHOT();
                        break;
                    default:
                        break;
                    } 
                    ++core_heap.cur_step;
                } 
            }
            if (core_heap.cur_step == core_heap.animation_queue.size()) {
                core_heap.isSkip = false;
                core_heap.hasAnimation = false;
            }

            // Vẽ và update isAnimate
            
            // update slider
            if (core_heap.animation_queue.size() > 0) {
                slider.setSize({slider_bg.getSize().x * core_heap.cur_step / core_heap.animation_queue.size(), slider_bg.getSize().y});
            } else {
                slider.setSize({0, slider_bg.getSize().y});
            }

            core_heap.isAnimate = false;
            for (auto& c: core_heap.edgelist) {
                c.draw(window);
            }
            for (int i=0; i<core_heap.nodelist.size(); ++i) {
                if (core_heap.isSkip) dt = 100000000;
                core_heap.nodelist[i].updatePosition(core_heap.animation_speed * dt);
                core_heap.nodelist[i].updateColor(core_heap.animation_speed * dt);
                if (core_heap.nodelist[i].isMoving || core_heap.nodelist[i].isColoring) core_heap.isAnimate = true;
                core_heap.nodelist[i].draw(window);
                window.draw(core_heap.indexlist[i]);
            }

    
            window.display();
        }
        
    }
}