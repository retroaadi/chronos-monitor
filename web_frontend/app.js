/**
 * Chronos Monitor - Master Application Controller
 * Coordinates the UI, Chart, Store, and API integration layer.
 */

import { ChronosAPI, CONFIG } from './api.js';
import { TelemetryChart } from './chart.js';

class ChronosApp {
  constructor() {
    this.chart = null;
    this.streamInterval = null;
    this.isStreaming = true;
    this.activeTab = 'overview';

    this.dom = {
      // Topbar & status
      lastUpdatedClock: document.getElementById('lastUpdatedClock'),
      streamToggleBtn: document.getElementById('streamToggleBtn'),
      streamBtnText: document.getElementById('streamBtnText'),
      manualRefreshBtn: document.getElementById('manualRefreshBtn'),
      topExportCsvBtn: document.getElementById('topExportCsvBtn'),
      backendBadge: document.getElementById('backendBadge'),
      navAlertCount: document.getElementById('navAlertCount'),

      // Page Title & Desc
      pageTitle: document.getElementById('pageTitle'),
      pageDesc: document.getElementById('pageDesc'),

      // Circular buffer ratio
      queueRatio: document.getElementById('queueRatio'),
      queueBarFill: document.getElementById('queueBarFill'),

      // Metric Cards
      cards: {
        CPU: {
          val: document.getElementById('val-CPU'),
          pill: document.getElementById('pill-CPU'),
          bar: document.getElementById('bar-CPU')
        },
        Memory: {
          val: document.getElementById('val-Memory'),
          pill: document.getElementById('pill-Memory'),
          bar: document.getElementById('bar-Memory')
        },
        Network: {
          val: document.getElementById('val-Network'),
          pill: document.getElementById('pill-Network'),
          bar: document.getElementById('bar-Network')
        },
        ResponseTime: {
          val: document.getElementById('val-ResponseTime'),
          pill: document.getElementById('pill-ResponseTime'),
          bar: document.getElementById('bar-ResponseTime')
        },
        ErrorCount: {
          val: document.getElementById('val-ErrorCount'),
          pill: document.getElementById('pill-ErrorCount'),
          bar: document.getElementById('bar-ErrorCount')
        }
      },

      // Tables
      overviewAlertsBody: document.getElementById('overviewAlertsBody'),
      overviewRecentBody: document.getElementById('overviewRecentBody'),
      fullHistoryTableBody: document.getElementById('fullHistoryTableBody'),
      heapAlertsBody: document.getElementById('heapAlertsBody'),
      historyAlertsBody: document.getElementById('historyAlertsBody'),

      // Filter controls
      historySearchInput: document.getElementById('historySearchInput'),
      historyMetricFilter: document.getElementById('historyMetricFilter'),
      historySeverityFilter: document.getElementById('historySeverityFilter'),
      historyCountSummary: document.getElementById('historyCountSummary'),
      historyExportBtn: document.getElementById('historyExportBtn'),

      // Stats
      statAvgCpu: document.getElementById('statAvgCpu'),
      statAvgMemory: document.getElementById('statAvgMemory'),
      statAvgNetwork: document.getElementById('statAvgNetwork'),
      statAvgResp: document.getElementById('statAvgResp'),
      totalRecordsStat: document.getElementById('totalRecordsStat'),
      totalAlertsStat: document.getElementById('totalAlertsStat'),
      criticalAlertsStat: document.getElementById('criticalAlertsStat'),
      warningAlertsStat: document.getElementById('warningAlertsStat'),

      // Manual Input Form
      manualInputForm: document.getElementById('manualInputForm'),
      manualMetricSelect: document.getElementById('manualMetricSelect'),
      manualValueInput: document.getElementById('manualValueInput'),
      manualThresholdHint: document.getElementById('manualThresholdHint'),
      manualResultBox: document.getElementById('manualResultBox'),

      // Modal
      settingsModal: document.getElementById('settingsModal'),
      backendSettingsModalBtn: document.getElementById('backendSettingsModalBtn'),
      modalCloseBtn: document.getElementById('modalCloseBtn'),
      modalOkBtn: document.getElementById('modalOkBtn')
    };

    this.init();
  }

