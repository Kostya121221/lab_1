#include "storage.h"

#include <iostream>

#include <stdexcept>

std::string jsonValue(const Json& item) {
    if (item.is_string()) return item.get<std::string>();
    if (item.is_number_integer() || item.is_number_unsigned() || item.is_number_float()) {
        return item.dump();
    }
    throw std::runtime_error("Значения контейнера должны быть строками или числами.");
}

bool loadArray(const Json& data, DynamicArray& array) {
    if (!data.is_array()) return false;
    for (const Json& value : data) arrayAppend(array, jsonValue(value));
    return true;
}

void saveArray(Json& db, const std::string& name, const DynamicArray& array) {
    Json data = Json::array();
    for (std::size_t i = 0; i < array.size; ++i) data.push_back(array.items[i]);
    db[name]["type"] = "array";
    db[name]["data"] = data;
}

bool loadSingly(const Json& data, SinglyList& list) {
    if (!data.is_array()) return false;
    for (const Json& value : data) singlyPushBack(list, jsonValue(value));
    return true;
}

void saveSingly(Json& db, const std::string& name, const SinglyList& list) {
    Json data = Json::array();
    for (SinglyNode* node = list.head; node != nullptr; node = node->next) {
        data.push_back(node->value);
    }
    db[name]["type"] = "flist";
    db[name]["data"] = data;
}

bool loadDoubly(const Json& data, DoublyList& list) {
    if (!data.is_array()) return false;
    for (const Json& value : data) doublyPushBack(list, jsonValue(value));
    return true;
}

void saveDoubly(Json& db, const std::string& name, const DoublyList& list) {
    Json data = Json::array();
    for (DoublyNode* node = list.head; node != nullptr; node = node->next) {
        data.push_back(node->value);
    }
    db[name]["type"] = "dlist";
    db[name]["data"] = data;
}

bool loadStack(const Json& data, StackData& stack) {
    if (!data.is_array()) return false;
    for (std::size_t i = data.size(); i > 0; --i) {
        stackPush(stack, jsonValue(data[i - 1]));
    }
    return true;
}

void saveStack(Json& db, const std::string& name, const StackData& stack) {
    Json data = Json::array();
    for (StackNode* node = stack.top; node != nullptr; node = node->next) {
        data.push_back(node->value);
    }
    db[name]["type"] = "stack";
    db[name]["data"] = data;
}

bool loadQueue(const Json& data, QueueData& queue) {
    if (!data.is_array()) return false;
    for (const Json& value : data) queuePush(queue, jsonValue(value));
    return true;
}

void saveQueue(Json& db, const std::string& name, const QueueData& queue) {
    Json data = Json::array();
    for (QueueNode* node = queue.front; node != nullptr; node = node->next) {
        data.push_back(node->value);
    }
    db[name]["type"] = "queue";
    db[name]["data"] = data;
}

void saveTreeNodes(Json& data, AVLNode* node) {
    if (node == nullptr) return;
    saveTreeNodes(data, node->left);
    data.push_back(node->value);
    saveTreeNodes(data, node->right);
}

bool loadTree(const Json& data, AVLTree& tree) {
    if (!data.is_array()) return false;
    for (const Json& value : data) avlInsert(tree, jsonValue(value));
    return true;
}

void saveTree(Json& db, const std::string& name, const AVLTree& tree) {
    Json data = Json::array();
    saveTreeNodes(data, tree.root);
    db[name]["type"] = "tree";
    db[name]["data"] = data;
}




const char* typeName(char type) {
    switch (type) {
        case 'M': return "array";
        case 'F': return "flist";
        case 'L': return "dlist";
        case 'S': return "stack";
        case 'Q': return "queue";
        case 'T': return "tree";
        default: return "";
    }
}

bool hasContainer(const Json& db, const std::string& name, const std::string& type) {
    if (!db.is_object() || !db.contains(name) || !db[name].is_object() ||
        !db[name].contains("type") || !db[name].contains("data")) {
        return false;
    }

    if (!type.empty() &&
        (!db[name]["type"].is_string() || db[name]["type"].get<std::string>() != type)) {
        return false;
    }

    return db[name]["data"].is_array();
}

bool createContainer(Json& db, char type, const std::string& name) {
    const char* nameType = typeName(type);
    if (*nameType == '\0' || name.empty() || db.contains(name)) return false;

    db[name] = Json{{"type", nameType}, {"data", Json::array()}};
    return true;
}

bool dropContainer(Json& db, char type, const std::string& name) {
    const char* nameType = typeName(type);
    if (*nameType == '\0' || !hasContainer(db, name, nameType)) return false;

    db.erase(name);
    return true;
}

void printContainer(const Json& db, char type, const std::string& name) {
    const char* nameType = typeName(type);
    if (*nameType == '\0' || !hasContainer(db, name, nameType)) {
        std::cout << "Контейнер не найден или тип указан неверно.\n";
        return;
    }

    const Json& data = db[name]["data"];
    std::cout << '[';
    for (std::size_t i = 0; i < data.size(); ++i) {
        if (i > 0) std::cout << ", ";
        if (data[i].is_string()) std::cout << data[i].get<std::string>();
        else std::cout << data[i].dump();
    }
    std::cout << "]\n";
}
