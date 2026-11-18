
/* =========================================
   NOVIA HOME V4 - AUTOMATION ENGINE
   Worker API + D1 Database
   Live Countdown Integration
========================================= */

(() => {
  "use strict";

  // =====================================
  // CONFIGURATION
  // =====================================

  const API_URL =
    "https://novia-home-api.ryo-tjz.workers.dev";

  const DEVICE_NAMES = [
    "Lampu Depan",
    "Lampu Tengah",
    "Lampu Kamar",
    "Lampu Belakang"
  ];

  const DAY_NAMES = [
    "Min", "Sen", "Sel", "Rab",
    "Kam", "Jum", "Sab"
  ];

  const $ = id => document.getElementById(id);

  let activeTimers = [];
  let serverTimeOffset = 0;

  let countdownInterval = null;
  let timerSyncInterval = null;

  const TIMER_STORAGE_KEY =
    "novia_home_timer_durations";


  // =====================================
  // STATUS
  // =====================================

  function setStatus(id, message, type = "") {
    const element = $(id);

    if (!element) return;

    element.textContent = message;
    element.className =
      "automation-status " + type;
  }


  // =====================================
  // PIN
  // =====================================

  function getPin() {
    const pin = window.prompt(
      "Masukkan PIN kontrol NOVIA HOME:"
    );

    if (pin === null || pin.trim() === "") {
      return null;
    }

    return pin.trim();
  }


  // =====================================
  // API REQUEST
  // =====================================

  async function request(path, options = {}) {
    const response = await fetch(
      API_URL + path,
      {
        cache: "no-store",
        ...options
      }
    );

    let data;

    try {
      data = await response.json();
    } catch {
      throw new Error(
        "Respons Worker tidak valid"
      );
    }

    if (
      !response.ok ||
      data.success !== true
    ) {
      throw new Error(
        data.error || "Permintaan gagal"
      );
    }

    return data;
  }


  // =====================================
  // POST
  // =====================================

  function post(path, body) {
    return request(path, {
      method: "POST",

      headers: {
        "Content-Type": "application/json"
      },

      body: JSON.stringify(body)
    });
  }


  // =====================================
  // BUTTON STATE
  // =====================================

  function setBusy(button, busy) {
    if (!button) return;

    button.disabled = busy;
  }


  // =====================================
  // TIMER STORAGE
  // =====================================

  function getTimerStorage() {
    try {
      return JSON.parse(
        localStorage.getItem(TIMER_STORAGE_KEY) || "{}"
      );
    } catch {
      return {};
    }
  }


  function saveTimerDuration(channel, deadline, duration) {
    const storage = getTimerStorage();

    storage[channel] = {
      deadline_at: deadline,
      duration_seconds: duration
    };

    try {
      localStorage.setItem(
        TIMER_STORAGE_KEY,
        JSON.stringify(storage)
      );
    } catch (error) {
      console.warn("Timer storage:", error);
    }
  }


  function removeTimerDuration(channel) {
    const storage = getTimerStorage();

    delete storage[channel];

    try {
      localStorage.setItem(
        TIMER_STORAGE_KEY,
        JSON.stringify(storage)
      );
    } catch (error) {
      console.warn("Timer storage:", error);
    }
  }


  // =====================================
  // COUNTDOWN FORMAT
  // =====================================

  function formatCountdown(seconds) {
    const safe = Math.max(
      0,
      Math.floor(seconds)
    );

    const hours = Math.floor(safe / 3600);

    const minutes = Math.floor(
      (safe % 3600) / 60
    );

    const secs = safe % 60;

    if (hours > 0) {
      return [
        String(hours).padStart(2, "0"),
        String(minutes).padStart(2, "0"),
        String(secs).padStart(2, "0")
      ].join(":");
    }

    return [
      String(minutes).padStart(2, "0"),
      String(secs).padStart(2, "0")
    ].join(":");
  }


  // =====================================
  // LIVE COUNTDOWN ENGINE
  // =====================================

  function renderTimerCountdown() {
    const panel = $("timerCountdown");

    if (!panel) return;

    const channel = Number(
      $("timerChannel")?.value
    );

    const timer = activeTimers.find(
      item => item.channel === channel
    );

    if (!timer) {
      panel.hidden = true;
      return;
    }

    panel.hidden = false;

    const now = Math.floor(
      (Date.now() + serverTimeOffset) / 1000
    );

    const remaining = Math.max(
      0,
      timer.deadline_at - now
    );

    const duration = Math.max(
      0,
      timer.duration_seconds
    );

    const progress = duration > 0
      ? Math.min(
          100,
          Math.max(0, remaining / duration * 100)
        )
      : null;

    const value = $("timerCountdownValue");
    const badge = $("timerCountdownBadge");
    const device = $("timerDeviceName");
    const label = $("timerRemainingLabel");
    const bar = $("timerProgressBar");

    if (value) {
      value.textContent =
        formatCountdown(remaining);
    }

    if (badge) {
      badge.textContent =
        remaining > 0
          ? "AKTIF"
          : "WAKTU HABIS";

      badge.className =
        remaining > 0
          ? "timer-countdown-badge active"
          : "timer-countdown-badge expired";
    }

    if (device) {
      device.textContent =
        DEVICE_NAMES[channel] || "Perangkat";
    }

    if (label) {
      label.textContent =
        remaining > 0
          ? "Perangkat akan mati otomatis"
          : "Menunggu konfirmasi OFF dari Worker";
    }

    if (bar) {
      if (progress === null) {
        bar.style.width = "0%";
      } else {
        bar.style.width = progress + "%";
      }
    }
  }


  // =====================================
  // LOAD ACTIVE TIMERS FROM WORKER
  // =====================================

  async function loadActiveTimers() {
    try {
      const data = await request(
        "/api/timers"
      );

      if (!Array.isArray(data.timers)) {
        throw new Error(
          "Format data timer tidak valid"
        );
      }

      const serverNow = Number(
        data.server_now
      );

      if (!Number.isFinite(serverNow)) {
        throw new Error(
          "Waktu server tidak valid"
        );
      }

      serverTimeOffset =
        serverNow * 1000 - Date.now();

      const storage = getTimerStorage();

      const nextTimers = data.timers.map(timer => {
        const channel = Number(timer.channel);
        const deadline = Number(timer.deadline_at);

        const saved = storage[channel];

        const duration =
          saved &&
          Number(saved.deadline_at) === deadline
            ? Number(saved.duration_seconds)
            : 0;

        return {
          channel,
          deadline_at: deadline,
          duration_seconds:
            Number.isFinite(duration) ? duration : 0
        };
      });

      activeTimers = nextTimers.filter(timer =>
        Number.isInteger(timer.channel) &&
        timer.channel >= 0 &&
        timer.channel <= 3 &&
        Number.isFinite(timer.deadline_at)
      );

      // Bersihkan data durasi yang sudah tidak aktif.
      Object.keys(storage).forEach(key => {
        const exists = activeTimers.some(
          timer =>
            timer.channel === Number(key) &&
            timer.deadline_at ===
              Number(storage[key].deadline_at)
        );

        if (!exists) {
          removeTimerDuration(Number(key));
        }
      });

      renderTimerCountdown();

    } catch (error) {
      console.warn(
        "NOVIA HOME timer synchronization:",
        error
      );
    }
  }


  // =====================================
  // TIMER
  // =====================================

  async function startTimer() {
    const button = $("startTimer");

    const channel = Number(
      $("timerChannel")?.value
    );

    const minutes = Number(
      $("timerMinutes")?.value
    );

    if (
      !Number.isInteger(channel) ||
      channel < 0 ||
      channel > 3 ||
      !Number.isInteger(minutes) ||
      minutes < 1 ||
      minutes > 10080
    ) {
      setStatus(
        "timerStatus",
        "Pilihan timer tidak valid. Maksimal 7 hari.",
        "error"
      );

      return;
    }

    const pin = getPin();

    if (pin === null) return;

    setBusy(button, true);

    let deviceTurnedOn = false;

    try {
      // 1. Nyalakan perangkat.
      await post("/api/control", {
        pin,
        channel,
        value: 1
      });

      deviceTurnedOn = true;

      // 2. Pasang timer.
      const result = await post("/api/timer", {
        pin,
        channel,
        minutes
      });

      // 3. Simpan durasi untuk progress bar.
      if (Number.isFinite(Number(result.deadline_at))) {
        saveTimerDuration(
          channel,
          Number(result.deadline_at),
          minutes * 60
        );
      }

      // 4. Sinkronisasi countdown.
      await loadActiveTimers();

      setStatus(
        "timerStatus",
        DEVICE_NAMES[channel] +
        " berhasil dinyalakan. Timer OFF dalam " +
        minutes + " menit.",
        "success"
      );

    } catch (error) {

      let message = error.message;

      // Jika pemasangan timer gagal setelah ON,
      // coba matikan kembali perangkat.
      if (deviceTurnedOn) {
        try {
          await post("/api/control", {
            pin,
            channel,
            value: 0
          });

          removeTimerDuration(channel);

          await loadActiveTimers();

          message +=
            ". Perangkat dikembalikan ke OFF.";

        } catch {
          message +=
            ". PERINGATAN: Perangkat mungkin masih ON.";
        }
      }

      setStatus(
        "timerStatus",
        "Gagal mengatur timer: " + message,
        "error"
      );

    } finally {
      setBusy(button, false);
    }
  }


  // =====================================
  // CANCEL TIMER
  // =====================================

  async function cancelTimer() {
    const button = $("cancelTimer");

    const channel = Number(
      $("timerChannel")?.value
    );

    if (
      !Number.isInteger(channel) ||
      channel < 0 ||
      channel > 3
    ) {
      return;
    }

    const pin = getPin();

    if (pin === null) return;

    setBusy(button, true);

    try {
      await post("/api/timer", {
        pin,
        channel,
        minutes: 0
      });

      removeTimerDuration(channel);

      await loadActiveTimers();

      setStatus(
        "timerStatus",
        "Timer " + DEVICE_NAMES[channel] +
        " berhasil dibatalkan. Status perangkat tidak berubah.",
        "success"
      );

    } catch (error) {
      setStatus(
        "timerStatus",
        "Gagal membatalkan timer: " +
        error.message,
        "error"
      );

    } finally {
      setBusy(button, false);
    }
  }


  // =====================================
  // DAILY SCHEDULE
  // =====================================

  function selectedWeekdays() {
    const boxes = document.querySelectorAll(
      ".weekday-grid input[type='checkbox']"
    );

    let mask = 0;

    boxes.forEach(box => {
      if (box.checked) {
        mask |= Number(box.value);
      }
    });

    return mask;
  }


  // =====================================
  // SAVE SCHEDULE
  // =====================================

  async function saveSchedule() {
    const button = $("saveSchedule");

    const channel = Number(
      $("scheduleChannel")?.value
    );

    const onTime = $("scheduleOn")?.value;
    const offTime = $("scheduleOff")?.value;

    const enabled = $("scheduleEnabled")?.checked
      ? 1
      : 0;

    const weekdayMask = selectedWeekdays();

    if (
      !Number.isInteger(channel) ||
      channel < 0 ||
      channel > 3 ||
      !onTime ||
      !offTime ||
      onTime === offTime ||
      weekdayMask < 1
    ) {
      setStatus(
        "scheduleStatus",
        "Periksa perangkat, jam ON/OFF dan hari aktif.",
        "error"
      );

      return;
    }

    const pin = getPin();

    if (pin === null) return;

    setBusy(button, true);

    try {
      await post("/api/schedule", {
        pin,
        channel,
        name: DEVICE_NAMES[channel],
        enabled,
        on_time: onTime,
        off_time: offTime,
        weekday_mask: weekdayMask
      });

      setStatus(
        "scheduleStatus",
        "Jadwal " + DEVICE_NAMES[channel] +
        " berhasil disimpan.",
        "success"
      );

      await loadSchedules();

    } catch (error) {
      setStatus(
        "scheduleStatus",
        "Gagal menyimpan jadwal: " +
        error.message,
        "error"
      );

    } finally {
      setBusy(button, false);
    }
  }


  // =====================================
  // LOAD SAVED SCHEDULES
  // =====================================

  async function loadSchedules() {
    const container = $("scheduleList");

    if (!container) return;

    try {
      const data = await request(
        "/api/schedules"
      );

      container.replaceChildren();

      if (
        !Array.isArray(data.schedules) ||
        data.schedules.length === 0
      ) {
        const empty = document.createElement("div");

        empty.className = "schedule-empty";

        empty.textContent =
          "Belum ada jadwal tersimpan.";

        container.appendChild(empty);

        return;
      }

      data.schedules.forEach(schedule => {
        const card = document.createElement("div");

        card.className = "schedule-item";

        const top = document.createElement("div");

        top.className = "schedule-item-top";

        const name = document.createElement("span");

        name.className = "schedule-item-name";

        const channel = Number(schedule.channel);

        name.textContent =
          DEVICE_NAMES[channel] || schedule.name;

        const state = document.createElement("span");

        const enabled =
          Number(schedule.enabled) === 1;

        state.className =
          "schedule-state" +
          (enabled ? "" : " off");

        state.textContent =
          enabled ? "AKTIF" : "NONAKTIF";

        top.append(name, state);

        const time = document.createElement("div");

        time.className = "schedule-item-time";

        time.textContent =
          schedule.on_time + " ON  →  " +
          schedule.off_time + " OFF";

        const days = document.createElement("div");

        days.className = "schedule-item-days";

        const mask = Number(
          schedule.weekday_mask
        );

        days.textContent = DAY_NAMES.filter(
          (_, index) => mask & (1 << index)
        ).join(" · ");

        card.append(top, time, days);

        container.appendChild(card);
      });

    } catch (error) {
      container.textContent =
        "Gagal memuat jadwal: " + error.message;
    }
  }


  // =====================================
  // INITIALIZE
  // =====================================

  function initialize() {
    if (!$("automation")) return;

    $("startTimer")?.addEventListener(
      "click",
      startTimer
    );

    $("cancelTimer")?.addEventListener(
      "click",
      cancelTimer
    );

    $("saveSchedule")?.addEventListener(
      "click",
      saveSchedule
    );

    $("timerChannel")?.addEventListener(
      "change",
      renderTimerCountdown
    );

    // Ambil data awal.
    loadActiveTimers();

    loadSchedules();

    // Countdown berjalan setiap detik.
    countdownInterval = setInterval(
      renderTimerCountdown,
      1000
    );

    // Sinkronisasi ulang dengan Worker.
    timerSyncInterval = setInterval(
      loadActiveTimers,
      10000
    );
  }


  // =====================================
  // DOM READY
  // =====================================

  if (document.readyState === "loading") {
    document.addEventListener(
      "DOMContentLoaded",
      initialize,
      { once: true }
    );

  } else {
    initialize();
  }

})();
           