  async init() {
    this.chart = new TelemetryChart('telemetryTrendCanvas');
    this.setupEventListeners();
    await this.refreshDashboard();
    await this.refreshHistoryTable();
    this.startStreaming();
  }

  setupEventListeners() {
    // Navigation items
    document.querySelectorAll('.nav-item').forEach((item) => {
      item.addEventListener('click', (e) => {
        e.preventDefault();
        const tab = item.getAttribute('data-tab');
        this.switchTab(tab);
      });
    });

    // In-page switch links (e.g. "View All History ->")
    document.querySelectorAll('.nav-switch-link').forEach((link) => {
      link.addEventListener('click', (e) => {
        e.preventDefault();
        const target = link.getAttribute('data-target');
        this.switchTab(target);
      });
    });

    // Chart legend toggles
    document.querySelectorAll('.legend-btn').forEach((btn) => {
      btn.addEventListener('click', () => {
        const series = btn.getAttribute('data-series');
        const isVisible = this.chart.toggleSeries(series);
        btn.classList.toggle('inactive', !isVisible);
      });
    });

    // Auto-stream toggle
    this.dom.streamToggleBtn.addEventListener('click', () => {
      this.toggleStreaming();
    });

    // Manual refresh / tick button
    this.dom.manualRefreshBtn.addEventListener('click', async () => {
      await this.triggerSimulationTick();
    });

    // CSV Exports
    this.dom.topExportCsvBtn.addEventListener('click', () => this.exportCSV());
    this.dom.historyExportBtn.addEventListener('click', () => this.exportCSV(true));

    // History filtering
    this.dom.historySearchInput.addEventListener('input', () => this.refreshHistoryTable());
    this.dom.historyMetricFilter.addEventListener('change', () => this.refreshHistoryTable());
    this.dom.historySeverityFilter.addEventListener('change', () => this.refreshHistoryTable());

    // Manual Input Form hint updates
    this.dom.manualMetricSelect.addEventListener('change', (e) => {
      const metric = e.target.value;
      let hint = '';
      if (metric === 'CPU') hint = 'For CPU: Normal < 70%, Warning 70-84%, Critical >= 85%';
      else if (metric === 'Memory') hint = 'For Memory: Normal < 75%, Warning 75-89%, Critical >= 90%';
      else if (metric === 'Network') hint = 'For Network: Normal < 75%, Warning 75-89%, Critical >= 90%';
      else if (metric === 'ResponseTime') hint = 'For ResponseTime: Normal < 300ms, Warning 300-499ms, Critical >= 500ms';
      else if (metric === 'ErrorCount') hint = 'For ErrorCount: Normal = 0, Warning 1-9, Critical >= 10';
      this.dom.manualThresholdHint.textContent = hint;
    });

    // Manual Input Form Submit
    this.dom.manualInputForm.addEventListener('submit', async (e) => {
      e.preventDefault();
      const metric = this.dom.manualMetricSelect.value;
      const val = parseFloat(this.dom.manualValueInput.value);

      const record = await ChronosAPI.submitManualRecord(metric, val);

      this.dom.manualResultBox.classList.remove('hidden');
      this.dom.manualResultBox.innerHTML = `
        <h4 style="color:#10b981; margin-bottom: 6px;">&#10003; Record #${record.record_ID} Processed Successfully</h4>
        <div style="font-family: var(--font-mono); font-size: 12px; line-height: 1.6;">
          Metric: <b>${record.metric_Name}</b> &bull; Value: <b>${record.value}</b><br>
          Status: <span class="sev-tag sev-${record.severity}">${record.status} (Severity ${record.severity})</span><br>
          Timestamp: ${record.time_Stamp}
        </div>
      `;

      await this.refreshDashboard();
      await this.refreshHistoryTable();
    });

    // Modal dialog controls
    const toggleModal = (show) => {
      this.dom.settingsModal.classList.toggle('hidden', !show);
    };

    this.dom.backendBadge.addEventListener('click', () => toggleModal(true));
    this.dom.backendSettingsModalBtn.addEventListener('click', () => toggleModal(true));
    this.dom.modalCloseBtn.addEventListener('click', () => toggleModal(false));
    this.dom.modalOkBtn.addEventListener('click', () => toggleModal(false));
  }

