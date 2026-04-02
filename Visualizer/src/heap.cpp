#include "../headers/heap.h"
#include <vector>
#include <string>

// Hàm tính tọa độ cho node. Lưu ý: 'heapIndex' phải bắt đầu từ 1 (1-based index)
// Nếu mảng của bạn bắt đầu từ 0, hãy truyền vào (index + 1)
sf::Vector2f getHeapNodePosition(int heapIndex, float windowWidth, float startY, float verticalSpacing) {
    
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
    float x = offsetX + (posInLevel + 0.5f) * segmentWidth;

    return sf::Vector2f(x, y);
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

enum ActionType {INSERT, POP, SWAPUI, HIGHLIGHT, UNHIGHLIGHT, SNAPSHOT};

    struct AnimationStep {
        ActionType type;
        int index1{-1};
        int index2{-1};
    };

    struct min_heap {
        std::string val{"0"};
        std::string build{"0"};
        std::vector<int> v;
        std::vector<node> nodelist;
        std::vector<edge> edgelist;
        std::vector<node> ss_nodelist;
        std::vector<edge> ss_edgelist;
        std::vector<AnimationStep> animation_queue;
        std::vector<int> codebox_queue;
        
        bool hasAnimation{0};
        bool isAnimate{0};
        int cur_step{0};
        double animation_speed{1};

        void heapify(int i) {
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
                if (v[l] < v[next]) next = l;
                animation_queue.push_back({HIGHLIGHT, l, -1});
                codebox_queue.push_back(4);
                animation_queue.push_back({UNHIGHLIGHT, l, -1});
                codebox_queue.push_back(4);
            }
            if (r<v.size()) {
                if (v[r] < v[next]) next = r;
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
            heapify(next);
        }

        void push(int x){

            v.push_back(x);
            
            int i = v.size()-1;
            animation_queue.push_back({INSERT, i, x});
            codebox_queue.push_back(0);

            animation_queue.push_back({HIGHLIGHT, i, -1});
            codebox_queue.push_back(3);
            while(i) {
                int par = (i-1)/2;
                animation_queue.push_back({HIGHLIGHT, par, -1});
                codebox_queue.push_back(4);
                if (v[i] < v[par]) {
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
            heapify(0);
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
        }

        void UIINSERT(int i, int x){
            // i = v.size() - 1 

            sf::Vector2f position = getHeapNodePosition(i+1, WINDOW_WIDTH, 50, 75);

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
        }

        void RESET() {
            edgelist = ss_edgelist;
            nodelist = ss_nodelist;
            animation_queue.clear();
            codebox_queue.clear();
            cur_step = 0;
        }

        void REBUILD(std::vector<int> &query) {
            v.clear();
            nodelist.clear();
            edgelist.clear();
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
                heapify(i);
            }
        }
    };

void heap_page(){
    // 2. Khai báo Texture và tải ảnh từ thư mục
    sf::Texture backgroundTexture;
    // Lưu ý: Đảm bảo file "background.png" nằm cùng thư mục với file thực thi (executable) 
    // hoặc bạn phải truyền đường dẫn tuyệt đối/tương đối chính xác.
    if (!backgroundTexture.loadFromFile("Visualizer/assets/bg_toty.png")) {
        std::cerr << "cannot load background" << std::endl;
    }

    // 3. Khai báo Sprite và gán Texture cho nó
    sf::Sprite backgroundSprite(backgroundTexture);

    button insert_button(50, WINDOW_HEIGHT - 150, 150, 50, sf::Color(232, 183, 81), "PUSH", 24);
    button pop_button(50, WINDOW_HEIGHT - 75, 150, 50, sf::Color(232, 183, 81), "POP", 24);
    button next_button(250, WINDOW_HEIGHT - 75, 100, 50, sf::Color(232, 183, 81), "NEXT", 24);
    button previous_button(400, WINDOW_HEIGHT - 75, 100, 50, sf::Color(232, 183, 81), "BACK", 24);
    button getout_button(15, 15, 100, 50, sf::Color(232, 183, 81), "RETURN", 24);
    button speed_inc(70, WINDOW_HEIGHT - 300, 50, 50, sf::Color(232, 183, 81), "+", 24);
    button speed_dec(130, WINDOW_HEIGHT - 300, 50, 50, sf::Color(232, 183, 81), "-", 24);
    button build_button(50, WINDOW_HEIGHT - 225, 150, 50, sf::Color(232, 183, 81), "BUILD", 24);
    RoundedRectangleShape slider({500, 30});
    slider.setPosition({550.f, float(WINDOW_HEIGHT) - 140.f});
    slider.setRadius(13);
    RoundedRectangleShape slider_bg({500, 30});
    slider_bg.setRadius(13);
    slider_bg.setPosition({550.f, float(WINDOW_HEIGHT) - 140.f});
    slider_bg.setOutlineThickness(3);
    slider_bg.setOutlineColor(sf::Color(233, 186, 85));
    slider_bg.setFillColor(sf::Color(140, 155, 191));
    slider.setFillColor(sf::Color(28, 41, 114));
    box valIn_box(250, WINDOW_HEIGHT - 150, 100, 50, sf::Color(138,155,192), "0", 24);
    box build_box(250, WINDOW_HEIGHT - 225, 275, 50, sf::Color(138,155,192), "0", 24);
    box speed_box(250, WINDOW_HEIGHT - 300, 50, 50, sf::Color(138,155,192), "0", 24);
    valIn_box.setOutline(sf::Color(179, 229, 228), 3);
    build_box.setOutline(sf::Color(179, 229, 228), 3);
    speed_box.setOutline(sf::Color(179, 229, 228), 3);
    RoundedRectangleShape bgmain({1500.f, 600.f});    
    bgmain.setOrigin(bgmain.getGeometricCenter());
    bgmain.setPosition({800 , 250});
    bgmain.setFillColor(sf::Color(243, 243, 251, 100));
    bgmain.setRadius(17);
    bgmain.setOutlineThickness(5);
    bgmain.setOutlineColor(sf::Color(217, 211, 209));
    insert_button.setRadius(25);
    pop_button.setRadius(25);
    next_button.setRadius(25);
    previous_button.setRadius(25);
    build_button.setRadius(25);
    speed_inc.setRadius(10);
    speed_dec.setRadius(10);
    
    


    sf::Clock clock;
    min_heap core_heap;

    // Create the CodeBox (width: 350, height: 400)
    CodeBox codeBox({550.f, 300.f}, font_impact, 23);
    codeBox.setOrigin({550, 300});
    codeBox.setPosition({float(WINDOW_WIDTH) + 20, float(WINDOW_HEIGHT - 25)});
    
    // The code we want to visualize
   std::string pushCode = 
        "v.push_back(x)\n"                 // Dòng 0: Insert vào cuối mảng
        "i = v.size() - 1\n"               // Dòng 1: (Chỉ định biến)
        "while (i > 0):\n"                 // Dòng 2: (Bắt đầu vòng lặp)
        "  par = (i - 1) / 2\n"            // Dòng 3: Tính index node cha
        "  if (v[i] < v[par]):\n"          // Dòng 4: So sánh tính chất Min-Heap
        "      swap(v[i], v[par]), i = par\n" // Dòng 5: Hoán vị nếu node con nhỏ hơn cha
        "  else: break\n"                  // Dòng 6: Dừng nếu đã thoả mãn Min-Heap
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done";                            // Dòng 8: Kết thúc
    std::string heapifyCode = 
        "swap(v[0], v.back())\n"           // Dòng 0: Hoán vị root với phần tử cuối
        "v.pop_back()\n"                   // Dòng 1: Xóa phần tử cuối
        "heapify(i):\n"                    // Dòng 2: Bắt đầu khôi phục cây
        "  l = 2i + 1, r = 2i + 2\n"       // Dòng 3: Lấy index 2 node con
        "  next = min_index(i, l, r)\n"    // Dòng 4: Tìm node có giá trị nhỏ nhất
        "  if (next != i): swap, heapify(next)\n" // Dòng 5: Đổi chỗ và đệ quy
        "  else: return\n"                 // Dòng 6: Dừng nếu root đã là nhỏ nhất
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done";                            // Dòng 8: Kết thúc
    std::string buildCode = 
        "nothing\n"                        // Dòng 0: Hoán vị root với phần tử cuối
        "\n"                               // Dòng 1: Xóa phần tử cuối
        "heapify(i):\n"                    // Dòng 2: Bắt đầu khôi phục cây
        "  l = 2i + 1, r = 2i + 2\n"       // Dòng 3: Lấy index 2 node con
        "  next = min_index(i, l, r)\n"    // Dòng 4: Tìm node có giá trị nhỏ nhất
        "  if (next != i): swap, heapify(next)\n" // Dòng 5: Đổi chỗ và đệ quy
        "  else: return\n"                 // Dòng 6: Dừng nếu root đã là nhỏ nhất
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done";                            // Dòng 8: Kết thúc

    bool isBoxPress = false;
    bool isBuildPress = false;
    while(window.isOpen()){
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        float dt = clock.getElapsedTime().asSeconds();
        if (dt>=1.f/60.f){
            clock.restart();
            while (const std::optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                    window.close();
                
                // 1. Chỉ xử lý khi có sự kiện CLICK CHUỘT (MouseButtonPressed)
                if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        // Truyền window vào để lấy tọa độ chuột tương đối so với cửa sổ
                        if (valIn_box.isClicked(mousePos)) {
                            isBoxPress = true;  // Click vào box -> Bật nhập liệu
                        } else {
                            isBoxPress = false; // Click ra ngoài -> Tắt nhập liệu
                        }
                        if (build_box.isClicked(mousePos)) {
                            isBuildPress = true;  // Click vào box -> Bật nhập liệu
                        } else {
                            isBuildPress = false; // Click ra ngoài -> Tắt nhập liệu
                        }
                    }
                }

                // 2. Chỉ xử lý khi có sự kiện GÕ PHÍM (TextEntered) và Box đang được chọn
                if (isBoxPress) {
                    if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                        char32_t unicode = textEvent->unicode;
                        
                        // Xử lý Backspace (mã ASCII = 8)
                        if (unicode == '\b' || unicode == 8) {
                            if (core_heap.val.size() > 0) core_heap.val.pop_back();
                        }
                        // Xử lý nhập số (từ '0' đến '9')
                        else if (unicode >= '0' && unicode <= '9') {
                            // Bỏ số '0' ở đầu nếu có
                            if (core_heap.val.size() == 1 && core_heap.val[0] == '0') {
                                core_heap.val = "";
                            }
                            // Giới hạn độ dài số nhập vào (ví dụ: tối đa 3 chữ số để số không tràn node)
                            if (core_heap.val.size() < 2) {
                                core_heap.val.push_back(static_cast<char>(unicode));
                            }
                        }
                    }
                }
                if (isBuildPress) {
                    if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                        char32_t unicode = textEvent->unicode;
                        
                        // Xử lý Backspace (mã ASCII = 8)
                        if (unicode == '\b' || unicode == 8) {
                            if (core_heap.build.size() > 0) core_heap.build.pop_back();
                        }
                        // Xử lý nhập số (từ '0' đến '9')
                        else if (unicode >= '0' && unicode <= '9') {
                            // Bỏ số '0' ở đầu nếu có
                            if (unicode!=',' && core_heap.build.size() == 1 && core_heap.build[0] == '0') {
                                core_heap.build = "";
                            }
                            if (core_heap.build.size() >= 2) {
                                int z = core_heap.build.size();
                                if (core_heap.build[z-1]!=',' && core_heap.build[z-2]!=',') continue;
                            }
                            // Giới hạn độ dài số nhập vào (ví dụ: tối đa 3 chữ số để số không tràn node)
                            if (core_heap.build.size() < 17) {
                                core_heap.build.push_back(static_cast<char>(unicode));
                            }
                        } else if (unicode == ',') {
                            if (!core_heap.build.empty() && core_heap.build.back()!=',' && core_heap.build.size() < 17) {
                                core_heap.build.push_back(static_cast<char>(unicode));
                            }
                        }
                    }
                }
            }

            // Cập nhật nhãn cho Box
            std::string sp = std::to_string(core_heap.animation_speed);
            sp = sp.substr(0, 3);
            speed_box.setLabel(sp);
            if (core_heap.val.size() == 0) core_heap.val = "0";
            valIn_box.setLabel(core_heap.val+'|');
            if (core_heap.build.size() == 0) core_heap.build = "0";
            build_box.setLabel(core_heap.build+'|');


            window.clear(sf::Color(212, 188, 112, 0.71));
            window.draw(backgroundSprite);

            window.draw(bgmain);
            getout_button.draw(window);
            insert_button.draw(window);
            pop_button.draw(window);
            next_button.draw(window);
            previous_button.draw(window);
            build_button.draw(window);
            window.draw(slider_bg);
            window.draw(slider);
            speed_dec.draw(window);
            speed_inc.draw(window);
            build_box.draw(window);
            valIn_box.draw(window);
            speed_box.draw(window);
            //codeBox.setStep(-1);
            window.draw(codeBox);

            // Update states of buttons
            bool insert_active = insert_button.update(mousePos);
            bool pop_active    = pop_button.update(mousePos);
            bool next_active   = next_button.update(mousePos);
            bool back_active   = previous_button.update(mousePos);
            bool out_active    = getout_button.update(mousePos);
            bool inc_active    = speed_inc.update(mousePos);
            bool dec_active    = speed_dec.update(mousePos);
            bool build_active  = build_button.update(mousePos);
            valIn_box.update(isBoxPress, mousePos);
            build_box.update(isBuildPress, mousePos);

            // BUTTONS

            if (out_active) {
                return;
            }

            if (inc_active) {
                core_heap.animation_speed = std::min(2.0, core_heap.animation_speed + 0.1);
                std::cout << core_heap.animation_speed << '\n';
            }
            if (dec_active) {
                core_heap.animation_speed = std::max(0.5, core_heap.animation_speed - 0.1);
                std::cout << core_heap.animation_speed << '\n';
            }

            if (!core_heap.hasAnimation && !core_heap.isAnimate){

            if (build_active) {
                std::vector<int> query = str_to_vec(core_heap.build);
                codeBox.setCode(buildCode);
                core_heap.REBUILD(query);
                core_heap.hasAnimation = true;
            }
            
            if (insert_active && core_heap.v.size()<63) {
                codeBox.setCode(pushCode);
                core_heap.RESET();
                core_heap.push(convert_str(core_heap.val));
                core_heap.hasAnimation = true;
                isBoxPress = false;
            }     
            
            if (pop_active){
                codeBox.setCode(heapifyCode);
                core_heap.RESET();
                core_heap.pop();
                core_heap.hasAnimation = true;
                isBoxPress = false;
            }

            if (back_active && core_heap.cur_step > 0) {
                
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
            
            if (next_active && core_heap.cur_step < core_heap.animation_queue.size()) { 

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
            if (core_heap.cur_step == core_heap.animation_queue.size()) 
            core_heap.hasAnimation = false;

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
            for (auto& x: core_heap.nodelist) {
                x.updatePosition(core_heap.animation_speed * dt);
                x.updateColor(core_heap.animation_speed * dt);
                if (x.isMoving || x.isColoring) core_heap.isAnimate = true;
                x.draw(window);
            }

    
            window.display();
        }
        
    }
}