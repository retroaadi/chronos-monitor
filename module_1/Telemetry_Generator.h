#ifndef TELEMETRY_GENERATOR_H
#define TELEMETRY_GENERATOR_H

#include <string>
#include "Telemetry_Record.h"

using namespace std;

class TelemetryGenerator
{
private:
    int next_Record_ID;

    // Generate current timestamp
    string get_Current_Time();

public:
    // Constructor
    TelemetryGenerator();

    // Generate one complete telemetry record
    TelemetryRecord generateRecord();

    // Generate a manually entered record
    TelemetryRecord manualInput();
};

#endif