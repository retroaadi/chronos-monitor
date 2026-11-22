#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>

#include "Telemetry_Generator.h"
#include "Telemetry_Record.h"
#include "Data_Manager.h"
#include "Alert_Manager.h"
#include "Search_Manager.h"
#include "Statistics_Manager.h"
#include "Export_Manager.h"

using namespace std;

int readInteger(const string& prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
            return value;

        cout << "Invalid input. Enter an integer.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void displaySearchResult(const vector<TelemetryRecord>& records, int index)
{
    if (index == -1)
    {
        cout << "\nRecord not found.\n";
        return;
    }

    cout << "\nRecord found:\n";
    records[index].displayRecord();
}

int main()
{
    // Keep one generator instance so Record IDs remain continuous.
    TelemetryGenerator telemetryGenerator;
    DataManager dataManager;
    AlertManager alertManager;
    SearchManager searchManager;
    StatisticsManager statisticsManager;
    ExportManager exportManager;

    int choice;

    do
    {
        cout << "\n========================================\n";
        cout << "       CHRONOS MONITOR - MAIN MENU\n";
        cout << "========================================\n";
        cout << "1. Generate telemetry automatically\n";
        cout << "2. Enter telemetry manually\n";
        cout << "3. Display recent telemetry records\n";
        cout << "4. Display complete telemetry history\n";
        cout << "5. Display pending priority alerts\n";
        cout << "6. Process highest-priority alert\n";
        cout << "7. Display complete alert history\n";
        cout << "8. Linear Search by Record ID\n";
        cout << "9. Binary Search by Record ID\n";
        cout << "10. Display sorted telemetry history\n";
        cout << "11. Display system counts\n";
        cout << "12. Statistics and Analytics\n";
        cout << "13. Export telemetry to CSV\n";
        cout << "14. Exit\n";
        cout << "========================================\n";

        choice = readInteger("Enter choice: ");

        switch (choice)
        {
        case 1:
        {
            TelemetryRecord record = telemetryGenerator.generateRecord();
            cout << "\nGenerated telemetry record:\n";
            record.displayRecord();

            dataManager.addRecord(record);
            alertManager.processRecord(record);
            cout << "\nRecord stored successfully.\n";
            break;
        }

        case 2:
        {
            TelemetryRecord record = telemetryGenerator.manualInput();
            cout << "\nEntered telemetry record:\n";
            record.displayRecord();

            dataManager.addRecord(record);
            alertManager.processRecord(record);
            cout << "\nRecord stored successfully.\n";
            break;
        }

        case 3:
            dataManager.displayRecentRecords();
            break;

        case 4:
            dataManager.displayHistory();
            break;

        case 5:
            alertManager.displayPriorityAlerts();
            break;

        case 6:
        {
            if (alertManager.isPriorityQueueEmpty())
            {
                cout << "\nNo pending alerts to process.\n";
            }
            else
            {
                Alert alert = alertManager.getHighestPriorityAlert();
                cout << "\nProcessed highest-priority alert:\n";
                alert.displayAlert();
            }
            break;
        }

        case 7:
            alertManager.displayAlertHistory();
            break;

        case 8:
        {
            const vector<TelemetryRecord>& history = dataManager.getHistory();
            int recordID = readInteger("Enter Record ID to search: ");
            int index = searchManager.linearSearch(history, recordID);
            displaySearchResult(history, index);
            break;
        }

        case 9:
        {
            // Binary search needs sorted data. Sort a copy to preserve Module 2 history.
            vector<TelemetryRecord> sortedHistory = dataManager.getHistory();

            sort(sortedHistory.begin(), sortedHistory.end(),
                 [](const TelemetryRecord& a, const TelemetryRecord& b)
                 {
                     return a.read_Record_ID() < b.read_Record_ID();
                 });

            int recordID = readInteger("Enter Record ID to search: ");
            int index = searchManager.binarySearch(sortedHistory, recordID);
            displaySearchResult(sortedHistory, index);
            break;
        }

        case 10:
        {
            // Display sorted copy; do not modify DataManager's stored history.
            vector<TelemetryRecord> sortedHistory = dataManager.getHistory();

            sort(sortedHistory.begin(), sortedHistory.end(),
                 [](const TelemetryRecord& a, const TelemetryRecord& b)
                 {
                     return a.read_Record_ID() < b.read_Record_ID();
                 });

            cout << "\n===== SORTED TELEMETRY HISTORY =====\n";
            if (sortedHistory.empty())
                cout << "No telemetry records available.\n";
            else
                for (const TelemetryRecord& record : sortedHistory)
                    record.displayRecord();

            break;
        }

        case 11:
            cout << "\n===== SYSTEM COUNTS =====\n";
            cout << "Total telemetry records: "
                 << dataManager.getHistoryCount() << endl;
            cout << "Recent records stored: "
                 << dataManager.getCount() << endl;
            cout << "Total alerts generated: "
                 << alertManager.getAlertCount() << endl;
            cout << "Pending alert queue: "
                 << (alertManager.isPriorityQueueEmpty() ? "Empty" : "Not empty")
                 << endl;
            break;

        case 12:
        {
            const vector<TelemetryRecord>& history = dataManager.getHistory();

            cout << "\n===== STATISTICS & ANALYTICS =====\n";
            cout << "Total records: "
                 << statisticsManager.getTotalRecords(history) << endl;
            cout << "Normal records: "
                 << statisticsManager.getNormalCount(history) << endl;
            cout << "Warning records: "
                 << statisticsManager.getWarningCount(history) << endl;
            cout << "Critical records: "
                 << statisticsManager.getCriticalCount(history) << endl;

            vector<string> metrics = {"CPU", "Memory", "Network", "ResponseTime", "ErrorCount"};
            cout << "\n----- METRIC-WISE STATISTICS -----\n";
            for (const string& metric : metrics)
            {
                int count = statisticsManager.getMetricCount(history, metric);
                cout << "\nMetric: " << metric << " | Records: " << count << endl;
                if (count > 0)
                {
                    cout << "Average: " << statisticsManager.getAverageValue(history, metric) << endl;
                    cout << "Maximum: " << statisticsManager.getMaximumValue(history, metric) << endl;
                    cout << "Minimum: " << statisticsManager.getMinimumValue(history, metric) << endl;
                }
                else
                    cout << "No records available for this metric.\n";
            }
            break;
        }

        case 13:
        {
            const vector<TelemetryRecord>& history = dataManager.getHistory();
            exportManager.exportTelemetryToCSV(history, "telemetry_export.csv");
            break;
        }

        case 14:
            cout << "\nExiting Chronos Monitor.\n";
            break;

        default:
            cout << "\nInvalid choice. Please try again.\n";
        }
    }
    while (choice != 14);

    return 0;
}
