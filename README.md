
# Chronos Monitor
### C++ Telemetry Monitoring, Data Management, Alerting & Web Dashboard

Chronos Monitor is a modular telemetry monitoring system developed in C++. It is designed to generate, manage, analyze, search, and export telemetry records while identifying abnormal system conditions through an alert management mechanism.

The project combines a modular C++ backend with a web-based dashboard to provide a centralized interface for monitoring telemetry data, viewing alerts, analyzing historical records, and interacting with the monitoring system.

---

## 📌 Project Overview

Chronos Monitor provides a structured approach to telemetry monitoring by integrating multiple modules responsible for different stages of the monitoring workflow.

The system can:

- Generate telemetry records.
- Store and manage telemetry data.
- Maintain recent and historical records.
- Detect warning and critical conditions.
- Manage alerts according to priority.
- Search telemetry records.
- Calculate statistical summaries.
- Export telemetry data.
- Present monitoring information through a web dashboard.

The project follows a modular design, where each module has a specific responsibility and can be developed, tested, and integrated independently.

---

## 🎯 Objectives

- Develop a modular telemetry monitoring application using C++.
- Implement appropriate data structures for efficient data management.
- Maintain telemetry records and historical information.
- Identify abnormal telemetry values and generate alerts.
- Support searching and statistical analysis of collected records.
- Provide data export functionality.
- Integrate the C++ system with a web-based graphical interface.
- Demonstrate the practical application of data structures and algorithms.

---

## 🏗️ System Architecture

Chronos Monitor is organized into six core modules, supported by a central application and a web interface.

```text
                    CHRONOS MONITOR
                           |
        +------------------+------------------+
        |                                     |
        |          C++ CORE SYSTEM            |
        |                                     |
        +-------------------------------------+
                           |
        +-------------------------------------+
        |       1. Telemetry Generation       |
        +-------------------------------------+
                           |
        +-------------------------------------+
        |       2. Data Management            |
        +-------------------------------------+
                           |
        +-------------------------------------+
        |       3. Alert Management           |
        +-------------------------------------+
                           |
        +-------------------------------------+
        |       4. Search Management          |
        +-------------------------------------+
                           |
        +-------------------------------------+
        |       5. Statistics Management      |
        +-------------------------------------+
                           |
        +-------------------------------------+
        |       6. Export Management          |
        +-------------------------------------+
                           |
                  Application Integration
                           |
                  Backend / API Bridge
                           |
        +-------------------------------------+
        |          WEB DASHBOARD             |
        +-------------------------------------+
```

### High-Level Data Flow

```text
Telemetry Generation
        |
        v
Telemetry Records
        |
        v
Data Management
        |
        +--------------------+
        |                    |
        v                    v
Alert Management       Search Management
        |                    |
        v                    v
Alert History         Matching Records
        |
        +--------------------+
                             |
                 Statistics Management
                             |
                             v
                    Export Management
                             |
                             v
                    Web Dashboard / API
```

---

## 📦 Core Modules

### Module 1 — Telemetry Generation

Responsible for creating telemetry records containing monitoring information.

#### Responsibilities

- Generate telemetry data.
- Create telemetry records.
- Assign record identifiers.
- Store metric names and values.
- Associate timestamps with records.
- Determine status or severity based on defined conditions.
- Support generated and manually supplied telemetry data.

#### Example Metrics

| Metric | Description |
|---|---|
| CPU Usage | Processor utilization |
| Memory Usage | Memory utilization |
| Network Usage | Network activity |
| Response Time | System response duration |
| Error Count | Number of errors observed |

#### Main Components

- `Telemetry_Record`
- `Telemetry_Generator`

---

### Module 2 — Data Management

Responsible for maintaining telemetry records and managing their storage.

#### Responsibilities

- Add telemetry records.
- Maintain recent telemetry data.
- Preserve historical records.
- Retrieve and display stored data.
- Remove or clear records where supported.
- Provide records to other modules.

#### Data Structures

- **Circular Queue** — Maintains a limited collection of recent telemetry records.
- **Vector** — Stores the broader historical collection of telemetry records.

The circular queue is useful for maintaining a fixed-size recent-data window, while the vector supports access to the accumulated historical records.

---

### Module 3 — Alert Management

Responsible for managing alerts generated from telemetry conditions.

#### Responsibilities

- Handle generated alerts.
- Maintain pending alerts.
- Prioritize alerts according to severity or priority.
- Retrieve and process high-priority alerts.
- Maintain alert history.

#### Data Structures

- **Binary Max Heap** — Organizes pending alerts according to priority.
- **Linked List** — Maintains alert history.

#### Alert Categories

- Normal
- Warning
- Critical

Alert severity helps identify telemetry conditions requiring attention.

---

### Module 4 — Search Management

Responsible for searching telemetry records and retrieving matching results.

#### Responsibilities

- Search telemetry records.
- Find records using supported search criteria.
- Retrieve matching telemetry information.
- Support efficient record lookup.

Search functionality allows users to locate relevant telemetry data without manually examining the entire record collection.

The module uses the search algorithms implemented in the project to perform record lookup.

---

### Module 5 — Statistics Management

