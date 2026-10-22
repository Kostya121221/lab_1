#include "stack.h"
#include "storage.h"
#include <iostream>

void stackPush(StackData& stack, const std::string& value) {
    stack.top = new StackNode{value, stack.top};
    ++stack.size;
}

bool stackPop(StackData& stack, std::string& value) {
    if (stack.top == nullptr) return false;
    StackNode* node = stack.top;
    value = node->value;
    stack.top = node->next;
    delete node;
    --stack.size;
    return true;
}

bool stackPeek(const StackData& stack, std::string& value) {
    if (stack.top == nullptr) return false;
    value = stack.top->value;
    return true;
}

void clearStack(StackData& stack) {
    while (stack.top != nullptr) {
        StackNode* node = stack.top;
        stack.top = node->next;
        delete node;
    }
    stack.size = 0;
}

void handleStack(Command command, Json& db, const Arguments& args) {
    if (args.size() < 2 || !hasContainer(db, args[1], "stack")) {
        std::cout << "Стек не найден. Сначала используйте CREATE S <имя>.\n";
        return;
    }

    StackData stack;
    loadStack(db[args[1]]["data"], stack);
    std::string value;
    bool changed = false;

    switch (command) {
        case Command::SPush:
            if (!countIs(args, 3, "SPUSH <имя> <значение>")) break;
            stackPush(stack, args[2]);
            std::cout << "-> " << args[2] << '\n';
            changed = true;
            break;
        case Command::SPop:
            if (!countIs(args, 2, "SPOP <имя>")) break;
            if (stackPop(stack, value)) {
                std::cout << value << '\n';
                changed = true;
            } else std::cout << "Ошибка: стек пуст.\n";
            break;
        case Command::SGet:
            if (!countIs(args, 2, "SGET <имя>")) break;
            if (stackPeek(stack, value)) std::cout << value << '\n';
            else std::cout << "Ошибка: стек пуст.\n";
            break;
        case Command::SRead:
            if (!countIs(args, 2, "SREAD <имя>")) break;
            std::cout << '[';
            {
                bool first = true;
                for (StackNode* node = stack.top; node != nullptr; node = node->next) {
                    if (!first) std::cout << ", ";
                    std::cout << node->value;
                    first = false;
                }
            }
            std::cout << "]\n";
            break;
        default:
            break;
    }

    if (changed) saveStack(db, args[1], stack);
    clearStack(stack);
}
