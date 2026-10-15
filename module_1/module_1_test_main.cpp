#include <iostream>

#include "Telemetry_Generator.h"

using namespace std;
int main(){
    TelemetryGenerator generator;
    int choice;
    cout << "========================================\n";
    cout << "          CHRONOS MONITOR\n";
    cout << "========================================\n";
    cout << "\nMODULE 1 - TELEMETRY INPUT\n";
    cout << "\n1. Generate Telemetry Automatically";
    cout << "\n2. Enter Telemetry Manually";
    cout << "\n3. Exit";
    cout << "\n\nEnter choice: ";
    cin >> choice;
    if (choice == 1){
        TelemetryRecord record = generator.generateRecord();
        if (record.validate()){
            cout << "\nTelemetry generated successfully.\n";
            record.displayRecord();
        }
        else{
            cout << "\nGenerated telemetry is invalid.\n";
        }
    }
    else if (choice == 2){
        TelemetryRecord record = generator.manualInput();
        if (record.validate()){
            cout << "\nTelemetry record created successfully.\n";
            record.displayRecord();
        }
        else{
            cout << "\nInvalid telemetry record.\n";
        }
    }
    else if (choice == 3)
        cout << "\nExiting Chronos Monitor...\n";
    

    else
    
        cout << "\nInvalid choice.\n";
    

    return 0;
}