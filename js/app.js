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

  function formatClock(date) {
    if (!date) return "Belum pernah";
    return date.toLocaleTimeString("id-ID", {
      hour: "2-digit",
      minute: "2-digit",
      second: "2-digit",
      hour12: false
    });
  }

  function setConnection(state, latency = null) {
    const connected = state === "online";
    const checking = state === "checking";

    if (connected) {
      lastSuccessfulCheck = new Date();
      lastLatency = latency;
    }

    const lastSeenText = formatClock(lastSuccessfulCheck);
    const latencyText = lastLatency === null
      ? "--"
      : `${lastLatency} ms`;

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
    if (refreshing) return;

    refreshing = true;
    setConnection("checking");

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
      setConnection("online", latency);

    } catch (error) {
      setConnection("offline");

      if (error.name === "AbortError") {
        console.warn(`NOVIA HOME API timeout (${API_TIMEOUT} ms)`);
      } else {
        console.error("NOVIA HOME status:", error);
      }

    } finally {
      window.clearTimeout(timeoutId);
      refreshing = false;
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

  // Monitoring otomatis: cek ulang setiap 5 detik.
  window.setInterval(loadStatus, API_CHECK_INTERVAL);

  console.log(
    "NOVIA HOME V4 - API Real-Time Monitor active",
    `interval=${API_CHECK_INTERVAL}ms`,
    `timeout=${API_TIMEOUT}ms`
  );
});
