#define _WIN32_WINNT 0x0601
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <random>
// Include existing C++ Chronos Monitor modules directly
#include "Telemetry_Record.h"
#include "Telemetry_Generator.h"
#include "Data_Manager.h"
#include "Alert_Manager.h"
#include "Search_Manager.h"
#include "Statistics_Manager.h"
#include "Export_Manager.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Global state holding the single instances of the C++ modules
static TelemetryGenerator g_telemetryGenerator;
static DataManager g_dataManager;
static AlertManager g_alertManager;
static SearchManager g_searchManager;
static StatisticsManager g_statisticsManager;
static ExportManager g_exportManager;
// Thread-safe mutex & lock guard using Windows native synchronization
// (Guarantees universal compilation across all MinGW/GCC versions without POSIX thread dependencies)
class Mutex {
    CRITICAL_SECTION cs;
public:
    Mutex() { InitializeCriticalSection(&cs); }
    ~Mutex() { DeleteCriticalSection(&cs); }
    void lock() { EnterCriticalSection(&cs); }
    void unlock() { LeaveCriticalSection(&cs); }
};

class LockGuard {
    Mutex& m;
public:
    explicit LockGuard(Mutex& mutex) : m(mutex) { m.lock(); }
    ~LockGuard() { m.unlock(); }
};

static Mutex g_moduleMutex;

// Generate realistic telemetry across all 5 metrics, overcoming MinGW's deterministic random_device bug
static TelemetryRecord generateRealisticRecord(int nextID) {
    static mt19937 prng(static_cast<unsigned int>(chrono::high_resolution_clock::now().time_since_epoch().count()));
    static int metricCycle = 0;

    const vector<string> metrics = {"CPU", "Memory", "Network", "ResponseTime", "ErrorCount"};
    string metric = metrics[metricCycle % metrics.size()];
    metricCycle++;

    double val = 0.0;
    if (metric == "CPU") {
        uniform_real_distribution<double> dist(25.0, 95.0);
        val = dist(prng);
    } else if (metric == "Memory") {
        uniform_real_distribution<double> dist(35.0, 95.0);
        val = dist(prng);
    } else if (metric == "Network") {
        uniform_real_distribution<double> dist(5.0, 95.0);
        val = dist(prng);
    } else if (metric == "ResponseTime") {
        uniform_real_distribution<double> dist(60.0, 650.0);
        val = dist(prng);
    } else if (metric == "ErrorCount") {
        uniform_int_distribution<int> dist(0, 18);
        val = dist(prng);
    }

    time_t now = time(nullptr);
    tm* lt = localtime(&now);
    char tBuf[16];
    strftime(tBuf, sizeof(tBuf), "%H:%M:%S", lt);

    return TelemetryRecord(nextID, metric, val, string(tBuf));
}

// Helper to escape JSON strings
static string jsonEscape(const string& str) {
    ostringstream o;
    for (char c : str) {
        switch (c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    o << "\\u" << hex << setfill('0') << setw(4) << (int)c;
                } else {
                    o << c;
                }
        }
    }
    return o.str();
}

// Convert a single TelemetryRecord to JSON
static string recordToJson(const TelemetryRecord& rec) {
    ostringstream oss;
    oss << "{"
        << "\"record_ID\":" << rec.read_Record_ID() << ","
        << "\"metric_Name\":\"" << jsonEscape(rec.read_Metric_Name()) << "\","
        << "\"value\":" << fixed << setprecision(1) << rec.read_Value() << ","
        << "\"time_Stamp\":\"" << jsonEscape(rec.read_Time_stamp()) << "\","
        << "\"status\":\"" << jsonEscape(rec.read_Status()) << "\","
        << "\"severity\":" << rec.read_Severity()
        << "}";
    return oss.str();
}

// Parse displayAlert output captured from std::cout
struct ParsedAlert {
    int alertID = 0;
    int recordID = 0;
    string metricName = "";
    double value = 0.0;
    string timestamp = "";
    string status = "";
    int severity = 0;
};

