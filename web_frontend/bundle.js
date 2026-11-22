/**
 * Chronos Monitor - Self-Contained Frontend Application Bundle
 * CONNECTED TO: C++ Native HTTP Bridge (Modules 1 - 6)
 */

(function () {
  'use strict';

  // 1. CONFIGURATION
  const CONFIG = {
    USE_MOCK_DATA: false, // Connected to actual C++ backend!
    BACKEND_URL: '/api',
    REFRESH_INTERVAL_MS: 3000,
    CIRCULAR_QUEUE_CAPACITY: 5,
    MAX_HISTORY_ITEMS: 100,
  };

  // 2. CHRONOS API CLIENT (Talking to C++ chronos_server.exe)
  const ChronosAPI = {
    isMockMode() {
      return CONFIG.USE_MOCK_DATA;
    },

    async getDashboardSummary() {
      try {
        const response = await fetch(`${CONFIG.BACKEND_URL}/summary`);
        if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
        const data = await response.json();

        return {
          mode: data.mode || 'CPP_NATIVE_BACKEND',
          activeMetrics: data.activeMetrics,
          stats: {
            avgCpu: data.activeMetrics.CPU ? data.activeMetrics.CPU.average : 0,
            avgMemory: data.activeMetrics.Memory ? data.activeMetrics.Memory.average : 0,
            avgNetwork: data.activeMetrics.Network ? data.activeMetrics.Network.average : 0,
            avgResponseTime: data.activeMetrics.ResponseTime ? data.activeMetrics.ResponseTime.average : 0,
            totalRecords: data.stats.totalRecords,
            normalCount: data.stats.normalCount,
            warningCount: data.stats.warningCount,
            criticalCount: data.stats.criticalCount,
            recentQueueCount: data.stats.recentQueueCount,
            recentQueueCapacity: data.stats.recentQueueCapacity,
            totalAlerts: data.stats.totalAlerts,
            criticalAlerts: data.stats.criticalCount,
            warningAlerts: data.stats.warningCount,
          },
          recentRecords: data.recentRecords || [],
          recentAlerts: data.recentAlerts || [],
          lastUpdated: data.lastUpdated || new Date().toLocaleTimeString()
        };
      } catch (err) {
        console.warn('C++ Backend unreachable, fallback notification:', err);
        throw err;
      }
    },

    async getTelemetryHistory(filterOptions = {}) {
      const params = new URLSearchParams();
      if (filterOptions.metric && filterOptions.metric !== 'ALL') params.append('metric', filterOptions.metric);
      if (filterOptions.severity && filterOptions.severity !== 'ALL') params.append('severity', filterOptions.severity);
      if (filterOptions.searchQuery) params.append('q', filterOptions.searchQuery);

      const response = await fetch(`${CONFIG.BACKEND_URL}/history?${params.toString()}`);
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      return await response.json();
    },

    async getAlerts() {
      const response = await fetch(`${CONFIG.BACKEND_URL}/alerts`);
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      return await response.json();
    },

    async triggerTelemetrySimulation() {
      const response = await fetch(`${CONFIG.BACKEND_URL}/record`, { method: 'POST' });
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      const data = await response.json();
      return data.record;
    },

    async submitManualRecord(metricName, value) {
      const url = `${CONFIG.BACKEND_URL}/record?metric=${encodeURIComponent(metricName)}&val=${encodeURIComponent(value)}`;
      const response = await fetch(url, { method: 'POST' });
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      const data = await response.json();
      return data.record;
    },

    async searchRecord(recordID, mode = 'linear') {
      const response = await fetch(`${CONFIG.BACKEND_URL}/search?id=${encodeURIComponent(recordID)}&mode=${encodeURIComponent(mode)}`);
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      return await response.json();
    },

    async popHighestPriorityAlert() {
      const response = await fetch(`${CONFIG.BACKEND_URL}/alerts/pop`, { method: 'POST' });
      if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
      return await response.json();
    }
  };

  // 3. CANVAS CHART
  class TelemetryChart {
    constructor(canvasId) {
      this.canvas = document.getElementById(canvasId);
      if (!this.canvas) return;
      this.ctx = this.canvas.getContext('2d');

      this.padding = { top: 20, right: 25, bottom: 35, left: 45 };
      this.hoverPos = null;
      this.visibleSeries = {
        CPU: true,
        Memory: true,
        Network: true
      };

      this.seriesConfig = {
        CPU: { label: 'CPU Usage (%)', color: '#ffc52f', unit: '%' },
        Memory: { label: 'Memory Usage (%)', color: '#ff5967', unit: '%' },
        Network: { label: 'Network (Mbps)', color: '#3de49b', unit: 'Mbps' }
      };

      this.dataPoints = [];
      this.setupListeners();
    }

    populateInitial(records) {
      if (!records || !records.length) return;
      this.dataPoints = [];
      const slice = records.slice(0, 15).reverse();
      slice.forEach(r => {
        this.addDataPoint(r.metric_Name, r.value, r.time_Stamp);
      });
      this.render();
    }

    addDataPoint(metricName, value, timeStr) {
      let last = this.dataPoints[this.dataPoints.length - 1];
      if (!last || last.time !== timeStr) {
        const newPoint = {
          time: timeStr,
          CPU: last ? last.CPU : 40,
          Memory: last ? last.Memory : 60,
          Network: last ? last.Network : 30
        };
        if (['CPU', 'Memory', 'Network'].includes(metricName)) {
          newPoint[metricName] = Number(value);
        }
        this.dataPoints.push(newPoint);
        if (this.dataPoints.length > 20) {
          this.dataPoints.shift();
        }
      } else {
        if (['CPU', 'Memory', 'Network'].includes(metricName)) {
          last[metricName] = Number(value);
        }
      }
      this.render();
    }

    toggleSeries(seriesName) {
      if (this.visibleSeries[seriesName] !== undefined) {
        this.visibleSeries[seriesName] = !this.visibleSeries[seriesName];
        this.render();
        return this.visibleSeries[seriesName];
      }
      return true;
    }

    setupListeners() {
      window.addEventListener('resize', () => this.render());

      this.canvas.addEventListener('mousemove', (e) => {
        const rect = this.canvas.getBoundingClientRect();
        this.hoverPos = {
          x: e.clientX - rect.left,
          y: e.clientY - rect.top
        };
        this.render();
      });

      this.canvas.addEventListener('mouseleave', () => {
        this.hoverPos = null;
        this.render();
      });
    }

    render() {
      if (!this.canvas) return;
      const rect = this.canvas.getBoundingClientRect();
      const dpr = window.devicePixelRatio || 1;

      const width = rect.width;
      const height = rect.height;

      this.canvas.width = width * dpr;
      this.canvas.height = height * dpr;
      this.ctx.scale(dpr, dpr);

      const { top, right, bottom, left } = this.padding;
      const chartW = width - left - right;
      const chartH = height - top - bottom;

      if (chartW <= 0 || chartH <= 0) return;

      this.ctx.clearRect(0, 0, width, height);

      this.ctx.font = '11px -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif';
      this.ctx.fillStyle = '#64748b';
      this.ctx.textAlign = 'right';
      this.ctx.textBaseline = 'middle';
      this.ctx.lineWidth = 1;

      const ySteps = 4;
      for (let i = 0; i <= ySteps; i++) {
        const yVal = Math.round(100 - (i * (100 / ySteps)));
        const yPos = top + (i * (chartH / ySteps));

        this.ctx.strokeStyle = '#1e293b';
        this.ctx.beginPath();
        this.ctx.moveTo(left, yPos);
        this.ctx.lineTo(width - right, yPos);
        this.ctx.stroke();

        this.ctx.fillText(`${yVal}`, left - 8, yPos);
      }

      if (this.dataPoints.length === 0) return;

      this.ctx.textAlign = 'center';
      this.ctx.textBaseline = 'top';
      const xCount = this.dataPoints.length;
      const stepX = chartW / Math.max(1, xCount - 1);

      const labelInterval = Math.max(1, Math.floor(xCount / 6));
      this.dataPoints.forEach((pt, idx) => {
        if (idx % labelInterval === 0 || idx === xCount - 1) {
          const xPos = left + idx * stepX;
          this.ctx.fillText(pt.time, xPos, height - bottom + 8);
        }
      });

      const seriesKeys = ['CPU', 'Memory', 'Network'];
      seriesKeys.forEach((key) => {
        if (!this.visibleSeries[key]) return;
        const conf = this.seriesConfig[key];

        this.ctx.strokeStyle = conf.color;
        this.ctx.lineWidth = 2.2;
        this.ctx.beginPath();

        this.dataPoints.forEach((pt, idx) => {
          const val = Math.max(0, Math.min(100, pt[key] || 0));
          const x = left + idx * stepX;
          const y = top + chartH * (1 - val / 100);

          if (idx === 0) this.ctx.moveTo(x, y);
          else this.ctx.lineTo(x, y);
        });
        this.ctx.stroke();

        this.ctx.fillStyle = conf.color;
        this.dataPoints.forEach((pt, idx) => {
          const val = Math.max(0, Math.min(100, pt[key] || 0));
          const x = left + idx * stepX;
          const y = top + chartH * (1 - val / 100);

          this.ctx.beginPath();
          this.ctx.arc(x, y, 2.5, 0, Math.PI * 2);
          this.ctx.fill();
        });
      });

      if (this.hoverPos && this.hoverPos.x >= left && this.hoverPos.x <= width - right) {
        const relX = this.hoverPos.x - left;
        const index = Math.min(xCount - 1, Math.max(0, Math.round(relX / stepX)));
        const activePt = this.dataPoints[index];

        if (activePt) {
          const snapX = left + index * stepX;

          this.ctx.strokeStyle = '#475569';
          this.ctx.setLineDash([4, 4]);
          this.ctx.lineWidth = 1;
          this.ctx.beginPath();
          this.ctx.moveTo(snapX, top);
          this.ctx.lineTo(snapX, height - bottom);
          this.ctx.stroke();
          this.ctx.setLineDash([]);

          const tooltipLines = [
            `Time: ${activePt.time}`,
            this.visibleSeries.CPU ? `CPU: ${activePt.CPU}%` : null,
            this.visibleSeries.Memory ? `Memory: ${activePt.Memory}%` : null,
            this.visibleSeries.Network ? `Network: ${activePt.Network} Mbps` : null,
          ].filter(Boolean);

          const boxW = 140;
          const boxH = tooltipLines.length * 18 + 12;
          let boxX = snapX + 10;
          if (boxX + boxW > width - right) boxX = snapX - boxW - 10;
          let boxY = top + 10;

          this.ctx.fillStyle = 'rgba(15, 23, 42, 0.94)';
          this.ctx.strokeStyle = '#334155';
          this.ctx.lineWidth = 1;
          this.ctx.beginPath();
          if (this.ctx.roundRect) {
            this.ctx.roundRect(boxX, boxY, boxW, boxH, 6);
          } else {
            this.ctx.rect(boxX, boxY, boxW, boxH);
          }
          this.ctx.fill();
          this.ctx.stroke();

          this.ctx.textAlign = 'left';
          this.ctx.textBaseline = 'top';
          tooltipLines.forEach((line, i) => {
            if (line.startsWith('Time')) this.ctx.fillStyle = '#cbd5e1';
            else if (line.startsWith('CPU')) this.ctx.fillStyle = this.seriesConfig.CPU.color;
            else if (line.startsWith('Memory')) this.ctx.fillStyle = this.seriesConfig.Memory.color;
            else if (line.startsWith('Network')) this.ctx.fillStyle = this.seriesConfig.Network.color;
            this.ctx.fillText(line, boxX + 10, boxY + 8 + i * 18);
          });
        }
      }
    }
  }

  // 4. MASTER CONTROLLER
  class ChronosApp {
    constructor() {
      this.chart = null;
      this.streamInterval = null;
      this.isStreaming = true;
      this.activeTab = 'overview';

      this.dom = {
        lastUpdatedClock: document.getElementById('lastUpdatedClock'),
        streamToggleBtn: document.getElementById('streamToggleBtn'),
        streamBtnText: document.getElementById('streamBtnText'),
        manualRefreshBtn: document.getElementById('manualRefreshBtn'),
        topExportCsvBtn: document.getElementById('topExportCsvBtn'),
        backendBadge: document.getElementById('backendBadge'),
        backendStatusLabel: document.getElementById('backendStatusLabel'),
        navAlertCount: document.getElementById('navAlertCount'),

        pageTitle: document.getElementById('pageTitle'),
        pageDesc: document.getElementById('pageDesc'),

        queueRatio: document.getElementById('queueRatio'),
        queueBarFill: document.getElementById('queueBarFill'),

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

        overviewAlertsBody: document.getElementById('overviewAlertsBody'),
        overviewRecentBody: document.getElementById('overviewRecentBody'),
        fullHistoryTableBody: document.getElementById('fullHistoryTableBody'),
        heapAlertsBody: document.getElementById('heapAlertsBody'),
        historyAlertsBody: document.getElementById('historyAlertsBody'),

        historySearchInput: document.getElementById('historySearchInput'),
        historyMetricFilter: document.getElementById('historyMetricFilter'),
        historySeverityFilter: document.getElementById('historySeverityFilter'),
        historyCountSummary: document.getElementById('historyCountSummary'),
        historyExportBtn: document.getElementById('historyExportBtn'),

        statAvgCpu: document.getElementById('statAvgCpu'),
        statAvgMemory: document.getElementById('statAvgMemory'),
        statAvgNetwork: document.getElementById('statAvgNetwork'),
        statAvgResp: document.getElementById('statAvgResp'),
        totalRecordsStat: document.getElementById('totalRecordsStat'),
        totalAlertsStat: document.getElementById('totalAlertsStat'),
        criticalAlertsStat: document.getElementById('criticalAlertsStat'),
        warningAlertsStat: document.getElementById('warningAlertsStat'),

        manualInputForm: document.getElementById('manualInputForm'),
        manualMetricSelect: document.getElementById('manualMetricSelect'),
        manualValueInput: document.getElementById('manualValueInput'),
        manualThresholdHint: document.getElementById('manualThresholdHint'),
        manualResultBox: document.getElementById('manualResultBox'),

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
      
      // Update badge to reflect C++ Native status
      if (this.dom.backendBadge) {
        this.dom.backendBadge.className = 'integration-badge';
        this.dom.backendBadge.style.background = 'rgba(16, 185, 129, 0.15)';
        this.dom.backendBadge.style.borderColor = 'rgba(16, 185, 129, 0.4)';
        const pulse = this.dom.backendBadge.querySelector('.pulse-indicator');
        if (pulse) {
          pulse.style.backgroundColor = '#10b981';
          pulse.style.boxShadow = '0 0 8px #10b981';
        }
      }
      if (this.dom.backendStatusLabel) {
        this.dom.backendStatusLabel.textContent = 'C++ Native Server (Active)';
        this.dom.backendStatusLabel.style.color = '#34d399';
      }

      const notice = document.querySelector('.source-notice-banner .notice-body');
      if (notice) {
        notice.innerHTML = '<strong>C++ Native Backend Active:</strong> All telemetry generation, storage (CircularQueue), alerts (Max Heap), searches (Module 4), statistics (Module 5), and CSV exports (Module 6) are powered by the compiled C++ binary.';
      }

      await this.refreshDashboard();
      await this.refreshHistoryTable();
      this.startStreaming();
    }

    setupEventListeners() {
      document.querySelectorAll('.nav-item').forEach((item) => {
        item.addEventListener('click', (e) => {
          e.preventDefault();
          const tab = item.getAttribute('data-tab');
          this.switchTab(tab);
        });
      });

      document.querySelectorAll('.nav-switch-link').forEach((link) => {
        link.addEventListener('click', (e) => {
          e.preventDefault();
          const target = link.getAttribute('data-target');
          this.switchTab(target);
        });
      });

      document.querySelectorAll('.legend-btn').forEach((btn) => {
        btn.addEventListener('click', () => {
          const series = btn.getAttribute('data-series');
          const isVisible = this.chart.toggleSeries(series);
          btn.classList.toggle('inactive', !isVisible);
        });
      });

      this.dom.streamToggleBtn.addEventListener('click', () => {
        this.toggleStreaming();
      });

      this.dom.manualRefreshBtn.addEventListener('click', async () => {
        await this.triggerSimulationTick();
      });

      this.dom.topExportCsvBtn.addEventListener('click', () => this.exportCSV());
      this.dom.historyExportBtn.addEventListener('click', () => this.exportCSV());

      this.dom.historySearchInput.addEventListener('input', () => this.refreshHistoryTable());
      this.dom.historyMetricFilter.addEventListener('change', () => this.refreshHistoryTable());
      this.dom.historySeverityFilter.addEventListener('change', () => this.refreshHistoryTable());

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

      this.dom.manualInputForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        const metric = this.dom.manualMetricSelect.value;
        const val = parseFloat(this.dom.manualValueInput.value);

        try {
          const record = await ChronosAPI.submitManualRecord(metric, val);

          this.dom.manualResultBox.classList.remove('hidden');
          this.dom.manualResultBox.innerHTML = `
            <h4 style="color:#10b981; margin-bottom: 6px;">&#10003; C++ Record #${record.record_ID} Processed Successfully</h4>
            <div style="font-family: var(--font-mono); font-size: 12px; line-height: 1.6;">
              Metric: <b>${record.metric_Name}</b> &bull; Value: <b>${record.value}</b><br>
              Status: <span class="sev-tag sev-${record.severity}">${record.status} (Severity ${record.severity})</span><br>
              Timestamp: ${record.time_Stamp} (Stored in C++ DataManager & AlertManager)
            </div>
          `;

          await this.refreshDashboard();
          await this.refreshHistoryTable();
        } catch (err) {
          alert('Failed to submit record to C++ backend.');
        }
      });

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

      document.querySelectorAll('.nav-item').forEach((item) => {
        item.classList.toggle('active', item.getAttribute('data-tab') === tabId);
      });

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
      try {
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
      } catch (err) {
        console.error('Tick error:', err);
      }
    }

    async refreshDashboard() {
      try {
        const summary = await ChronosAPI.getDashboardSummary();
        this.dom.lastUpdatedClock.textContent = summary.lastUpdated;

        for (const [key, metricData] of Object.entries(summary.activeMetrics)) {
          const card = this.dom.cards[key];
          if (card) {
            card.val.textContent = metricData.value;
            card.pill.textContent = metricData.status;
            card.pill.className = `severity-pill ${metricData.status.toLowerCase()}`;

            let pct = 0;
            if (key === 'CPU' || key === 'Memory' || key === 'Network') {
              pct = Math.min(100, Math.max(0, metricData.value));
            } else if (key === 'ResponseTime') {
              pct = Math.min(100, Math.max(0, (metricData.value / 1000) * 100));
            } else if (key === 'ErrorCount') {
              pct = Math.min(100, Math.max(0, (metricData.value / 20) * 100));
            }
            card.bar.style.width = `${pct}%`;

            if (metricData.status === 'CRITICAL') card.bar.style.backgroundColor = 'var(--accent-red)';
            else if (metricData.status === 'WARNING') card.bar.style.backgroundColor = 'var(--accent-yellow)';
            else card.bar.style.backgroundColor = 'var(--accent-blue)';
          }
        }

        const qCount = summary.stats.recentQueueCount;
        const qCap = summary.stats.recentQueueCapacity;
        this.dom.queueRatio.textContent = `${qCount} / ${qCap}`;
        this.dom.queueBarFill.style.width = `${(qCount / qCap) * 100}%`;

        this.dom.statAvgCpu.textContent = `${summary.stats.avgCpu}%`;
        this.dom.statAvgMemory.textContent = `${summary.stats.avgMemory}%`;
        this.dom.statAvgNetwork.textContent = `${summary.stats.avgNetwork} Mbps`;
        this.dom.statAvgResp.textContent = `${summary.stats.avgResponseTime} ms`;

        this.dom.totalRecordsStat.textContent = summary.stats.totalRecords;
        this.dom.totalAlertsStat.textContent = summary.stats.totalAlerts;
        this.dom.criticalAlertsStat.textContent = summary.stats.criticalAlerts;
        this.dom.warningAlertsStat.textContent = summary.stats.warningAlerts;
        this.dom.navAlertCount.textContent = summary.stats.criticalAlerts + summary.stats.warningAlerts;

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

        this.dom.overviewAlertsBody.innerHTML = summary.recentAlerts.map(a => `
          <tr>
            <td class="mono-cell">ALT-${a.alertID}</td>
            <td><b>${a.metricName}</b></td>
            <td class="mono-cell">${a.value}</td>
            <td><span class="sev-tag sev-${a.severity}">SEV ${a.severity} (${a.status})</span></td>
            <td class="mono-cell">${a.timestamp}</td>
          </tr>
        `).join('') || '<tr><td colspan="5" style="text-align:center; color: var(--text-muted);">No active priority alerts</td></tr>';
      } catch (err) {
        console.error('Failed to refresh dashboard:', err);
      }
    }

    async refreshHistoryTable() {
      try {
        const filters = {
          metric: this.dom.historyMetricFilter.value,
          severity: this.dom.historySeverityFilter.value,
          searchQuery: this.dom.historySearchInput.value
        };

        const data = await ChronosAPI.getTelemetryHistory(filters);
        this.dom.historyCountSummary.textContent = `Showing ${data.records.length} of ${data.total} records in DataManager`;

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
      } catch (err) {
        console.error('Failed to load history:', err);
      }
    }

    async refreshAlertsView() {
      try {
        const alertsData = await ChronosAPI.getAlerts();

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

        this.dom.historyAlertsBody.innerHTML = alertsData.alertHistory.map(a => `
          <tr>
            <td class="mono-cell">ALT-${a.alertID}</td>
            <td><b>${a.metricName}</b></td>
            <td><span class="sev-tag sev-${a.severity}">${a.status}</span></td>
            <td class="mono-cell">Severity ${a.severity}</td>
            <td class="mono-cell">${a.timestamp}</td>
          </tr>
        `).join('') || '<tr><td colspan="5" style="text-align:center; padding: 20px; color: var(--text-muted);">No alerts logged in history</td></tr>';
      } catch (err) {
        console.error('Failed to load alerts:', err);
      }
    }

    exportCSV() {
      // Direct stream from C++ Module 6 ExportManager endpoint
      window.location.href = '/api/export';
    }
  }

  // Initialize on DOM load
  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', () => {
      window.chronosApp = new ChronosApp();
    });
  } else {
    window.chronosApp = new ChronosApp();
  }
})();
