#pragma once

#include <cstddef>
#include <string>

struct StackNode {
    std::string value;
    StackNode* next = nullptr;
};

struct StackData {
    StackNode* top = nullptr;
    std::size_t size = 0;
};

void stackPush(StackData& stack, const std::string& value);
bool stackPop(StackData& stack, std::string& value);
bool stackPeek(const StackData& stack, std::string& value);
void clearStack(StackData& stack);