  switchTab(tabId) {
    this.activeTab = tabId;

    // Update active nav button
    document.querySelectorAll('.nav-item').forEach((item) => {
      item.classList.toggle('active', item.getAttribute('data-tab') === tabId);
    });

    // Update page title
    const titles = {
      'overview': { title: 'Dashboard Overview', desc: 'Real-time telemetry, max-heap alert queues, and data management.' },
      'telemetry-feed': { title: 'Live Telemetry Feed', desc: 'Real-time streaming telemetry trends and live channel indicators.' },
      'alerts-section': { title: 'Alert Manager', desc: 'Binary Max Heap priority queue and linked list history.' },
      'history-section': { title: 'Telemetry History', desc: 'Complete searchable record log managed by DataManager.' },
      'statistics-section': { title: 'System Analytics & Statistics', desc: 'Performance statistics, averages, and alert breakdowns.' },
      'manual-input': { title: 'Manual Telemetry Generator', desc: 'Interactive console-equivalent simulation for record creation.' }
    };

    const info = titles[tabId] || titles['overview'];
    this.dom.pageTitle.textContent = info.title;
    this.dom.pageDesc.textContent = info.desc;

    // Toggle container views
    const allTabs = ['overview', 'telemetry-feed', 'alerts-section', 'history-section', 'statistics-section', 'manual-input'];
    allTabs.forEach((id) => {
      const el = document.getElementById(`tab-${id}`);
      if (el) {
        if (id === tabId || (tabId === 'telemetry-feed' && id === 'overview')) {
          el.classList.remove('hidden');
        } else {
          el.classList.add('hidden');
        }
      }
    });

    if (tabId === 'history-section') {
      this.refreshHistoryTable();
    } else if (tabId === 'alerts-section') {
      this.refreshAlertsView();
    }
  }

  startStreaming() {
    if (this.streamInterval) clearInterval(this.streamInterval);
    this.streamInterval = setInterval(() => {
      this.triggerSimulationTick();
    }, CONFIG.REFRESH_INTERVAL_MS);
    this.isStreaming = true;
    this.dom.streamBtnText.textContent = 'Auto-Stream: ON';
    this.dom.streamToggleBtn.querySelector('.stream-dot').classList.add('pulsing');
  }

  stopStreaming() {
    if (this.streamInterval) {
      clearInterval(this.streamInterval);
      this.streamInterval = null;
    }
    this.isStreaming = false;
    this.dom.streamBtnText.textContent = 'Auto-Stream: PAUSED';
    this.dom.streamToggleBtn.querySelector('.stream-dot').classList.remove('pulsing');
  }

  toggleStreaming() {
    if (this.isStreaming) this.stopStreaming();
    else this.startStreaming();
  }

  async triggerSimulationTick() {
    const record = await ChronosAPI.triggerTelemetrySimulation();
    if (record) {
      this.chart.addDataPoint(record.metric_Name, record.value, record.time_Stamp);
      await this.refreshDashboard();
      if (this.activeTab === 'history-section') {
        await this.refreshHistoryTable();
      } else if (this.activeTab === 'alerts-section') {
        await this.refreshAlertsView();
      }
    }
  }

