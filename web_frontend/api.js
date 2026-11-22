/**
 * Chronos Monitor - Data Source & API Integration Layer
 * 
 * INTEGRATION STATUS:
 * [x] Mock Mode Active (Generates realistic telemetry according to Module 1, 2, and 3 logic)
 * [ ] C++ Native Backend Connected (Will connect via HTTP REST / WebSocket once backend server is compiled)
 * 
 * To switch to C++ Backend in the future:
 * Set CONFIG.USE_MOCK_DATA = false and configure CONFIG.BACKEND_URL.
 */

export const CONFIG = {
  USE_MOCK_DATA: true,
  BACKEND_URL: 'http://localhost:8080/api',
  REFRESH_INTERVAL_MS: 3000,
  CIRCULAR_QUEUE_CAPACITY: 5,
  MAX_HISTORY_ITEMS: 100,
};

// State store mimicking C++ DataManager and AlertManager
class ChronosStore {
  constructor() {
    this.nextRecordId = 101;
    this.nextAlertId = 1;
    this.history = []; // mirrors vector<TelemetryRecord> history
    this.recentQueue = []; // mirrors recentRecords[QUEUE_SIZE] (size 5)
    this.priorityAlerts = []; // mirrors Alert priorityQueue[100] (Max Heap by severity)
    this.alertHistory = []; // mirrors AlertNode* history linked list
    this.activeMetrics = {
      CPU: { value: 45.2, status: 'NORMAL', severity: 0, unit: '%' },
      Memory: { value: 68.4, status: 'NORMAL', severity: 0, unit: '%' },
      Network: { value: 38.1, status: 'NORMAL', severity: 0, unit: 'Mbps' },
      ResponseTime: { value: 185.0, status: 'NORMAL', severity: 0, unit: 'ms' },
      ErrorCount: { value: 0, status: 'NORMAL', severity: 0, unit: 'errors' }
    };

    this.initMockHistory();
  }

  // Exact threshold formulas from C++ Telemetry_Record.cpp: calculateStatus()
  calculateStatusAndSeverity(metricName, value) {
    let status = 'NORMAL';
    let severity = 0;

    switch (metricName) {
      case 'CPU':
        if (value >= 85) { status = 'CRITICAL'; severity = 2; }
        else if (value >= 70) { status = 'WARNING'; severity = 1; }
        else { status = 'NORMAL'; severity = 0; }
        break;

      case 'Memory':
        if (value >= 90) { status = 'CRITICAL'; severity = 2; }
        else if (value >= 75) { status = 'WARNING'; severity = 1; }
        else { status = 'NORMAL'; severity = 0; }
        break;

      case 'Network':
        if (value >= 90) { status = 'CRITICAL'; severity = 2; }
        else if (value >= 75) { status = 'WARNING'; severity = 1; }
        else { status = 'NORMAL'; severity = 0; }
        break;

      case 'ResponseTime':
        if (value >= 500) { status = 'CRITICAL'; severity = 2; }
        else if (value >= 300) { status = 'WARNING'; severity = 1; }
        else { status = 'NORMAL'; severity = 0; }
        break;

      case 'ErrorCount':
        if (value >= 10) { status = 'CRITICAL'; severity = 2; }
        else if (value > 0) { status = 'WARNING'; severity = 1; }
        else { status = 'NORMAL'; severity = 0; }
        break;

      default:
        status = 'NORMAL';
        severity = 0;
    }

    return { status, severity };
  }

  // Formats timestamps like C++ TelemetryGenerator::get_Current_Time() (HH:MM:SS)
  getCurrentTimestamp(dateObj = new Date()) {
    const pad = (n) => String(n).padStart(2, '0');
    return `${pad(dateObj.getHours())}:${pad(dateObj.getMinutes())}:${pad(dateObj.getSeconds())}`;
  }

