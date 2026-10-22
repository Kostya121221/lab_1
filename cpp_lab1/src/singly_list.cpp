#include "singly_list.h"
#include "storage.h"
#include <iostream>

SinglyNode* findSinglyNode(SinglyNode* head, const std::string& value) {
    while (head != nullptr && head->value != value) head = head->next;
    return head;
}

SinglyNode* addSinglyTail(SinglyNode* head, const std::string& value) {
    SinglyNode* added = new SinglyNode{value, nullptr};
    if (head == nullptr) return added;

    SinglyNode* last = head;
    while (last->next != nullptr) last = last->next;
    last->next = added;
    return head;
}

SinglyNode* addSinglyBefore(SinglyNode* head, const std::string& target
                                  ,const std::string& value, bool& inserted) {
    if (head == nullptr) return head;
    if (head->value == target) {
        inserted = true;
        return new SinglyNode{value, head};
    }

    SinglyNode* previous = head;
    while (previous->next != nullptr && previous->next->value != target)
        previous = previous->next;

    if (previous->next == nullptr) return head;
    previous->next = new SinglyNode{value, previous->next};
    inserted = true;
    return head;
}

SinglyNode* addSinglyAfter(SinglyNode* head, const std::string& target
                                 ,const std::string& value, bool& inserted) {
    SinglyNode* node = findSinglyNode(head, target);
    if (node == nullptr) return head;
    node->next = new SinglyNode{value, node->next};
    inserted = true;
    return head;
}

SinglyNode* popSinglyFront(SinglyNode* head, std::string* removed = nullptr) {
    if (head == nullptr) return nullptr;
    SinglyNode* next = head->next;
    if (removed != nullptr) *removed = head->value;
    delete head;
    return next;
}

SinglyNode* popSinglyTail(SinglyNode* head, std::string* removed = nullptr) {
    if (head == nullptr || head->next == nullptr) return popSinglyFront(head, removed);

    SinglyNode* previous = head;
    while (previous->next->next != nullptr) previous = previous->next;
    if (removed != nullptr) *removed = previous->next->value;
    delete previous->next;
    previous->next = nullptr;
    return head;
}

SinglyNode* eraseSinglyValue(SinglyNode* head, const std::string& value, bool& erased) {
    if (head == nullptr) return nullptr;
    if (head->value == value) {
        erased = true;
        return popSinglyFront(head);
    }

    SinglyNode* previous = head;
    while (previous->next != nullptr && previous->next->value != value)
        previous = previous->next;
    if (previous->next == nullptr) return head;

    SinglyNode* removed = previous->next;
    previous->next = removed->next;
    delete removed;
    erased = true;
    return head;
}

SinglyNode* eraseSinglyBefore(SinglyNode* head, const std::string& target, bool& erased) {
    if (head == nullptr || head->value == target) return head;
    if (head->next != nullptr && head->next->value == target) {
        erased = true;
        return popSinglyFront(head);
    }

    SinglyNode* previous = head;
    SinglyNode* node = head->next;
    while (node != nullptr && node->next != nullptr && node->next->value != target) {
        previous = node;
        node = node->next;
    }
    if (node == nullptr || node->next == nullptr) return head;

    previous->next = node->next;
    delete node;
    erased = true;
    return head;
}

SinglyNode* eraseSinglyAfter(SinglyNode* head, const std::string& target, bool& erased) {
    SinglyNode* node = findSinglyNode(head, target);
    if (node == nullptr || node->next == nullptr) return head;

    SinglyNode* removed = node->next;
    node->next = removed->next;
    delete removed;
    erased = true;
    return head;
}

void singlyPushBack(SinglyList& list, const std::string& value) {
    list.head = addSinglyTail(list.head, value);
}

void singlyPushFront(SinglyList& list, const std::string& value) {
    list.head = new SinglyNode{value, list.head};
}

bool singlyInsertBefore(SinglyList& list, const std::string& target, const std::string& value) {
    bool inserted = false;
    list.head = addSinglyBefore(list.head, target, value, inserted);
    return inserted;
}

bool singlyInsertAfter(SinglyList& list, const std::string& target, const std::string& value) {
    bool inserted = false;
    list.head = addSinglyAfter(list.head, target, value, inserted);
    return inserted;
}

bool singlyEraseFront(SinglyList& list, std::string* removed) {
    if (list.head == nullptr) return false;
    list.head = popSinglyFront(list.head, removed);
    return true;
}

bool singlyEraseBack(SinglyList& list, std::string* removed) {
    if (list.head == nullptr) return false;
    list.head = popSinglyTail(list.head, removed);
    return true;
}

bool singlyEraseValue(SinglyList& list, const std::string& value) {
    bool erased = false;
    list.head = eraseSinglyValue(list.head, value, erased);
    return erased;
}

bool singlyEraseBefore(SinglyList& list, const std::string& target) {
    bool erased = false;
    list.head = eraseSinglyBefore(list.head, target, erased);
    return erased;
}

bool singlyEraseAfter(SinglyList& list, const std::string& target) {
    bool erased = false;
    list.head = eraseSinglyAfter(list.head, target, erased);
    return erased;
}