Responsible for analyzing telemetry data and calculating statistical summaries.

#### Responsibilities

- Calculate average values.
- Identify minimum values.
- Identify maximum values.
- Summarize telemetry data.
- Analyze severity distribution.
- Provide numerical information for monitoring and reporting.

Statistics provide an overview of the collected telemetry data and help identify trends or unusual conditions.

---

### Module 6 — Export Management

Responsible for exporting telemetry information into an external format.

#### Responsibilities

- Prepare telemetry records for export.
- Generate structured output.
- Support CSV-based data export.
- Make telemetry information available for external analysis.

CSV format allows exported data to be opened using spreadsheet applications and processed using other analytical tools.

---

## 🖥️ Web Dashboard

Chronos Monitor includes a web-based dashboard designed to provide a graphical interface for the monitoring system.

The dashboard presents telemetry information, alerts, historical records, and statistics in an organized and accessible format.

### Dashboard Features

- Monitoring overview.
- Metric summary cards.
- Telemetry charts and graphs.
- Alert monitoring.
- Historical telemetry records.
- Statistical summaries.
- Telemetry search.
- Filtering and record inspection.
- Manual telemetry generation.
- Data export interface.

### Dashboard Sections

| Section | Purpose |
|---|---|
| Dashboard | Provides an overview of monitoring activity and metrics |
| Alert Manager | Displays and manages alerts |
| Telemetry History | Displays stored telemetry records |
| Statistics / Analytics | Presents statistical summaries |
| Search | Locates relevant telemetry records |
| Manual Generator | Allows telemetry input or generation |
| Export | Provides telemetry data export functionality |

### Interface Design

The dashboard uses a dark-themed interface with:

- Sidebar navigation.
- Metric cards.
- Graphical data presentation.
- Status indicators.
- Severity-based colors.
- Tables for records and alerts.
- Dedicated sections for monitoring operations.

---

## 🔗 C++ and Web Integration

The project integrates its C++ monitoring logic with a web-based frontend through a backend/API communication layer.

### Integration Structure

```text
+------------------------------------------+
|              WEB FRONTEND               |
|           HTML / CSS / JS              |
+------------------------------------------+
                    |
                    | HTTP Requests
                    v
+------------------------------------------+
|           BACKEND / API BRIDGE         |
|       Request Handling and JSON         |
+------------------------------------------+
                    |
                    v
+------------------------------------------+
|             C++ CORE SYSTEM           |
|                                          |
| Telemetry | Data | Alerts | Search       |
| Statistics | Export                    |
+------------------------------------------+
                    |
                    v
+------------------------------------------+
|       Response Data to Frontend        |
+------------------------------------------+
```

The API layer acts as a communication bridge between the web interface and the C++ monitoring functionality.

It allows the frontend to request data and operations from the backend rather than independently managing the monitoring logic.

### API Responsibilities

- Process frontend requests.
- Retrieve telemetry information.
- Provide summary data.
- Handle search requests.
- Process telemetry-related operations.
- Provide export functionality.
- Return structured responses to the frontend.

> The C++ modules remain responsible for the core monitoring and data-processing logic. The frontend is responsible for visualization and user interaction.

---

## 📊 Data Structures Used

Chronos Monitor demonstrates the practical use of data structures in a telemetry monitoring environment.

| Data Structure | Module | Purpose |
|---|---|---|
| Circular Queue | Data Management | Maintains recent telemetry records within a fixed capacity |
| Vector | Data Management | Stores historical telemetry records |
| Binary Max Heap | Alert Management | Prioritizes pending alerts |
| Linked List | Alert Management | Maintains alert history |
| Search Structures / Algorithms | Search Management | Locates telemetry records |
| Statistical Processing | Statistics Management | Calculates summaries from telemetry data |

### Why These Data Structures?

- **Circular Queue:** Efficiently reuses storage space for recent records.
- **Vector:** Supports dynamically growing historical storage and indexed access.
- **Binary Max Heap:** Provides priority-based access to pending alerts.
- **Linked List:** Supports maintaining an alert history collection.
- **Search Algorithms:** Allow records to be located using implemented searching techniques.

---

## ⚙️ Technology Stack

### Core System

- C++
- Object-Oriented Programming
- Modular software architecture
- Data Structures and Algorithms

### Web Interface

- HTML
- CSS
- JavaScript

### Backend / Integration

- HTTP-based communication
- API endpoints
- JSON data exchange
- Backend bridge connecting the frontend and C++ modules

### Data Export

- CSV

---

## 📁 Project Structure

The following represents the logical organization of the project. The actual directory layout may differ.

```text
Chronos-Monitor/
│
├── Telemetry/
│   ├── Telemetry_Record.h
│   ├── Telemetry_Record.cpp
│   ├── Telemetry_Generator.h
│   └── Telemetry_Generator.cpp
│
├── DataManagement/
│   ├── Data_Manager.h
│   └── Data_Manager.cpp
│
├── AlertManagement/
│   ├── Alert.h
│   ├── Alert.cpp
│   ├── Alert_Manager.h
│   └── Alert_Manager.cpp
│
├── Search/
│   ├── Search_Manager.h
│   └── Search_Manager.cpp
│
├── Statistics/
│   ├── Statistics_Manager.h
│   └── Statistics_Manager.cpp
│
├── Export/
│   ├── Export_Manager.h
│   └── Export_Manager.cpp
│
├── Frontend/
│   ├── index.html
│   ├── styles/
│   ├── scripts/
│   └── assets/
│
├── Backend/
│   └── API / Integration Files
│
├── Tests/
│
├── main.cpp
├── README.md
└── Build Configuration Files
```

