#include "array.h"
#include "storage.h"
#include <iostream>

void growArray(DynamicArray& array, std::size_t required) {
    if (array.capacity >= required) return;

    std::size_t capacity = array.capacity == 0 ? 4 : array.capacity;
    while (capacity < required) capacity *= 2;

    std::string* items = new std::string[capacity];
    for (std::size_t i = 0; i < array.size; ++i) items[i] = array.items[i];
    delete[] array.items;
    array.items = items;
    array.capacity = capacity;
}

void arrayAppend(DynamicArray& array, const std::string& value) {
    growArray(array, array.size + 1);
    array.items[array.size++] = value;
}

bool arrayInsert(DynamicArray& array, std::size_t index, const std::string& value) {
    if (index > array.size) return false;
    growArray(array, array.size + 1);
    for (std::size_t i = array.size; i > index; --i) array.items[i] = array.items[i - 1];
    array.items[index] = value;
    ++array.size;
    return true;
}

bool arrayGet(const DynamicArray& array, std::size_t index, std::string& value) {
    if (index >= array.size) return false;
    value = array.items[index];
    return true;
}

bool arraySet(DynamicArray& array, std::size_t index, const std::string& value) {
    if (index >= array.size) return false;
    array.items[index] = value;
    return true;
}

bool arrayErase(DynamicArray& array, std::size_t index) {
    if (index >= array.size) return false;
    for (std::size_t i = index; i + 1 < array.size; ++i) array.items[i] = array.items[i + 1];
    --array.size;
    return true;
}

void clearArray(DynamicArray& array) {
    delete[] array.items;
    array.items = nullptr;
    array.size = 0;
    array.capacity = 0;
}

void handleArray(Command command, Json& db, const Arguments& args) {
    if (args.size() < 2 || !hasContainer(db, args[1], "array")) {
        std::cout << "Массив не найден. Сначала используйте CREATE M <имя>.\n";
        return;
    }

    DynamicArray array;
    loadArray(db[args[1]]["data"], array);

    std::size_t index = 0;
    std::string value;
    bool changed = false;

    switch (command) {
        case Command::MPush:
            if (!countIs(args, 3, "MPUSH <имя> <значение>")) break;
            arrayAppend(array, args[2]);
            std::cout << "-> " << args[2] << '\n';
            changed = true;
            break;
        case Command::MInsert:
            if (!countIs(args, 4, "MINSERT <имя> <индекс> <значение>") ||
                !readIndex(args, 2, index)) break;
            if (arrayInsert(array, index, args[3])) {
                std::cout << "-> " << args[3] << '\n';
                changed = true;
            } else {
                std::cout << "Ошибка: индекс вне диапазона.\n";
            }
            break;
        case Command::MGet:
            if (!countIs(args, 3, "MGET <имя> <индекс>") || !readIndex(args, 2, index)) break;
            if (arrayGet(array, index, value)) std::cout << value << '\n';
            else std::cout << "Ошибка: индекс вне диапазона.\n";
            break;
        case Command::MSet:
            if (!countIs(args, 4, "MSET <имя> <индекс> <значение>") ||
                !readIndex(args, 2, index)) break;
            if (arraySet(array, index, args[3])) {
                std::cout << "OK\n";
                changed = true;
            } else {
                std::cout << "Ошибка: индекс вне диапазона.\n";
            }
            break;
        case Command::MDel:
            if (!countIs(args, 3, "MDEL <имя> <индекс>") || !readIndex(args, 2, index)) break;
            if (arrayErase(array, index)) {
                std::cout << "OK\n";
                changed = true;
            } else {
                std::cout << "Ошибка: индекс вне диапазона.\n";
            }
            break;
        case Command::MLen:
            if (countIs(args, 2, "MLEN <имя>")) std::cout << array.size << '\n';
            break;
        case Command::MRead:
            if (!countIs(args, 2, "MREAD <имя>")) break;
            std::cout << '[';
            for (std::size_t i = 0; i < array.size; ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << array.items[i];
            }
            std::cout << "]\n";
            break;
        default:
            break;
    }

    if (changed) saveArray(db, args[1], array);
    clearArray(array);
}
