#pragma once

// ============================================================
// 28. HTML DASHBOARD
// ============================================================

const char INDEX_HTML[] PROGMEM = R"NOVIAHTML(

<!DOCTYPE html>
<html lang="id">

<head>

<meta charset="UTF-8">

<meta name="viewport"
content="width=device-width,initial-scale=1,maximum-scale=1">

<meta name="theme-color" content="#151a1d">

<title>NOVIA HOME V4.4</title>

<style>
:root {
  color-scheme: dark;

  --bg: #151a1d;
  --surface: #20282b;
  --surface2: #263034;

  --text: #f1f5f2;
  --muted: #91a39a;

  --accent: #83c99f;
  --accent-bg: #315b46;

  --border: rgba(255,255,255,.07);
  --danger: #df7777;

  --shadow: 0 8px 25px rgba(0,0,0,.12);
}

body.light {
  color-scheme: light;

  --bg: #f1f5f2;
  --surface: #ffffff;
  --surface2: #e9efeb;

  --text: #24332b;
  --muted: #74847a;

  --accent: #438e65;
  --accent-bg: #dcefe3;

  --border: rgba(20,60,35,.09);
  --danger: #c65c5c;

  --shadow: 0 8px 25px rgba(30,60,40,.06);
}


/* =====================================================
   02. GLOBAL
===================================================== */

*,
*::before,
*::after {
  box-sizing: border-box;
}

html {
  scroll-behavior: smooth;
}

body {
  margin: 0;
  padding: 18px 18px 35px;

  background: var(--bg);
  color: var(--text);

  font-family: Arial, sans-serif;

  transition:
    background .25s ease,
    color .25s ease;
}

button,
input,
select {
  font: inherit;
}

button {
  cursor: pointer;
}

.container {
  width: 100%;
  max-width: 850px;
  margin: 0 auto;
}


/* =====================================================
   03. HEADER
===================================================== */

.header {
  display: flex;
  align-items: center;
  justify-content: space-between;

  margin-bottom: 23px;
}

.brand {
  display: flex;
  align-items: center;
  gap: 14px;
}

.brand-icon {
  width: 46px;
  height: 46px;

  flex-shrink: 0;

  display: grid;
  place-items: center;

  border-radius: 12px;

  background: var(--accent-bg);
  color: var(--accent);

  font-size: 23px;
}

.brand strong {
  font-size: 19px;
  letter-spacing: 1px;
}

.brand small {
  display: block;
  margin-top: 5px;

  color: var(--muted);

  font-size: 10px;
  letter-spacing: 1.5px;
}

.icon-button {
  width: 43px;
  height: 43px;

  border: 1px solid var(--border);
  border-radius: 14px;

  background: var(--surface);
  color: var(--text);

  font-size: 20px;

  transition: .2s;
}

.icon-button:hover {
  border-color: var(--accent);
}


/* =====================================================
   04. HERO / CLOCK
===================================================== */

.hero {
  padding: 20px;
  margin-bottom: 15px;

  background: var(--surface);

  border: 1px solid var(--border);
  border-radius: 20px;

  box-shadow: var(--shadow);
}

.eyebrow {
  color: var(--muted);

  font-size: 11px;
  letter-spacing: 1.5px;
}

.clock {
  margin: 8px 0;

  font-size: 35px;
  font-weight: bold;
  letter-spacing: 1px;
}

.date {
  color: var(--muted);
  font-size: 13px;
}


/* =====================================================
   05. STATUS CARDS
===================================================== */

.status-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));

  gap: 12px;
  margin-bottom: 15px;
}

.status-card {
  padding: 15px;

  background: var(--surface);

  border: 1px solid var(--border);
  border-radius: 17px;
}

.status-top {
  display: flex;
  align-items: center;
  gap: 8px;

  color: var(--muted);
  font-size: 12px;
}

.dot {
  width: 8px;
  height: 8px;

  flex-shrink: 0;

  border-radius: 50%;
  background: var(--danger);
}

.dot.online {
  background: var(--accent);

  box-shadow: 0 0 9px var(--accent);
}

.status-value {
  display: block;
  margin-top: 9px;

  font-size: 14px;
  font-weight: bold;
}


/* =====================================================
   06. SECTION
===================================================== */

.section {
  margin-top: 24px;
}

.section-heading {
  display: flex;
  align-items: center;
  justify-content: space-between;

  margin-bottom: 13px;
}

.section-heading h3 {
  margin: 0;

  font-size: 15px;
  font-weight: 700;
}

.section-heading small {
  color: var(--muted);
  font-size: 11px;
}


/* =====================================================
   07. STATISTICS
===================================================== */

.stat-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));

  gap: 10px;
}

.stat {
  padding: 15px 10px;

  background: var(--surface);

  border: 1px solid var(--border);
  border-radius: 15px;
}

.stat strong {
  display: block;

  font-size: 24px;
}

.stat span {
  display: block;
  margin-top: 6px;

  color: var(--muted);
  font-size: 10px;
}


/* =====================================================
   08. ROOM CONTROL
   MINIMALIST SMART HOME DESIGN
===================================================== */

.relay-grid {
  display: grid;

  grid-template-columns: repeat(2, minmax(0, 1fr));

  gap: 11px;
}

.relay-card {
  position: relative;

  min-width: 0;

  padding: 14px;

  background: var(--surface);

  border: 1px solid var(--border);
  border-radius: 15px;

  box-shadow: none;

  transition:
    background .2s ease,
    border-color .2s ease,
    transform .2s ease;
}

.relay-card.active {
  background: var(--surface);
  border-color: var(--accent);
}

