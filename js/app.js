
/*
  =========================================
  NOVIA HOME V4
  LIVE BLYNK INTEGRATION
  =========================================
  Dashboard : GitHub Pages
  API       : Cloudflare Worker
  Device    : ESP32 NOVIA HOME V3
  Datastream: V0 - V3

  Firmware V3 tidak diubah.
*/

document.addEventListener("DOMContentLoaded", () => {

  const API_URL =
    "https://novia-home-api.ryo-tjz.workers.dev";

  const statusElement =
    document.getElementById("connectionStatus");

  const cards =
    document.querySelectorAll(".device-card");

  const demoNote =
    document.querySelector(".demo-note");

  let controlPin = null;
  let refreshing = false;

  const channelNames = [
    "Lampu Depan",
    "Lampu Tengah",
    "Lampu Kamar",
    "Lampu Belakang"
  ];

  // =====================================
  // UPDATE TAMPILAN
  // =====================================

  function updateCard(channel, value) {

    const card = document.querySelector(
      `.device-card[data-channel="${channel}"]`
    );

    if (!card) return;

    const button =
      card.querySelector(".toggle-button");

    const badge =
      card.querySelector(".state-badge");

    const label =
      card.querySelector(".toggle-label");

    const isOn = value === 1;

    card.classList.toggle("is-on", isOn);

    if (button) {
      button.setAttribute(
        "aria-pressed",
        String(isOn)
      );
      button.disabled = false;
    }

    if (badge) {
      badge.textContent = isOn ? "ONLINE · ON" : "ONLINE · OFF";
    }

    if (label) {
      label.textContent = isOn ? "MATIKAN" : "NYALAKAN";
    }
  }

  // =====================================
  // BACA STATUS BLYNK
  // =====================================

  async function loadStatus() {

    if (refreshing) return;

    refreshing = true;

    try {

      const response = await fetch(
        `${API_URL}/api/status`,
        {
          method: "GET",
          cache: "no-store"
        }
      );

      if (!response.ok) {
        throw new Error("API status failed");
      }

      const data = await response.json();

      if (!data.success || !data.channels) {
        throw new Error("Invalid API response");
      }

      for (let i = 0; i < 4; i++) {

        const value = Number(
          data.channels[`V${i}`]
        );

        if (value !== 0 && value !== 1) {
          throw new Error("Invalid channel value");
        }

        updateCard(i, value);
      }

      if (statusElement) {
        statusElement.textContent = "CONNECTED";
      }

      if (demoNote) {
        demoNote.textContent =
          "Status perangkat diperbarui melalui Blynk IoT.";
      }

    } catch (error) {

      if (statusElement) {
        statusElement.textContent = "CONNECTION ERROR";
      }

      if (demoNote) {
        demoNote.textContent =
          "Gagal membaca status perangkat. Periksa koneksi API.";
      }

      console.error("NOVIA HOME:", error);

    } finally {
      refreshing = false;
    }
  }

  // =====================================
  // KIRIM PERINTAH KE WORKER
  // =====================================

  async function controlDevice(channel, value) {

    if (!controlPin) {

      controlPin = window.prompt(
        "Masukkan PIN kontrol NOVIA HOME:"
      );

      if (!controlPin) {
        controlPin = null;
        return;
      }
    }

    const card = document.querySelector(
      `.device-card[data-channel="${channel}"]`
    );

    const button =
      card?.querySelector(".toggle-button");

    if (!button) return;

    button.disabled = true;

    try {

      const response = await fetch(
        `${API_URL}/api/control`,
        {
          method: "POST",

          headers: {
            "Content-Type": "application/json"
          },

          body: JSON.stringify({
            pin: controlPin,
            channel,
            value
          })
        }
      );

      const result = await response.json();

      if (response.status === 401) {
        controlPin = null;
        throw new Error("PIN kontrol tidak valid");
      }

      if (!response.ok || !result.success) {
        throw new Error(
          result.error || "Perintah gagal"
        );
      }

      if (statusElement) {
        statusElement.textContent = "UPDATING";
      }

      // Baca kembali nilai aktual dari Blynk
      await loadStatus();

    } catch (error) {

      alert(
        `Gagal mengontrol ${channelNames[channel]}:\n${error.message}`
      );

      console.error("NOVIA CONTROL:", error);

      await loadStatus();

    } finally {

      button.disabled = false;

    }
  }

  // =====================================
  // EVENT TOMBOL
  // =====================================

  cards.forEach((card) => {

    const button =
      card.querySelector(".toggle-button");

    if (!button) return;

    const channel = Number(
      card.dataset.channel
    );

    if (
      !Number.isInteger(channel) ||
      channel < 0 ||
      channel > 3
    ) return;

    button.addEventListener("click", () => {

      if (button.disabled) return;

      const currentState =
        button.getAttribute("aria-pressed") === "true";

      const nextValue = currentState ? 0 : 1;

      controlDevice(channel, nextValue);

    });

  });

  // =====================================
  // START INTEGRATION
  // =====================================

  if (statusElement) {
    statusElement.textContent = "CONNECTING...";
  }

  loadStatus();

  // Sinkronisasi otomatis setiap 5 detik
  setInterval(loadStatus, 5000);

  console.log(
    "NOVIA HOME V4 - Blynk integration initialized"
  );

});
