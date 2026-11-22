#include "Data_Manager.h"
#include "Telemetry_Record.h"
#include <iostream>

using namespace std;

int main()
{
    DataManager dataManager;

    // Create test telemetry records
    TelemetryRecord record1(1, "CPU", 65, "10:00:01");
    TelemetryRecord record2(2, "Memory", 70, "10:00:02");
    TelemetryRecord record3(3, "Network", 45, "10:00:03");
    TelemetryRecord record4(4, "ResponseTime", 250, "10:00:04");
    TelemetryRecord record5(5, "ErrorCount", 2, "10:00:05");
    TelemetryRecord record6(6, "CPU", 90, "10:00:06");


    // Calculate status for each record
    record1.calculateStatus();
    record2.calculateStatus();
    record3.calculateStatus();
    record4.calculateStatus();
    record5.calculateStatus();
    record6.calculateStatus();


    // --------------------------------------------------
    // TEST 1: Add records
    // --------------------------------------------------

    cout << "\n========== TEST 1: ADD RECORDS ==========\n";

    dataManager.addRecord(record1);
    dataManager.addRecord(record2);
    dataManager.addRecord(record3);

    cout << "3 records added successfully.\n";

    cout << "Current queue count: "
         << dataManager.getCount() << endl;


    // --------------------------------------------------
    // TEST 2: Display recent records
    // --------------------------------------------------

    cout << "\n========== TEST 2: RECENT RECORDS ==========\n";

    dataManager.displayRecentRecords();


    // --------------------------------------------------
    // TEST 3: Display complete history
    // --------------------------------------------------

    cout << "\n========== TEST 3: COMPLETE HISTORY ==========\n";

    dataManager.displayHistory();

    cout << "Total history records: "
         << dataManager.getHistoryCount() << endl;


    // --------------------------------------------------
    // TEST 4: Fill the circular queue
    // --------------------------------------------------

    cout << "\n========== TEST 4: FILL QUEUE ==========\n";

    dataManager.addRecord(record4);
    dataManager.addRecord(record5);

    cout << "Two more records added.\n";

    cout << "Current queue count: "
         << dataManager.getCount() << endl;

    cout << "Queue full status: ";

    if (dataManager.isFull())
        cout << "FULL\n";
    else
        cout << "NOT FULL\n";

    dataManager.displayRecentRecords();


    // --------------------------------------------------
    // TEST 5: Circular Queue replacement
    // --------------------------------------------------

    cout << "\n========== TEST 5: CIRCULAR REPLACEMENT ==========\n";

    cout << "Adding Record 6...\n";
    cout << "The oldest recent record should be replaced.\n";

    dataManager.addRecord(record6);

    cout << "\nRecent records after adding Record 6:\n";

    dataManager.displayRecentRecords();


    // --------------------------------------------------
    // TEST 6: Check complete history
    // --------------------------------------------------

    cout << "\n========== TEST 6: HISTORY CHECK ==========\n";

    cout << "Complete history should contain all 6 records:\n";

    dataManager.displayHistory();

    cout << "Total history records: "
         << dataManager.getHistoryCount() << endl;


    // --------------------------------------------------
    // TEST 7: Remove oldest record
    // --------------------------------------------------

    cout << "\n========== TEST 7: REMOVE OLDEST ==========\n";

    dataManager.removeOldestRecord();

    cout << "Oldest recent record removed.\n";

    cout << "Current queue count: "
         << dataManager.getCount() << endl;

    dataManager.displayRecentRecords();


    // --------------------------------------------------
    // TEST 8: Clear all records
    // --------------------------------------------------

    cout << "\n========== TEST 8: CLEAR RECORDS ==========\n";

    dataManager.clearRecords();

    cout << "All records cleared.\n";

    cout << "Queue count: "
         << dataManager.getCount() << endl;

    cout << "History count: "
         << dataManager.getHistoryCount() << endl;


    // --------------------------------------------------
    // TEST 9: Check empty queue
    // --------------------------------------------------

    cout << "\n========== TEST 9: EMPTY CHECK ==========\n";

    if (dataManager.isEmpty())
        cout << "Queue is empty. TEST PASSED.\n";
    else
        cout << "Queue is not empty. TEST FAILED.\n";


    cout << "\n========================================\n";
    cout << "       MODULE 2 TEST COMPLETED\n";
    cout << "========================================\n";


    return 0;
}