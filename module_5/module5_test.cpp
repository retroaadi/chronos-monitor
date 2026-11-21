#include <iostream>
#include <vector>

#include "Telemetry_Record.h"
#include "Data_Manager.h"
#include "Statistics_Manager.h"

using namespace std;

int main()
{
    DataManager dataManager;
    StatisticsManager statisticsManager;

    // --------------------------------------------------------
    // Insert sample telemetry records into Module 2
    // --------------------------------------------------------
    dataManager.addRecord(TelemetryRecord(1, "CPU", 40.0, "10:00:00"));
    dataManager.addRecord(TelemetryRecord(2, "CPU", 75.0, "10:00:01"));
    dataManager.addRecord(TelemetryRecord(3, "CPU", 90.0, "10:00:02"));

    dataManager.addRecord(TelemetryRecord(4, "Memory", 60.0, "10:00:03"));
    dataManager.addRecord(TelemetryRecord(5, "Memory", 80.0, "10:00:04"));

    dataManager.addRecord(TelemetryRecord(6, "Network", 95.0, "10:00:05"));

    // --------------------------------------------------------
    // Retrieve complete history from Module 2
    // --------------------------------------------------------
    const vector<TelemetryRecord>& history = dataManager.getHistory();

    // --------------------------------------------------------
    // Display statistics
    // --------------------------------------------------------
    cout << "\\n========================================\\n";
    cout << "       MODULE 5: STATISTICS TEST\\n";
    cout << "========================================\\n";

    cout << "Total records: "
         << statisticsManager.getTotalRecords(history) << endl;

    cout << "\\n--- CPU Statistics ---\\n";
    cout << "CPU record count: "
         << statisticsManager.getMetricCount(history, "CPU") << endl;
    cout << "CPU average: "
         << statisticsManager.getAverageValue(history, "CPU") << endl;
    cout << "CPU maximum: "
         << statisticsManager.getMaximumValue(history, "CPU") << endl;
    cout << "CPU minimum: "
         << statisticsManager.getMinimumValue(history, "CPU") << endl;

    cout << "\\n--- Memory Statistics ---\\n";
    cout << "Memory record count: "
         << statisticsManager.getMetricCount(history, "Memory") << endl;
    cout << "Memory average: "
         << statisticsManager.getAverageValue(history, "Memory") << endl;
    cout << "Memory maximum: "
         << statisticsManager.getMaximumValue(history, "Memory") << endl;
    cout << "Memory minimum: "
         << statisticsManager.getMinimumValue(history, "Memory") << endl;

    cout << "\\n--- Network Statistics ---\\n";
    cout << "Network record count: "
         << statisticsManager.getMetricCount(history, "Network") << endl;
    cout << "Network average: "
         << statisticsManager.getAverageValue(history, "Network") << endl;
    cout << "Network maximum: "
         << statisticsManager.getMaximumValue(history, "Network") << endl;
    cout << "Network minimum: "
         << statisticsManager.getMinimumValue(history, "Network") << endl;

    cout << "\\n--- Severity Statistics ---\\n";
    cout << "Critical records: "
         << statisticsManager.getCriticalCount(history) << endl;
    cout << "Warning records: "
         << statisticsManager.getWarningCount(history) << endl;
    cout << "Normal records: "
         << statisticsManager.getNormalCount(history) << endl;

    cout << "\\n========================================\\n";
    cout << "       MODULE 5 TEST COMPLETED\\n";
    cout << "========================================\\n";

    return 0;
}
