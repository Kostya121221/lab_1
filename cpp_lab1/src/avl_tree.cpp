#include "avl_tree.h"
#include "storage.h"
#include <algorithm>
#include <iostream>

int avlHeight(AVLNode* node) {
    return node == nullptr ? 0 : node->height;
}

void updateAVLHeight(AVLNode* node) {
    node->height = 1 + std::max(avlHeight(node->left), avlHeight(node->right));
}

int avlBalance(AVLNode* node) {
    return node == nullptr ? 0 : avlHeight(node->left) - avlHeight(node->right);
}

AVLNode* rotateRight(AVLNode* root) {
    AVLNode* left = root->left;
    root->left = left->right;
    left->right = root;
    updateAVLHeight(root);
    updateAVLHeight(left);
    return left;
}

AVLNode* rotateLeft(AVLNode* root) {
    AVLNode* right = root->right;
    root->right = right->left;
    right->left = root;
    updateAVLHeight(root);
    updateAVLHeight(right);
    return right;
}

AVLNode* balanceAVLNode(AVLNode* root) {
    if (root == nullptr) return nullptr;
    updateAVLHeight(root);

    if (avlBalance(root) > 1) {
        if (avlBalance(root->left) < 0) root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    if (avlBalance(root) < -1) {
        if (avlBalance(root->right) > 0) root->right = rotateRight(root->right);
        return rotateLeft(root);
    }
    return root;
}

AVLNode* insertAVLNode(AVLNode* root, const std::string& value, bool& inserted) {
    if (root == nullptr) {
        inserted = true;
        return new AVLNode{value, nullptr, nullptr, 1};
    }

    if (value < root->value) root->left = insertAVLNode(root->left, value, inserted);
    else if (value > root->value) root->right = insertAVLNode(root->right, value, inserted);
    else return root;
    return balanceAVLNode(root);
}

AVLNode* smallestAVLNode(AVLNode* node) {
    while (node != nullptr && node->left != nullptr) node = node->left;
    return node;
}

AVLNode* eraseAVLNode(AVLNode* root, const std::string& value, bool& erased) {
    if (root == nullptr) return nullptr;

    if (value < root->value) root->left = eraseAVLNode(root->left, value, erased);
    else if (value > root->value) root->right = eraseAVLNode(root->right, value, erased);
    else {
        erased = true;
        if (root->left == nullptr || root->right == nullptr) {
            AVLNode* child = root->left != nullptr ? root->left : root->right;
            delete root;
            return child;
        }

        AVLNode* successor = smallestAVLNode(root->right);
        root->value = successor->value;
        bool ignored = false;
        root->right = eraseAVLNode(root->right, successor->value, ignored);
    }
    return balanceAVLNode(root);
}

void deleteAVLNodes(AVLNode* root) {
    if (root == nullptr) return;
    deleteAVLNodes(root->left);
    deleteAVLNodes(root->right);
    delete root;
}

bool avlInsert(AVLTree& tree, const std::string& value) {
    bool inserted = false;
    tree.root = insertAVLNode(tree.root, value, inserted);
    if (inserted) ++tree.size;
    return inserted;
}

bool avlContains(const AVLTree& tree, const std::string& value) {
    AVLNode* node = tree.root;
    while (node != nullptr) {
        if (value == node->value) return true;
        node = value < node->value ? node->left : node->right;
    }
    return false;
}

bool avlErase(AVLTree& tree, const std::string& value) {
    bool erased = false;
    tree.root = eraseAVLNode(tree.root, value, erased);
    if (erased) --tree.size;
    return erased;
}

void clearAVLTree(AVLTree& tree) {
    deleteAVLNodes(tree.root);
    tree.root = nullptr;
    tree.size = 0;
}

void printAVLValues(AVLNode* node, bool& first) {
    if (node == nullptr) return;
    if (!first) std::cout << ", ";
    std::cout << node->value;
    first = false;
    printAVLValues(node->left, first);
    printAVLValues(node->right, first);
}

void handleTree(Command command, Json& db, const Arguments& args) {
    if (args.size() < 2 || !hasContainer(db, args[1], "tree")) {
        std::cout << "AVL-дерево не найдено. Сначала используйте CREATE T <имя>.\n";
        return;
    }

    AVLTree tree;
    loadTree(db[args[1]]["data"], tree);
    bool changed = false;

    switch (command) {
        case Command::TInsert:
            if (!countIs(args, 3, "TINSERT <имя> <значение>")) break;
            if (avlInsert(tree, args[2])) {
                std::cout << "-> " << args[2] << '\n';
                changed = true;
            } else std::cout << "Такое значение уже есть в AVL-дереве.\n";
            break;
        case Command::TDel:
            if (!countIs(args, 3, "TDEL <имя> <значение>")) break;
            if (avlErase(tree, args[2])) {
                std::cout << "OK\n";
                changed = true;
            } else std::cout << "Значение не найдено.\n";
            break;
        case Command::TSearch:
        case Command::IsMember:
            if (!countIs(args, 3, args[0] + " <имя> <значение>")) break;
            std::cout << (avlContains(tree, args[2]) ? "TRUE" : "FALSE") << '\n';
            break;
        case Command::TGet:
        case Command::TRead:
            if (!countIs(args, 2, args[0] + " <имя>")) break;
            std::cout << '[';
            {
                bool first = true;
                printAVLValues(tree.root, first);
            }
            std::cout << "]\n";
            break;
        default:
            break;
    }

    if (changed) saveTree(db, args[1], tree);
    clearAVLTree(tree);
}