  async refreshDashboard() {
    const summary = await ChronosAPI.getDashboardSummary();
    this.dom.lastUpdatedClock.textContent = summary.lastUpdated;

    // 1. Update 5 Metric Cards
    for (const [key, metricData] of Object.entries(summary.activeMetrics)) {
      const card = this.dom.cards[key];
      if (card) {
        card.val.textContent = metricData.value;
        card.pill.textContent = metricData.status;
        card.pill.className = `severity-pill ${metricData.status.toLowerCase()}`;

        // Compute progress bar percentage
        let pct = 0;
        if (key === 'CPU' || key === 'Memory' || key === 'Network') {
          pct = Math.min(100, Math.max(0, metricData.value));
        } else if (key === 'ResponseTime') {
          pct = Math.min(100, Math.max(0, (metricData.value / 1000) * 100));
        } else if (key === 'ErrorCount') {
          pct = Math.min(100, Math.max(0, (metricData.value / 20) * 100));
        }
        card.bar.style.width = `${pct}%`;

        // Color coding progress bar
        if (metricData.status === 'CRITICAL') card.bar.style.backgroundColor = 'var(--accent-red)';
        else if (metricData.status === 'WARNING') card.bar.style.backgroundColor = 'var(--accent-yellow)';
        else card.bar.style.backgroundColor = 'var(--accent-blue)';
      }
    }

    // 2. Update Circular Queue Indicator (Module 2)
    const qCount = summary.stats.recentQueueCount;
    const qCap = summary.stats.recentQueueCapacity;
    this.dom.queueRatio.textContent = `${qCount} / ${qCap}`;
    this.dom.queueBarFill.style.width = `${(qCount / qCap) * 100}%`;

    // 3. Update Quick Stats Box
    this.dom.statAvgCpu.textContent = `${summary.stats.avgCpu}%`;
    this.dom.statAvgMemory.textContent = `${summary.stats.avgMemory}%`;
    this.dom.statAvgNetwork.textContent = `${summary.stats.avgNetwork} Mbps`;
    this.dom.statAvgResp.textContent = `${summary.stats.avgResponseTime} ms`;

    // 4. Update Statistics Tab numbers
    this.dom.totalRecordsStat.textContent = summary.stats.totalRecords;
    this.dom.totalAlertsStat.textContent = summary.stats.totalAlerts;
    this.dom.criticalAlertsStat.textContent = summary.stats.criticalAlerts;
    this.dom.warningAlertsStat.textContent = summary.stats.warningAlerts;
    this.dom.navAlertCount.textContent = summary.stats.criticalAlerts + summary.stats.warningAlerts;

    // 5. Render Overview Recent Queue Table (5 items)
    this.dom.overviewRecentBody.innerHTML = summary.recentRecords.map((r, idx) => `
      <tr>
        <td class="mono-cell">[Slot ${idx}]</td>
        <td class="mono-cell">#${r.record_ID}</td>
        <td><b>${r.metric_Name}</b></td>
        <td class="mono-cell">${r.value}</td>
        <td><span class="sev-tag sev-${r.severity}">${r.status}</span></td>
        <td class="mono-cell">${r.time_Stamp}</td>
      </tr>
    `).join('') || '<tr><td colspan="6" style="text-align:center;">No recent records</td></tr>';

    // 6. Render Overview Priority Alerts
    this.dom.overviewAlertsBody.innerHTML = summary.recentAlerts.map(a => `
      <tr>
        <td class="mono-cell">ALT-${a.alertID}</td>
        <td><b>${a.metricName}</b></td>
        <td class="mono-cell">${a.value}</td>
        <td><span class="sev-tag sev-${a.severity}">SEV ${a.severity} (${a.status})</span></td>
        <td class="mono-cell">${a.timestamp}</td>
      </tr>
    `).join('') || '<tr><td colspan="5" style="text-align:center; color: var(--text-muted);">No active priority alerts</td></tr>';
  }

