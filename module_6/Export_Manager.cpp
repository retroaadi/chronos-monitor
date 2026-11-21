
#include "Export_Manager.h"
#include <fstream>
#include <iostream>

bool ExportManager::exportTelemetryToCSV(
    const vector<TelemetryRecord>& records,
    const string& filename)
{
    ofstream file(filename);

    if (!file.is_open())
    {
        cout << "Error: Unable to create export file.\n";
        return false;
    }

    // CSV header
    file << "RecordID,Metric,Value,Timestamp,Severity,Status\n";

    // Export each telemetry record
    for (const TelemetryRecord& record : records)
    {
        file << record.read_Record_ID() << ","
             << record.read_Metric_Name() << ","
             << record.read_Value() << ","
             << record.read_Time_stamp() << ","
             << record.read_Severity() << ","
             << record.read_Status() << "\n";
    }

    file.close();

    cout << "Telemetry exported successfully to "
         << filename << "\n";

    return true;
}
