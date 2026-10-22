#pragma once

#include <cstddef>
#include <string>

struct SinglyNode {
    std::string value;
    SinglyNode* next = nullptr;
};

struct SinglyList {
    SinglyNode* head = nullptr;
};

void singlyPushBack(SinglyList& list, const std::string& value);
void singlyPushFront(SinglyList& list, const std::string& value);
bool singlyInsertBefore(SinglyList& list, const std::string& target, const std::string& value);
bool singlyInsertAfter(SinglyList& list, const std::string& target, const std::string& value);
bool singlyEraseFront(SinglyList& list, std::string* removed = nullptr);
bool singlyEraseBack(SinglyList& list, std::string* removed = nullptr);
bool singlyEraseValue(SinglyList& list, const std::string& value);
bool singlyEraseBefore(SinglyList& list, const std::string& target);
bool singlyEraseAfter(SinglyList& list, const std::string& target);
bool singlyGet(const SinglyList& list, std::size_t index, std::string& value);
long long singlyFind(const SinglyList& list, const std::string& value);
void clearSinglyList(SinglyList& list);
