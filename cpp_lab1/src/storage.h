#pragma once

#include <nlohmann/json.hpp>
#include <cctype>
#include <cstddef>
#include <string>

using Json = nlohmann::json;

inline std::string upper(std::string value) {
    for (char& ch : value) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return value;
}

struct Arguments {
    std::string items[4];
    std::size_t count = 0;

    std::size_t size() const { return count; }
    bool empty() const { return count == 0; }
    const std::string& operator[](std::size_t index) const { return items[index]; }

    bool pushBack(const std::string& value) {
        if (count == 4) return false;
        items[count++] = value;
        return true;
    }
};

enum class Command {
    Unknown, Help, Exit, Quit, Save, Create, Drop, Print,
    MCreate, MPush, MInsert, MGet, MSet, MDel, MLen, MRead,
    FCreate, FPush, FPushFront, FInsertBefore, FInsertAfter, FDel, FDelHead, FDelTail, FDelBefore, FDelAfter, FGet, FRead, FSearch,
    LCreate, LPush, LPushFront, LInsertBefore, LInsertAfter, LDel, LDelHead, LDelTail, LDelBefore, LDelAfter, LGet, LRead, LSearch,
    SCreate, SPush, SPop, SGet, SRead,
    QCreate, QPush, QPop, QGet, QRead,
    TCreate, TInsert, TGet, TRead, TSearch, TDel, IsMember
};


#include "array.h"
#include "singly_list.h"
#include "doubly_list.h"
#include "stack.h"
#include "queue.h"
#include "avl_tree.h"

bool loadArray(const Json& data, DynamicArray& array);
void saveArray(Json& db, const std::string& name, const DynamicArray& array);
bool loadSingly(const Json& data, SinglyList& list);
void saveSingly(Json& db, const std::string& name, const SinglyList& list);
bool loadDoubly(const Json& data, DoublyList& list);
void saveDoubly(Json& db, const std::string& name, const DoublyList& list);
bool loadStack(const Json& data, StackData& stack);
void saveStack(Json& db, const std::string& name, const StackData& stack);
bool loadQueue(const Json& data, QueueData& queue);
void saveQueue(Json& db, const std::string& name, const QueueData& queue);
bool loadTree(const Json& data, AVLTree& tree);
void saveTree(Json& db, const std::string& name, const AVLTree& tree);


bool hasContainer(const Json& db, const std::string& name, const std::string& type = "");
bool createContainer(Json& db, char type, const std::string& name);
bool dropContainer(Json& db, char type, const std::string& name);
void printContainer(const Json& db, char type, const std::string& name);

bool countIs(const Arguments& args, std::size_t count, const std::string& usage);
bool readIndex(const Arguments& args, std::size_t position, std::size_t& index);
