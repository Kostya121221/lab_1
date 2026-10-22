#pragma once

#include <cstddef>
#include <string>

struct DoublyNode {
    std::string value;
    DoublyNode* previous = nullptr;
    DoublyNode* next = nullptr;
};

struct DoublyList {
    DoublyNode* head = nullptr;
    DoublyNode* tail = nullptr;
    std::size_t size = 0;
};

void doublyPushBack(DoublyList& list, const std::string& value);
void doublyPushFront(DoublyList& list, const std::string& value);
bool doublyInsertBefore(DoublyList& list, const std::string& target, const std::string& value);
bool doublyInsertAfter(DoublyList& list, const std::string& target, const std::string& value);
bool doublyEraseFront(DoublyList& list, std::string* removed = nullptr);
bool doublyEraseBack(DoublyList& list, std::string* removed = nullptr);
bool doublyEraseValue(DoublyList& list, const std::string& value);
bool doublyEraseBefore(DoublyList& list, const std::string& target);
bool doublyEraseAfter(DoublyList& list, const std::string& target);
bool doublyGet(const DoublyList& list, std::size_t index, std::string& value);
long long doublyFind(const DoublyList& list, const std::string& value);
void clearDoublyList(DoublyList& list);
