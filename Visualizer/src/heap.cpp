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
    // Số lượng node tối đa có thể chứa ở level hiện tại
    int maxNodesInLevel = 1 << level; 

    // Vị trí thứ tự của node trong level đó (bắt đầu từ 0)
    // VD: Node 2 và 3 ở level 1 sẽ có posInLevel lần lượt là 0 và 1
    int posInLevel = heapIndex - maxNodesInLevel;

    // Chiều rộng của mỗi không gian chia đều cho các node ở level này
    float segmentWidth = windowWidth / maxNodesInLevel;

    // Đặt X ở chính giữa (0.5) của segment tương ứng
    float x = (posInLevel + 0.5f) * segmentWidth;

    return sf::Vector2f(x, y);
}

int convert_str(std::string &s){
    int res = 0;
    for (char c: s){
        res = res*10 + (c-'0');
    }
    return res;
}
enum ActionType {INSERT, POP, SWAPUI, SWAPNODE, HIGHLIGHT, UNHIGHLIGHT, SNAPSHOT};

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
            int next = i;
            if (l<v.size()) {
                if (v[l] < v[next]) next = l;
            }
            if (r<v.size()) {
                if (v[r] < v[next]) next = r;
            }
            if (next == i) {
                animation_queue.push_back({UNHIGHLIGHT, i, i});
                animation_queue.push_back({SNAPSHOT, -1, -1});
                return;
            }
            std::swap(v[next], v[i]);
            animation_queue.push_back({HIGHLIGHT, i, next});
            animation_queue.push_back({SWAPUI, i, next});
            animation_queue.push_back({SWAPNODE, i, next});
            animation_queue.push_back({UNHIGHLIGHT, i, next});
            heapify(next);
        }

        void push(int x){

            v.push_back(x);
            
            int i = v.size()-1;
            animation_queue.push_back({INSERT, i, x});

            while(i) {
                int par = (i-1)/2;
                if (v[i] < v[par]) {
                    std::swap(v[i], v[par]);
                    animation_queue.push_back({HIGHLIGHT, i, par});
                    animation_queue.push_back({SWAPUI, i, par});
                    animation_queue.push_back({SWAPNODE, i, par});
                    animation_queue.push_back({UNHIGHLIGHT, i, par});
                    i = par;
                } else break;
            }
            animation_queue.push_back({SNAPSHOT, -1, -1});
        }

        void pop(){
            if (!v.size()) return;
            std::swap(v.back(), v[0]);
            animation_queue.push_back({ActionType::HIGHLIGHT, int(0), int(v.size()-1)});
            animation_queue.push_back({SWAPUI, int(0), int(v.size()-1)});
            animation_queue.push_back({SWAPNODE, int(0), int(v.size()-1)});
            animation_queue.push_back({UNHIGHLIGHT, int(0), int(v.size()-1)});
            animation_queue.push_back({POP, int(v.size()-1), v.back()});
            v.pop_back();
            heapify(0);
        }

        void buildHeap(std::vector<int> &a) {
            for (int c: a) push(c);
        }
        
        void UISWAPUI(node &a, node& b){
            startNodeMovement(a, b.currentPos, 2);
            startNodeMovement(b, a.currentPos, 2);
        }

        void UISWAPNODE(int i, int j){
            std::swap(nodelist[i], nodelist[j]);
        }

        void UIHIGHLIGHT(int i, int j){
            if (i>=0) {
                startNodeColor(nodelist[i], sf::Color::Red, 0.5);
            }
            if (j>=0 && j!=i) {
                startNodeColor(nodelist[j], sf::Color::Red, 0.5);
            }
        }

        void UIUNHIGHLIGHT(int i, int j){
            if (i>=0) {
                startNodeColor(nodelist[i], sf::Color::Yellow, 0.5);
            }
            if (j>=0 && j!=i) {
                startNodeColor(nodelist[j], sf::Color::Yellow, 0.5);
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
            node mem(position.x, position.y, 20, sf::Color::Cyan, sf::Color::Yellow, 5);
            mem.setLabel(std::to_string(x), 15);
            nodelist.push_back(mem);
            
            // INSERTEDGE
            edge canh(position.x, position.y, nodelist[(i-1)/2].currentPos.x, nodelist[(i-1)/2].currentPos.y, 
            sf::Color::White, 5);
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

    button insert_button(10, WINDOW_HEIGHT - 300, 100, 50, sf::Color::Cyan, "insert", 24);
    button pop_button(10, WINDOW_HEIGHT - 225, 100, 50, sf::Color::Cyan, "pop", 24);
    button next_button(250, WINDOW_HEIGHT - 225, 100, 50, sf::Color::Cyan, "next", 24);
    button previous_button(400, WINDOW_HEIGHT - 225, 100, 50, sf::Color::Cyan, "previous", 24);
    sf::RectangleShape slider({300, 30});
    slider.setPosition({600.f, float(WINDOW_HEIGHT) - 225.f});
    sf::RectangleShape slider_bg({300, 30});
    slider_bg.setPosition({600.f, float(WINDOW_HEIGHT) - 225.f});
    slider_bg.setOutlineThickness(5);
    slider_bg.setOutlineColor(sf::Color::Black);
    slider_bg.setFillColor(sf::Color::Magenta);
    slider.setFillColor(sf::Color::Blue);
    box valIn_box(135, WINDOW_HEIGHT - 300, 75, 50, sf::Color::Magenta, "0", 24);
    sf::RectangleShape bgmain({1500.f, 600.f});    
    bgmain.setOrigin(bgmain.getGeometricCenter());
    bgmain.setPosition({800 , 300});
    bgmain.setFillColor(sf::Color(85, 145, 56));
    
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
    
    int test = 100;
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
            
            window.draw(bgmain);
            insert_button.draw(window);
            pop_button.draw(window);
            valIn_box.draw(window);
            next_button.draw(window);
            previous_button.draw(window);
            window.draw(slider_bg);
            window.draw(slider);

            insert_button.isPress = insert_button.isClicked(sf::Mouse::getPosition(window));
            pop_button.isPress = pop_button.isClicked(sf::Mouse::getPosition(window));
            next_button.isPress = next_button.isClicked(sf::Mouse::getPosition(window));
            previous_button.isPress = previous_button.isClicked(sf::Mouse::getPosition(window));

            // BUTTONS
            if (!core_heap.hasAnimation && !core_heap.isAnimate){

            if (insert_button.isPress && !insert_button.onPress && core_heap.v.size()<31) {
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
                case ActionType::SWAPNODE:
                    // 1. Thực hiện undo hiệu ứng di chuyển UI
                    core_heap.UISWAPUI(core_heap.nodelist[animation.index1], core_heap.nodelist[animation.index2]);
                    // 2. Thực hiện undo việc đổi chỗ data trong mảng
                    core_heap.UISWAPNODE(animation.index1, animation.index2);
                    
                    // 3. "Nuốt" step SWAPUI nằm ngay trước nó để lần bấm Previous tiếp theo không bị khựng
                    if (core_heap.cur_step > 0 && 
                        core_heap.animation_queue[core_heap.cur_step - 1].type == ActionType::SWAPUI) {
                        core_heap.cur_step--; 
                    }
                    break;
                case ActionType::SWAPUI:
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
                    // 1. Chạy hiệu ứng di chuyển
                    core_heap.UISWAPUI(core_heap.nodelist[animation.index1], core_heap.nodelist[animation.index2]);
                    // 2. Chạy logic đổi data luôn trong cùng 1 click
                    core_heap.UISWAPNODE(animation.index1, animation.index2);
                    
                    // 3. "Nuốt" step SWAPNODE ngay phía sau (nếu có) để lần click Next tiếp theo bỏ qua nó
                    if (core_heap.cur_step + 1 < core_heap.animation_queue.size() && 
                        core_heap.animation_queue[core_heap.cur_step + 1].type == ActionType::SWAPNODE) {
                        ++core_heap.cur_step; 
                    }
                    break;
                case ActionType::SWAPNODE:
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
                    case ActionType::SWAPNODE:
                        core_heap.UISWAPNODE(animation.index1, animation.index2);
                        break;
                    case ActionType::SWAPUI:
                        core_heap.UISWAPUI(core_heap.nodelist[animation.index1], core_heap.nodelist[animation.index2]);
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