---

## 🚀 Getting Started

### Prerequisites

Ensure that the following tools are installed:

- C++ compiler supporting the project's C++ standard.
- C++ development environment.
- Build tools used by the project.
- Python, if required by the backend.
- A modern web browser.

Additional libraries or dependencies may be required depending on the HTTP bridge and build configuration.

### Installation

Clone the repository:

```bash
git clone https://github.com/retroaadi/chronos-monitor.git
```

Navigate to the project directory:

```bash
cd chronos-monitor
```

Install the required dependencies according to the project's build configuration and backend requirements.

### Build

Compile the C++ components using the project's configured build system.

For a simple g++-based project, compilation may resemble:

```bash
g++ -std=c++17 *.cpp -o chronos_monitor
```

> Use the project's actual build command if it includes additional source directories, libraries, or HTTP dependencies. The command above is only a generic example.

### Run

Start the compiled C++ application:

```bash
./chronos_monitor
```

If the project includes a separate backend/API server, start it using the project's configured startup command.

Then open the web dashboard in your browser using the local URL configured by the application.

> Refer to the actual project configuration or startup scripts for the exact execution commands and port.

---

## 🧪 Testing

Testing is performed at the module level and during system integration.

### Module Testing

Each module can be tested independently to verify its functionality.

| Module | Testing Focus |
|---|---|
| Telemetry Generation | Record creation, values, timestamps, status |
| Data Management | Record insertion, retrieval, recent-data handling, history |
| Alert Management | Alert creation, severity, priority handling, history |
| Search | Search results and matching records |
| Statistics | Average, minimum, maximum, and summaries |
| Export | Output formatting and exported data |

### Integration Testing

Integration testing verifies that the modules communicate and operate correctly together.

The overall workflow should verify:

1. Telemetry records are generated.
2. Records are passed to data management.
3. Records are stored and retrieved correctly.
4. Alert conditions are evaluated.
5. Alerts are maintained and prioritized.
6. Search retrieves the appropriate records.
7. Statistics are calculated from available telemetry.
8. Export produces the intended data.
9. The frontend receives and presents the backend information correctly.

### Web Interface Testing

The dashboard should also be tested for:

- Navigation between sections.
- Correct metric display.
- Chart updates.
- Search and filtering.
- Manual telemetry input.
- Alert presentation.
- Export operations.
- API communication.
- Error handling.

---

## 📈 Expected Workflow

```text
        START
          |
          v
  Generate Telemetry
          |
          v
   Create Telemetry Record
          |
          v
    Store the Record
          |
          v
  Evaluate Alert Conditions
          |
     +----+----+
     |         |
     v         v
   Normal   Warning/Critical
     |         |
     |         v
     |    Create Alert
     |         |
     |         v
     |    Prioritize Alert
     |         |
     +----+----+
          |
          v
  Search / Statistics
          |
          v
       Export
          |
          v
  Display Through Web UI
          |
          v
         END
```

---

## 🔮 Future Scope

Potential future improvements include:

- Persistent telemetry storage.
- Advanced filtering and search.
- More detailed analytics.
- Additional metric categories.
- Configurable alert thresholds.
- Improved alert lifecycle management.
- Historical trend analysis.
- Real-time dashboard updates.
- Authentication and authorization.
- Multi-user support.
- Remote monitoring capabilities.
- Additional export formats.
- Performance optimization for larger datasets.
- Deployment on a dedicated monitoring server.

---

## 👥 Team

### ByteCare

Chronos Monitor was developed as a collaborative project involving modular development, implementation, testing, integration, and documentation.

Team members:

- Aradhya Uniyal
- Uddhav Bhargava
- Nikita

The project follows a collaborative development approach in which modules are developed and tested independently before being integrated into the overall application.

---

## 🎓 Learning Outcomes

Through Chronos Monitor, the project demonstrates practical understanding of:

- C++ programming.
- Object-oriented design.
- Modular software development.
- Data structures and algorithms.
- Telemetry record management.
- Priority-based alerting.
- Searching and statistical analysis.
- File export.
- Backend/frontend communication.
- Software integration and testing.
- Development of a graphical monitoring application.

---

## 📌 Conclusion

Chronos Monitor combines telemetry generation, data management, alert handling, search, statistical analysis, and export functionality into a unified monitoring application.

Its modular C++ core demonstrates the practical use of data structures and algorithms, while its web dashboard provides a graphical interface for monitoring and interacting with the system.

The project represents an integrated approach to building a monitoring application by combining structured data processing, modular programming, and frontend visualization.

---

**Chronos Monitor — Observe. Analyze. Respond.**
```

---