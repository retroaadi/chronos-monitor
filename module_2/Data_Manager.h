#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "Telemetry_Record.h"
#include <vector>

using namespace std;

class DataManager
{
private:

    // Circular Queue configuration
    static const int QUEUE_SIZE = 5;

    // Circular Queue storage
    TelemetryRecord recentRecords[QUEUE_SIZE];

    // Complete telemetry history
    vector<TelemetryRecord> history;

    // Queue pointers
    int front;
    int rear;

    // Number of records currently stored in queue
    int count;

public:

    // Constructor
    DataManager();

    // Add a telemetry record
    void addRecord(const TelemetryRecord& record);

    // Remove the oldest telemetry record
    void removeOldestRecord();

    // Display recent records
    void displayRecentRecords() const;

    // Display complete telemetry history
    void displayHistory() const;

    // Check whether queue is empty
    bool isEmpty() const;

    // Check whether queue is full
    bool isFull() const;

    // Return number of records in recent queue
    int getCount() const;

    // Return total number of records in history
    int getHistoryCount() const;

    // Remove all records
    void clearRecords();
};

#endif