bool singlyGet(const SinglyList& list, std::size_t index, std::string& value) {
    SinglyNode* node = list.head;
    while (node != nullptr && index > 0) {
        node = node->next;
        --index;
    }
    if (node == nullptr) return false;
    value = node->value;
    return true;
}

long long singlyFind(const SinglyList& list, const std::string& value) {
    long long index = 0;
    for (SinglyNode* node = list.head; node != nullptr; node = node->next, ++index)
        if (node->value == value) return index;
    return -1;
}

void clearSinglyList(SinglyList& list) {
    while (list.head != nullptr) list.head = popSinglyFront(list.head);
}

void printSinglyReverse(SinglyNode* node, bool& first) {
    if (node == nullptr) return;
    printSinglyReverse(node->next, first);
    if (!first) std::cout << ", ";
    std::cout << node->value;
    first = false;
}

void handleSingly(Command command, Json& db, const Arguments& args) {
    if (args.size() < 2 || !hasContainer(db, args[1], "flist")) {
        std::cout << "Список не найден. Сначала используйте CREATE F <имя>.\n";
        return;
    }

    SinglyList list;
    loadSingly(db[args[1]]["data"], list);

    std::string removed;
    std::string value;
    std::size_t index = 0;
    bool changed = false;

    switch (command) {
        case Command::FPush:
            if (!countIs(args, 3, "FPUSH <имя> <значение>")) break;
            singlyPushBack(list, args[2]);
            std::cout << "-> " << args[2] << '\n';
            changed = true;
            break;
        case Command::FPushFront:
            if (!countIs(args, 3, "FPUSHFRONT <имя> <значение>")) break;
            singlyPushFront(list, args[2]);
            std::cout << "-> " << args[2] << '\n';
            changed = true;
            break;
        case Command::FInsertBefore:
            if (!countIs(args, 4, "FINSERTBEFORE <имя> <целевой элемент> <значение>")) break;
            if (singlyInsertBefore(list, args[2], args[3])) {
                std::cout << "-> " << args[3] << '\n';
                changed = true;
            } else std::cout << "Ошибка вставки: список или элемент не найден.\n";
            break;
        case Command::FInsertAfter:
            if (!countIs(args, 4, "FINSERTAFTER <имя> <целевой элемент> <значение>")) break;
            if (singlyInsertAfter(list, args[2], args[3])) {
                std::cout << "-> " << args[3] << '\n';
                changed = true;
            } else std::cout << "Ошибка вставки: список или элемент не найден.\n";
            break;
        case Command::FDel:
            if (!countIs(args, 3, "FDEL <имя> <значение>")) break;
            if (singlyEraseValue(list, args[2])) {
                std::cout << "OK\n";
                changed = true;
            } else std::cout << "Ошибка удаления: элемент не найден.\n";
            break;
        case Command::FDelHead:
            if (!countIs(args, 2, "FDELHEAD <имя>")) break;
            if (singlyEraseFront(list, &removed)) {
                std::cout << removed << '\n';
                changed = true;
            } else std::cout << "Ошибка: список пуст.\n";
            break;
        case Command::FDelTail:
            if (!countIs(args, 2, "FDELTAIL <имя>")) break;
            if (singlyEraseBack(list, &removed)) {
                std::cout << removed << '\n';
                changed = true;
            } else std::cout << "Ошибка: список пуст.\n";
            break;
        case Command::FDelBefore:
            if (!countIs(args, 3, "FDELBEFORE <имя> <целевой элемент>")) break;
            if (singlyEraseBefore(list, args[2])) {
                std::cout << "OK\n";
                changed = true;
            } else std::cout << "Ошибка удаления: нет целевого элемента или предыдущего соседа.\n";
            break;
        case Command::FDelAfter:
            if (!countIs(args, 3, "FDELAFTER <имя> <целевой элемент>")) break;
            if (singlyEraseAfter(list, args[2])) {
                std::cout << "OK\n";
                changed = true;
            } else std::cout << "Ошибка удаления: нет целевого элемента или следующего соседа.\n";
            break;
        case Command::FGet:
            if (!countIs(args, 3, "FGET <имя> <индекс>") || !readIndex(args, 2, index)) break;
            if (singlyGet(list, index, value)) std::cout << value << '\n';
            else std::cout << "Ошибка: индекс вне диапазона.\n";
            break;
        case Command::FRead:
            if (args.size() != 2 && args.size() != 3) {
                std::cout << "Использование: FREAD <имя> [REVERSE]\n";
                break;
            }
            std::cout << '[';
            if (args.size() == 3 && upper(args[2]) == "REVERSE") {
                bool first = true;
                printSinglyReverse(list.head, first);
            } else {
                bool first = true;
                for (SinglyNode* node = list.head; node != nullptr; node = node->next) {
                    if (!first) std::cout << ", ";
                    std::cout << node->value;
                    first = false;
                }
            }
            std::cout << "]\n";
            break;
        case Command::FSearch: {
            if (!countIs(args, 3, "FSEARCH <имя> <значение>")) break;
            long long position = singlyFind(list, args[2]);
            if (position < 0) std::cout << "NOT_FOUND\n";
            else std::cout << position << '\n';
            break;
        }
        default:
            break;
    }

    if (changed) saveSingly(db, args[1], list);
    clearSinglyList(list);
}
