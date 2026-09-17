# Chronos Monitor

## A Telemetry Monitoring and Alert Management System

The Chronos Monitor is a **C++ based simulation system for monitoring telemetry and managing alerts**. It was created as an academic project by Team ByteCare.

The Chronos Monitor simulates telemetry monitoring like the processing of CPU usage, memory usage, network activities, response time, and error count. The system handles telemetry records, finds threshold violations, creates and prioritizes alerts, keeps alert history, and performs search, sort, statistics, and reporting through the console-based interface.

This project revolves around the practical implementation of Data Structure and Object Oriented Programming (OOP).


## Project Information

 Detail  Information

**Project Name**  Chronos Monitor 
**Project Type** Academic Project 
**Language** C++ 
**Interface** Console-Based 
**Domain** Systems Monitoring & Alerting 
**Team** ByteCare 
**Team ID** DSCPP-III-2026-T125d 
**Semester** 3rd Semester 
**Courses** Data Structures in C (TCS302), Object Oriented Programming with C++ (TCS307) 
**Mentor** Mr. Utkarsh Pant Sir 


## 1. Problem Statement

Computing systems produce performance data like processor usage, memory use, networking activities, response time, and error rates on an ongoing basis.

A monitoring system needs to efficiently do the following :-

- Receive monitoring data
- Validate telemetry records
- Store recent and historical records
- Detect abnormal conditions
- Generate alerts
- Prioritize alerts according to severity
- Maintain alert history
- Search and sort monitoring records
- Generate useful reports

Chronos Monitor provides a simplified academic implementation of these operations, while demonstrating how Data Structures and OOP concepts can be applied to a real-world monitoring problem.

## 2. Objectives

- To develop a C++-based monitoring and alert management system.
- Simulate or accept monitoring data.
- Monitor CPU, memory, network, response time, and error metrics.
- Validate and store telemetry records.
- Detect threshold violations.
- Generate alerts with different severity levels.
- Prioritize critical alerts.
- Maintain alert history.
- Implement searching and sorting operations.
- Calculate average, minimum, and maximum values.
- Generate console-based monitoring reports.
- Demonstrate practical Data Structures and OOP.

## 3. System Modules

### 3.1 Telemetry Input Module

Responsible for receiving and validating monitoring data.

- Simulated telemetry generation
- Manual telemetry input
- Input validation
- Telemetry record creation

### 3.2 Data Storage Module

Data Structure  Purpose 
**Circular Queue** : Maintains a bounded collection of recent telemetry records 
**Vector / Dynamic Array** : Maintains complete telemetry history 
**Hash Table** : Provides metric-based lookup 

### 3.3 Alert Management Module

Responsible for:

- Threshold checking
- Alert generation
- Severity assignment
- Alert prioritization
- Alert history management

A **Priority Queue** processes higher-severity alerts before lower-severity alerts. A **Linked List** maintains alert history.

### 3.4 Search and Sorting Module

Searching:

- Linear Search
- Binary Search

Sorting:

- Bubble Sort
- Selection Sort
- Insertion Sort
- Merge Sort

The exact algorithms implemented may change during development.

### 3.5 Statistics and Reporting Module

Provides:

- Average metric value
- Minimum and maximum values
- Total alerts
- Alert counts by severity
- Metric summaries
- Monitoring reports


## 4. Data Structures Used

Data Structure  Application
**Circular Queue** : Recent telemetry records 
**Vector / Dynamic Array** : Complete telemetry history 
**Linked List** : Alert history 
**Priority Queue / Heap** : Alert prioritization 
**Hash Table** : Fast metric lookup 
**Array** : Basic storage and processing 

The project is to emphasizes the understanding and implementation of underlying Data Structure concepts rather than relying entirely on ready-made containers.

## 5. Object-Oriented Design

### `TelemetryRecord`

Represents an individual monitoring record.

Possible fields:

```text
Id
Time_stamp
Metric_Name
Metric_Type
Value
Status
Severity
```

### `MonitoringSystem`

Coordinates telemetry input, validation, storage, and overall monitoring operations.

### `AlertManager`

Handles threshold checking, alert creation, severity assignment, priority-based processing, and alert history.

### `ReportManager`

Handles searching, sorting, statistics, report generation, and console output.

---

## 6. OOP Concepts

The project demonstrates:

- Classes and Objects
- Encapsulation
- Abstraction
- Constructors
- Destructors
- Member Functions
- Access Specifiers
- Composition
- Pointers
- Dynamic Memory Allocation

---

## 7. Alert Detection

Example threshold configuration:

 Metric  Example Threshold  Severity

CPU Utilization  > 80%  HIGH 
Memory Usage  > 85%  HIGH 
Response Time  > 500 ms  MEDIUM
Error Count  > 10 CRITICAL

Example:

```text
CPU Usage: 45%
Status: NORMAL
```

No alert is generated.

If:

```text
CPU Usage: 87%
```

the system can generate:

```text
[HIGH] CPU usage exceeded threshold.
```

If:

```text
Error Count: 15
```

the system can generate:

```text
[CRITICAL] Error count exceeded threshold.
```

Threshold values are configurable and may change during implementation and testing.

---


## 8. Subject Integration

### TCS302 — Data Structures in C

- Arrays
- Pointers
- Dynamic Memory Allocation
- Circular Queue
- Linked List
- Priority Queue
- Hash Table
- Searching
- Sorting

### TCS307 — Object Oriented Programming with C++

- Classes
- Objects
- Encapsulation
- Abstraction
- Constructors
- Destructors
- Composition
- Member Functions
- Access Specifiers
- Pointers
- STL

The project integrates these concepts into a single monitoring and alert-management workflow.

---

## 9. Technology Stack

