#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include "Alert.h"

class AlertManager
{
private:
    // Maximum number of pending alerts
    static const int MAX_ALERTS = 100;

    // Priority Queue implemented using a Binary Max Heap
    Alert priorityQueue[MAX_ALERTS];
    int priorityQueueSize;

    // Linked List node for alert history
    struct AlertNode
    {
        Alert alert;
        AlertNode* next;

        AlertNode(const Alert& a)
        {
            alert = a;
            next = nullptr;
        }
    };

    // Linked List pointers
    AlertNode* historyHead;
    AlertNode* historyTail;

    int historyCount;

    // Generates unique alert IDs
    int nextAlertID;

    // Heap operations
    void heapifyUp(int index);
    void heapifyDown(int index);
    void swapAlerts(Alert& first, Alert& second);

public:
    // Constructor and destructor
    AlertManager();
    ~AlertManager();

    // Process a telemetry record and generate an alert if required
    void processRecord(const TelemetryRecord& record);

    // Priority Queue operations
    bool isPriorityQueueEmpty() const;
    bool isPriorityQueueFull() const;

    void addToPriorityQueue(const Alert& alert);
    Alert getHighestPriorityAlert();

    void displayPriorityAlerts() const;

    // Linked List operations
    void addToHistory(const Alert& alert);
    void displayAlertHistory() const;

    // Information
    int getAlertCount() const;

    // Clear all alerts
    void clearAlerts();
};

#endif