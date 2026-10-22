#pragma once

#include <cstddef>
#include <string>

struct DynamicArray {
    std::string* items = nullptr;
    std::size_t size = 0;
    std::size_t capacity = 0;
};

void arrayAppend(DynamicArray& array, const std::string& value);
bool arrayInsert(DynamicArray& array, std::size_t index, const std::string& value);
bool arrayGet(const DynamicArray& array, std::size_t index, std::string& value);
bool arraySet(DynamicArray& array, std::size_t index, const std::string& value);
bool arrayErase(DynamicArray& array, std::size_t index);
void clearArray(DynamicArray& array);
