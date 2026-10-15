#include "Alert_Manager.h"
#include "Telemetry_Record.h"
#include <iostream>

using namespace std;

int main()
{
    AlertManager alertManager;

    // --------------------------------------------------------
    // Create test telemetry records
    // --------------------------------------------------------

    TelemetryRecord record1(
        1,
        "CPU",
        65,
        "10:00:01"
    );

    TelemetryRecord record2(
        2,
        "Memory",
        82,
        "10:00:02"
    );

    TelemetryRecord record3(
        3,
        "ErrorCount",
        12,
        "10:00:03"
    );

    TelemetryRecord record4(
        4,
        "CPU",
        92,
        "10:00:04"
    );

    TelemetryRecord record5(
        5,
        "ResponseTime",
        600,
        "10:00:05"
    );


    // --------------------------------------------------------
    // Calculate status and severity
    // --------------------------------------------------------

    record1.calculateStatus();
    record2.calculateStatus();
    record3.calculateStatus();
    record4.calculateStatus();
    record5.calculateStatus();


    // ========================================================
    // TEST 1: NORMAL RECORD
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       TEST 1: NORMAL RECORD\n";
    cout << "========================================\n";

    alertManager.processRecord(record1);

    cout << "Total alerts: "
         << alertManager.getAlertCount()
         << endl;


    // ========================================================
    // TEST 2: WARNING RECORD
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       TEST 2: WARNING RECORD\n";
    cout << "========================================\n";

    alertManager.processRecord(record2);

    cout << "Total alerts: "
         << alertManager.getAlertCount()
         << endl;


    // ========================================================
    // TEST 3: MULTIPLE ALERTS
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       TEST 3: MULTIPLE ALERTS\n";
    cout << "========================================\n";

    alertManager.processRecord(record3);
    alertManager.processRecord(record4);
    alertManager.processRecord(record5);

    cout << "Total alerts: "
         << alertManager.getAlertCount()
         << endl;


    // ========================================================
    // TEST 4: ALERT HISTORY
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       TEST 4: ALERT HISTORY\n";
    cout << "========================================\n";

    alertManager.displayAlertHistory();


    // ========================================================
    // TEST 5: PRIORITY QUEUE
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       TEST 5: PRIORITY QUEUE\n";
    cout << "========================================\n";

    alertManager.displayPriorityAlerts();


    // ========================================================
    // TEST 6: HIGHEST PRIORITY ALERT
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       TEST 6: HIGHEST PRIORITY\n";
    cout << "========================================\n";

    Alert highestPriority =
        alertManager.getHighestPriorityAlert();

    cout << "\nHighest priority alert removed from queue:\n";

    highestPriority.displayAlert();


    // ========================================================
    // TEST 7: REMAINING PRIORITY QUEUE
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       TEST 7: REMAINING ALERTS\n";
    cout << "========================================\n";

    alertManager.displayPriorityAlerts();


    // ========================================================
    // TEST 8: ALERT HISTORY AFTER REMOVAL
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       TEST 8: HISTORY AFTER REMOVAL\n";
    cout << "========================================\n";

    alertManager.displayAlertHistory();


    // ========================================================
    // TEST 9: CLEAR ALERTS
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       TEST 9: CLEAR ALERTS\n";
    cout << "========================================\n";

    alertManager.clearAlerts();

    cout << "Alert count after clearing: "
         << alertManager.getAlertCount()
         << endl;

    cout << "Pending alerts after clearing:\n";

    alertManager.displayPriorityAlerts();


    // ========================================================
    // FINAL MESSAGE
    // ========================================================

    cout << "\n\n========================================\n";
    cout << "       MODULE 3 TEST COMPLETED\n";
    cout << "========================================\n";

    return 0;
}