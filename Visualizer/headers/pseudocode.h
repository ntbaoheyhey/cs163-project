#pragma once
#include <string>
#include <vector>

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
    }
    };
};