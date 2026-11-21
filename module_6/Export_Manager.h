
#ifndef EXPORT_MANAGER_H
#define EXPORT_MANAGER_H

#include <string>
#include <vector>
#include "../module_1/Telemetry_Record.h"

using namespace std;

class ExportManager
{
public:
    bool exportTelemetryToCSV(
        const vector<TelemetryRecord>& records,
        const string& filename
    );

};

#endif

