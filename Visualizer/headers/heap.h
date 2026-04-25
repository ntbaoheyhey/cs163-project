#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include "global.h"
#include "../objects/canvas.h"
#define WIN32_LEAN_AND_MEAN

void heap_page();

// Hàm mở file
    inline std::string openFileDialog() {
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
    inline sf::Vector2f getHeapNodePosition(int heapIndex, float windowWidth, float startY, float startX, float verticalSpacing) {
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
    inline void handleNumericInput(std::string& targetStr, char32_t unicode, size_t maxLength = 2) {
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
    inline void handleArrayInput(std::string& targetStr, char32_t unicode, size_t maxLength = 17) {
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

    inline int convert_str(std::string &s){
    int res = 0;
    for (char c: s){
        res = res*10 + (c-'0');
    }
    return res;
}

    inline std::vector<int> str_to_vec(std::string s){
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

        int peek() {
            animation_queue.push_back({HIGHLIGHT, 0, 0});
            codebox_queue.push_back(0);
            animation_queue.push_back({UNHIGHLIGHT, 0, 0});
            codebox_queue.push_back(1);
            return v[0];
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

    namespace PseudoCode {
    inline std::vector<std::vector<std::string>> heapCode = {
         // Min Heap
        {
   //std::string pushCode = 
        "v.push_back(x)\n"                 // Dòng 0: Insert vào cuối mảng
        "i = v.size() - 1\n"               // Dòng 1: (Chỉ định biến)
        "upheap(i):\n"                     // Dòng 2: (Bắt đầu vòng lặp)
        "  par = (i - 1) / 2\n"            // Dòng 3: Tính index node cha
        "  if (v[i] < v[par]):\n"          // Dòng 4: So sánh tính chất Min-Heap
        "      swap, upheap(par)\n"        // Dòng 5: Hoán vị nếu node con nhỏ hơn cha
        "  else: return\n"                 // Dòng 6: Dừng nếu đã thoả mãn Min-Heap
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    //std::string popCode = 
        "swap(v[0], v.back())\n"           // Dòng 0: Hoán vị root với phần tử cuối
        "v.pop_back()\n"                   // Dòng 1: Xóa phần tử cuối
        "downheap(i):\n"                   // Dòng 2: Bắt đầu khôi phục cây
        "  l = 2i + 1, r = 2i + 2\n"       // Dòng 3: Lấy index 2 node con
        "  next = index(min(v[i], v[l], v[r]))\n"    // Dòng 4: Tìm node có giá trị nhỏ nhất
        "  if (next != i): swap, downheap(next)\n" // Dòng 5: Đổi chỗ và đệ quy
        "  else: return\n"                 // Dòng 6: Dừng nếu root đã là nhỏ nhất
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    //std::string buildCode = 
        "i from n-1 to 0\n"                // Dòng 0: Lặp từ n đến 1
        "\n"                               // Dòng 1: Xóa phần tử cuối
        "downheap(i):\n"                   // Dòng 2: Bắt đầu khôi phục cây
        "  l = 2i + 1, r = 2i + 2\n"       // Dòng 3: Lấy index 2 node con
        "  next = index(min(v[i], v[l], v[r]))\n"    // Dòng 4: Tìm node có giá trị nhỏ nhất
        "  if (next != i): swap, downheap(next)\n" // Dòng 5: Đổi chỗ và đệ quy
        "  else: return\n"                 // Dòng 6: Dừng nếu root đã là nhỏ nhất
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    //std::string upCode = 
        "new val should be upheap\n"       // Dòng 0: Upheap
        "\n"                               // Dòng 1: (Chỉ định biến)
        "upheap(i):\n"                     // Dòng 2: (Bắt đầu vòng lặp)
        "  par = (i - 1) / 2\n"            // Dòng 3: Tính index node cha
        "  if (v[i] < v[par]):\n"          // Dòng 4: So sánh tính chất Min-Heap
        "      swap, upheap(par)\n"        // Dòng 5: Hoán vị nếu node con nhỏ hơn cha
        "  else: return\n"                 // Dòng 6: Dừng nếu đã thoả mãn Min-Heap
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    //std::string downCode = 
        "new val should be downheap\n"     // Dòng 0: Downheap
        "\n"                               // Dòng 1: Xóa phần tử cuối
        "downheap(i):\n"                   // Dòng 2: Bắt đầu khôi phục cây
        "  l = 2i + 1, r = 2i + 2\n"       // Dòng 3: Lấy index 2 node con
        "  next = index(min(v[i], v[l], v[r]))\n"    // Dòng 4: Tìm node có giá trị nhỏ nhất
        "  if (next != i): swap, downheap(next)\n" // Dòng 5: Đổi chỗ và đệ quy
        "  else: return\n"                 // Dòng 6: Dừng nếu root đã là nhỏ nhất
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    },
    // Max Heap
    {
   //std::string pushCode = 
        "v.push_back(x)\n"                 // Dòng 0: Insert vào cuối mảng
        "i = v.size() - 1\n"               // Dòng 1: (Chỉ định biến)
        "upheap(i):\n"                     // Dòng 2: (Bắt đầu vòng lặp)
        "  par = (i - 1) / 2\n"            // Dòng 3: Tính index node cha
        "  if (v[i] > v[par]):\n"          // Dòng 4: So sánh tính chất Min-Heap
        "      swap, upheap(par)\n"        // Dòng 5: Hoán vị nếu node con nhỏ hơn cha
        "  else: return\n"                 // Dòng 6: Dừng nếu đã thoả mãn Min-Heap
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    //std::string popCode = 
        "swap(v[0], v.back())\n"           // Dòng 0: Hoán vị root với phần tử cuối
        "v.pop_back()\n"                   // Dòng 1: Xóa phần tử cuối
        "downheap(i):\n"                   // Dòng 2: Bắt đầu khôi phục cây
        "  l = 2i + 1, r = 2i + 2\n"       // Dòng 3: Lấy index 2 node con
        "  next = index(max(v[i], v[l], v[r]))\n"    // Dòng 4: Tìm node có giá trị nhỏ nhất
        "  if (next != i): swap, downheap(next)\n" // Dòng 5: Đổi chỗ và đệ quy
        "  else: return\n"                 // Dòng 6: Dừng nếu root đã là nhỏ nhất
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    //std::string buildCode = 
        "i from n-1 to 0\n"                // Dòng 0: Lặp từ n đến 1
        "\n"                               // Dòng 1: Xóa phần tử cuối
        "downheap(i):\n"                   // Dòng 2: Bắt đầu khôi phục cây
        "  l = 2i + 1, r = 2i + 2\n"       // Dòng 3: Lấy index 2 node con
        "  next = index(max(v[i], v[l], v[r]))\n"    // Dòng 4: Tìm node có giá trị nhỏ nhất
        "  if (next != i): swap, downheap(next)\n" // Dòng 5: Đổi chỗ và đệ quy
        "  else: return\n"                 // Dòng 6: Dừng nếu root đã là nhỏ nhất
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    //std::string upCode = 
        "new val should be upheap\n"       // Dòng 0: Upheap
        "\n"                               // Dòng 1: (Chỉ định biến)
        "upheap(i):\n"                     // Dòng 2: (Bắt đầu vòng lặp)
        "  par = (i - 1) / 2\n"            // Dòng 3: Tính index node cha
        "  if (v[i] > v[par]):\n"          // Dòng 4: So sánh tính chất Min-Heap
        "      swap, upheap(par)\n"        // Dòng 5: Hoán vị nếu node con nhỏ hơn cha
        "  else: return\n"                 // Dòng 6: Dừng nếu đã thoả mãn Min-Heap
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    //std::string downCode = 
        "new val should be downheap\n"     // Dòng 0: Downheap
        "\n"                               // Dòng 1: Xóa phần tử cuối
        "downheap(i):\n"                   // Dòng 2: Bắt đầu khôi phục cây
        "  l = 2i + 1, r = 2i + 2\n"       // Dòng 3: Lấy index 2 node con
        "  next = index(max(v[i], v[l], v[r]))\n"    // Dòng 4: Tìm node có giá trị nhỏ nhất
        "  if (next != i): swap, downheap(next)\n" // Dòng 5: Đổi chỗ và đệ quy
        "  else: return\n"                 // Dòng 6: Dừng nếu root đã là nhỏ nhất
        "\n"                               // Dòng 7: (Dòng trống đệm)
        "Done",                            // Dòng 8: Kết thúc
    },
    {
        "Return v[0]\n"
        "Done",
    }
    };
};