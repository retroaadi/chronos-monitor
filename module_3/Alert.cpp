#include "Alert.h"
#include <iostream>

using namespace std;

// Default constructor
Alert::Alert()
{
    alertID = 0;
    recordID = 0;

    metricName = "";
    value = 0.0;
    timestamp = "";

    status = "NORMAL";
    severity = 0;
}

// Parameterized constructor
Alert::Alert(int id,
             int recID,
             string metric,
             double val,
             string time,
             string stat,
             int sev)
{
    alertID = id;
    recordID = recID;

    metricName = metric;
    value = val;
    timestamp = time;

    status = stat;
    severity = sev;
}

// Getter functions

int Alert::getAlertID() const
{
    return alertID;
}

int Alert::getRecordID() const
{
    return recordID;
}

string Alert::getMetricName() const
{
    return metricName;
}

double Alert::getValue() const
{
    return value;
}

string Alert::getTimestamp() const
{
    return timestamp;
}

string Alert::getStatus() const
{
    return status;
}

int Alert::getSeverity() const
{
    return severity;
}

// Display alert information
void Alert::displayAlert() const
{
    cout << "\n";
    cout << "                 ALERT\n";
    cout << "\n";

    cout << "Alert ID        : " << alertID << endl;
    cout << "Record ID       : " << recordID << endl;
    cout << "Metric          : " << metricName << endl;
    cout << "Value           : " << value << endl;
    cout << "Timestamp       : " << timestamp << endl;
    cout << "Status          : " << status << endl;
    cout << "Severity        : " << severity << endl;

    cout << "\n";
}
