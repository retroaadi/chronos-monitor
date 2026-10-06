#include "Telemetry_Record.h"
#include <iostream>

using namespace std;
TelemetryRecord::TelemetryRecord(){
    record_ID = 0;
    metric_Name = "Unknown";
    value = 0.0;
    time_Stamp = "00:00:00";
    severity = 0;
    status = "NORMAL";
}

TelemetryRecord::TelemetryRecord(int id, string metric, double val, string time){
    record_ID = id;
    metric_Name = metric;
    value = val;
    time_Stamp = time;
    severity = 0;
    status = "NORMAL";
    calculateStatus();
}

bool TelemetryRecord::validate(){

    if (record_ID <= 0)
        return false;

    if (metric_Name.empty())
        return false;

    if (value < 0)
        return false;

    if ((metric_Name == "CPU" || metric_Name == "Memory" ||
         metric_Name == "Network") && value > 100)
        return false;

    if (metric_Name == "ResponseTime" && value < 0)
        return false;

    if (metric_Name == "ErrorCount" && value < 0)
        return false;

    if (time_Stamp.empty())
        return false;

    return true;
}


void TelemetryRecord::calculateStatus(){
    // for CPU
    if (metric_Name == "CPU"){
        if (value >= 85){
            status = "CRITICAL";
            severity = 2;
        }
        else if (value >= 70){
            status = "WARNING";
            severity = 1;
        }
        else
        {
            status = "NORMAL";
            severity = 0;
        }
    }

    // for Memory
    else if (metric_Name == "Memory"){
        if (value >= 90){
            status = "CRITICAL";
            severity = 2;
        }
        else if (value >= 75){
            status = "WARNING";
            severity = 1;
        }
        else{
            status = "NORMAL";
            severity = 0;
        }
    }

    // for Network_Usage
    else if (metric_Name == "Network"){
        if (value >= 90){
            status = "CRITICAL";
            severity = 2;
        }
        else if (value >= 75){
            status = "WARNING";
            severity = 1;
        }
        else{
            status = "NORMAL";
            severity = 0;
        }
    }

    // for Response_Time
    else if (metric_Name == "ResponseTime"){
        if (value >= 500){
            status = "CRITICAL";
            severity = 2;
        }
        else if (value >= 300){
            status = "WARNING";
            severity = 1;
        }
        else{
            status = "NORMAL";
            severity = 0;
        }
    }

    // for Error_Count
    else if (metric_Name == "ErrorCount"){
        if (value >= 10){
            status = "CRITICAL";
            severity = 2;
        }
        else if (value > 0){
            status = "WARNING";
            severity = 1;
        }
        else{
            status = "NORMAL";
            severity = 0;
        }
    }

    // for Unknown_metric
    else{
        status = "NORMAL";
        severity = 0;
    }
}

// to Display_record
void TelemetryRecord::displayRecord() const{
    cout << "\n========================================\n";
    cout << "         TELEMETRY RECORD\n";
    cout << "========================================\n";
    cout << "Record ID       : " << record_ID << endl;
    cout << "Metric          : " << metric_Name << endl;
    cout << "Value           : " << value << endl;
    cout << "Timestamp       : " << time_Stamp << endl;
    cout << "Status          : " << status << endl;
    cout << "Severity        : " << severity << endl;
    cout << "========================================\n";
}


int TelemetryRecord::read_Record_ID() const{
    return record_ID;
}

string TelemetryRecord::read_Metric_Name() const{
    return metric_Name;
}

double TelemetryRecord::read_Value() const{
    return value;
}

string TelemetryRecord::read_Time_stamp() const{
    return time_Stamp;
}

int TelemetryRecord::read_Severity() const{
    return severity;
}

string TelemetryRecord::read_Status() const{
    return status;
}