/* DEVICE ICON */

.relay-icon {
  width: 34px;
  height: 34px;

  display: grid;
  place-items: center;

  margin-bottom: 12px;

  border-radius: 10px;

  background: var(--surface2);
  color: var(--muted);

  font-size: 18px;

  transition:
    background .2s ease,
    color .2s ease;
}

.relay-card.active .relay-icon {
  background: var(--accent-bg);
  color: var(--accent);
}

/* DEVICE NAME */

.relay-name {
  color: var(--text);

  font-size: 13px;
  font-weight: 600;
  letter-spacing: .1px;

  overflow-wrap: break-word;
}

.relay-description {
  margin-top: 5px;

  color: var(--muted);

  font-size: 9px;
  letter-spacing: 1px;
}

/* CONTROL AREA */

.relay-bottom {
  display: flex;
  align-items: center;
  justify-content: space-between;

  gap: 8px;

  margin-top: 15px;
  padding-top: 11px;

  border-top: 1px solid var(--border);
}

/* ON / OFF STATUS */

.state-label {
  color: var(--muted);

  font-size: 9px;
  font-weight: 700;
  letter-spacing: 1px;

  transition: color .2s ease;
}

.relay-card.active .state-label {
  color: var(--accent);
}

/* MINIMAL TOGGLE SWITCH */

.switch {
  position: relative;

  display: block;

  width: 38px;
  height: 22px;

  flex-shrink: 0;

  padding: 0;

  border: 0;
  border-radius: 30px;

  background: var(--surface2);

  transition: background .2s ease;
}

.switch::after {
  content: "";

  position: absolute;

  top: 2px;
  left: 2px;

  width: 18px;
  height: 18px;

  border-radius: 50%;

  background: var(--muted);

  transition:
    transform .2s ease,
    background .2s ease;
}

.switch.active {
  background: var(--accent-bg);
}

.switch.active::after {
  transform: translateX(16px);
  background: var(--accent);
}

.switch:focus-visible {
  outline: 2px solid var(--accent);
  outline-offset: 3px;
}


/* =====================================================
   09. GENERAL PANEL
===================================================== */

.panel {
  padding: 18px;

  background: var(--surface);

  border: 1px solid var(--border);
  border-radius: 18px;
}

.panel h3 {
  margin: 0 0 15px;

  font-size: 15px;
}


/* =====================================================
   10. FORM CONTROLS
===================================================== */

.form-control {
  width: 100%;

  padding: 13px;
  margin-bottom: 10px;

  border: 1px solid var(--border);
  border-radius: 11px;

  outline: none;

  background: var(--bg);
  color: var(--text);

  transition: border-color .2s;
}

.form-control:focus {
  border-color: var(--accent);
}

.primary-button {
  width: 100%;

  padding: 13px;

  border: 0;
  border-radius: 11px;

  background: var(--accent-bg);
  color: var(--accent);

  font-weight: bold;

  transition: opacity .2s;
}

.primary-button:hover {
  opacity: .85;
}

.secondary-button {
  width: 100%;

  padding: 12px;
  margin-bottom: 10px;

  border: 1px solid var(--border);
  border-radius: 11px;

  background: var(--surface2);
  color: var(--text);
}


/* =====================================================
   11. WIFI NETWORK
===================================================== */

.network-list {
  margin-top: 10px;

  color: var(--muted);
  font-size: 12px;
}

.network-item {
  display: flex;
  align-items: center;
  justify-content: space-between;

  padding: 11px;
  margin-top: 7px;

  background: var(--surface2);

  border-radius: 10px;

  cursor: pointer;

  transition: outline .2s;
}

.network-item:hover {
  outline: 1px solid var(--accent);
}


/* =====================================================
   12. INFORMATION ROW
===================================================== */

.info-row {
  display: flex;
  align-items: center;
  justify-content: space-between;

  gap: 12px;

  padding: 12px 0;

  border-bottom: 1px solid var(--border);

  font-size: 12px;
}

.info-row:last-child {
  border-bottom: 0;
}

.info-row span {
  color: var(--muted);
}


/* =====================================================
   13. MESSAGE
===================================================== */

.message {
  display: none;

  padding: 12px;
  margin-top: 10px;

  background: var(--surface2);

  border-radius: 10px;

  font-size: 12px;
}


/* =====================================================
   14. FOOTER
===================================================== */

.footer {
  margin-top: 30px;

  color: var(--muted);

  text-align: center;

  font-size: 10px;
  letter-spacing: 1px;
}


/* =====================================================
   15. MOBILE RESPONSIVE
===================================================== */

@media (max-width: 380px) {

  body {
    padding: 12px 12px 25px;
  }

  .header {
    margin-bottom: 18px;
  }

  .brand {
    gap: 11px;
  }

  .brand-icon {
    width: 41px;
    height: 41px;

    font-size: 20px;
  }

  .brand strong {
    font-size: 17px;
  }

  .hero {
    padding: 16px;
    border-radius: 17px;
  }

  .clock {
    font-size: 30px;
  }

  .status-grid {
    gap: 9px;
  }

  .status-card {
    padding: 12px;
  }

  .relay-grid {
    gap: 8px;
  }

  .relay-card {
    padding: 12px;
    border-radius: 13px;
  }

  .relay-icon {
    width: 31px;
    height: 31px;

    margin-bottom: 10px;

    font-size: 16px;
  }

  .relay-name {
    font-size: 12px;
  }

  .relay-description {
    font-size: 8px;
  }

  .relay-bottom {
    margin-top: 12px;
    padding-top: 9px;
  }

  .state-label {
    font-size: 8px;
  }

  .section {
    margin-top: 20px;
  }

}