static vector<ParsedAlert> parseAlertsFromStream(const string& text) {
    vector<ParsedAlert> list;
    istringstream iss(text);
    string line;
    ParsedAlert cur;
    bool inAlert = false;

    while (getline(iss, line)) {
        if (line.find("ALERT") != string::npos && line.find("==========") == string::npos) {
            inAlert = true;
            cur = ParsedAlert();
        } else if (line.find("Alert ID") != string::npos) {
            size_t colon = line.find(':');
            if (colon != string::npos) cur.alertID = atoi(line.substr(colon + 1).c_str());
        } else if (line.find("Record ID") != string::npos) {
            size_t colon = line.find(':');
            if (colon != string::npos) cur.recordID = atoi(line.substr(colon + 1).c_str());
        } else if (line.find("Metric") != string::npos) {
            size_t colon = line.find(':');
            if (colon != string::npos) {
                string val = line.substr(colon + 1);
                size_t start = val.find_first_not_of(" \t\r\n");
                size_t end = val.find_last_not_of(" \t\r\n");
                cur.metricName = (start != string::npos) ? val.substr(start, end - start + 1) : "";
            }
        } else if (line.find("Value") != string::npos) {
            size_t colon = line.find(':');
            if (colon != string::npos) cur.value = atof(line.substr(colon + 1).c_str());
        } else if (line.find("Timestamp") != string::npos) {
            size_t colon = line.find(':');
            if (colon != string::npos) {
                string val = line.substr(colon + 1);
                size_t start = val.find_first_not_of(" \t\r\n");
                size_t end = val.find_last_not_of(" \t\r\n");
                cur.timestamp = (start != string::npos) ? val.substr(start, end - start + 1) : "";
            }
        } else if (line.find("Status") != string::npos) {
            size_t colon = line.find(':');
            if (colon != string::npos) {
                string val = line.substr(colon + 1);
                size_t start = val.find_first_not_of(" \t\r\n");
                size_t end = val.find_last_not_of(" \t\r\n");
                cur.status = (start != string::npos) ? val.substr(start, end - start + 1) : "";
            }
        } else if (line.find("Severity") != string::npos) {
            size_t colon = line.find(':');
            if (colon != string::npos) {
                cur.severity = atoi(line.substr(colon + 1).c_str());
                if (cur.alertID > 0) {
                    list.push_back(cur);
                    cur = ParsedAlert();
                }
            }
        }
    }
    return list;
}

static string alertToJson(const ParsedAlert& a) {
    ostringstream oss;
    oss << "{"
        << "\"alertID\":" << a.alertID << ","
        << "\"recordID\":" << a.recordID << ","
        << "\"metricName\":\"" << jsonEscape(a.metricName) << "\","
        << "\"value\":" << fixed << setprecision(1) << a.value << ","
        << "\"timestamp\":\"" << jsonEscape(a.timestamp) << "\","
        << "\"status\":\"" << jsonEscape(a.status) << "\","
        << "\"severity\":" << a.severity
        << "}";
    return oss.str();
}

// Helper to capture cout from an alert display method
static vector<ParsedAlert> captureAlertsFromMethod(void (*func)(AlertManager&)) {
    stringstream buffer;
    streambuf* oldCout = cout.rdbuf(buffer.rdbuf());
    func(g_alertManager);
    cout.rdbuf(oldCout);
    return parseAlertsFromStream(buffer.str());
}

static void callDisplayPriority(AlertManager& am) {
    am.displayPriorityAlerts();
}

static void callDisplayHistory(AlertManager& am) {
    am.displayAlertHistory();
}

// Safe transmission loop for large HTTP payloads (HTML, CSS, JS, CSV)
static void sendAll(SOCKET s, const string& data) {
    int total = 0;
    int len = (int)data.size();
    while (total < len) {
        int sent = send(s, data.c_str() + total, len - total, 0);
        if (sent <= 0) break;
        total += sent;
    }
    shutdown(s, SD_SEND);
    closesocket(s);
}

// Generate HTTP response
static string makeHttpResponse(int statusCode, const string& contentType, const string& body, const string& extraHeaders = "") {
    string statusText = (statusCode == 200) ? "OK" : ((statusCode == 404) ? "Not Found" : "Internal Server Error");
    ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    oss << "Access-Control-Allow-Headers: Content-Type\r\n";
    if (!extraHeaders.empty()) oss << extraHeaders;
    oss << "Connection: close\r\n\r\n";
    oss << body;
    return oss.str();
}

// Safe URL decoding
static string urlDecode(const string& in) {
    string out;
    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '+') {
            out += ' ';
        } else if (in[i] == '%' && i + 2 < in.size()) {
            string hexStr = in.substr(i + 1, 2);
            char c = (char)strtol(hexStr.c_str(), nullptr, 16);
            out += c;
            i += 2;
        } else {
            out += in[i];
        }
    }
    return out;
}