  async refreshHistoryTable() {
    const filters = {
      metric: this.dom.historyMetricFilter.value,
      severity: this.dom.historySeverityFilter.value,
      searchQuery: this.dom.historySearchInput.value
    };

    const data = await ChronosAPI.getTelemetryHistory(filters);
    this.dom.historyCountSummary.textContent = `Showing ${data.records.length} of ${data.total} filtered records`;

    this.dom.fullHistoryTableBody.innerHTML = data.records.map(r => `
      <tr>
        <td class="mono-cell">#${r.record_ID}</td>
        <td><b>${r.metric_Name}</b></td>
        <td class="mono-cell">${r.value}</td>
        <td class="mono-cell">${r.time_Stamp}</td>
        <td><span class="sev-tag sev-${r.severity}">${r.status}</span></td>
        <td class="mono-cell">Severity ${r.severity}</td>
      </tr>
    `).join('') || '<tr><td colspan="6" style="text-align:center; padding: 24px; color: var(--text-muted);">No matching telemetry records found</td></tr>';
  }

  async refreshAlertsView() {
    const alertsData = await ChronosAPI.getAlerts();

    // Binary Max Heap Table
    this.dom.heapAlertsBody.innerHTML = alertsData.priorityQueue.map(a => `
      <tr>
        <td class="mono-cell">ALT-${a.alertID}</td>
        <td class="mono-cell">#${a.recordID}</td>
        <td><b>${a.metricName}</b></td>
        <td class="mono-cell">${a.value}</td>
        <td><span class="sev-tag sev-${a.severity}">Severity ${a.severity} (${a.status})</span></td>
        <td class="mono-cell">${a.timestamp}</td>
      </tr>
    `).join('') || '<tr><td colspan="6" style="text-align:center; padding: 20px; color: var(--text-muted);">Priority queue is empty</td></tr>';

    // Linked List History Table
    this.dom.historyAlertsBody.innerHTML = alertsData.alertHistory.map(a => `
      <tr>
        <td class="mono-cell">ALT-${a.alertID}</td>
        <td><b>${a.metricName}</b></td>
        <td><span class="sev-tag sev-${a.severity}">${a.status}</span></td>
        <td class="mono-cell">Severity ${a.severity}</td>
        <td class="mono-cell">${a.timestamp}</td>
      </tr>
    `).join('') || '<tr><td colspan="5" style="text-align:center; padding: 20px; color: var(--text-muted);">No alerts logged in history</td></tr>';
  }

  async exportCSV(filteredOnly = false) {
    let records = [];
    if (filteredOnly) {
      const filters = {
        metric: this.dom.historyMetricFilter.value,
        severity: this.dom.historySeverityFilter.value,
        searchQuery: this.dom.historySearchInput.value
      };
      const res = await ChronosAPI.getTelemetryHistory(filters);
      records = res.records;
    } else {
      const res = await ChronosAPI.getTelemetryHistory({});
      records = res.records;
    }

    if (!records.length) {
      alert('No records available to export.');
      return;
    }

    const headers = ['RecordID', 'Timestamp', 'MetricName', 'Value', 'Status', 'Severity'];
    const rows = records.map(r => [
      r.record_ID,
      r.time_Stamp,
      r.metric_Name,
      r.value,
      r.status,
      r.severity
    ]);

    const csvContent = [
      headers.join(','),
      ...rows.map(row => row.map(val => `"${String(val).replace(/"/g, '""')}"`).join(','))
    ].join('\r\n');

    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
    const url = URL.createObjectURL(blob);
    const link = document.createElement('a');
    link.href = url;
    link.setAttribute('download', `chronos_telemetry_${Date.now()}.csv`);
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    URL.revokeObjectURL(url);
  }
}

// Bootstrap on DOM ready
document.addEventListener('DOMContentLoaded', () => {
  window.chronosApp = new ChronosApp();
});