/* =====================================================
   16. EXTRA SMALL DEVICES
===================================================== */

@media (max-width: 320px) {

  .relay-grid {
    gap: 7px;
  }

  .relay-card {
    padding: 10px;
  }

  .relay-name {
    font-size: 11px;
  }

  .switch {
    width: 35px;
    height: 21px;
  }

  .switch::after {
    width: 17px;
    height: 17px;
  }

  .switch.active::after {
    transform: translateX(14px);
  }

}


/* =====================================================
   17. REDUCED MOTION
===================================================== */

@media (prefers-reduced-motion: reduce) {

  *,
  *::before,
  *::after {
    scroll-behavior: auto !important;
    transition-duration: .01ms !important;
    animation-duration: .01ms !important;
  }

}

/* NOVIA TAB NAVIGATION */
.tab-nav {
  position: fixed;
  left: 0;
  right: 0;
  bottom: 0;
  z-index: 20;
  display: grid;
  grid-template-columns: repeat(3, minmax(0,1fr));
  gap: 5px;
  padding: 5px;
  margin: 0 0 16px;
  border: 1px solid var(--border);
  border-radius: 14px;
  background: var(--surface);
  box-shadow: var(--shadow);
}
.tab-button {
  min-width: 0;
  padding: 11px 5px;
  border: 0;
  border-radius: 10px;
  background: transparent;
  color: var(--muted);
  font-size: 12px;
  font-weight: 700;
  white-space: nowrap;
}
.tab-button[aria-selected="true"] {
  background: var(--accent-bg);
  color: var(--accent);
}
.tab-panel { display: none; }
.tab-panel.active { display: block; }
.tab-panel > .section:first-child { margin-top: 0; }
@media (max-width: 380px) {
  .tab-button { font-size: 11px; padding: 10px 2px; }
}



/* Bottom navigation: mobile-first, never covers page content */
.tab-nav {
  position: fixed;
  left: 0;
  right: 0;
  bottom: 0;
  z-index: 1000;
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 6px;
  padding: 8px max(12px, env(safe-area-inset-right)) calc(8px + env(safe-area-inset-bottom)) max(12px, env(safe-area-inset-left));
  margin: 0;
  border: 0;
  border-top: 1px solid var(--border);
  border-radius: 16px 16px 0 0;
  background: var(--surface);
  box-shadow: 0 -6px 22px rgba(0,0,0,.14);
}
.tab-button {
  min-width: 0;
  min-height: 46px;
  padding: 10px 4px;
  border: 0;
  border-radius: 11px;
  background: transparent;
  color: var(--muted);
  font-size: 12px;
  font-weight: 700;
  white-space: nowrap;
  touch-action: manipulation;
}
.tab-button[aria-selected="true"] {
  background: var(--accent-bg);
  color: var(--accent);
}
.tab-panel { display: none; min-width: 0; }
.tab-panel.active { display: block; }
.tab-panel > .section:first-child { margin-top: 0; }
.container {
  width: 100%;
  max-width: 850px;
  margin: 0 auto;
  padding-bottom: calc(100px + env(safe-area-inset-bottom));
}
.helper-text { margin: 6px 0 12px; color: var(--muted); font-size: 12px; line-height: 1.55; }
.field-hint { display: block; margin: 8px 0 6px; color: var(--muted); font-size: 11px; }
.timer-countdown { margin-top: 12px; padding: 13px 14px; border: 1px solid var(--border); border-radius: 12px; background: var(--surface2); color: var(--accent); font-size: 13px; font-weight: 700; text-align: center; }
@media (max-width: 380px) {
  body { padding: 12px 12px 20px; }
  .tab-button { font-size: 11px; padding-left: 2px; padding-right: 2px; }
  .container { padding-bottom: calc(96px + env(safe-area-inset-bottom)); }
}
@media (min-width: 760px) {
  .tab-nav {
    left: 50%;
    right: auto;
    width: min(680px, 100%);
    transform: translateX(-50%);
    border: 1px solid var(--border);
    border-bottom: 0;
  }
}
</style>

</head>

<body>

<div class="container" id="dashboardApp">

<!-- HEADER -->

<header class="header">

  <div class="brand">

    <div class="brand-icon">⌂</div>

    <div>
      <strong>NOVIA HOME</strong>
      <small>SMART HOME SYSTEM · V4.4</small>
    </div>

  </div>

  <div style="display:flex;gap:8px">
  <button
    class="icon-button"
    id="themeButton"
    onclick="toggleTheme()"
    aria-label="Ganti tema">
    ☾
  </button>
</div>

</header>

<!-- CLOCK -->

<section class="hero">

  <div class="eyebrow">WELCOME HOME</div>

  <div class="clock" id="clock">--:--:--</div>

  <div class="date" id="date">Loading date...</div>

</section>

<!-- MONITORING TAB -->
<div class="tab-panel" id="panelMonitor" role="tabpanel" aria-labelledby="tabMonitor">

<!-- CONNECTION STATUS -->

<div class="status-grid">

  <div class="status-card">

    <div class="status-top">

      <span class="dot" id="wifiDot"></span>

      WIFI CONNECTION

    </div>

    <strong class="status-value" id="wifiStatus">
      CHECKING
    </strong>

  </div>

  <div class="status-card">

    <div class="status-top">

      <span class="dot" id="blynkDot"></span>

      BLYNK CLOUD

    </div>

    <strong class="status-value" id="blynkStatus">
      CHECKING
    </strong>

  </div>

