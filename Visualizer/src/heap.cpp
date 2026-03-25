#include "../headers/heap.h"
#include <vector>

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

enum ActionType {INSERT, POP, SWAPUI, SWAPNODE, HIGHLIGHT, UNHIGHLIGHT};

    struct AnimationStep {
        ActionType type;
        int index1{-1};
        int index2{-1};
    };

    struct min_heap {
        std::vector<int> v;
        std::vector<node> nodelist;
        std::vector<edge> edgelist;
        std::vector<AnimationStep> animation_queue;
        bool hasAnimation{0};
        bool isAnimate{0};
        int cur_step{0};
        void heapify(int i) {
            if (v.size() == 0) return;
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
                return;
            }
            std::swap(v[next], v[i]);
            animation_queue.push_back({HIGHLIGHT, i, next});
            animation_queue.push_back({SWAPUI, i, next});
            animation_queue.push_back({UNHIGHLIGHT, i, next});
            animation_queue.push_back({SWAPNODE, i, next});
            heapify(next);
        }

        void push(int x){

            v.push_back(x);
            sf::Vector2f position = getHeapNodePosition(v.size(), WINDOW_WIDTH, 50, 75);
            node mem(position.x, position.y, 20, sf::Color::Cyan, sf::Color::Yellow, 5);
            mem.setLabel(std::to_string(x), 15);
            nodelist.push_back(mem);
            
            int i = v.size() - 1;
            edge canh(position.x, position.y, nodelist[(i-1)/2].currentPos.x, nodelist[(i-1)/2].currentPos.y, 
            sf::Color::White, 10);
            edgelist.push_back(canh);

            while(i) {
                int par = (i-1)/2;
                if (v[i] < v[par]) {
                    std::swap(v[i], v[par]);
                    animation_queue.push_back({HIGHLIGHT, i, par});
                    animation_queue.push_back({SWAPUI, i, par});
                    animation_queue.push_back({UNHIGHLIGHT, i, par});
                    animation_queue.push_back({SWAPNODE, i, par});
                    i = par;
                } else break;
            }
        }

        void pop(){
            std::swap(v.back(), v[0]);
            animation_queue.push_back({ActionType::HIGHLIGHT, int(0), int(v.size()-1)});
            animation_queue.push_back({SWAPUI, int(0), int(v.size()-1)});
            animation_queue.push_back({SWAPNODE, int(0), int(v.size()-1)});
            animation_queue.push_back({UNHIGHLIGHT, int(0), int(v.size()-1)});
            v.pop_back();
            animation_queue.push_back({POP, -1, -1});
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
                nodelist[i].setOutlineColor(sf::Color::Red);
            }
            if (j>=0) {
                nodelist[j].setOutlineColor(sf::Color::Red);
            }
        }

        void UIUNHIGHLIGHT(int i, int j){
            if (i>=0) {
                nodelist[i].setOutlineColor(sf::Color::Yellow);
            }
            if (j>=0) {
                nodelist[j].setOutlineColor(sf::Color::Yellow);
            }
        }

        void UIPOP(){
            nodelist.pop_back();
            edgelist.pop_back();
        }
    };

void heap_page(){

    button insert_button(0, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "insert", 24);
    button pop_button(200, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "pop", 24);
    button undo_button(400, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "undo", 24);
    
    sf::Clock clock;
    
    min_heap core_heap;
    bool isInsertPress = false;
    bool onInsertPress = false;
    bool isPopPress = false;
    bool onPopPress = false;
    bool isUndoPress = false;
    bool onUndoPress = false;
    int test = 100;
    while(window.isOpen()){

        float dt = clock.getElapsedTime().asSeconds();
        if (dt>=1.f/60.f){
            clock.restart();
            while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

            window.clear(sf::Color::Black);
            
            insert_button.draw(window);
            pop_button.draw(window);
            undo_button.draw(window);


            isInsertPress = insert_button.isClicked(sf::Mouse::getPosition(window));
            isPopPress = pop_button.isClicked(sf::Mouse::getPosition(window));
    
            if (isInsertPress && !onInsertPress && core_heap.hasAnimation == false) {
                core_heap.push(test--);
                core_heap.hasAnimation = true;
            }
            onInsertPress = isInsertPress;

            if (isPopPress && !onPopPress && core_heap.hasAnimation == false) {
                core_heap.pop();
            }
            onPopPress = isPopPress;
            

            if (!core_heap.isAnimate) {
                if (core_heap.cur_step < core_heap.animation_queue.size()) {
                    auto animation = core_heap.animation_queue[core_heap.cur_step];
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
                    default:
                        break;
                    } 
                    ++core_heap.cur_step;
                } else core_heap.hasAnimation = false;
            }


            core_heap.isAnimate = false;

            for (auto& c: core_heap.edgelist) {
                c.draw(window);
            }
            for (auto& x: core_heap.nodelist) {
                x.updatePosition(dt);
                if (x.isMoving) core_heap.isAnimate = true;
                x.draw(window);
            }

            

            window.display();
        }
        
    }
}