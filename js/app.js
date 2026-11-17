document.addEventListener("DOMContentLoaded", () => {
  const API_URL = "https://novia-home-api.ryo-tjz.workers.dev";

  // ==========================================
  // NOVIA HOME V4 - THEME SWITCHER
  // ==========================================

  const themeButton = document.createElement("button");

  themeButton.type = "button";
  themeButton.className = "theme-toggle";
  themeButton.setAttribute("aria-label", "Ganti tema");

  const savedTheme = (() => {
    try {
      return localStorage.getItem("novia-home-theme");
    } catch (error) {
      return null;
    }
  })();

  let currentTheme =
    savedTheme === "light" ? "light" : "dark";

  function applyTheme(theme) {
    currentTheme = theme;

    document.documentElement.setAttribute(
      "data-theme",
      theme
    );

    const isLight = theme === "light";

    themeButton.innerHTML = `
      <span class="theme-toggle-icon">
        ${isLight ? "☀" : "☾"}
      </span>
      <span>${isLight ? "LIGHT" : "DARK"}</span>
    `;

    themeButton.setAttribute(
      "aria-label",
      isLight ? "Aktifkan Dark Mode" : "Aktifkan Light Mode"
    );

    try {
      localStorage.setItem("novia-home-theme", theme);
    } catch (error) {
      console.warn("Tema tidak dapat disimpan:", error);
    }
  }

  const topbar = document.querySelector(".topbar");

  if (topbar) {
    topbar.appendChild(themeButton);
  }

  themeButton.addEventListener("click", () => {
    applyTheme(
      currentTheme === "dark" ? "light" : "dark"
    );
  });

  applyTheme(currentTheme);

  // ==========================================
  // API REAL-TIME MONITOR
  // ==========================================

  const API_CHECK_INTERVAL = 5000;
  const API_TIMEOUT = 3000;

  const statusElement = document.getElementById("connectionStatus");
  const statusPill = statusElement
    ? statusElement.closest(".demo-pill")
    : null;

  const cards = [...document.querySelectorAll(".device-card")];
  const note = document.querySelector(".demo-note");
  const summaryCards = [...document.querySelectorAll(".summary-card")];
  const systemRows = [...document.querySelectorAll(".system-list div")];

  const channelNames = [
    "Lampu Depan",
    "Lampu Tengah",
    "Lampu Kamar",
    "Kipas Angin"
  ];

  let controlPin = null;
  let refreshing = false;
  let lastSuccessfulCheck = null;
  let lastLatency = null;
  let consecutiveFailures = 0;
  let nextCheckTimer = null;
  let requestInFlight = false;
  let previousApiState = null;
  let previousBrowserState = null;
  const HISTORY_KEY = "novia-home-connection-history-v1";
  const HISTORY_LIMIT = 50;
  const historyList = document.getElementById("connectionHistoryList");
  const historyCount = document.getElementById("historyCount");
  const historyStorageState = document.getElementById("historyStorageState");
  const clearHistoryButton = document.getElementById("clearConnectionHistory");
  const refreshButton = document.getElementById("refreshStatus");
  const browserHealth = document.getElementById("browserHealth");
  const apiHealth = document.getElementById("apiHealth");

  function setHealthCard(element, textId, state, message) {
    if (!element) return;
    element.dataset.health = state;
    const value = document.getElementById(textId);
    if (value) value.textContent = message;
  }

  // ==========================================
  // CONNECTION HISTORY - LOCAL DEVICE STORAGE
  // ==========================================

  function readConnectionHistory() {
    try {
      const parsed = JSON.parse(localStorage.getItem(HISTORY_KEY) || "[]");
      return Array.isArray(parsed) ? parsed.filter(item =>
        item && typeof item.time === "string" &&
        typeof item.source === "string" &&
        typeof item.title === "string" &&
        typeof item.detail === "string"
      ).slice(0, HISTORY_LIMIT) : [];
    } catch (error) {
      if (historyStorageState) historyStorageState.textContent = "Penyimpanan tidak tersedia";
      return [];
    }
  }

  let connectionHistory = readConnectionHistory();

  function renderConnectionHistory() {
    if (historyCount) {
      historyCount.textContent = `${connectionHistory.length} kejadian tersimpan`;
    }
    if (!historyList) return;

    historyList.replaceChildren();
    if (!connectionHistory.length) {
      const empty = document.createElement("li");
      empty.className = "history-empty";
      empty.textContent = "Belum ada riwayat koneksi.";
      historyList.appendChild(empty);
      return;
    }

    connectionHistory.forEach(item => {
      const entry = document.createElement("li");
      entry.className = `history-entry history-${item.state || "info"}`;

      const marker = document.createElement("span");
      marker.className = "history-marker";
      marker.setAttribute("aria-hidden", "true");

      const content = document.createElement("div");
      content.className = "history-content";
      const heading = document.createElement("strong");
      heading.textContent = item.title;
      const detail = document.createElement("span");
      detail.textContent = item.detail;
      const time = document.createElement("time");
      const date = new Date(item.time);
      time.dateTime = item.time;
      time.textContent = Number.isNaN(date.getTime()) ? item.time : date.toLocaleString("id-ID", {
        day: "2-digit", month: "short", year: "numeric",
        hour: "2-digit", minute: "2-digit", second: "2-digit", hour12: false
      });
      content.append(heading, detail, time);
      entry.append(marker, content);
      historyList.appendChild(entry);
    });
  }

  function recordConnectionEvent(source, state, title, detail) {
    const item = {
      time: new Date().toISOString(),
      source, state, title, detail
    };
    connectionHistory.unshift(item);
    connectionHistory = connectionHistory.slice(0, HISTORY_LIMIT);
    try {
      localStorage.setItem(HISTORY_KEY, JSON.stringify(connectionHistory));
      if (historyStorageState) historyStorageState.textContent = "Tersimpan di perangkat ini";
    } catch (error) {
      if (historyStorageState) historyStorageState.textContent = "Gagal menyimpan riwayat";
      console.warn("NOVIA HOME: riwayat tidak dapat disimpan", error);
    }
    renderConnectionHistory();
  }

  if (clearHistoryButton) {
    clearHistoryButton.addEventListener("click", () => {
      if (!connectionHistory.length) return;
      if (!window.confirm("Hapus seluruh riwayat koneksi pada perangkat ini?")) return;
      connectionHistory = [];
      try {
        localStorage.removeItem(HISTORY_KEY);
        if (historyStorageState) historyStorageState.textContent = "Tersimpan di perangkat ini";
      } catch (error) {
        if (historyStorageState) historyStorageState.textContent = "Penyimpanan tidak tersedia";
      }
      renderConnectionHistory();
    });
  }

  renderConnectionHistory();

  function updateReliability(state, latency = null) {
    const browserOnline = navigator.onLine;
    setHealthCard(browserHealth, "browserHealthText", browserOnline ? "online" : "offline", browserOnline ? "Browser online" : "Tidak ada jaringan");
    const apiMessage = state === "online"
      ? "Terhubung · Blynk terbaca"
      : state === "checking"
        ? "Memeriksa layanan..."
        : "Tidak terjangkau";
    setHealthCard(apiHealth, "apiHealthText", state, apiMessage);
    const sync = document.getElementById("lastSyncText");
    const latencyNode = document.getElementById("latencyText");
    const failures = document.getElementById("failureCountText");
    if (sync) sync.textContent = formatClock(lastSuccessfulCheck);
    if (latencyNode) latencyNode.textContent = lastLatency === null ? "--" : `${lastLatency} ms`;
    if (failures) failures.textContent = String(consecutiveFailures);
  }

  function scheduleNextCheck(delay) {
    if (nextCheckTimer) window.clearTimeout(nextCheckTimer);
    nextCheckTimer = window.setTimeout(() => {
      nextCheckTimer = null;
      loadStatus();
    }, delay);
  }

  function formatClock(date) {
    if (!date) return "Belum pernah";
    return date.toLocaleTimeString("id-ID", {
      hour: "2-digit",
      minute: "2-digit",
      second: "2-digit",
      hour12: false
    });
  }

  function setConnection(state, latency = null, recordTransition = true) {
    const connected = state === "online";
    const checking = state === "checking";

    if (recordTransition && state !== "checking" && state !== previousApiState) {
      if (state === "online") {
        recordConnectionEvent("API", "online", previousApiState === "offline" ? "API pulih" : "API terhubung", latency === null ? "Layanan API berhasil dijangkau." : `Layanan API merespons dalam ${latency} ms.`);
      } else if (state === "offline") {
        recordConnectionEvent("API", "offline", previousApiState === "online" ? "API terputus" : "API tidak tersedia", "Dashboard gagal membaca status dari API.");
      }
      previousApiState = state;
    }

    if (connected) {
      lastSuccessfulCheck = new Date();
      lastLatency = latency;
    }

    const lastSeenText = formatClock(lastSuccessfulCheck);
    const latencyText = lastLatency === null
      ? "--"
      : `${lastLatency} ms`;

    updateReliability(state, latency);

    if (statusElement) {
      if (connected) {
        statusElement.textContent = `API ONLINE · ${latencyText}`;
      } else if (checking) {
        statusElement.textContent = "API CONNECTING...";
      } else {
        statusElement.textContent = "API OFFLINE";
      }

      statusElement.title =
        `Last seen: ${lastSeenText}` +
        (lastLatency === null ? "" : ` | Respons terakhir: ${latencyText}`);
    }

    if (statusPill) {
      statusPill.classList.toggle("is-online", connected);
      statusPill.classList.toggle("is-offline", state === "offline");
      statusPill.classList.toggle("is-checking", checking);
      statusPill.setAttribute(
        "aria-label",
        connected
          ? `API online, respons ${latencyText}, terakhir terlihat ${lastSeenText}`
          : checking
            ? "Sedang memeriksa koneksi API"
            : `API offline, terakhir terlihat ${lastSeenText}`
      );
    }

    if (summaryCards[0]) {
      const strong = summaryCards[0].querySelector("strong");
      const small = summaryCards[0].querySelector(".summary-note");

      if (strong) {
        strong.textContent = connected
          ? "Terhubung"
          : checking
            ? "Memeriksa..."
            : "Tidak terhubung";
      }

      if (small) {
        small.textContent = connected
          ? `Worker aktif · ${latencyText} · ${lastSeenText}`
          : checking
            ? "Memeriksa API..."
            : `Last seen: ${lastSeenText}`;
      }
    }

    if (summaryCards[1]) {
      const strong = summaryCards[1].querySelector("strong");
      const small = summaryCards[1].querySelector(".summary-note");
      if (strong) strong.textContent = "4 channel terdaftar";
      if (small) small.textContent = "Sinkronisasi Blynk IoT";
    }

    if (systemRows[2]) {
      const label = systemRows[2].querySelector("span");
      const value = systemRows[2].querySelector("strong");
      if (label) label.textContent = "Komunikasi";
      if (value) value.textContent = "Blynk IoT";
    }

    if (systemRows[4]) {
      const value = systemRows[4].querySelector("strong");
      if (value) {
        value.textContent = connected
          ? `API sehat · ${latencyText}`
          : checking
            ? "Memeriksa koneksi API..."
            : `API gagal · Last seen ${lastSeenText}`;
        value.classList.remove("pending");
      }
    }

    if (note) {
      note.textContent = connected
        ? `Status channel disinkronkan dengan Blynk IoT. Respons API ${latencyText}.`
        : checking
          ? "Memeriksa koneksi API dan sinkronisasi channel..."
          : "Gagal memperbarui status. Dashboard akan mencoba kembali otomatis.";
    }
  }

  function updateCard(channel, value) {
    const card = cards.find(
      item => Number(item.dataset.channel) === channel
    );
    if (!card) return;

    const button = card.querySelector(".toggle-button");
    const badge = card.querySelector(".state-badge");
    const label = card.querySelector(".toggle-label");
    const isOn = value === 1;

    card.classList.toggle("is-on", isOn);

    if (button) {
      button.setAttribute("aria-pressed", String(isOn));
      button.setAttribute(
        "aria-label",
        `${channelNames[channel]}: ${isOn ? "ON" : "OFF"}`
      );
    }

    if (badge) badge.textContent = isOn ? "ON" : "OFF";
    if (label) label.textContent = isOn ? "MATIKAN" : "NYALAKAN";
  }

  async function loadStatus() {
    // Cegah request status bertumpuk.
    if (refreshing || document.visibilityState === "hidden") return;

    refreshing = true;
    requestInFlight = true;
    if (!lastSuccessfulCheck) setConnection("checking");
    else updateReliability("checking");
    if (refreshButton) refreshButton.disabled = true;

    const controller = new AbortController();
    const timeoutId = window.setTimeout(
      () => controller.abort(),
      API_TIMEOUT
    );

    const startedAt = performance.now();

    try {
      const response = await fetch(`${API_URL}/api/status`, {
        method: "GET",
        cache: "no-store",
        signal: controller.signal,
        headers: {
          "Accept": "application/json"
        }
      });

      if (!response.ok) {
        throw new Error(`HTTP ${response.status}`);
      }

      const data = await response.json();

      if (
        data.success !== true ||
        !data.channels ||
        typeof data.channels !== "object"
      ) {
        throw new Error("Respons API tidak valid");
      }

      const values = [0, 1, 2, 3].map(index =>
        Number(data.channels[`V${index}`])
      );

      if (values.some(value => value !== 0 && value !== 1)) {
        throw new Error("Nilai datastream tidak valid");
      }

      const latency = Math.max(
        0,
        Math.round(performance.now() - startedAt)
      );

      values.forEach((value, index) => updateCard(index, value));
      consecutiveFailures = 0;
      setConnection("online", latency);

    } catch (error) {
      consecutiveFailures += 1;
      setConnection("offline");

      if (error.name === "AbortError") {
        console.warn(`NOVIA HOME API timeout (${API_TIMEOUT} ms)`);
      } else {
        console.error("NOVIA HOME status:", error);
      }

    } finally {
      window.clearTimeout(timeoutId);
      refreshing = false;
      requestInFlight = false;
      if (refreshButton) refreshButton.disabled = false;
      const retryDelay = consecutiveFailures === 0
        ? API_CHECK_INTERVAL
        : Math.min(API_CHECK_INTERVAL * (2 ** Math.min(consecutiveFailures, 3)), 30000);
      scheduleNextCheck(retryDelay);
    }
  }

  async function controlDevice(channel, value, button) {
    if (!controlPin) {
      const enteredPin = window.prompt(
        "Masukkan PIN kontrol NOVIA HOME:"
      );

      if (!enteredPin) return;
      controlPin = enteredPin;
    }

    const label = button.querySelector(".toggle-label");
    const previousLabel = label ? label.textContent : "";

    button.disabled = true;
    if (label) label.textContent = "MENGIRIM...";

    try {
      const response = await fetch(`${API_URL}/api/control`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json"
        },
        body: JSON.stringify({
          pin: controlPin,
          channel,
          value
        })
      });

      const result = await response.json().catch(() => ({}));

      if (response.status === 401) {
        controlPin = null;
        throw new Error("PIN kontrol salah.");
      }

      if (!response.ok || !result.success) {
        throw new Error(result.error || "Perintah gagal");
      }

      if (statusElement) {
        statusElement.textContent = "MENYINKRONKAN...";
      }

      await loadStatus();

    } catch (error) {
      if (label) label.textContent = previousLabel;

      window.alert(
        `Gagal mengontrol ${channelNames[channel]}: ${error.message}`
      );

      console.error("NOVIA HOME control:", error);
      await loadStatus();

    } finally {
      button.disabled = false;
    }
  }

  cards.forEach(card => {
    const button = card.querySelector(".toggle-button");
    if (!button) return;

    const channel = Number(card.dataset.channel);

    if (!Number.isInteger(channel) || channel < 0 || channel > 3) {
      return;
    }

    button.addEventListener("click", () => {
      if (button.disabled) return;

      const currentState =
        button.getAttribute("aria-pressed") === "true";

      controlDevice(channel, currentState ? 0 : 1, button);
    });
  });

  setConnection("checking");
  loadStatus();

  if (refreshButton) {
    refreshButton.addEventListener("click", () => {
      if (nextCheckTimer) window.clearTimeout(nextCheckTimer);
      nextCheckTimer = null;
      loadStatus();
    });
  }

  function recordBrowserNetworkState(isOnline) {
    const state = isOnline ? "online" : "offline";
    if (state === previousBrowserState) return;
    if (previousBrowserState !== null) {
      recordConnectionEvent(
        "Browser", state,
        isOnline ? "Jaringan browser pulih" : "Jaringan browser terputus",
        isOnline ? "Perangkat kembali memiliki koneksi jaringan." : "Browser mendeteksi perangkat tidak memiliki koneksi jaringan."
      );
    }
    previousBrowserState = state;
  }

  recordBrowserNetworkState(navigator.onLine);

  window.addEventListener("online", () => {
    recordBrowserNetworkState(true);
    updateReliability(lastSuccessfulCheck ? "online" : "checking");
    if (nextCheckTimer) window.clearTimeout(nextCheckTimer);
    nextCheckTimer = null;
    loadStatus();
  });

  window.addEventListener("offline", () => {
    recordBrowserNetworkState(false);
    // Jaringan browser putus tidak otomatis membuktikan API server ikut offline.
    setConnection("offline", null, false);
  });

  document.addEventListener("visibilitychange", () => {
    if (document.visibilityState === "visible" && !requestInFlight) {
      if (nextCheckTimer) window.clearTimeout(nextCheckTimer);
      nextCheckTimer = null;
      loadStatus();
    }
  });

  // Polling adaptif: 5 detik saat sehat, backoff sampai 30 detik saat gagal.

  console.log(
    "NOVIA HOME V5 - Reliability Monitor + Connection History active",
    `interval=${API_CHECK_INTERVAL}ms`,
    `timeout=${API_TIMEOUT}ms`
  );
});