</div>

<!-- STATISTICS -->

<section class="section">

  <div class="section-heading">

    <h3>HOME OVERVIEW</h3>

    <small>REAL-TIME STATUS</small>

  </div>

  <div class="stat-grid">

    <div class="stat">

      <strong id="activeCount">0</strong>

      <span>ACTIVE RELAY</span>

    </div>

    <div class="stat">

      <strong id="offCount">4</strong>

      <span>INACTIVE RELAY</span>

    </div>

    <div class="stat">

      <strong>4</strong>

      <span>TOTAL CHANNEL</span>

    </div>

  </div>

</section>

</div><!-- /panelMonitor -->

<!-- CONTROL TAB -->
<div class="tab-panel active" id="panelControl" role="tabpanel" aria-labelledby="tabControl">

<!-- RELAY CONTROL -->

<section class="section">

  <div class="section-heading">

    <h3>ROOM CONTROL</h3>

    <small>4 CHANNELS</small>

  </div>

  <div class="relay-grid">

    <div class="relay-card" id="card0">

      <div class="relay-icon">✳</div>

      <div class="relay-name">Lampu Depan</div>

      <div class="relay-description">CHANNEL 01</div>

      <div class="relay-bottom">

        <span class="state-label" id="state0">OFF</span>

        <button
          class="switch"
          id="switch0"
          onclick="toggleRelay(0)"
          aria-label="Lampu Depan">
        </button>

      </div>

    </div>

    <div class="relay-card" id="card1">

      <div class="relay-icon">✳</div>

      <div class="relay-name">Lampu Tengah</div>

      <div class="relay-description">CHANNEL 02</div>

      <div class="relay-bottom">

        <span class="state-label" id="state1">OFF</span>

        <button
          class="switch"
          id="switch1"
          onclick="toggleRelay(1)"
          aria-label="Lampu Tengah">
        </button>

      </div>

    </div>

    <div class="relay-card" id="card2">

      <div class="relay-icon">✳</div>

      <div class="relay-name">Lampu Kamar</div>

      <div class="relay-description">CHANNEL 03</div>

      <div class="relay-bottom">

        <span class="state-label" id="state2">OFF</span>

        <button
          class="switch"
          id="switch2"
          onclick="toggleRelay(2)"
          aria-label="Lampu Kamar">
        </button>

      </div>

    </div>

    <div class="relay-card" id="card3">

      <div class="relay-icon">✳</div>

      <div class="relay-name">Kipas Angin</div>

      <div class="relay-description">CHANNEL 04</div>

      <div class="relay-bottom">

        <span class="state-label" id="state3">OFF</span>

        <button
          class="switch"
          id="switch3"
          onclick="toggleRelay(3)"
          aria-label="Kipas Angin">
        </button>

      </div>

    </div>

  </div>

</section>

</div><!-- /panelControl -->

<!-- AUTOMATION TAB -->
<div class="tab-panel" id="panelAutomation" role="tabpanel" aria-labelledby="tabAutomation">

<!-- AUTOMATION -->
<section class="section">
  <div class="section-heading"><h3>AUTOMATION</h3><small>TIMER & DAILY SCHEDULE</small></div>
  <div class="panel">
    <h3>Operating Mode</h3>
    <select class="form-control" id="modeSelect" onchange="setMode(this.value)">
      <option value="MANUAL">MANUAL - Jadwal nonaktif</option>
      <option value="AUTO">AUTO - Jalankan jadwal harian</option>
    </select>
    <div class="info-row"><span>Waktu NTP (WIB)</span><strong id="timeSync">MENUNGGU</strong></div>
    <h3 style="margin-top:18px">Timer OFF</h3>
    <p class="helper-text">Pilih perangkat dan durasi. Perangkat akan mati otomatis setelah hitungan mundur selesai. Isi 0 untuk menghapus timer.</p>
    <select class="form-control" id="timerChannel"><option value="1">Lampu Depan</option><option value="2">Lampu Tengah</option><option value="3">Lampu Kamar</option><option value="4">Kipas Angin</option></select>
    <label class="field-hint" for="timerMinutes">Durasi timer · 1 menit sampai 24 jam</label>
    <input class="form-control" id="timerMinutes" type="number" min="0" max="1440" value="30" placeholder="Durasi menit">
    <button class="primary-button" onclick="saveTimer()">SIMPAN TIMER OFF</button>
    <div class="timer-countdown" id="timerCountdown">Belum ada timer aktif untuk perangkat ini</div>
    <div class="message" id="automationMessage"></div>
    <h3 style="margin-top:20px">Jadwal Harian</h3>
    <p class="helper-text">Atur jam perangkat menyala dan mati setiap hari. Aktifkan jadwal dan pilih mode AUTO agar jadwal dijalankan otomatis.</p>
    <select class="form-control" id="scheduleChannel"><option value="1">Lampu Depan</option><option value="2">Lampu Tengah</option><option value="3">Lampu Kamar</option><option value="4">Kipas Angin</option></select>
    <label style="display:block;color:var(--muted);font-size:12px;margin:8px 0">Waktu ON</label>
    <input class="form-control" id="scheduleOn" type="time" value="18:00">
    <label style="display:block;color:var(--muted);font-size:12px;margin:8px 0">Waktu OFF</label>
    <input class="form-control" id="scheduleOff" type="time" value="23:00">
    <label style="display:flex;align-items:center;gap:9px;margin:12px 0;color:var(--text);font-size:13px"><input id="scheduleEnabled" type="checkbox"> Aktifkan jadwal channel ini</label>
    <button class="primary-button" onclick="saveSchedule()">SIMPAN JADWAL</button>
    <p class="field-hint" id="scheduleHint">Jadwal akan berulang setiap hari saat mode AUTO aktif.</p>
    <div class="info-row"><span>Timer aktif</span><strong id="timerSummary">-</strong></div>
    <div class="info-row"><span>Total pemakaian relay</span><strong id="usageSummary">-</strong></div>
  </div>
