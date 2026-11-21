
#include <iostream>
#include <vector>

#include "../module_1/Telemetry_Record.h"
#include "Export_Manager.h"

using namespace std;

int main()
{
    ExportManager exportManager;

    vector<TelemetryRecord> records;

    records.emplace_back(1, "CPU", 75.5, "10:00:00");
    records.emplace_back(2, "Memory", 82.0, "10:01:00");
    records.emplace_back(3, "Network", 95.0, "10:02:00");

    cout << "===== MODULE 6 TEST =====\n";

    exportManager.exportTelemetryToCSV(
        records,
        "module6_test_export.csv"
    );

    return 0;
}
