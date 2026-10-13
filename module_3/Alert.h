#ifndef ALERT_H
#define ALERT_H

#include "Telemetry_Record.h"
#include <string>

using namespace std;

class Alert
{
private:
    int alertID;
    int recordID;

    string metricName;
    double value;
    string timestamp;

    string status;
    int severity;

public:
    // Default constructor
    Alert();

    // Parameterized constructor
    Alert(int id,
          int recID,
          string metric,
          double val,
          string time,
          string stat,
          int sev);

    // Getter functions
    int getAlertID() const;
    int getRecordID() const;

    string getMetricName() const;
    double getValue() const;
    string getTimestamp() const;

    string getStatus() const;
    int getSeverity() const;

    // Display alert information
    void displayAlert() const;
};

#endif