</section>

<!-- WIFI MANAGEMENT -->

<section class="section">

  <div class="section-heading">

    <h3>NETWORK MANAGEMENT</h3>

    <small>WIFI SETTINGS</small>

  </div>

  <div class="panel">

    <h3>Access Point NOVIA HOME</h3>
    <p style="color:var(--muted);font-size:13px;line-height:1.5">
      Ubah nama dan password hotspot NOVIA HOME. Setelah disimpan, ESP32 restart dan HP perlu disambungkan ulang ke nama AP yang baru.
    </p>
    <form id="apForm">
      <input class="form-control" id="apSsid" name="ssid" maxlength="32" placeholder="Nama AP NOVIA HOME" required>
      <input class="form-control" id="apPass" name="pass" type="password" minlength="8" maxlength="63" placeholder="Password AP baru (minimal 8 karakter)" required autocomplete="new-password">
      <button class="primary-button" type="submit">SIMPAN NAMA & PASSWORD AP</button>
    </form>
    <div class="message" id="apMessage"></div>

    <h3 style="margin-top:24px">WiFi Scanner / Router Rumah</h3>

    <button
      class="secondary-button"
      id="scanButton"
      onclick="scanWiFi()">
      SCAN AVAILABLE NETWORKS
    </button>

    <div class="network-list" id="networkList">
      Tekan tombol scan untuk mencari jaringan.
    </div>

    <form id="wifiForm">

      <input
        class="form-control"
        id="ssid"
        name="ssid"
        placeholder="Nama WiFi"
        maxlength="32"
        required>

      <input
        class="form-control"
        id="pass"
        name="pass"
        type="password"
        placeholder="Password WiFi"
        maxlength="63">

      <button
        class="primary-button"
        type="submit">
        SIMPAN KONFIGURASI WIFI
      </button>

    </form>

    <div class="message" id="wifiMessage"></div>

  </div>

</section>

<!-- DEVICE INFORMATION -->

<section class="section">

  <div class="section-heading">

    <h3>DEVICE INFORMATION</h3>

    <small>NOVIA V4.4</small>

  </div>

  <div class="panel">

    <div class="info-row">

      <span>Device</span>

      <strong>ESP32 DevKit V1</strong>

    </div>

    <div class="info-row">

      <span>Local IP</span>

      <strong id="deviceIP">-</strong>

    </div>

    <div class="info-row">

      <span>WiFi Signal</span>

      <strong id="wifiRSSI">-</strong>

    </div>

    <div class="info-row">

      <span>Access Point</span>

      <strong id="apSSIDInfo">NOVIA-HOME</strong>

    </div>

    <div class="info-row">
      <span>AP Dashboard</span>
      <strong id="apIPInfo">192.168.4.1</strong>
    </div>

    <div class="info-row">

      <span>Firmware</span>

      <strong>V4.4 MINIMAL</strong>

    </div>

    <div class="info-row">
      <span>Storage</span>
      <strong>Preferences · NVS</strong>
    </div>
  </div>

</section>

</div><!-- /panelAutomation -->

<nav class="tab-nav" role="tablist" aria-label="Menu NOVIA HOME">
  <button class="tab-button" id="tabControl" role="tab" aria-selected="true" aria-controls="panelControl" onclick="showTab('control')">Saklar</button>
  <button class="tab-button" id="tabMonitor" role="tab" aria-selected="false" aria-controls="panelMonitor" onclick="showTab('monitor')">Monitoring</button>
  <button class="tab-button" id="tabAutomation" role="tab" aria-selected="false" aria-controls="panelAutomation" onclick="showTab('automation')">Otomasi</button>
</nav>



<footer class="footer">

  NOVIA HOME V4.4

  <br><br>

  SMART CONTROL · SMART LIVING

</footer>

</div>

<script>

// =================================================
// JAVASCRIPT CONFIGURATION
// =================================================

let states = [0,0,0,0];

let busy = [false,false,false,false];

let scanTimer = null;
let scheduleFormLoadedFor = -1;
let timerEndsAt = [0,0,0,0];
const relayNames = ["Lampu Depan","Lampu Tengah","Lampu Kamar","Kipas Angin"];

// Dashboard diakses langsung melalui jaringan lokal; tidak memakai autentikasi PIN.
const noviaNativeFetch = window.fetch.bind(window);

// =================================================
// TAB NAVIGATION
// =================================================
function showTab(name) {
  const tabs = ['control', 'monitor', 'automation'];
  tabs.forEach(tab => {
    const active = tab === name;
    const panel = document.getElementById('panel' + tab.charAt(0).toUpperCase() + tab.slice(1));
    const button = document.getElementById('tab' + tab.charAt(0).toUpperCase() + tab.slice(1));
    if (panel) panel.classList.toggle('active', active);
    if (button) button.setAttribute('aria-selected', active ? 'true' : 'false');
  });
  window.scrollTo({top: 0, behavior: 'smooth'});
}

// =================================================
// THEME MANAGEMENT
// =================================================

function setTheme(theme) {

  document.body.classList.toggle(
    'light',
    theme === 'light'
  );

  document.getElementById('themeButton').innerText =
    theme === 'light' ? '☀' : '☾';

  localStorage.setItem('noviaTheme',theme);

}

