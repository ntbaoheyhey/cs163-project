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

void hover_button(button &a, bool isOver){
    if (isOver) {
        a.setColor(sf::Color(172, 123, 42));
    } else {
        a.setColor(sf::Color(232, 183, 81));
    }
}

void hover_box(box &a, bool isOver, bool isClick) {
    if (isOver || isClick) {
        a.setColor(sf::Color(214, 217, 224));
    } else {
        a.setColor(sf::Color(138,155,192));
    }
}
enum ActionType {INSERT, POP, SWAPUI, HIGHLIGHT, UNHIGHLIGHT, SNAPSHOT};

    struct AnimationStep {
        ActionType type;
        int index1{-1};
        int index2{-1};
    };

    struct min_heap {
        std::string val{"0"};
        std::vector<int> v;
        std::vector<node> nodelist;
        std::vector<edge> edgelist;
        std::vector<node> ss_nodelist;
        std::vector<edge> ss_edgelist;
        std::vector<AnimationStep> animation_queue;
        bool hasAnimation{0};
        bool isAnimate{0};
        int cur_step{0};
        void heapify(int i) {
            if (v.size() == 0) {
                animation_queue.push_back({SNAPSHOT, -1, -1});
                return;
            }
            int l = 2*i + 1;
            int r = 2*i +2;
            animation_queue.push_back({HIGHLIGHT, i, -1});
            int next = i;
            if (l<v.size()) {
                if (v[l] < v[next]) next = l;
                animation_queue.push_back({HIGHLIGHT, l, -1});
                animation_queue.push_back({UNHIGHLIGHT, l, -1});
            }
            if (r<v.size()) {
                if (v[r] < v[next]) next = r;
                animation_queue.push_back({HIGHLIGHT, r, -1});
                animation_queue.push_back({UNHIGHLIGHT, r, -1});
            }
            if (next == i) {
                animation_queue.push_back({UNHIGHLIGHT, i, -1});
                animation_queue.push_back({SNAPSHOT, -1, -1});
                return;
            }
            std::swap(v[next], v[i]);
            animation_queue.push_back({HIGHLIGHT, next, -1});
            animation_queue.push_back({SWAPUI, i, next});
            animation_queue.push_back({UNHIGHLIGHT, i, -1});
            heapify(next);
        }

        void push(int x){

            v.push_back(x);
            
            int i = v.size()-1;
            animation_queue.push_back({INSERT, i, x});

            while(i) {
                int par = (i-1)/2;
                animation_queue.push_back({HIGHLIGHT, i, par});
                if (v[i] < v[par]) {
                    std::swap(v[i], v[par]);
                    animation_queue.push_back({SWAPUI, i, par});
                    animation_queue.push_back({UNHIGHLIGHT, i, -1});
                    i = par;
                } else {
                    animation_queue.push_back({UNHIGHLIGHT, par, i});
                    break;
                }
            }
            if (!i) animation_queue.push_back({UNHIGHLIGHT, i, -1});
            animation_queue.push_back({SNAPSHOT, -1, -1});
        }

        void pop(){
            if (!v.size()) return;
            std::swap(v.back(), v[0]);
            animation_queue.push_back({ActionType::HIGHLIGHT, int(0), int(v.size()-1)});
            animation_queue.push_back({SWAPUI, int(0), int(v.size()-1)});
            animation_queue.push_back({UNHIGHLIGHT, int(0), int(v.size()-1)});
            animation_queue.push_back({POP, int(v.size()-1), v.back()});
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
            cur_step = 0;
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
    valIn_box.setOutline(sf::Color(179, 229, 228), 3);
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


    sf::Clock clock;
    
    min_heap core_heap;
    bool isBoxPress = false;
    bool isInsertPress = false;
    bool onInsertPress = false;
    bool isPopPress = false;
    bool onPopPress = false;
    bool isNextPress = false;
    bool onNextPress = false;
    bool isPreviousPress = false;
    bool onPreviousPress = false;
    bool isOutPress = false;
    bool onOutPress = false;
    bool isBoxOver = false;
    bool isInsertOver = false;
    bool isPopOver = false;
    bool isNextOver = false;
    bool isPreviousOver = false;
    bool isOutOver = false;

    

    // Create the CodeBox (width: 350, height: 400)
    CodeBox codeBox({500.f, 300.f}, font_impact, 23);

    // The code we want to visualize
    std::string pushCode = 
        "v.push_back(x)  \n"
        "i = v.size() - 1\n"
        "\n"
        "while (i)\n"
        "if v[(i-1) / 2 ] > v[i]\n"
        "swap(v[par], v[i]) , i = par\n"
        "else break\n"
        "\n"
        "Done";
    //std::string heapifyCode = 

    codeBox.setCode(pushCode);
    codeBox.setOrigin({500, 300});
    codeBox.setPosition({float(WINDOW_WIDTH) + 20, float(WINDOW_HEIGHT - 25)});
    while(window.isOpen()){

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
                        if (valIn_box.isClicked(sf::Mouse::getPosition(window))) {
                            isBoxPress = true;  // Click vào box -> Bật nhập liệu
                        } else {
                            isBoxPress = false; // Click ra ngoài -> Tắt nhập liệu
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
                            if (core_heap.val.size() < 3) {
                                core_heap.val.push_back(static_cast<char>(unicode));
                            }
                        }
                    }
                }
            }

            // Cập nhật nhãn cho Box
            if (core_heap.val.size() == 0) core_heap.val = "0";
            valIn_box.setLabel(core_heap.val);

            window.clear(sf::Color(212, 188, 112, 0.71));
            window.draw(backgroundSprite);

            window.draw(bgmain);
            getout_button.draw(window);
            insert_button.draw(window);
            pop_button.draw(window);
            valIn_box.draw(window);
            next_button.draw(window);
            previous_button.draw(window);
            window.draw(slider_bg);
            window.draw(slider);
            window.draw(codeBox);

            insert_button.isPress = insert_button.isClicked(sf::Mouse::getPosition(window));
            pop_button.isPress = pop_button.isClicked(sf::Mouse::getPosition(window));
            next_button.isPress = next_button.isClicked(sf::Mouse::getPosition(window));
            previous_button.isPress = previous_button.isClicked(sf::Mouse::getPosition(window));
            isInsertOver = insert_button.contains(sf::Mouse::getPosition(window));
            isPopOver = pop_button.contains(sf::Mouse::getPosition(window));
            isNextOver = next_button.contains(sf::Mouse::getPosition(window));
            isPreviousOver = previous_button.contains(sf::Mouse::getPosition(window));
            isBoxOver = valIn_box.contains(sf::Mouse::getPosition(window));
            isOutPress = getout_button.isClicked(sf::Mouse::getPosition(window));
            isOutOver = getout_button.contains(sf::Mouse::getPosition(window));

            hover_button(insert_button, isInsertOver);
            hover_button(pop_button, isPopOver);
            hover_button(next_button, isNextOver);
            hover_button(previous_button, isPreviousOver);
            hover_box(valIn_box, isBoxOver, isBoxPress);
            hover_button(getout_button, isOutOver); 
            // BUTTONS

            if (isOutPress && !onOutPress) {
                return;
            }
            onOutPress = isOutPress;

            if (!core_heap.hasAnimation && !core_heap.isAnimate){

            if (insert_button.isPress && !insert_button.onPress && core_heap.v.size()<63) {
                core_heap.RESET();
                core_heap.push(convert_str(core_heap.val));
                core_heap.hasAnimation = true;
                isBoxPress = false;
            }     
            insert_button.onPress = insert_button.isPress;
            
            if (pop_button.isPress && !pop_button.onPress){
                core_heap.RESET();
                core_heap.pop();
                core_heap.hasAnimation = true;
                isBoxPress = false;
            }
            pop_button.onPress = pop_button.isPress;

            if (previous_button.isPress && !previous_button.onPress && core_heap.cur_step > 0) {
                core_heap.hasAnimation = false;
                core_heap.cur_step--; // Lùi lại 1 step để lấy hành động vừa xảy ra
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
            previous_button.onPress = previous_button.isPress;
            
            if (next_button.isPress && !next_button.onPress &&
                core_heap.cur_step < core_heap.animation_queue.size()) { 

                core_heap.hasAnimation = false;
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
            next_button.onPress = next_button.isPress;
        }

            // ANIMATION
            if (!core_heap.isAnimate && core_heap.hasAnimation) {
                if (core_heap.cur_step < core_heap.animation_queue.size()) {
                    core_heap.hasAnimation = true;
                    auto animation = core_heap.animation_queue[core_heap.cur_step];
                    std::cout << animation.type << '\n';
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
            slider.setSize({slider_bg.getSize().x * core_heap.cur_step / core_heap.animation_queue.size(), slider_bg.getSize().y});

            core_heap.isAnimate = false;
            for (auto& c: core_heap.edgelist) {
                c.draw(window);
            }
            for (auto& x: core_heap.nodelist) {
                x.updatePosition(dt);
                x.updateColor(dt);
                if (x.isMoving || x.isColoring) core_heap.isAnimate = true;
                x.draw(window);
            }

            

            window.display();
        }
        
    }
}