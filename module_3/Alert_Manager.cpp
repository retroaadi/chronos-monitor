#include "Alert_Manager.h"
#include <iostream>

using namespace std;

AlertManager::AlertManager()
{
    priorityQueueSize = 0;

    historyHead = nullptr;
    historyTail = nullptr;

    historyCount = 0;

    nextAlertID = 1;
}

AlertManager::~AlertManager()
{
    clearAlerts();
}

// ~ SWAP TWO ALERTS

void AlertManager::swapAlerts(Alert& first, Alert& second)
{
    Alert temp = first;
    first = second;
    second = temp;
}

// HEAPIFY UP
// Maintains Max Heap property after insertion

void AlertManager::heapifyUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;

        if (priorityQueue[parent].getSeverity() >= priorityQueue[index].getSeverity())
        {
            break;
        }

        swapAlerts(priorityQueue[parent],
                   priorityQueue[index]);

        index = parent;
    }
}

// HEAPIFY DOWN
// Maintains Max Heap property after removing root

void AlertManager::heapifyDown(int index)
{
    while (true)
    {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;

        int largest = index;

        if (leftChild < priorityQueueSize &&
            priorityQueue[leftChild].getSeverity() > priorityQueue[largest].getSeverity())
        {
            largest = leftChild;
        }

        if (rightChild < priorityQueueSize && priorityQueue[rightChild].getSeverity() >
            priorityQueue[largest].getSeverity())
        {
            largest = rightChild;
        }

        if (largest == index)
        {
            break;
        }
        swapAlerts(priorityQueue[index],
                   priorityQueue[largest]);

        index = largest;
    }
}

// ~ PROCESS TELEMETRY RECORD

void AlertManager::processRecord(const TelemetryRecord& record)
{
    if (record.read_Severity() == 0)
{
    return;
}

Alert alert(
    nextAlertID,
    record.read_Record_ID(),
    record.read_Metric_Name(),
    record.read_Value(),
    record.read_Time_stamp(),
    record.read_Status(),
    record.read_Severity()
);
    nextAlertID++;

    addToHistory(alert);

    addToPriorityQueue(alert);
}

// ~ CHECK IF PRIORITY QUEUE IS EMPTY

bool AlertManager::isPriorityQueueEmpty() const
{
    return priorityQueueSize == 0;
}

// CHECK IF PRIORITY QUEUE IS FULL

bool AlertManager::isPriorityQueueFull() const
{
    return priorityQueueSize == MAX_ALERTS;
}

// ADD ALERT TO PRIORITY QUEUE

void AlertManager::addToPriorityQueue(const Alert& alert)
{
    if (isPriorityQueueFull())
    {
        cout << "\nPriority queue is full.\n";
        cout << "Alert ID " << alert.getAlertID()
             << " cannot be added to pending alerts.\n";

        return;
    }

    priorityQueue[priorityQueueSize] = alert;

    priorityQueueSize++;

    heapifyUp(priorityQueueSize - 1);
}


Alert AlertManager::getHighestPriorityAlert()
{
    if (isPriorityQueueEmpty())
    {
        cout << "\nNo pending alerts.\n";

        return Alert();
    }

    Alert highestPriority = priorityQueue[0];

    priorityQueue[0] =
        priorityQueue[priorityQueueSize - 1];

    priorityQueueSize--;

    if (priorityQueueSize > 0)
    {
        heapifyDown(0);
    }

    return highestPriority;
}

// DISPLAY PRIORITY QUEUE

void AlertManager::displayPriorityAlerts() const
{
    if (isPriorityQueueEmpty())
    {
        cout << "\nNo pending alerts.\n";
        return;
    }

    cout << "\n";
    cout << "         PRIORITY ALERT QUEUE\n";
    cout << "\n";

    Alert tempQueue[MAX_ALERTS];

    int tempSize = priorityQueueSize;

    for (int i = 0; i < tempSize; i++)
    {
        tempQueue[i] = priorityQueue[i];
    }

    // Display alerts in priority order.

    while (tempSize > 0)
    {
        Alert highest = tempQueue[0];

        highest.displayAlert();

        tempQueue[0] = tempQueue[tempSize - 1];

        tempSize--;

        int index = 0;

        while (true)
        {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;

            int largest = index;

            if (leftChild < tempSize &&
                tempQueue[leftChild].getSeverity() >
                tempQueue[largest].getSeverity())
            {
                largest = leftChild;
            }

            if (rightChild < tempSize &&
                tempQueue[rightChild].getSeverity() >
                tempQueue[largest].getSeverity())
            {
                largest = rightChild;
            }

            if (largest == index)
            {
                break;
            }

            Alert temp = tempQueue[index];
            tempQueue[index] = tempQueue[largest];
            tempQueue[largest] = temp;

            index = largest;
        }
    }

    cout << "\n";
}

// ADD ALERT TO LINKED LIST HISTORY

void AlertManager::addToHistory(const Alert& alert)
{
    AlertNode* newNode = new AlertNode(alert);

    if (historyHead == nullptr)
    {
        historyHead = newNode;
        historyTail = newNode;
    }
    else
    {
        historyTail->next = newNode;
        historyTail = newNode;
    }

    historyCount++;
}

// DISPLAY ALERT HISTORY

void AlertManager::displayAlertHistory() const
{
    if (historyHead == nullptr)
    {
        cout << "\nNo alert history available.\n";
        return;
    }

    cout << "\n";
    cout << "             ALERT HISTORY\n";
    cout << "\n";

    AlertNode* current = historyHead;

    while (current != nullptr)
    {
        current->alert.displayAlert();

        current = current->next;
    }

    cout << "\n";
}

// GET TOTAL ALERT COUNT

int AlertManager::getAlertCount() const
{
    return historyCount;
}

// CLEAR ALL ALERTS

void AlertManager::clearAlerts()
{
    priorityQueueSize = 0;

    AlertNode* current = historyHead;

    while (current != nullptr)
    {
        AlertNode* next = current->next;

        delete current;

        current = next;
    }

    historyHead = nullptr;
    historyTail = nullptr;

    historyCount = 0;

    nextAlertID = 1;
}
