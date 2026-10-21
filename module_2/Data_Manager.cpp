#include "Data_Manager.h"
#include <iostream>

using namespace std;

DataManager::DataManager()
{
    front = 0;
    rear = -1;
    count = 0;
}


// Add a telemetry record
void DataManager::addRecord(const TelemetryRecord& record)
{
    // Store record in complete history
    history.push_back(record);
    rear = (rear + 1) % QUEUE_SIZE;

    // Store the new record in recent record queue
    recentRecords[rear] = record;

    if (count < QUEUE_SIZE)
    {
        count++;
    }
    else
    {
        front = (front + 1) % QUEUE_SIZE;
    }
}

void DataManager::removeOldestRecord()
{
    if (isEmpty())
    {
        cout << "Queue is empty. No record to remove.\n";
        return;
    }
    front = (front + 1) % QUEUE_SIZE;

    count--;
    
    if (count == 0)
    {
        rear = -1;
        front = 0;
    }
}


// Display all recent records
void DataManager::displayRecentRecords() const
{
    if (isEmpty())
    {
        cout << "\nNo telemetry records stored.\n";
        return;
    }

    cout << "\n";
    cout << "       RECENT TELEMETRY RECORDS\n";
    cout << "\n";

    int currentIndex = front;

    for (int i = 0; i < count; i++)
    {
        recentRecords[currentIndex].displayRecord();
        currentIndex = (currentIndex + 1) % QUEUE_SIZE;
    }

    cout << "\n";
}


// Display complete telemetry history
void DataManager::displayHistory() const
{
    if (history.empty())
    {
        cout << "\nNo telemetry history available.\n";
        return;
    }

    cout << "\n";
    cout << "---COMPLETE TELEMETRY HISTORY---\n";
    cout << "\n";

    for (const auto& record : history)
    {
        record.displayRecord();
    }

    cout << "\n";
}


// Check whether queue is empty
bool DataManager::isEmpty() const
{
    return count == 0;
}


// Check whether queue is full
bool DataManager::isFull() const
{
    return count == QUEUE_SIZE;
}


// Return number of stored recent records
int DataManager::getCount() const
{
    return count;
}


// Return total number of historical records
int DataManager::getHistoryCount() const
{
    return static_cast<int>(history.size());
}


// Remove all records
void DataManager::clearRecords()
{
    front = 0;
    rear = -1;
    count = 0;

    history.clear();
}
