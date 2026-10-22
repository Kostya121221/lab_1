#include "doubly_list.h"
#include "storage.h"
#include <iostream>

DoublyNode* findDoublyNode(const DoublyList& list, const std::string& value) {
    DoublyNode* node = list.head;
    while (node != nullptr && node->value != value) node = node->next;
    return node;
}

void insertDoublyBetween(DoublyList& list, DoublyNode* previous,
                                DoublyNode* next, const std::string& value) {
    DoublyNode* added = new DoublyNode{value, previous, next};
    if (previous == nullptr) list.head = added;
    else previous->next = added;
    if (next == nullptr) list.tail = added;
    else next->previous = added;
    ++list.size;
}

void eraseDoublyNode(DoublyList& list, DoublyNode* node,
                            std::string* removed = nullptr) {
    if (removed != nullptr) *removed = node->value;
    if (node->previous == nullptr) list.head = node->next;
    else node->previous->next = node->next;
    if (node->next == nullptr) list.tail = node->previous;
    else node->next->previous = node->previous;
    delete node;
    --list.size;
}

void doublyPushBack(DoublyList& list, const std::string& value) {
    insertDoublyBetween(list, list.tail, nullptr, value);
}

void doublyPushFront(DoublyList& list, const std::string& value) {
    insertDoublyBetween(list, nullptr, list.head, value);
}

bool doublyInsertBefore(DoublyList& list, const std::string& target, const std::string& value) {
    DoublyNode* targetNode = findDoublyNode(list, target);
    if (targetNode == nullptr) return false;
    insertDoublyBetween(list, targetNode->previous, targetNode, value);
    return true;
}

bool doublyInsertAfter(DoublyList& list, const std::string& target, const std::string& value) {
    DoublyNode* targetNode = findDoublyNode(list, target);
    if (targetNode == nullptr) return false;
    insertDoublyBetween(list, targetNode, targetNode->next, value);
    return true;
}

bool doublyEraseFront(DoublyList& list, std::string* removed) {
    if (list.head == nullptr) return false;
    eraseDoublyNode(list, list.head, removed);
    return true;
}

bool doublyEraseBack(DoublyList& list, std::string* removed) {
    if (list.tail == nullptr) return false;
    eraseDoublyNode(list, list.tail, removed);
    return true;
}

bool doublyEraseValue(DoublyList& list, const std::string& value) {
    DoublyNode* node = findDoublyNode(list, value);
    if (node == nullptr) return false;
    eraseDoublyNode(list, node);
    return true;
}

bool doublyEraseBefore(DoublyList& list, const std::string& target) {
    DoublyNode* node = findDoublyNode(list, target);
    if (node == nullptr || node->previous == nullptr) return false;
    eraseDoublyNode(list, node->previous);
    return true;
}

bool doublyEraseAfter(DoublyList& list, const std::string& target) {
    DoublyNode* node = findDoublyNode(list, target);
    if (node == nullptr || node->next == nullptr) return false;
    eraseDoublyNode(list, node->next);
    return true;
}

bool doublyGet(const DoublyList& list, std::size_t index, std::string& value) {
    if (index >= list.size) return false;

    DoublyNode* node;
    if (index < list.size / 2) {
        node = list.head;
        while (index-- > 0) node = node->next;
    } else {
        std::size_t steps = list.size - index - 1;
        node = list.tail;
        while (steps-- > 0) node = node->previous;
    }
    value = node->value;
    return true;
}

long long doublyFind(const DoublyList& list, const std::string& value) {
    long long index = 0;
    for (DoublyNode* node = list.head; node != nullptr; node = node->next, ++index)
        if (node->value == value) return index;
    return -1;
}

void clearDoublyList(DoublyList& list) {
    while (list.head != nullptr) eraseDoublyNode(list, list.head);
}

