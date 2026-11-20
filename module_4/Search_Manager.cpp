#include "Search_Manager.h"

// ============================================================
// LINEAR SEARCH
// Searches for a record by Record ID.
// Works regardless of the order of records.
//
// Returns:
//   Index of the matching record if found
//   -1 if the record does not exist
// ============================================================

int SearchManager::linearSearch(
    const vector<TelemetryRecord>& records,
    int recordID) const
{
    for (int i = 0; i < static_cast<int>(records.size()); i++)
    {
        if (records[i].read_Record_ID() == recordID)
        {
            return i;
        }
    }

    return -1;
}


// ============================================================
// BINARY SEARCH
// Searches for a record by Record ID.
//
// IMPORTANT:
// The records must be sorted in ascending order
// by Record ID before calling this function.
//
// Returns:
//   Index of the matching record if found
//   -1 if the record does not exist
// ============================================================

int SearchManager::binarySearch(
    const vector<TelemetryRecord>& records,
    int recordID) const
{
    int left = 0;
    int right = static_cast<int>(records.size()) - 1;

    while (left <= right)
    {
        // Calculate the middle index safely.
        int mid = left + (right - left) / 2;

        int currentID = records[mid].read_Record_ID();

        if (currentID == recordID)
        {
            return mid;
        }
        else if (currentID < recordID)
        {
            // Search the right half.
            left = mid + 1;
        }
        else
        {
            // Search the left half.
            right = mid - 1;
        }
    }

    return -1;
}