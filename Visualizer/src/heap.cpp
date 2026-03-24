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

enum ActionType { SWAPUI, SWAPNODE, HIGHLIGHT };

struct AnimationStep {
    ActionType type;
    int index1;
    int index2;
};

    struct min_heap {
        std::vector<int> v;
        std::vector<node> nodelist;
        std::vector<AnimationStep> animation_queue;
        bool hasAnimation{0};
        bool isAnimate{0};
        int cur_step{0};
        void heapify(int i) {
            int l = 2*i + 1;
            int r = 2*i +2;
            int next = i;
            if (l<v.size()) {
                if (v[l] < v[next]) next = l;
            }
            if (r<v.size()) {
                if (v[r] < v[next]) next = r;
            }
            if (next == i) return;
            std::swap(v[next], v[i]);
            animation_queue.push_back({SWAPUI, i, next});
            animation_queue.push_back({SWAPNODE, i, next});
            heapify(next);
        }

        void push(int x){
            v.push_back(x);
            sf::Vector2f position = getHeapNodePosition(v.size(), WINDOW_WIDTH, 50, 75);
            node mem(position.x, position.y, 20, sf::Color::Cyan, sf::Color::Yellow, 5);
            mem.setLabel(std::to_string(x), 15);
            nodelist.push_back(mem);

            int i = v.size()-1;
            while(i) {
                int par = (i-1)/2;
                if (v[i] < v[par]) {
                    std::swap(v[i], v[par]);
                    animation_queue.push_back({SWAPUI, i, par});
                    animation_queue.push_back({SWAPNODE, i, par});
                    i = par;
                } else break;
            }
        }

        void buildHeap(std::vector<int> &a) {
            for (int c: a) push(c);
        }
        
        void swapui(node &a, node& b){
            startNodeMovement(a, b.currentPos, 2);
            startNodeMovement(b, a.currentPos, 2);
        }

        void swapnode(int i, int j){
            std::swap(nodelist[i], nodelist[j]);
        }
    };

void heap_page(){

    button insert_button(0, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "insert", 24);
    button next_button(200, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "next", 24);
    button undo_button(400, WINDOW_HEIGHT - 100, 100, 50, sf::Color::Cyan, "undo", 24);
    
    sf::Clock clock;
    
    min_heap core_heap;
    bool isPress = false;
    bool onPress = false;
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
            next_button.draw(window);
            undo_button.draw(window);


            isPress = insert_button.isClicked(sf::Mouse::getPosition(window));
            if (isPress && !onPress && core_heap.hasAnimation == false) {
                core_heap.push(test--);
                core_heap.hasAnimation = true;
            }
            onPress = isPress;
            

            if (!core_heap.isAnimate) {
                if (core_heap.cur_step < core_heap.animation_queue.size()) {
                    auto animation = core_heap.animation_queue[core_heap.cur_step];
                    switch (animation.type)
                    {
                    case ActionType::SWAPNODE:
                        core_heap.swapnode(animation.index1, animation.index2);
                        break;
                    case ActionType::SWAPUI:
                        core_heap.swapui(core_heap.nodelist[animation.index1], core_heap.nodelist[animation.index2]);
                        break;
                    default:
                        break;
                    } 
                    ++core_heap.cur_step;
                } else core_heap.hasAnimation = false;
            }


            core_heap.isAnimate = false;
            for (auto& x: core_heap.nodelist) {
                x.updatePosition(dt);
                if (x.isMoving) core_heap.isAnimate = true;
                x.draw(window);
            }

            

            window.display();
        }
        
    }
}