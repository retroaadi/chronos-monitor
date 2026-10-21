#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include "TelemetryRecord.h"

class CircularQueue
{
private:
    TelemetryRecord* records;

    int capacity;
    int front;
    int rear;
    int count;

public:
    CircularQueue(int size = 10);
    ~CircularQueue();

    void enqueue(const TelemetryRecord& record);

    bool dequeue(TelemetryRecord& record);

    bool isEmpty() const;
    bool isFull() const;

    int size() const;
    int getCapacity() const;

    void display() const;
    void clear();
};

#endif