  // Seed realistic historical data
  initMockHistory() {
    const now = Date.now();
    const metrics = ['CPU', 'Memory', 'Network', 'ResponseTime', 'ErrorCount'];

    for (let i = 25; i >= 1; i--) {
      const timeStr = this.getCurrentTimestamp(new Date(now - i * 15000));
      const metric = metrics[i % metrics.length];
      let val = 0;

      if (metric === 'CPU') val = +(35 + Math.random() * 55).toFixed(1);
      else if (metric === 'Memory') val = +(60 + Math.random() * 34).toFixed(1);
      else if (metric === 'Network') val = +(20 + Math.random() * 70).toFixed(1);
      else if (metric === 'ResponseTime') val = +(100 + Math.random() * 450).toFixed(0);
      else if (metric === 'ErrorCount') val = Math.floor(Math.random() * 6);

      const { status, severity } = this.calculateStatusAndSeverity(metric, val);

      const record = {
        record_ID: this.nextRecordId++,
        metric_Name: metric,
        value: val,
        time_Stamp: timeStr,
        status,
        severity
      };

      this.addRecord(record);
    }
  }

  // Add record mimicking DataManager::addRecord & AlertManager::processRecord
  addRecord(record) {
    // 1. DataManager: History
    this.history.unshift(record);
    if (this.history.length > CONFIG.MAX_HISTORY_ITEMS) {
      this.history.pop();
    }

    // 2. DataManager: CircularQueue (Recent 5 records)
    this.recentQueue.unshift(record);
    if (this.recentQueue.length > CONFIG.CIRCULAR_QUEUE_CAPACITY) {
      this.recentQueue.pop();
    }

    // 3. Update active metrics current reading
    if (this.activeMetrics[record.metric_Name]) {
      this.activeMetrics[record.metric_Name].value = record.value;
      this.activeMetrics[record.metric_Name].status = record.status;
      this.activeMetrics[record.metric_Name].severity = record.severity;
      this.activeMetrics[record.metric_Name].lastUpdated = record.time_Stamp;
    }

    // 4. AlertManager: processRecord
    if (record.severity > 0) {
      const alert = {
        alertID: this.nextAlertId++,
        recordID: record.record_ID,
        metricName: record.metric_Name,
        value: record.value,
        timestamp: record.time_Stamp,
        status: record.status,
        severity: record.severity
      };

      // Add to alert history (linked list)
      this.alertHistory.unshift(alert);

      // Add to priority queue (Max Heap simulation - highest severity first)
      this.priorityAlerts.push(alert);
      this.priorityAlerts.sort((a, b) => b.severity - a.severity || b.alertID - a.alertID);
      if (this.priorityAlerts.length > 100) this.priorityAlerts.pop();
    }
  }

  // Generates 1 new simulated metric matching C++ TelemetryGenerator::generateRecord()
  generateSingleSimulatedMetric() {
    const choices = ['CPU', 'Memory', 'Network', 'ResponseTime', 'ErrorCount'];
    const choice = choices[Math.floor(Math.random() * choices.length)];
    let val = 0;

    switch (choice) {
      case 'CPU':
        // C++ range: 20.0 to 100.0
        val = +(20.0 + Math.random() * 80.0).toFixed(1);
        break;
      case 'Memory':
        // C++ range: 20.0 to 100.0
        val = +(20.0 + Math.random() * 80.0).toFixed(1);
        break;
      case 'Network':
        // C++ range: 0.0 to 100.0
        val = +(0.0 + Math.random() * 100.0).toFixed(1);
        break;
      case 'ResponseTime':
        // C++ range: 50.0 to 1000.0
        val = +(50.0 + Math.random() * 950.0).toFixed(0);
        break;
      case 'ErrorCount':
        // C++ range: 0 to 20
        val = Math.floor(Math.random() * 21);
        break;
    }

    const { status, severity } = this.calculateStatusAndSeverity(choice, val);

    const record = {
      record_ID: this.nextRecordId++,
      metric_Name: choice,
      value: val,
      time_Stamp: this.getCurrentTimestamp(),
      status,
      severity
    };

    this.addRecord(record);
    return record;
  }
}

const localStore = new ChronosStore();

/**
 * Clean API Service Layer
 * In mock mode: delegates to localStore.
 * In production mode: delegates to fetch(`${CONFIG.BACKEND_URL}/...`).
 */