function toggleTheme() {

  const light =
    document.body.classList.contains('light');

  setTheme(light ? 'dark' : 'light');

}

// Restore saved theme.

setTheme(
  localStorage.getItem('noviaTheme') || 'dark'
);

// =================================================
// DIGITAL CLOCK
// =================================================

function updateClock() {

  const now = new Date();

  document.getElementById('clock').innerText =
    now.toLocaleTimeString('id-ID',{
      hour12:false
    });

  document.getElementById('date').innerText =
    now.toLocaleDateString('id-ID',{
      weekday:'long',
      day:'numeric',
      month:'long',
      year:'numeric'
    });

}

setInterval(updateClock,1000);

updateClock();

// =================================================
// RENDER RELAY
// =================================================

function renderRelays() {

  let active = 0;

  for(let i=0;i<4;i++) {

    const on = Number(states[i]) === 1;

    if(on) active++;

    document.getElementById('state'+i).innerText =
      on ? 'ON' : 'OFF';

    document.getElementById('switch'+i)
      .classList.toggle('active',on);

    document.getElementById('card'+i)
      .classList.toggle('active',on);

  }

  document.getElementById('activeCount').innerText =
    active;

  document.getElementById('offCount').innerText =
    4-active;

}

// =================================================
// UPDATE DEVICE STATE
// =================================================

async function updateStatus() {

  try {

    const response = await fetch(
      '/api/state?t='+Date.now(),
      {cache:'no-store'}
    );

    if(!response.ok) throw new Error();

    const data = await response.json();

    if (!Array.isArray(data.channels) || data.channels.length !== 4) {
      throw new Error('Invalid channel payload');
    }

    states = data.channels.map(value => Number(value) === 1 ? 1 : 0);

    renderRelays();

    const wifi = data.wifi;

    const blynk = data.blynk;

    document.getElementById('wifiStatus').innerText =
      wifi ? 'CONNECTED' : 'DISCONNECTED';

    document.getElementById('blynkStatus').innerText =
      blynk ? 'CONNECTED' : 'DISCONNECTED';

    document.getElementById('wifiDot')
      .classList.toggle('online',wifi);

    document.getElementById('blynkDot')
      .classList.toggle('online',blynk);

    document.getElementById('deviceIP').innerText =
      data.ip;

    if (data.apSSID) {
      document.getElementById('apSSIDInfo').innerText = data.apSSID;
      const apInput = document.getElementById('apSsid');
      if (document.activeElement !== apInput && !apInput.value) apInput.value = data.apSSID;
    }
    if (data.apIP) document.getElementById('apIPInfo').innerText = data.apIP;

    document.getElementById('wifiRSSI').innerText =
      wifi ? data.rssi+' dBm' : 'NOT CONNECTED';
    if (data.mode) document.getElementById('modeSelect').value = data.mode;
    document.getElementById('timeSync').innerText = data.timeSynced ? 'TERSINKRON' : 'BELUM TERSINKRON';
    if (Array.isArray(data.timers)) {
      data.timers.forEach((seconds,i) => { timerEndsAt[i] = seconds > 0 ? Date.now() + seconds * 1000 : 0; });
      const running = data.timers.map((v,i)=>v>0?relayNames[i]+': '+formatCountdown(v):null).filter(Boolean);
      document.getElementById('timerSummary').innerText = running.length ? running.join(' · ') : 'Tidak ada';
      renderTimerCountdown();
    }
    if (Array.isArray(data.usageSeconds)) {
      document.getElementById('usageSummary').innerText = data.usageSeconds.map((v,i)=>'CH'+(i+1)+' '+formatDuration(v)).join(' | ');
    }
    if (Array.isArray(data.scheduleEnabled)) {
      const selected = Number(document.getElementById('scheduleChannel').value) - 1;
      if (selected !== scheduleFormLoadedFor) {
        scheduleFormLoadedFor = selected;
        document.getElementById('scheduleEnabled').checked = !!data.scheduleEnabled[selected];
        updateScheduleHint();
        if (Array.isArray(data.scheduleOn) && Array.isArray(data.scheduleOff)) {
          const asTime = n => String(Math.floor(n/60)).padStart(2,'0')+':'+String(n%60).padStart(2,'0');
          document.getElementById('scheduleOn').value = asTime(data.scheduleOn[selected]);
          document.getElementById('scheduleOff').value = asTime(data.scheduleOff[selected]);
        }
      }
    }

  } catch(error) {

    document.getElementById('wifiStatus').innerText =
      'UNAVAILABLE';

    document.getElementById('blynkStatus').innerText =
      'UNKNOWN';

    document.getElementById('wifiDot')
      .classList.remove('online');

    document.getElementById('blynkDot')
      .classList.remove('online');

  }

}

// =================================================
// RELAY CONTROL
// =================================================

async function toggleRelay(ch) {

  if(busy[ch]) return;

  busy[ch] = true;

  const next = states[ch] ? 0 : 1;

  const button =
    document.getElementById('switch'+ch);

  button.disabled = true;

  try {

    const body = new URLSearchParams();
    body.append('ch', String(ch + 1));
    body.append('state', String(next));

    const response = await fetch('/api/set', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded'
      },
      body: body.toString(),
      cache: 'no-store'
    });

    if(!response.ok) throw new Error();

    const result = await response.json();

    if(!result.ok) throw new Error();

    await updateStatus();

  } catch(error) {

    showMessage(
      'wifiMessage',
      'Kontrol gagal. Periksa koneksi perangkat.'
    );

  } finally {

    busy[ch] = false;

    button.disabled = false;

  }

}

