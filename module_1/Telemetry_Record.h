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

public:
    // Default constructor
    TelemetryRecord();

    // Parameterized constructor
    TelemetryRecord(int id, string metric, double val, string time);

    // Validation
    bool validate();

    // Calculate status and severity
    void calculateStatus();

    // Display record
    void displayRecord() const;
    
    // Getters
    int read_Record_ID() const;
    string read_Metric_Name() const;
    double read_Value() const;
    string read_Time_stamp() const;
    int read_Severity() const;
    string read_Status() const;
};

#endif