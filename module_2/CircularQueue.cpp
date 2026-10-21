#include "CircularQueue.h"
#include <iostream>

using namespace std;


CircularQueue::CircularQueue(int size)
{
    if (size <= 0)
    {
        size = 10;
    }

    capacity = size;

    records = new TelemetryRecord[capacity];

    front = 0;
    rear = -1;
    count = 0;
}


CircularQueue::~CircularQueue()
{
    delete[] records;
}


void CircularQueue::enqueue(const TelemetryRecord& record)
{
    if (isFull())
    {
        front = (front + 1) % capacity;
        count--;
    }

    rear = (rear + 1) % capacity;

    records[rear] = record;

    count++;
}


bool CircularQueue::dequeue(TelemetryRecord& record)
{
    if (isEmpty())
    {
        return false;
    }

    record = records[front];

    front = (front + 1) % capacity;

    count--;

    return true;
}


bool CircularQueue::isEmpty() const
{
    return count == 0;
}


bool CircularQueue::isFull() const
{
    return count == capacity;
}


int CircularQueue::size() const
{
    return count;
}


int CircularQueue::getCapacity() const
{
    return capacity;
}


void CircularQueue::display() const
{
    if (isEmpty())
    {
        cout << "\nNo recent telemetry records available.\n";
        return;
    }

    cout << "\n---RECENT TELEMETRY---\n";

    int index = front;

    for (int i = 0; i < count; i++)
    {
        records[index].displayRecord();

        index = (index + 1) % capacity;
    }

    cout << "\n";
}


void CircularQueue::clear()
{
    front = 0;
    rear = -1;
    count = 0;
}