- **C++** — Primary programming language
- **Data Structures** — Core data management
- **C++ Standard Library (STL)** — Supporting functionality and comparison
- **C++ Compiler** — Compilation
- **VS Code** — Development environment
- **Git** — Version control
- **GitHub** — Repository and collaboration
- **Console Interface** — Primary user interface

File handling may be added for persistence where required.



## 10. STL Usage

The C++ Standard Library may be used selectively for supporting functionality and comparison.

Relevant components include:

```cpp
std::vector
std::list
std::queue
std::priority_queue
std::unordered_map
std::algorithm
```

Where Data Structures are specifically being demonstrated, core structures can be implemented manually so their operations can be studied and explained during evaluation.



## 11. Development Phases

### Phase 1 — Requirement Analysis and Design

- Define requirements
- Identify monitoring parameters
- Define alert thresholds
- Identify Data Structures
- Design classes
- Design system architecture

### Phase 2 — Working Implementation and Technical Progress

The Phase 2 focus is:

- Working C++ implementation
- Technical development of project modules
- Integration of Data Structures and OOP
- Subject integration
- Mentor interactions and feedback
- Individual team-member contribution
- Testing and demonstration

### Phase 3 — Integration, Evaluation and Finalization

- Complete searching and sorting
- Statistics and reporting
- Module integration
- Testing
- Performance evaluation
- Documentation
- Final presentation

## 12. Team Contributions

### Aradhya Uniyal — Team Lead

- Project coordination
- Integration
- Testing
- Statistics and reporting
- Documentation
- Presentation

### Nikita

- OOP class design
- Telemetry input
- Class implementation
- Object-oriented system organization

### Uddhav Bhargava

- Data Structure implementation
- Data storage
- Alert management
- Priority-based alert processing
- Searching and sorting

---

## 13. Project Scope

The current scope includes:

- Simulated telemetry
- Manual telemetry input
- Local execution
- In-memory data processing
- Threshold-based alerts
- Alert prioritization
- Alert history
- Searching
- Sorting
- Statistics
- Console-based reporting

The core project does not depend on:

- Cloud infrastructure
- Distributed systems
- Artificial intelligence
- Machine learning
- Multithreaded processing
- Lock-free programming
- Atomic synchronization
- Complex caching systems
- External real-time monitoring servers

These technologies are outside the current academic scope.

---

## 14. Performance Evaluation

Possible comparisons include:

```text
Linear Search vs Binary Search vs Hashing

Bubble Sort vs Insertion Sort vs Merge Sort

Array vs Vector vs Linked List

Queue vs Priority Queue
```

Possible evaluation parameters:

- Execution time
- Number of operations
- Memory usage
- Search performance
- Sorting performance
- Behavior with increasing data size

---

## 15. Repository Structure

```text
chronos-monitor
│
├── README.md
├── include
│   ├── TelemetryRecord.h
│   ├── MonitoringSystem.h
│   ├── AlertManager.h
│   └── ReportManager.h
│
├── src
│   ├── TelemetryRecord.cpp
│   ├── MonitoringSystem.cpp
│   ├── AlertManager.cpp
│   └── ReportManager.cpp
│
├── data
│   ├── telemetry.txt
│   └── alerts.txt
│
├── tests
│   └── test_cases.cpp
│
└── main.cpp
```

The actual structure may change during implementation.

---

## 16. How to Run

### Requirements

- C++ compiler
- VS Code or another C++ development environment
- Git (optional)

### Compile

For a single-file implementation:

```bash
g++ main.cpp -o chronos
```

### Windows

```bash
chronos.exe
```

### Linux / macOS

```bash
./chronos
```

For a multi-file implementation:

```bash
g++ main.cpp src/*.cpp -o chronos
```

---

## 17. Example Console Interface

```text
========================================
           CHRONOS MONITOR
========================================

1. Add Telemetry
2. Generate Telemetry
3. View Recent Records
4. View Telemetry History
5. View Alerts
6. Search Records
7. Sort Records
8. View Statistics
9. Generate Report
0. Exit

Enter your choice:
```

---

## 18. Limitations

- Telemetry data is simulated or manually entered.
- Direct operating-system metric collection is outside the current core scope.
- Data is primarily maintained in memory.
- The primary interface is console-based.
- Alert thresholds are predefined/configurable.
- The project is intended as an academic prototype rather than a production monitoring platform.

---

## 19. Future Enhancements

Possible future enhancements include:

- Basic GUI using Qt
- Persistent database storage
- Real system metric collection
- Configurable thresholds through a GUI
- Advanced monitoring dashboards
- Report export
- Additional monitoring metrics
- Extended performance analysis

These are optional extensions and are not required for the core implementation.

---

## 20. References

- [C++ Reference](https://en.cppreference.com/)
- [GeeksforGeeks](https://www.geeksforgeeks.org/)
- [Programiz](https://www.programiz.com/cpp-programming)
- [IBM Documentation](https://www.ibm.com/docs/)

---

## 21. Team

### ByteCare

**Project:** Chronos Monitor  <br>
**Team ID:** DSCPP-III-2026-T125d <br> 
**Semester:** 3rd Semester<br>
<br>
 Members: <br>
 Aradhya Uniyal: Team Lead <br>
 Nikita:  OOP/Telemetry Input <br>
 Uddhav Bhargava: Data Structures/Alert Management <br>

**Mentor:** Mr. Utkarsh Pant Sir



## Project Summary

> **Chronos Monitor is a C++-based simulated telemetry monitoring and alert management system that collects and processes monitoring data, detects threshold violations, prioritizes alerts, maintains alert history, and generates reports using Data Structures and Object-Oriented Programming.**



**Chronos Monitor — Team ByteCare**

*Academic Project | C++ | Data Structures | Object-Oriented Programming*