export const ChronosAPI = {
  isMockMode() {
    return CONFIG.USE_MOCK_DATA;
  },

  async getDashboardSummary() {
    if (CONFIG.USE_MOCK_DATA) {
      // Calculate statistics
      const cpuRecords = localStore.history.filter(r => r.metric_Name === 'CPU');
      const memRecords = localStore.history.filter(r => r.metric_Name === 'Memory');
      const netRecords = localStore.history.filter(r => r.metric_Name === 'Network');
      const respRecords = localStore.history.filter(r => r.metric_Name === 'ResponseTime');

      const calcAvg = (arr) => arr.length ? +(arr.reduce((acc, r) => acc + r.value, 0) / arr.length).toFixed(1) : 0;

      return {
        mode: 'MOCK_DATA',
        activeMetrics: localStore.activeMetrics,
        stats: {
          avgCpu: calcAvg(cpuRecords),
          avgMemory: calcAvg(memRecords),
          avgNetwork: calcAvg(netRecords),
          avgResponseTime: calcAvg(respRecords),
          totalRecords: localStore.history.length,
          recentQueueCount: localStore.recentQueue.length,
          recentQueueCapacity: CONFIG.CIRCULAR_QUEUE_CAPACITY,
          totalAlerts: localStore.alertHistory.length,
          criticalAlerts: localStore.alertHistory.filter(a => a.severity === 2).length,
          warningAlerts: localStore.alertHistory.filter(a => a.severity === 1).length,
        },
        recentRecords: [...localStore.recentQueue],
        recentAlerts: localStore.priorityAlerts.slice(0, 5),
        lastUpdated: localStore.getCurrentTimestamp()
      };
    } else {
      // Future C++ API Endpoint
      const response = await fetch(`${CONFIG.BACKEND_URL}/summary`);
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      return await response.json();
    }
  },

  async getTelemetryHistory(filterOptions = {}) {
    if (CONFIG.USE_MOCK_DATA) {
      let filtered = [...localStore.history];

      if (filterOptions.metric && filterOptions.metric !== 'ALL') {
        filtered = filtered.filter(r => r.metric_Name === filterOptions.metric);
      }
      if (filterOptions.severity && filterOptions.severity !== 'ALL') {
        const targetSev = parseInt(filterOptions.severity, 10);
        filtered = filtered.filter(r => r.severity === targetSev);
      }
      if (filterOptions.searchQuery) {
        const q = filterOptions.searchQuery.toLowerCase().trim();
        filtered = filtered.filter(r => 
          String(r.record_ID).includes(q) ||
          r.metric_Name.toLowerCase().includes(q) ||
          r.time_Stamp.toLowerCase().includes(q) ||
          r.status.toLowerCase().includes(q) ||
          String(r.value).includes(q)
        );
      }

      return {
        total: filtered.length,
        records: filtered
      };
    } else {
      const params = new URLSearchParams(filterOptions);
      const response = await fetch(`${CONFIG.BACKEND_URL}/history?${params.toString()}`);
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      return await response.json();
    }
  },

  async getAlerts() {
    if (CONFIG.USE_MOCK_DATA) {
      return {
        priorityQueue: [...localStore.priorityAlerts],
        alertHistory: [...localStore.alertHistory],
        totalAlerts: localStore.alertHistory.length
      };
    } else {
      const response = await fetch(`${CONFIG.BACKEND_URL}/alerts`);
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      return await response.json();
    }
  },

  async triggerTelemetrySimulation() {
    if (CONFIG.USE_MOCK_DATA) {
      return localStore.generateSingleSimulatedMetric();
    } else {
      const response = await fetch(`${CONFIG.BACKEND_URL}/generate`, { method: 'POST' });
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      return await response.json();
    }
  },

  async submitManualRecord(metricName, value) {
    if (CONFIG.USE_MOCK_DATA) {
      const { status, severity } = localStore.calculateStatusAndSeverity(metricName, value);
      const record = {
        record_ID: localStore.nextRecordId++,
        metric_Name: metricName,
        value: Number(value),
        time_Stamp: localStore.getCurrentTimestamp(),
        status,
        severity
      };
      localStore.addRecord(record);
      return record;
    } else {
      const response = await fetch(`${CONFIG.BACKEND_URL}/record`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ metricName, value })
      });
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      return await response.json();
    }
  },

  async clearAll() {
    if (CONFIG.USE_MOCK_DATA) {
      localStore.history = [];
      localStore.recentQueue = [];
      localStore.priorityAlerts = [];
      localStore.alertHistory = [];
      return { success: true };
    } else {
      const response = await fetch(`${CONFIG.BACKEND_URL}/clear`, { method: 'POST' });
      return await response.json();
    }
  }
};