// AUTOMATION CONTROL
async function postForm(path, values) {
  const body = new URLSearchParams();
  Object.keys(values).forEach(k => body.append(k, String(values[k])));
  const response = await fetch(path, {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:body.toString(), cache:'no-store'});
  if (!response.ok) throw new Error('Request gagal');
  return response.json();
}
async function setMode(mode) {
  try { await postForm('/api/mode', {mode}); showMessage('automationMessage','Mode '+mode+' tersimpan.'); updateStatus(); }
  catch(e) { showMessage('automationMessage','Gagal mengubah mode.'); }
}
async function saveTimer() {
  const ch = document.getElementById('timerChannel').value;
  const minutes = Number(document.getElementById('timerMinutes').value);
  if (!Number.isInteger(minutes) || minutes < 0 || minutes > 1440) { showMessage('automationMessage','Durasi harus 0 sampai 1440 menit.'); return; }
  try { await postForm('/api/timer',{ch,minutes}); timerEndsAt[Number(ch)-1] = minutes > 0 ? Date.now()+minutes*60000 : 0; renderTimerCountdown(); showMessage('automationMessage',minutes===0?'Timer dihapus.':'Timer OFF tersimpan.'); updateStatus(); }
  catch(e) { showMessage('automationMessage','Gagal menyimpan timer.'); }
}
function formatCountdown(totalSeconds) {
  const s = Math.max(0, Math.ceil(totalSeconds));
  const h = Math.floor(s / 3600), m = Math.floor((s % 3600) / 60), sec = s % 60;
  return h ? `${h}j ${String(m).padStart(2,'0')}m ${String(sec).padStart(2,'0')}d` : `${m}m ${String(sec).padStart(2,'0')}d`;
}
function renderTimerCountdown() {
  const ch = Number(document.getElementById('timerChannel')?.value || 1) - 1;
  const left = timerEndsAt[ch] ? Math.max(0, Math.ceil((timerEndsAt[ch] - Date.now()) / 1000)) : 0;
  const el = document.getElementById('timerCountdown');
  if (el) el.innerText = left > 0 ? `${relayNames[ch]} akan OFF dalam ${formatCountdown(left)}` : `Tidak ada hitungan mundur aktif untuk ${relayNames[ch]}`;
  const summary = document.getElementById('timerSummary');
  if (summary && timerEndsAt.some(t => t > Date.now())) {
    summary.innerText = timerEndsAt.map((t,i)=>t>Date.now()?relayNames[i]+': '+formatCountdown((t-Date.now())/1000):null).filter(Boolean).join(' · ');
  }
}
function updateScheduleHint() {
  const on = document.getElementById('scheduleOn').value;
  const off = document.getElementById('scheduleOff').value;
  const enabled = document.getElementById('scheduleEnabled').checked;
  const mode = document.getElementById('modeSelect').value;
  const hint = document.getElementById('scheduleHint');
  if (!hint) return;
  hint.innerText = !enabled ? 'Jadwal belum aktif. Centang Aktifkan jadwal untuk menggunakannya.' :
    mode !== 'AUTO' ? `Jadwal ${on}–${off} tersimpan, tetapi belum berjalan. Ubah mode ke AUTO.` :
    `Aktif: perangkat ON pukul ${on} dan OFF pukul ${off}, berulang setiap hari.`;
}
async function saveSchedule() {
  const ch = document.getElementById('scheduleChannel').value;
  const on = document.getElementById('scheduleOn').value;
  const off = document.getElementById('scheduleOff').value;
  if (!on || !off || on === off) { showMessage('automationMessage','Isi waktu ON dan OFF yang berbeda.'); return; }
  const toMinute = t => { const a=t.split(':').map(Number); return a[0]*60+a[1]; };
  try { await postForm('/api/schedule',{ch,on:toMinute(on),off:toMinute(off),enabled:document.getElementById('scheduleEnabled').checked?1:0}); showMessage('automationMessage','Jadwal harian tersimpan. Mode AUTO diperlukan untuk menjalankannya.'); }
  catch(e) { showMessage('automationMessage','Gagal menyimpan jadwal.'); }
}
function formatDuration(seconds) {
  const h=Math.floor(seconds/3600), m=Math.floor((seconds%3600)/60);
  return h+'j '+m+'m';
}

// =================================================
// WIFI SCANNER
// =================================================

async function scanWiFi() {

  const button = document.getElementById('scanButton');
  const list = document.getElementById('networkList');

  if (scanTimer) {
    clearInterval(scanTimer);
    scanTimer = null;
  }

  button.disabled = true;
  button.innerText = 'SCANNING...';
  list.innerText = 'Mencari jaringan WiFi sekitar...';

  try {
    const startResponse = await fetch('/api/scan', {
      method: 'GET',
      cache: 'no-store',
      credentials: 'same-origin'
    });

    const startData = await startResponse.json();
    if (!startResponse.ok || !startData.ok) {
      throw new Error(startData.message || ('HTTP ' + startResponse.status));
    }

    let completed = false;

    // Sequential polling prevents overlapping requests to WebServer.
    for (let attempt = 0; attempt < 35; attempt++) {
      await new Promise(resolve => setTimeout(resolve, 800));

      const statusResponse = await fetch('/api/scan/status', {
        method: 'GET',
        cache: 'no-store',
        credentials: 'same-origin'
      });

      const statusData = await statusResponse.json();
      if (!statusResponse.ok || statusData.error) {
        throw new Error(statusData.error || ('HTTP ' + statusResponse.status));
      }

      if (statusData.scanning) {
        list.innerText = 'Sedang memindai... (' + (attempt + 1) + ')';
        continue;
      }

      renderNetworks(statusData.networks || []);
      completed = true;
      break;
    }

    if (!completed) {
      throw new Error('Waktu pemindaian habis. Coba scan kembali.');
    }

  } catch (error) {
    console.error('[NOVIA WIFI SCAN]', error);
    list.innerText = 'Scan gagal: ' + (error.message || 'Kesalahan tidak diketahui');
  } finally {
    button.disabled = false;
    button.innerText = 'SCAN AVAILABLE NETWORKS';
  }
}

