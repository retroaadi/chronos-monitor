#include "Alert_Manager.h"
#include <iostream>

using namespace std;


// ============================================================
// CONSTRUCTOR
// ============================================================

AlertManager::AlertManager()
{
    priorityQueueSize = 0;

    historyHead = nullptr;
    historyTail = nullptr;

    historyCount = 0;

    nextAlertID = 1;
}


// ============================================================
// DESTRUCTOR
// ============================================================

AlertManager::~AlertManager()
{
    clearAlerts();
}


// ============================================================
// SWAP TWO ALERTS
// ============================================================

void AlertManager::swapAlerts(Alert& first, Alert& second)
{
    Alert temp = first;
    first = second;
    second = temp;
}


// ============================================================
// HEAPIFY UP
// Maintains Max Heap property after insertion
// ============================================================

void AlertManager::heapifyUp(int index)
{
    while (index > 0)
    {
        // Calculate parent index
        int parent = (index - 1) / 2;

        // If parent already has greater or equal severity,
        // Max Heap property is satisfied.
        if (priorityQueue[parent].getSeverity() >=
            priorityQueue[index].getSeverity())
        {
            break;
        }

        // Otherwise, swap child with parent
        swapAlerts(priorityQueue[parent],
                   priorityQueue[index]);

        // Continue checking from the parent's position
        index = parent;
    }
}


// ============================================================
// HEAPIFY DOWN
// Maintains Max Heap property after removing root
// ============================================================

void AlertManager::heapifyDown(int index)
{
    while (true)
    {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;

        int largest = index;

        // Check left child
        if (leftChild < priorityQueueSize &&
            priorityQueue[leftChild].getSeverity() >
            priorityQueue[largest].getSeverity())
        {
            largest = leftChild;
        }

        // Check right child
        if (rightChild < priorityQueueSize &&
            priorityQueue[rightChild].getSeverity() >
            priorityQueue[largest].getSeverity())
        {
            largest = rightChild;
        }

        // Current node is already the largest
        if (largest == index)
        {
            break;
        }

        // Swap current node with larger child
        swapAlerts(priorityQueue[index],
                   priorityQueue[largest]);

        // Continue from the child's position
        index = largest;
    }
}


// ============================================================
// PROCESS TELEMETRY RECORD
// ============================================================

void AlertManager::processRecord(const TelemetryRecord& record)
{
    // Severity 0 means NORMAL.
    // Normal records do not generate alerts.
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
    // Every generated alert receives a unique ID.
    nextAlertID++;

    // Store the alert permanently in history.
    addToHistory(alert);

    // Add the alert to the pending priority queue.
    addToPriorityQueue(alert);
}


// ============================================================
// CHECK IF PRIORITY QUEUE IS EMPTY
// ============================================================

bool AlertManager::isPriorityQueueEmpty() const
{
    return priorityQueueSize == 0;
}


// ============================================================
// CHECK IF PRIORITY QUEUE IS FULL
// ============================================================

bool AlertManager::isPriorityQueueFull() const
{
    return priorityQueueSize == MAX_ALERTS;
}


// ============================================================
// ADD ALERT TO PRIORITY QUEUE
// ============================================================

void AlertManager::addToPriorityQueue(const Alert& alert)
{
    // Check capacity before insertion.
    if (isPriorityQueueFull())
    {
        cout << "\nPriority queue is full.\n";
        cout << "Alert ID " << alert.getAlertID()
             << " cannot be added to pending alerts.\n";

        return;
    }

    // Insert at the end of the heap.
    priorityQueue[priorityQueueSize] = alert;

    priorityQueueSize++;

    // Restore Max Heap property.
    heapifyUp(priorityQueueSize - 1);
}


// ============================================================
// GET HIGHEST PRIORITY ALERT
// Removes and returns the root of the Max Heap
// ============================================================

Alert AlertManager::getHighestPriorityAlert()
{
    // No pending alerts
    if (isPriorityQueueEmpty())
    {
        cout << "\nNo pending alerts.\n";

        // Return default Alert object
        return Alert();
    }

    // Root of Max Heap contains highest priority alert.
    Alert highestPriority = priorityQueue[0];

    // Move last element to root.
    priorityQueue[0] =
        priorityQueue[priorityQueueSize - 1];

    // Reduce heap size.
    priorityQueueSize--;

    // Restore Max Heap property.
    if (priorityQueueSize > 0)
    {
        heapifyDown(0);
    }

    return highestPriority;
}


// ============================================================
// DISPLAY PRIORITY QUEUE
// Displays alerts from highest to lowest priority
// without modifying the actual queue.
// ============================================================

void AlertManager::displayPriorityAlerts() const
{
    if (isPriorityQueueEmpty())
    {
        cout << "\nNo pending alerts.\n";
        return;
    }

    cout << "\n========================================\n";
    cout << "         PRIORITY ALERT QUEUE\n";
    cout << "========================================\n";

    // Create a temporary copy of only the heap.
    // This does NOT affect the original priority queue.
    Alert tempQueue[MAX_ALERTS];

    int tempSize = priorityQueueSize;

    for (int i = 0; i < tempSize; i++)
    {
        tempQueue[i] = priorityQueue[i];
    }

    // --------------------------------------------------------
    // Display alerts in priority order.
    //
    // We repeatedly remove the root from the temporary heap.
    // The actual priority queue remains unchanged.
    // --------------------------------------------------------

    while (tempSize > 0)
    {
        // Root contains the highest severity.
        Alert highest = tempQueue[0];

        highest.displayAlert();

        // Replace root with last element.
        tempQueue[0] = tempQueue[tempSize - 1];

        tempSize--;

        // Restore Max Heap property in temporary heap.
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

    cout << "========================================\n";
}


// ============================================================
// ADD ALERT TO LINKED LIST HISTORY
// ============================================================

void AlertManager::addToHistory(const Alert& alert)
{
    // Create a new node dynamically.
    AlertNode* newNode = new AlertNode(alert);

    // If list is empty
    if (historyHead == nullptr)
    {
        historyHead = newNode;
        historyTail = newNode;
    }
    else
    {
        // Add node at the end.
        historyTail->next = newNode;
        historyTail = newNode;
    }

    historyCount++;
}


// ============================================================
// DISPLAY ALERT HISTORY
// ============================================================

void AlertManager::displayAlertHistory() const
{
    if (historyHead == nullptr)
    {
        cout << "\nNo alert history available.\n";
        return;
    }

    cout << "\n========================================\n";
    cout << "             ALERT HISTORY\n";
    cout << "========================================\n";

    AlertNode* current = historyHead;

    // Traverse linked list
    while (current != nullptr)
    {
        current->alert.displayAlert();

        current = current->next;
    }

    cout << "========================================\n";
}


// ============================================================
// GET TOTAL ALERT COUNT
// ============================================================

int AlertManager::getAlertCount() const
{
    return historyCount;
}


// ============================================================
// CLEAR ALL ALERTS
// ============================================================

void AlertManager::clearAlerts()
{
    // Clear priority queue.
    // No dynamic memory is used by the heap itself.
    priorityQueueSize = 0;

    // Traverse linked list and release
    // every dynamically allocated node.
    AlertNode* current = historyHead;

    while (current != nullptr)
    {
        AlertNode* next = current->next;

        delete current;

        current = next;
    }

    // Reset linked list.
    historyHead = nullptr;
    historyTail = nullptr;

    historyCount = 0;

    // Restart alert IDs.
    nextAlertID = 1;
}