// Query parameters parser
static string getQueryParam(const string& url, const string& param) {
    size_t qPos = url.find('?');
    if (qPos == string::npos) return "";
    string query = url.substr(qPos + 1);
    size_t pos = 0;
    while (pos < query.size()) {
        size_t nextPos = query.find('&', pos);
        string pair = (nextPos == string::npos) ? query.substr(pos) : query.substr(pos, nextPos - pos);
        size_t eqPos = pair.find('=');
        if (eqPos != string::npos) {
            string key = pair.substr(0, eqPos);
            if (key == param) {
                return urlDecode(pair.substr(eqPos + 1));
            }
        }
        if (nextPos == string::npos) break;
        pos = nextPos + 1;
    }
    return "";
}

// Read whole binary file
static bool readFile(const string& path, string& content) {
    ifstream f(path, ios::binary);
    if (!f.is_open()) return false;
    ostringstream ss;
    ss << f.rdbuf();
    content = ss.str();
    return true;
}

// Determine content type from extension
static string getMimeType(const string& path) {
    if (path.rfind(".html") != string::npos) return "text/html; charset=utf-8";
    if (path.rfind(".css") != string::npos) return "text/css; charset=utf-8";
    if (path.rfind(".js") != string::npos) return "application/javascript; charset=utf-8";
    if (path.rfind(".json") != string::npos) return "application/json; charset=utf-8";
    if (path.rfind(".csv") != string::npos) return "text/csv; charset=utf-8";
    if (path.rfind(".png") != string::npos) return "image/png";
    return "text/plain; charset=utf-8";
}

