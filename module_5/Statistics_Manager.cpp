#include "Statistics_Manager.h"
#include <limits>

int StatisticsManager::getTotalRecords(
    const vector<TelemetryRecord>& records) const
{
    return static_cast<int>(records.size());
}

int StatisticsManager::getMetricCount(
    const vector<TelemetryRecord>& records,
    string metricName) const
{
    int count = 0;

    for (const TelemetryRecord& record : records)
    {
        if (record.read_Metric_Name() == metricName)
            count++;
    }

    return count;
}

double StatisticsManager::getAverageValue(
    const vector<TelemetryRecord>& records,
    string metricName) const
{
    double sum = 0.0;
    int count = 0;

    for (const TelemetryRecord& record : records)
    {
        if (record.read_Metric_Name() == metricName)
        {
            sum += record.read_Value();
            count++;
        }
    }

    // Avoid division by zero when the metric is absent.
    if (count == 0)
        return 0.0;

    return sum / count;
}

double StatisticsManager::getMaximumValue(
    const vector<TelemetryRecord>& records,
    string metricName) const
{
    double maximum = numeric_limits<double>::lowest();
    bool found = false;

    for (const TelemetryRecord& record : records)
    {
        if (record.read_Metric_Name() == metricName)
        {
            if (record.read_Value() > maximum)
                maximum = record.read_Value();

            found = true;
        }
    }

    // Return 0 when no matching metric exists.
    if (!found)
        return 0.0;

    return maximum;
}

double StatisticsManager::getMinimumValue(
    const vector<TelemetryRecord>& records,
    string metricName) const
{
    double minimum = numeric_limits<double>::max();
    bool found = false;

    for (const TelemetryRecord& record : records)
    {
        if (record.read_Metric_Name() == metricName)
        {
            if (record.read_Value() < minimum)
                minimum = record.read_Value();

            found = true;
        }
    }

    // Return 0 when no matching metric exists.
    if (!found)
        return 0.0;

    return minimum;
}

int StatisticsManager::getCriticalCount(
    const vector<TelemetryRecord>& records) const
{
    int count = 0;

    for (const TelemetryRecord& record : records)
    {
        if (record.read_Severity() == 2)
            count++;
    }

    return count;
}

int StatisticsManager::getWarningCount(
    const vector<TelemetryRecord>& records) const
{
    int count = 0;

    for (const TelemetryRecord& record : records)
    {
        if (record.read_Severity() == 1)
            count++;
    }

    return count;
}

int StatisticsManager::getNormalCount(
    const vector<TelemetryRecord>& records) const
{
    int count = 0;

    for (const TelemetryRecord& record : records)
    {
        if (record.read_Severity() == 0)
            count++;
    }

    return count;
}
