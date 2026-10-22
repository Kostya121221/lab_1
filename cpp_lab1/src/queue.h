#pragma once

#include <cstddef>
#include <string>

struct QueueNode {
    std::string value;
    QueueNode* next = nullptr;
};

struct QueueData {
    QueueNode* front = nullptr;
    QueueNode* back = nullptr;
    std::size_t size = 0;
};

void queuePush(QueueData& queue, const std::string& value);
bool queuePop(QueueData& queue, std::string& value);
bool queuePeek(const QueueData& queue, std::string& value);
void clearQueue(QueueData& queue);