void handleDoubly(Command command, Json& db, const Arguments& args) {
    if (args.size() < 2 || !hasContainer(db, args[1], "dlist")) {
        std::cout << "Список не найден. Сначала используйте CREATE L <имя>.\n";
        return;
    }

    DoublyList list;
    loadDoubly(db[args[1]]["data"], list);

    std::string removed;
    std::string value;
    std::size_t index = 0;
    bool changed = false;

    switch (command) {
        case Command::LPush:
            if (!countIs(args, 3, "LPUSH <имя> <значение>")) break;
            doublyPushBack(list, args[2]);
            std::cout << "-> " << args[2] << '\n';
            changed = true;
            break;
        case Command::LPushFront:
            if (!countIs(args, 3, "LPUSHFRONT <имя> <значение>")) break;
            doublyPushFront(list, args[2]);
            std::cout << "-> " << args[2] << '\n';
            changed = true;
            break;
        case Command::LInsertBefore:
            if (!countIs(args, 4, "LINSERTBEFORE <имя> <целевой элемент> <значение>")) break;
            if (doublyInsertBefore(list, args[2], args[3])) {
                std::cout << "-> " << args[3] << '\n';
                changed = true;
            } else std::cout << "Ошибка вставки: список или элемент не найден.\n";
            break;
        case Command::LInsertAfter:
            if (!countIs(args, 4, "LINSERTAFTER <имя> <целевой элемент> <значение>")) break;
            if (doublyInsertAfter(list, args[2], args[3])) {
                std::cout << "-> " << args[3] << '\n';
                changed = true;
            } else std::cout << "Ошибка вставки: список или элемент не найден.\n";
            break;
        case Command::LDel:
            if (!countIs(args, 3, "LDEL <имя> <значение>")) break;
            if (doublyEraseValue(list, args[2])) {
                std::cout << "OK\n";
                changed = true;
            } else std::cout << "Ошибка удаления: элемент не найден.\n";
            break;
        case Command::LDelHead:
            if (!countIs(args, 2, "LDELHEAD <имя>")) break;
            if (doublyEraseFront(list, &removed)) {
                std::cout << removed << '\n';
                changed = true;
            } else std::cout << "Ошибка: список пуст.\n";
            break;
        case Command::LDelTail:
            if (!countIs(args, 2, "LDELTAIL <имя>")) break;
            if (doublyEraseBack(list, &removed)) {
                std::cout << removed << '\n';
                changed = true;
            } else std::cout << "Ошибка: список пуст.\n";
            break;
        case Command::LDelBefore:
            if (!countIs(args, 3, "LDELBEFORE <имя> <целевой элемент>")) break;
            if (doublyEraseBefore(list, args[2])) {
                std::cout << "OK\n";
                changed = true;
            } else std::cout << "Ошибка удаления: нет целевого элемента или предыдущего соседа.\n";
            break;
        case Command::LDelAfter:
            if (!countIs(args, 3, "LDELAFTER <имя> <целевой элемент>")) break;
            if (doublyEraseAfter(list, args[2])) {
                std::cout << "OK\n";
                changed = true;
            } else std::cout << "Ошибка удаления: нет целевого элемента или следующего соседа.\n";
            break;
        case Command::LGet:
            if (!countIs(args, 3, "LGET <имя> <индекс>") || !readIndex(args, 2, index)) break;
            if (doublyGet(list, index, value)) std::cout << value << '\n';
            else std::cout << "Ошибка: индекс вне диапазона.\n";
            break;
        case Command::LRead:
            if (args.size() != 2 && args.size() != 3) {
                std::cout << "Использование: LREAD <имя> [REVERSE]\n";
                break;
            }
            std::cout << '[';
            {
                bool first = true;
                if (args.size() == 3 && upper(args[2]) == "REVERSE") {
                    for (DoublyNode* node = list.tail; node != nullptr; node = node->previous) {
                        if (!first) std::cout << ", ";
                        std::cout << node->value;
                        first = false;
                    }
                } else {
                    for (DoublyNode* node = list.head; node != nullptr; node = node->next) {
                        if (!first) std::cout << ", ";
                        std::cout << node->value;
                        first = false;
                    }
                }
            }
            std::cout << "]\n";
            break;
        case Command::LSearch: {
            if (!countIs(args, 3, "LSEARCH <имя> <значение>")) break;
            long long position = doublyFind(list, args[2]);
            if (position < 0) std::cout << "NOT_FOUND\n";
            else std::cout << position << '\n';
            break;
        }
        default:
            break;
    }

    if (changed) saveDoubly(db, args[1], list);
    clearDoublyList(list);
}
