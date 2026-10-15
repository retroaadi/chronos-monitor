#ifndef TELEMETRY_RECORD_H
#define TELEMETRY_RECORD_H

#include <string>
using namespace std;

class TelemetryRecord
{
private:
    int record_ID;
    string metric_Name;
    double value;
    string time_Stamp;
    int severity;
    string status;
};
#endif