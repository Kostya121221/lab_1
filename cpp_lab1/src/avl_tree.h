#pragma once

#include <cstddef>
#include <string>

struct AVLNode {
    std::string value;
    AVLNode* left = nullptr;
    AVLNode* right = nullptr;
    int height = 1;
};

struct AVLTree {
    AVLNode* root = nullptr;
    std::size_t size = 0;
};

bool avlInsert(AVLTree& tree, const std::string& value);
bool avlContains(const AVLTree& tree, const std::string& value);
bool avlErase(AVLTree& tree, const std::string& value);
void clearAVLTree(AVLTree& tree);