// Handle client connection
static void handleClient(SOCKET clientSocket) {
    char buffer[8192];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        closesocket(clientSocket);
        return;
    }
    buffer[bytesRead] = '\0';
    string requestStr(buffer, bytesRead);

    istringstream reqStream(requestStr);
    string method, fullPath, protocol;
    reqStream >> method >> fullPath >> protocol;

    // Handle OPTIONS CORS preflight
    if (method == "OPTIONS") {
        string res = makeHttpResponse(200, "text/plain", "");
        sendAll(clientSocket, res);
        return;
    }

    string path = fullPath;
    size_t qPos = path.find('?');
    if (qPos != string::npos) path = path.substr(0, qPos);

    string response;

    // -------------------------------------------------------------
    // ENDPOINT: GET /api/summary
    // Integrates: Module 1, 2, 3, 5
    // -------------------------------------------------------------
    if (path == "/api/summary" && method == "GET") {
        LockGuard lock(g_moduleMutex);
        const vector<TelemetryRecord>& history = g_dataManager.getHistory();

        // Metric averages, min, max from Module 5 (StatisticsManager)
        vector<string> metrics = {"CPU", "Memory", "Network", "ResponseTime", "ErrorCount"};
        ostringstream metricsJson;
        metricsJson << "{";

        for (size_t i = 0; i < metrics.size(); ++i) {
            const string& m = metrics[i];
            double avg = g_statisticsManager.getAverageValue(history, m);
            double maxV = g_statisticsManager.getMaximumValue(history, m);
            double minV = g_statisticsManager.getMinimumValue(history, m);

            // Latest observed value for this metric
            double latestVal = 0.0;
            string latestStat = "NORMAL";
            int latestSev = 0;
            string latestTime = "--:--:--";
            for (auto it = history.rbegin(); it != history.rend(); ++it) {
                if (it->read_Metric_Name() == m) {
                    latestVal = it->read_Value();
                    latestStat = it->read_Status();
                    latestSev = it->read_Severity();
                    latestTime = it->read_Time_stamp();
                    break;
                }
            }

            metricsJson << "\"" << m << "\":{"
                        << "\"value\":" << fixed << setprecision(1) << latestVal << ","
                        << "\"status\":\"" << latestStat << "\","
                        << "\"severity\":" << latestSev << ","
                        << "\"lastUpdated\":\"" << latestTime << "\","
                        << "\"average\":" << fixed << setprecision(1) << avg << ","
                        << "\"max\":" << maxV << ","
                        << "\"min\":" << minV
                        << "}" << (i + 1 < metrics.size() ? "," : "");
        }
        metricsJson << "}";

        // Stats from Module 5
        ostringstream statsJson;
        statsJson << "{"
                  << "\"totalRecords\":" << g_statisticsManager.getTotalRecords(history) << ","
                  << "\"normalCount\":" << g_statisticsManager.getNormalCount(history) << ","
                  << "\"warningCount\":" << g_statisticsManager.getWarningCount(history) << ","
                  << "\"criticalCount\":" << g_statisticsManager.getCriticalCount(history) << ","
                  << "\"recentQueueCount\":" << g_dataManager.getCount() << ","
                  << "\"recentQueueCapacity\": 5,"
                  << "\"totalAlerts\":" << g_alertManager.getAlertCount()
                  << "}";

        // Recent 5 records from Module 2 (Circular Queue)
        ostringstream recentJson;
        recentJson << "[";
        int startIdx = max(0, (int)history.size() - 5);
        for (int i = (int)history.size() - 1; i >= startIdx; --i) {
            recentJson << recordToJson(history[i]);
            if (i > startIdx) recentJson << ",";
        }
        recentJson << "]";

        // Priority alerts from Module 3 (Binary Max Heap)
        vector<ParsedAlert> pAlerts = captureAlertsFromMethod(callDisplayPriority);
        ostringstream alertsJson;
        alertsJson << "[";
        for (size_t i = 0; i < pAlerts.size() && i < 10; ++i) {
            alertsJson << alertToJson(pAlerts[i]);
            if (i + 1 < pAlerts.size() && i + 1 < 10) alertsJson << ",";
        }
        alertsJson << "]";

        time_t now = time(nullptr);
        tm* lt = localtime(&now);
        char timeBuf[16];
        strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", lt);

        ostringstream body;
        body << "{"
             << "\"status\":\"success\","
             << "\"mode\":\"CPP_NATIVE_BACKEND\","
             << "\"lastUpdated\":\"" << timeBuf << "\","
             << "\"activeMetrics\":" << metricsJson.str() << ","
             << "\"stats\":" << statsJson.str() << ","
             << "\"recentRecords\":" << recentJson.str() << ","
             << "\"recentAlerts\":" << alertsJson.str()
             << "}";

        response = makeHttpResponse(200, "application/json", body.str());
    }

    // -------------------------------------------------------------
    // ENDPOINT: GET /api/history
    // Integrates: Module 2 (DataManager complete history)
    // -------------------------------------------------------------
    else if (path == "/api/history" && method == "GET") {
        LockGuard lock(g_moduleMutex);
        const vector<TelemetryRecord>& history = g_dataManager.getHistory();
        string metricFilter = getQueryParam(fullPath, "metric");
        string severityFilter = getQueryParam(fullPath, "severity");
        string query = getQueryParam(fullPath, "q");

        ostringstream body;
        body << "{\"total\":" << history.size() << ",\"records\":[";
        bool first = true;

        for (int i = (int)history.size() - 1; i >= 0; --i) {
            const TelemetryRecord& r = history[i];
            if (!metricFilter.empty() && metricFilter != "ALL" && r.read_Metric_Name() != metricFilter) continue;
            if (!severityFilter.empty() && severityFilter != "ALL" && to_string(r.read_Severity()) != severityFilter) continue;
            if (!query.empty()) {
                string qLower = query;
                transform(qLower.begin(), qLower.end(), qLower.begin(), ::tolower);
                string mLower = r.read_Metric_Name();
                transform(mLower.begin(), mLower.end(), mLower.begin(), ::tolower);
                string sLower = r.read_Status();
                transform(sLower.begin(), sLower.end(), sLower.begin(), ::tolower);
                string idStr = to_string(r.read_Record_ID());

                if (mLower.find(qLower) == string::npos &&
                    sLower.find(qLower) == string::npos &&
                    idStr.find(qLower) == string::npos) {
                    continue;
                }
            }

            if (!first) body << ",";
            body << recordToJson(r);
            first = false;
        }
        body << "]}";

        response = makeHttpResponse(200, "application/json", body.str());
    }

    // -------------------------------------------------------------
    // ENDPOINT: GET /api/search?id=...&mode=linear|binary
    // Integrates: Module 4 (SearchManager) & Module 2 (DataManager)
    // -------------------------------------------------------------
    else if (path == "/api/search" && method == "GET") {
        LockGuard lock(g_moduleMutex);
        string idStr = getQueryParam(fullPath, "id");
        string mode = getQueryParam(fullPath, "mode");
        int targetID = atoi(idStr.c_str());

        const vector<TelemetryRecord>& history = g_dataManager.getHistory();
        int foundIndex = -1;

        if (mode == "binary") {
            // Binary search requires ascending order by Record ID
            vector<TelemetryRecord> sortedCopy = history;
            sort(sortedCopy.begin(), sortedCopy.end(), [](const TelemetryRecord& a, const TelemetryRecord& b) {
                return a.read_Record_ID() < b.read_Record_ID();
            });
            foundIndex = g_searchManager.binarySearch(sortedCopy, targetID);
            if (foundIndex != -1) {
                ostringstream body;
                body << "{\"found\":true,\"mode\":\"binary\",\"index\":" << foundIndex
                     << ",\"record\":" << recordToJson(sortedCopy[foundIndex]) << "}";
                response = makeHttpResponse(200, "application/json", body.str());
            } else {
                response = makeHttpResponse(200, "application/json", "{\"found\":false,\"mode\":\"binary\"}");
            }
        } else {
            // Linear search
            foundIndex = g_searchManager.linearSearch(history, targetID);
            if (foundIndex != -1) {
                ostringstream body;
                body << "{\"found\":true,\"mode\":\"linear\",\"index\":" << foundIndex
                     << ",\"record\":" << recordToJson(history[foundIndex]) << "}";
                response = makeHttpResponse(200, "application/json", body.str());
            } else {
                response = makeHttpResponse(200, "application/json", "{\"found\":false,\"mode\":\"linear\"}");
            }
        }
    }

    // -------------------------------------------------------------
    // ENDPOINT: GET /api/alerts
    // Integrates: Module 3 (AlertManager)
    // -------------------------------------------------------------
    else if (path == "/api/alerts" && method == "GET") {
        LockGuard lock(g_moduleMutex);
        vector<ParsedAlert> heapAlerts = captureAlertsFromMethod(callDisplayPriority);
        vector<ParsedAlert> historyAlerts = captureAlertsFromMethod(callDisplayHistory);

        ostringstream body;
        body << "{\"totalAlerts\":" << g_alertManager.getAlertCount()
             << ",\"priorityQueue\":[";
        for (size_t i = 0; i < heapAlerts.size(); ++i) {
            body << alertToJson(heapAlerts[i]);
            if (i + 1 < heapAlerts.size()) body << ",";
        }
        body << "],\"alertHistory\":[";
        for (size_t i = 0; i < historyAlerts.size(); ++i) {
            body << alertToJson(historyAlerts[i]);
            if (i + 1 < historyAlerts.size()) body << ",";
        }
        body << "]}";

        response = makeHttpResponse(200, "application/json", body.str());
    }

    // -------------------------------------------------------------
    // ENDPOINT: POST /api/alerts/pop
    // Integrates: Module 3 (AlertManager::getHighestPriorityAlert())
    // -------------------------------------------------------------
    else if (path == "/api/alerts/pop" && method == "POST") {
        LockGuard lock(g_moduleMutex);
        if (g_alertManager.isPriorityQueueEmpty()) {
            response = makeHttpResponse(200, "application/json", "{\"success\":false,\"message\":\"Priority queue is empty\"}");
        } else {
            Alert top = g_alertManager.getHighestPriorityAlert();
            ostringstream body;
            body << "{\"success\":true,\"alert\":{"
                 << "\"alertID\":" << top.getAlertID() << ","
                 << "\"recordID\":" << top.getRecordID() << ","
                 << "\"metricName\":\"" << jsonEscape(top.getMetricName()) << "\","
                 << "\"value\":" << fixed << setprecision(1) << top.getValue() << ","
                 << "\"timestamp\":\"" << jsonEscape(top.getTimestamp()) << "\","
                 << "\"status\":\"" << jsonEscape(top.getStatus()) << "\","
                 << "\"severity\":" << top.getSeverity()
                 << "}}";
            response = makeHttpResponse(200, "application/json", body.str());
        }
    }

    // -------------------------------------------------------------
    // ENDPOINT: POST /api/record
    // Integrates: Module 1 (TelemetryGenerator), Module 2, Module 3
    // -------------------------------------------------------------
    else if (path == "/api/record" && method == "POST") {
        LockGuard lock(g_moduleMutex);
        string mode = getQueryParam(fullPath, "mode");
        string metric = getQueryParam(fullPath, "metric");
        string valStr = getQueryParam(fullPath, "val");

        TelemetryRecord record;

        if (!metric.empty() && !valStr.empty()) {
            // Manual record generation with Module 1 TelemetryRecord
            double val = atof(valStr.c_str());
            time_t now = time(nullptr);
            tm* lt = localtime(&now);
            char tBuf[16];
            strftime(tBuf, sizeof(tBuf), "%H:%M:%S", lt);

            int nextID = g_dataManager.getHistoryCount() + 1;
            record = TelemetryRecord(nextID, metric, val, string(tBuf));
        } else {
            // Automatic generation across all 5 metrics
            int nextID = g_dataManager.getHistoryCount() + 1;
            record = generateRealisticRecord(nextID);
        }

        // Store in Module 2 DataManager and process in Module 3 AlertManager
        g_dataManager.addRecord(record);
        g_alertManager.processRecord(record);

        ostringstream body;
        body << "{\"success\":true,\"record\":" << recordToJson(record) << "}";
        response = makeHttpResponse(200, "application/json", body.str());
    }

    // -------------------------------------------------------------
    // ENDPOINT: GET /api/export
    // Integrates: Module 6 (ExportManager)
    // -------------------------------------------------------------
    else if (path == "/api/export" && method == "GET") {
        LockGuard lock(g_moduleMutex);
        const vector<TelemetryRecord>& history = g_dataManager.getHistory();
        const string exportFilename = "telemetry_export.csv";

        // Call Module 6 exportTelemetryToCSV
        bool ok = g_exportManager.exportTelemetryToCSV(history, exportFilename);

        string csvContent;
        if (ok && readFile(exportFilename, csvContent)) {
            response = makeHttpResponse(200, "text/csv; charset=utf-8", csvContent,
                "Content-Disposition: attachment; filename=\"chronos_telemetry_export.csv\"\r\n");
        } else {
            response = makeHttpResponse(500, "application/json", "{\"error\":\"Failed to export CSV\"}");
        }
    }

    // -------------------------------------------------------------
    // STATIC FILE SERVING FOR FRONTEND
    // -------------------------------------------------------------
    else {
        string filePath = "web_frontend";
        if (path == "/" || path.empty()) {
            filePath += "/index.html";
        } else {
            filePath += path;
        }

        string fileContent;
        if (readFile(filePath, fileContent)) {
            response = makeHttpResponse(200, getMimeType(filePath), fileContent);
        } else {
            response = makeHttpResponse(404, "text/plain", "404 Not Found");
        }
    }

    sendAll(clientSocket, response);
}

