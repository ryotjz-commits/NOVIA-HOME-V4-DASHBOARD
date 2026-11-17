
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
  // END THEME SWITCHER
  // ==========================================
  

  const statusElement = document.getElementById("connectionStatus");
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

  function setConnection(connected) {
    if (statusElement) {
      statusElement.textContent = connected ? "API CONNECTED" : "API OFFLINE";
    }

    if (summaryCards[0]) {
      const strong = summaryCards[0].querySelector("strong");
      const small = summaryCards[0].querySelector(".summary-note");
      if (strong) strong.textContent = connected ? "Terhubung" : "Tidak terhubung";
      if (small) small.textContent = connected
        ? "Cloudflare Worker aktif"
        : "Periksa koneksi API";
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
      if (value) value.textContent = connected
        ? "API tersambung · Data terbaca"
        : "Koneksi API gagal";
      if (value) value.classList.remove("pending");
    }

    if (note) {
      note.textContent = connected
        ? "Status channel disinkronkan dengan Blynk IoT."
        : "Gagal memperbarui status. Periksa koneksi internet dan API.";
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
    if (refreshing) return;

    refreshing = true;

    try {
      const response = await fetch(`${API_URL}/api/status`, {
        method: "GET",
        cache: "no-store"
      });

      if (!response.ok) {
        throw new Error(`HTTP ${response.status}`);
      }

      const data = await response.json();

      if (!data.success || !data.channels) {
        throw new Error("Respons API tidak valid");
      }

      const values = [0, 1, 2, 3].map(index =>
        Number(data.channels[`V${index}`])
      );

      if (values.some(value => value !== 0 && value !== 1)) {
        throw new Error("Nilai datastream tidak valid");
      }

      values.forEach((value, index) => updateCard(index, value));
      setConnection(true);

    } catch (error) {
      setConnection(false);
      console.error("NOVIA HOME status:", error);

    } finally {
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

  if (statusElement) {
    statusElement.textContent = "MENGHUBUNGKAN...";
  }

  if (note) {
    note.textContent = "Menghubungkan dashboard dengan Blynk IoT...";
  }

  loadStatus();

  window.setInterval(loadStatus, 5000);

  console.log("NOVIA HOME V4 - Live integration active");
});
    
