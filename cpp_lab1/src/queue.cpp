#include "queue.h"
#include "storage.h"
#include <iostream>

void queuePush(QueueData& queue, const std::string& value) {
    QueueNode* node = new QueueNode{value, nullptr};
    if (queue.back == nullptr) queue.front = node;
    else queue.back->next = node;
    queue.back = node;
    ++queue.size;
}

bool queuePop(QueueData& queue, std::string& value) {
    if (queue.front == nullptr) return false;
    QueueNode* node = queue.front;
    value = node->value;
    queue.front = node->next;
    if (queue.front == nullptr) queue.back = nullptr;
    delete node;
    --queue.size;
    return true;
}

bool queuePeek(const QueueData& queue, std::string& value) {
    if (queue.front == nullptr) return false;
    value = queue.front->value;
    return true;
}

void clearQueue(QueueData& queue) {
    while (queue.front != nullptr) {
        QueueNode* node = queue.front;
        queue.front = node->next;
        delete node;
    }
    queue.back = nullptr;
    queue.size = 0;
}

void handleQueue(Command command, Json& db, const Arguments& args) {
    if (args.size() < 2 || !hasContainer(db, args[1], "queue")) {
        std::cout << "Очередь не найдена. Сначала используйте CREATE Q <имя>.\n";
        return;
    }

    QueueData queue;
    loadQueue(db[args[1]]["data"], queue);
    std::string value;
    bool changed = false;

    switch (command) {
        case Command::QPush:
            if (!countIs(args, 3, "QPUSH <имя> <значение>")) break;
            queuePush(queue, args[2]);
            std::cout << "-> " << args[2] << '\n';
            changed = true;
            break;
        case Command::QPop:
            if (!countIs(args, 2, "QPOP <имя>")) break;
            if (queuePop(queue, value)) {
                std::cout << value << '\n';
                changed = true;
            } else std::cout << "Ошибка: очередь пуста.\n";
            break;
        case Command::QGet:
            if (!countIs(args, 2, "QGET <имя>")) break;
            if (queuePeek(queue, value)) std::cout << value << '\n';
            else std::cout << "Ошибка: очередь пуста.\n";
            break;
        case Command::QRead:
            if (!countIs(args, 2, "QREAD <имя>")) break;
            std::cout << '[';
            {
                bool first = true;
                for (QueueNode* node = queue.front; node != nullptr; node = node->next) {
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

    if (changed) saveQueue(db, args[1], queue);
    clearQueue(queue);
}