// Thread entry point using Win32 CreateThread (compatible with all MinGW/GCC runtimes)
static DWORD WINAPI clientThreadProc(LPVOID lpParam) {
    SOCKET clientSocket = (SOCKET)reinterpret_cast<uintptr_t>(lpParam);
    handleClient(clientSocket);
    return 0;
}

int main() {
    cout << "========================================\n";
    cout << "     CHRONOS MONITOR - HTTP BRIDGE\n";
    cout << "  Connecting Modules 1 - 6 to Web UI\n";
    cout << "========================================\n";

    // 1. Pre-populate initial records across all 5 metrics using Module 1, 2, and 3
    cout << "Initializing telemetry foundation...\n";
    for (int i = 0; i < 15; ++i) {
        TelemetryRecord rec = generateRealisticRecord(i + 1);
        g_dataManager.addRecord(rec);
        g_alertManager.processRecord(rec);
    }
    cout << "Seeded " << g_dataManager.getHistoryCount() << " telemetry records into DataManager.\n";
    cout << "Registered " << g_alertManager.getAlertCount() << " alerts in AlertManager.\n";

    // 2. Initialize Winsock
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0) {
        cerr << "WSAStartup failed: " << wsResult << "\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    int port = 8080;
    serverAddr.sin_port = htons(port);

    if (::bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed on port " << port << ". Error: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed. Error: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "\n>>> Chronos Server is RUNNING at http://localhost:" << port << " <<<\n";
    cout << "Endpoints active:\n";
    cout << "  - GET  /api/summary    (Modules 1, 2, 3, 5)\n";
    cout << "  - GET  /api/history    (Module 2)\n";
    cout << "  - GET  /api/search     (Module 4)\n";
    cout << "  - GET  /api/alerts     (Module 3)\n";
    cout << "  - POST /api/alerts/pop (Module 3)\n";
    cout << "  - POST /api/record     (Module 1, 2, 3)\n";
    cout << "  - GET  /api/export     (Module 6)\n";
    cout << "  - GET  /               (Serves Web Frontend)\n";
    cout << "Press Ctrl+C to stop.\n\n";

    // Automatically open the web dashboard in default browser
    ShellExecuteA(NULL, "open", "http://localhost:8080/index.html", NULL, NULL, SW_SHOWNORMAL);

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            HANDLE hThread = CreateThread(NULL, 0, clientThreadProc, reinterpret_cast<LPVOID>(static_cast<uintptr_t>(clientSocket)), 0, NULL);
            if (hThread) {
                CloseHandle(hThread);
            }
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
