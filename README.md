
# Chronos Monitor
## Phase 2 — Working Implementation and Technical Progress

Chronos Monitor is a C++ academic project developed by Team ByteCare to demonstrate telemetry monitoring and alert management using Data Structures and Object-Oriented Programming (OOP).

This README documents **Phase 2 only**: the implementation completed so far and the work planned for the next phase. It does not present planned features as already completed.

## Project Information

| Detail | Information |
|---|---|
| Project | Chronos Monitor |
| Phase | Phase 2 — Working Implementation and Technical Progress |
| Project Type | Academic Project |
| Language | C++ |
| Interface | Console-Based |
| Team | ByteCare |
| Team ID | DSCPP-III-2026-T125d |
| Semester | 3rd Semester |
| Courses | TCS302 — Data Structures in C; TCS307 — Object Oriented Programming with C++ |
| Mentor | Mr. Utkarsh Pant Sir |

## 1. Phase 2 Work Completed

### Module 1 — Telemetry Records and Generation

The telemetry module provides the basic monitoring data used by the project.

**Work completed:**
- Implemented a `Telemetry_Record` representation for individual telemetry entries.
- Implemented a telemetry generator to produce simulated monitoring records.
- Added a test program to exercise telemetry record and generation functionality.
- Established the input data that later modules can process.

**Current status:** The implementation uses simulated telemetry. Direct collection of live operating-system metrics is not claimed as completed.

### Module 2 — Data Storage

The data-storage module provides the storage layer for telemetry processing.

**Work completed so far:**
- Data storage is part of the Phase 2 implementation.
- Further storage behavior, persistence, and integration remain to be completed or verified against the current source code.

### Module 3 — Alert Management

The alert-management module processes telemetry against alert conditions and organizes generated alerts.

**Work completed:**
- Implemented alert-related classes, including `Alert` and `Alert_Manager`.
- Added a test program for alert-management functionality.
- Established the alert-processing component for use with telemetry data.

The exact threshold rules, severity levels, and processing behavior should be verified against the current implementation before being described as final.

## 2. Current Phase 2 Status

At the end of Phase 2, the project has working implementations and test programs for the core telemetry and alert-management components, with the storage component forming part of the ongoing module development.

Phase 2 has focused on:
- C++ module implementation.
- Class-based design and OOP practices.
- Applying Data Structures concepts.
- Testing modules individually.
- Preparing modules for integration and demonstration.

## 3. Future Work — Phase 3

The following tasks are planned and should not be treated as completed functionality:

- Integrate Modules 1, 2, and 3 into one end-to-end workflow.
- Verify data flow from telemetry generation through storage and alert processing.
- Complete and validate searching and sorting features.
- Add or finalize statistics and reporting.
- Expand integration tests and test edge cases.
- Improve error handling and consistency across modules.
- Review performance with larger amounts of telemetry data.
- Update documentation and prepare the final project demonstration.
- Investigate live system-metric collection as a possible extension, if time and project scope allow.

## 4. Team Contributions

### Aradhya Uniyal — Team Lead
- Project coordination.
- Integration planning.
- Testing.
- Documentation.
- Presentation.

### Nikita
- OOP class design.
- Telemetry input and generation.
- Object-oriented system organization.

### Uddhav Bhargava
- Data Structures implementation.
- Data storage.
- Alert management.

## 5. Technology Stack

- **C++** — Primary programming language.
- **Data Structures** — Core data management.
- **C++ Standard Library** — Supporting functionality, where applicable.
- **C++ Compiler** — Compilation and execution.
- **Visual Studio Code** — Development environment.
- **Git and GitHub** — Version control and collaboration.
- **Console Interface** — Primary user interface.

## 6. Phase 2 Summary

Phase 2 has established the core C++ components for telemetry records and generation, data storage, and alert management.

The next phase will focus on integrating the modules, validating the complete workflow, finishing remaining search, sorting, statistics, and reporting work, and preparing the final demonstration.

---

**Chronos Monitor — Team ByteCare**

*Academic Project | C++ | Data Structures | Object-Oriented Programming*