// =================================================
// RENDER NETWORK LIST
// =================================================

function renderNetworks(networks) {

  const list =
    document.getElementById('networkList');

  list.innerHTML = '';

  if(networks.length === 0) {

    list.innerText = 'Tidak ada jaringan ditemukan.';

    return;
  }

  networks.sort((a,b)=>b.rssi-a.rssi);

  networks.forEach(function(network) {

    const item = document.createElement('div');

    item.className = 'network-item';

    const name = document.createElement('span');

    name.textContent = network.ssid || '(Hidden SSID - tidak dapat dipilih)';

    const signal = document.createElement('span');

    signal.textContent =
      network.rssi+' dBm '+
      (network.secure ? '🔒' : 'OPEN');

    item.appendChild(name);

    item.appendChild(signal);

    if (network.ssid) {
      item.onclick = function() {
        document.getElementById('ssid').value = network.ssid;
        document.getElementById('pass').focus();
      };
    } else {
      item.style.opacity = '0.65';
      item.style.cursor = 'default';
    }

    list.appendChild(item);

  });

}

// =================================================
// ACCESS POINT CONFIGURATION SAVE
// =================================================

document.getElementById('apForm').addEventListener('submit', async function(event) {
  event.preventDefault();
  const ssid = document.getElementById('apSsid').value.trim();
  const pass = document.getElementById('apPass').value;
  const button = this.querySelector('button[type="submit"]');
  const original = button.innerText;

  if (!ssid || ssid.length > 32 || pass.length < 8 || pass.length > 63) {
    showMessage('apMessage', 'SSID harus 1-32 karakter dan password AP 8-63 karakter.');
    return;
  }

  button.disabled = true;
  button.innerText = 'MENYIMPAN & RESTART...';
  const body = new URLSearchParams();
  body.append('ssid', ssid);
  body.append('pass', pass);

  try {
    const response = await fetch('/saveap', {
      method: 'POST',
      headers: {'Content-Type': 'application/x-www-form-urlencoded'},
      body: body.toString(),
      cache: 'no-store'
    });
    const result = await response.json();
    if (!response.ok || !result.ok) throw new Error(result.error || 'Gagal menyimpan AP');
    showMessage('apMessage', 'Pengaturan AP tersimpan. ESP32 restart; sambungkan HP ke SSID baru setelah hotspot muncul kembali.');
    document.getElementById('apPass').value = '';
  } catch (error) {
    showMessage('apMessage', 'Permintaan dikirim. Jika koneksi terputus, tunggu AP aktif kembali lalu sambungkan ke SSID baru.');
  } finally {
    button.disabled = false;
    button.innerText = original;
  }
});

// =================================================
// WIFI CONFIGURATION SAVE
// =================================================

document.getElementById('wifiForm')
.addEventListener('submit',async function(event) {

  event.preventDefault();

  const ssid =
    document.getElementById('ssid').value.trim();

  const pass =
    document.getElementById('pass').value;

  const submitButton = this.querySelector('button[type="submit"]');
  submitButton.disabled = true;
  const originalButtonText = submitButton.innerText;
  submitButton.innerText = 'MENYIMPAN...';

  if(!ssid || ssid.length > 32 || pass.length > 63) {

    showMessage(
      'wifiMessage',
      'Nama WiFi atau password tidak valid.'
    );

    submitButton.disabled = false;
    submitButton.innerText = originalButtonText;
    return;
  }

  const formData = new URLSearchParams();

  formData.append('ssid',ssid);

  formData.append('pass',pass);

  try {

    const response = await fetch(
      '/savewifi',
      {
        method:'POST',
        headers:{
          'Content-Type':
          'application/x-www-form-urlencoded'
        },
        body:formData.toString()
      }
    );

    if(!response.ok) throw new Error();

    showMessage(
      'wifiMessage',
      'Konfigurasi tersimpan. ESP32 sedang mencoba koneksi WiFi baru.'
    );

    document.getElementById('pass').value = '';

  } catch(error) {

    showMessage(
      'wifiMessage',
      'Gagal menyimpan konfigurasi WiFi.'
    );

  } finally {
    submitButton.disabled = false;
    submitButton.innerText = originalButtonText;
  }

});

// =================================================
// MESSAGE
// =================================================

function showMessage(id,message) {

  const element = document.getElementById(id);

  element.innerText = message;

  element.style.display = 'block';

}

// =================================================
// AUTO UPDATE
// =================================================

document.getElementById('scheduleChannel').addEventListener('change', updateStatus);
document.getElementById('timerChannel').addEventListener('change', renderTimerCountdown);
['scheduleOn','scheduleOff','scheduleEnabled'].forEach(id => document.getElementById(id).addEventListener('change', updateScheduleHint));
document.getElementById('modeSelect').addEventListener('change', updateScheduleHint);

setInterval(updateStatus,2000);
setInterval(renderTimerCountdown,1000);
updateStatus();

</script>

</body>

</html>

)NOVIAHTML";

