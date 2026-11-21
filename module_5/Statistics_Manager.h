
#ifndef STATISTICS_MANAGER_H
#define STATISTICS_MANAGER_H

#include "Telemetry_Record.h"
#include <vector>

using namespace std;

class StatisticsManager
{
public:

    // Total number of telemetry records
    int getTotalRecords(const vector<TelemetryRecord>& records) const;

    // Count records belonging to a particular metric
    int getMetricCount(const vector<TelemetryRecord>& records,
                       string metricName) const;

    // Calculate average value of a metric
    double getAverageValue(const vector<TelemetryRecord>& records,
                          string metricName) const;

    // Find maximum value of a metric
    double getMaximumValue(const vector<TelemetryRecord>& records,
                          string metricName) const;

    // Find minimum value of a metric
    double getMinimumValue(const vector<TelemetryRecord>& records,
                          string metricName) const;

    // Count records by severity
    int getCriticalCount(const vector<TelemetryRecord>& records) const;
    int getWarningCount(const vector<TelemetryRecord>& records) const;
    int getNormalCount(const vector<TelemetryRecord>& records) const;
};

#endif
