#ifndef SEARCH_MANAGER_H
#define SEARCH_MANAGER_H

#include "Telemetry_Record.h"
#include <vector>

using namespace std;

class SearchManager
{
public:

    // Linear Search
    // Searches for a record by Record ID.
    int linearSearch(const vector<TelemetryRecord>& records,
                     int recordID) const;

    // Binary Search
    // Searches for a record by Record ID.
    // Records must be sorted by Record ID.
    int binarySearch(const vector<TelemetryRecord>& records,
                     int recordID) const;
};

#endif