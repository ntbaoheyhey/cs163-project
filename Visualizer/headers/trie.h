#pragma once
#include <SFML/Graphics.hpp>    
#include <SFML/System.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "global.h"
#include "../objects/canvas.h"

class node;
class edge;
class RoundedRectangleShape;

const int LOWERCASE_CHAR = 26;

struct NodeTrie{
    NodeTrie *pnext[LOWERCASE_CHAR];
    bool isend;
    int block_need;
    bool isleaf;
    node* visual_node;  
    edge* visual_edge[LOWERCASE_CHAR];
    
    NodeTrie();
};

// --- Operation Type ---
enum class OperationType {
    None,
    Add,
    Delete,
    Find
};

// --- Animation Step ---
enum class StepType {
    Move,     // Di chuyển qua node đã tồn tại (chỉ highlight)
    Lerp,     // Tạo node mới vào data, dịch chuyển các node cũ
    Create,   // Hiện node mới ngay lập tức, highlight
    MarkEnd,  // Đánh dấu node là cuối từ

    NotFound,          // Tìm kiếm thất bại: tắt highlight node cuối
    Found,             // Tìm kiếm thành công: highlight xanh cho node cuối (dành cho bộ Find)
    UnmarkEnd,         // Tắt cờ isend và đưa về màu thường
    DeleteNodeMark,    // Đóng băng con, đánh dấu cha bằng màu Delete
    DeleteNodeNotMark, // Đóng băng con, không đổi màu cha
    DeleteLerp         // Cắt nhánh tháo vào túi ảo và co gọn Layout
};

struct AnimStep {
    StepType type;
    NodeTrie* node;   // Move/MarkEnd: node cần highlight
                      // Lerp/Create : node CHA (để tìm pnext[char_id])
    int char_id;      // Lerp/Create : chỉ số ký tự trong pnext[]
    NodeTrie* stored_subtree; // Dùng để cất giữ nhánh khi bị tháo lúc Back
};

node* create_node(int block_x, int block_y, bool isend = false);

struct Trie{
    NodeTrie* root;

    Trie();
    
    // Logic function
    void init();
    void add(std::string s);
    bool find(std::string s);
    void remove(std::string s);
    void clear();
    
    // Animation function
    std::vector<AnimStep> add_with_anim(std::string s);
    std::vector<AnimStep> remove_with_anim(std::string s);
    void highlight_node(NodeTrie* node, sf::Color color);
    void unhighlight_node(NodeTrie* node);
    void update_edges(NodeTrie* pnode);   // Cập nhật endpoints của toàn bộ edge
    
    // Recursive support function
    bool cal_block(NodeTrie *pnode);
    void cre_node(NodeTrie *pnode, int block_x, int block_y, int char_branch = -1);
    void cre_edge(NodeTrie *pnode);
    void drawing(NodeTrie *pnode, sf::RenderWindow& window);
    void clear_travese(NodeTrie *pnode, bool isRoot = false);
    
    // Visual function
    void calculate_block();
    void create_visual_node();
    void create_visual_edge();
    void create_visual();                 // Tính lại toàn bộ layout + animate dịch chuyển
    void create_visual_lerp(float speed); // Tính lại layout, kích hoạt lerp với tốc độ cho trước
    
    void draw(sf::RenderWindow& window);
};


void trie_page();