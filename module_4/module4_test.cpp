
#include <iostream>
#include <algorithm>
#include <vector>

#include "Telemetry_Record.h"
#include "Data_Manager.h"
#include "Search_Manager.h"

using namespace std;

int main()
{
    DataManager dataManager;
    SearchManager searchManager;

    // Insert sample records into Module 2
    dataManager.addRecord(
        TelemetryRecord(1, "CPU", 40.0, "10:00:00"));

    dataManager.addRecord(
        TelemetryRecord(2, "Memory", 80.0, "10:00:01"));

    dataManager.addRecord(
        TelemetryRecord(3, "Network", 95.0, "10:00:02"));

    dataManager.addRecord(
        TelemetryRecord(4, "CPU", 90.0, "10:00:03"));

    dataManager.addRecord(
        TelemetryRecord(5, "Memory", 50.0, "10:00:04"));

    dataManager.addRecord(
        TelemetryRecord(6, "Network", 60.0, "10:00:05"));

    int choice;
    int recordID;

    do
    {
        cout << "\n===== MODULE 4: SEARCH TEST =====\n";
        cout << "1. Display complete history\n";
        cout << "2. Linear Search\n";
        cout << "3. Binary Search\n";
        cout << "4. Sort history by Record ID\n";
        cout << "5. Display recent records\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        // Access Module 2 complete history
        const vector<TelemetryRecord>& history =
            dataManager.getHistory();

        switch (choice)
        {
        case 1:
            dataManager.displayHistory();
            break;

        case 2:
        {
            cout << "Enter Record ID: ";
            cin >> recordID;

            int index =
                searchManager.linearSearch(history, recordID);

            if (index != -1)
            {
                cout << "\nRecord found:\n";
                history[index].displayRecord();
            }
            else
            {
                cout << "\nRecord not found.\n";
            }

            break;
        }

        case 3:
        {
            bool sorted = is_sorted(
                history.begin(),
                history.end(),
                [](const TelemetryRecord& a,
                   const TelemetryRecord& b)
                {
                    return a.read_Record_ID() <
                           b.read_Record_ID();
                });

            if (!sorted)
            {
                cout << "\nHistory is not sorted.\n";
                cout << "Choose option 4 first.\n";
                break;
            }

            cout << "Enter Record ID: ";
            cin >> recordID;

            int index =
                searchManager.binarySearch(history, recordID);

            if (index != -1)
            {
                cout << "\nRecord found:\n";
                history[index].displayRecord();
            }
            else
            {
                cout << "\nRecord not found.\n";
            }

            break;
        }

        case 4:
        {
            // Sort the actual stored history
            vector<TelemetryRecord>& mutableHistory =
                const_cast<vector<TelemetryRecord>&>(history);

            sort(
                mutableHistory.begin(),
                mutableHistory.end(),
                [](const TelemetryRecord& a,
                   const TelemetryRecord& b)
                {
                    return a.read_Record_ID() <
                           b.read_Record_ID();
                });

            cout << "\nHistory sorted by Record ID.\n";
            break;
        }

        case 5:
            dataManager.displayRecentRecords();
            break;

        case 6:
            cout << "\nExiting Module 4 test.\n";
            break;

        default:
            cout << "\nInvalid choice.\n";
        }

    } while (choice != 6);

    return 0;
}