// NOVIA HOME V5: navigasi tab utama
document.addEventListener("DOMContentLoaded", () => {
  const tabs = Array.from(document.querySelectorAll(".main-tab[data-tab]"));
  const panels = Array.from(document.querySelectorAll(".tab-panel"));
  if (!tabs.length || !panels.length) return;

  function activateTab(tab, updateHash = true) {
    const targetId = tab.dataset.tab;
    tabs.forEach(item => {
      const active = item === tab;
      item.classList.toggle("is-active", active);
      item.setAttribute("aria-selected", String(active));
      item.tabIndex = active ? 0 : -1;
    });
    panels.forEach(panel => {
      const active = panel.id === targetId;
      panel.hidden = !active;
      panel.classList.toggle("is-active", active);
    });
    if (updateHash) history.replaceState(null, "", `#${targetId}`);
  }

  tabs.forEach((tab, index) => {
    tab.addEventListener("click", () => activateTab(tab));
    tab.addEventListener("keydown", event => {
      if (event.key !== "ArrowRight" && event.key !== "ArrowLeft") return;
      event.preventDefault();
      const direction = event.key === "ArrowRight" ? 1 : -1;
      const next = tabs[(index + direction + tabs.length) % tabs.length];
      activateTab(next);
      next.focus();
    });
  });

  const requested = document.querySelector(`.main-tab[data-tab="${location.hash.slice(1)}"]`);
  if (requested) activateTab(requested, false);
  else activateTab(tabs[0], false);
});
