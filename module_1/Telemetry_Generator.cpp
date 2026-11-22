#include "Telemetry_Generator.h"

#include <iostream>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;


// Constructor
TelemetryGenerator::TelemetryGenerator()
{
    next_Record_ID = 1;
}


// Generate current time
string TelemetryGenerator::get_Current_Time()
{
    time_t currentTime = time(nullptr);

    tm* localTime = localtime(&currentTime);

    stringstream timeStream;

    timeStream << setfill('0')
               << setw(2) << localTime->tm_hour << ":"
               << setw(2) << localTime->tm_min << ":"
               << setw(2) << localTime->tm_sec;

    return timeStream.str();
}


// Generate automatic telemetry record
TelemetryRecord TelemetryGenerator::generateRecord()
{
    random_device rd;
    mt19937 generator(rd());

    // Random distributions
    uniform_real_distribution<double> cpuDistribution(20.0, 100.0);
    uniform_real_distribution<double> memoryDistribution(20.0, 100.0);
    uniform_real_distribution<double> networkDistribution(0.0, 100.0);
    uniform_real_distribution<double> responseDistribution(50.0, 1000.0);
    uniform_int_distribution<int> errorDistribution(0, 20);

    // Generate values
    double cpu = cpuDistribution(generator);
    double memory = memoryDistribution(generator);
    double network = networkDistribution(generator);
    double responseTime = responseDistribution(generator);
    int errors = errorDistribution(generator);

    /*
        For Module 1, one TelemetryRecord represents
        one metric measurement.

        Therefore, generate one metric at a time.
    */

    // Randomly select a metric
    uniform_int_distribution<int> metricChoice(0, 4);

    int choice = metricChoice(generator);

    string metric_Name;
    double value;

    switch (choice)
    {
        case 0:
            metric_Name = "CPU";
            value = cpu;
            break;

        case 1:
            metric_Name = "Memory";
            value = memory;
            break;

        case 2:
            metric_Name = "Network";
            value = network;
            break;

        case 3:
            metric_Name = "ResponseTime";
            value = responseTime;
            break;

        case 4:
            metric_Name = "ErrorCount";
            value = errors;
            break;
    }

    TelemetryRecord record(
        next_Record_ID,
        metric_Name,
        value,
        get_Current_Time()
    );

    next_Record_ID++;

    return record;
}


// Manual telemetry input
TelemetryRecord TelemetryGenerator::manualInput()
{
    string metric_Name;
    double value;

    cout << "\n========================================\n";
    cout << "         MANUAL TELEMETRY INPUT\n";
    cout << "========================================\n";

    cout << "Available Metrics:\n";
    cout << "1. CPU\n";
    cout << "2. Memory\n";
    cout << "3. Network\n";
    cout << "4. ResponseTime\n";
    cout << "5. ErrorCount\n";

    int choice;

    cout << "\nEnter metric choice: ";
    cin >> choice;

    switch (choice)
    {
        case 1:
            metric_Name = "CPU";
            break;

        case 2:
            metric_Name = "Memory";
            break;

        case 3:
            metric_Name = "Network";
            break;

        case 4:
            metric_Name = "ResponseTime";
            break;

        case 5:
            metric_Name = "ErrorCount";
            break;

        default:
            cout << "Invalid metric choice.\n";
            return TelemetryRecord();
    }

    cout << "Enter value: ";
    cin >> value;

    TelemetryRecord record(
        next_Record_ID,
        metric_Name,
        value,
        get_Current_Time()
    );

    next_Record_ID++;

    return record;
}