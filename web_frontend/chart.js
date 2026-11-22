/**
 * Chronos Monitor - Interactive High-Performance Telemetry Canvas Chart
 * Zero dependencies, hardware accelerated canvas rendering with hover tooltips and metric toggles.
 */

export class TelemetryChart {
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

    // Store up to 20 recent time points
    this.dataPoints = [];
    this.initSampleData();

    this.setupListeners();
    this.render();
  }

  initSampleData() {
    const now = Date.now();
    for (let i = 15; i >= 0; i--) {
      const d = new Date(now - i * 12000);
      const pad = (n) => String(n).padStart(2, '0');
      const timeStr = `${pad(d.getHours())}:${pad(d.getMinutes())}:${pad(d.getSeconds())}`;

      this.dataPoints.push({
        time: timeStr,
        CPU: +(35 + Math.random() * 50).toFixed(1),
        Memory: +(55 + Math.random() * 40).toFixed(1),
        Network: +(15 + Math.random() * 75).toFixed(1)
      });
    }
  }

  addDataPoint(metricName, value, timeStr) {
    let last = this.dataPoints[this.dataPoints.length - 1];
    
    // If last point has the same time or needs new slot
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

    // Clear background
    this.ctx.clearRect(0, 0, width, height);

    // Draw horizontal grid lines & Y labels (0 to 100%)
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

    // Draw X-axis timestamps
    this.ctx.textAlign = 'center';
    this.ctx.textBaseline = 'top';
    const xCount = this.dataPoints.length;
    const stepX = chartW / Math.max(1, xCount - 1);

    // Pick labels to show (every ~3 items)
    const labelInterval = Math.max(1, Math.floor(xCount / 6));
    this.dataPoints.forEach((pt, idx) => {
      if (idx % labelInterval === 0 || idx === xCount - 1) {
        const xPos = left + idx * stepX;
        this.ctx.fillText(pt.time, xPos, height - bottom + 8);
      }
    });

    // Draw lines for each series
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

      // Draw subtle point dots
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

    // Hover tooltip
    if (this.hoverPos && this.hoverPos.x >= left && this.hoverPos.x <= width - right) {
      const relX = this.hoverPos.x - left;
      const index = Math.min(xCount - 1, Math.max(0, Math.round(relX / stepX)));
      const activePt = this.dataPoints[index];

      if (activePt) {
        const snapX = left + index * stepX;

        // Vertical cursor bar
        this.ctx.strokeStyle = '#475569';
        this.ctx.setLineDash([4, 4]);
        this.ctx.lineWidth = 1;
        this.ctx.beginPath();
        this.ctx.moveTo(snapX, top);
        this.ctx.lineTo(snapX, height - bottom);
        this.ctx.stroke();
        this.ctx.setLineDash([]);

        // Render Tooltip box
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

        this.ctx.fillStyle = 'rgba(15, 23, 42, 0.92)';
        this.ctx.strokeStyle = '#334155';
        this.ctx.lineWidth = 1;
        this.ctx.beginPath();
        this.ctx.roundRect ? this.ctx.roundRect(boxX, boxY, boxW, boxH, 6) : this.ctx.rect(boxX, boxY, boxW, boxH);
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
