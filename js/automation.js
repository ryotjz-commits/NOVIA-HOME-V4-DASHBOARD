
/* =========================================
   NOVIA HOME V4 - AUTOMATION ENGINE
   Worker API + D1 Database
========================================= */

(() => {
  "use strict";

  const API_URL =
    "https://novia-home-api.ryo-tjz.workers.dev";

  const DEVICE_NAMES = [
    "Lampu Depan",
    "Lampu Tengah",
    "Lampu Kamar",
    "Kipas Angin"
  ];

  const DAY_NAMES = [
    "Min", "Sen", "Sel", "Rab",
    "Kam", "Jum", "Sab"
  ];

  const $ = id => document.getElementById(id);

  function setStatus(id, message, type = "") {
    const element = $(id);
    if (!element) return;

    element.textContent = message;
    element.className = "automation-status " + type;
  }

  function getPin() {
    return window.prompt(
      "Masukkan PIN kontrol NOVIA HOME:"
    );
  }

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
      throw new Error("Respons Worker tidak valid");
    }

    if (!response.ok || data.success !== true) {
      throw new Error(
        data.error || "Permintaan gagal"
      );
    }

    return data;
  }

  function post(path, body) {
    return request(path, {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(body)
    });
  }

  function setBusy(button, busy) {
    if (!button) return;

    button.disabled = busy;
  }

  // =====================================
  // TIMER
  // =====================================

  async function startTimer() {
    const button = $("startTimer");

    const channel = Number(
      $("timerChannel").value
    );

    const minutes = Number(
      $("timerMinutes").value
    );

    if (
      !Number.isInteger(channel) ||
      channel < 0 ||
      channel > 3 ||
      !Number.isInteger(minutes) ||
      minutes < 1
    ) {
      setStatus(
        "timerStatus",
        "Pilihan timer tidak valid.",
        "error"
      );
      return;
    }

    const pin = getPin();

    if (pin === null || pin.trim() === "") {
      return;
    }

    setBusy(button, true);

    try {
      // Nyalakan perangkat terlebih dahulu.
      await post("/api/control", {
        pin,
        channel,
        value: 1
      });

      // Pasang timer setelah kontrol ON berhasil.
      await post("/api/timer", {
        pin,
        channel,
        minutes
      });

      setStatus(
        "timerStatus",
        DEVICE_NAMES[channel] +
        " berhasil dinyalakan. Timer OFF dalam " +
        minutes + " menit.",
        "success"
      );

    } catch (error) {
      setStatus(
        "timerStatus",
        "Gagal mengatur timer: " + error.message,
        "error"
      );
    } finally {
      setBusy(button, false);
    }
  }

  async function cancelTimer() {
    const button = $("cancelTimer");

    const channel = Number(
      $("timerChannel").value
    );

    const pin = getPin();

    if (pin === null || pin.trim() === "") {
      return;
    }

    setBusy(button, true);

    try {
      await post("/api/timer", {
        pin,
        channel,
        minutes: 0
      });

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

  async function saveSchedule() {
    const button = $("saveSchedule");

    const channel = Number(
      $("scheduleChannel").value
    );

    const onTime = $("scheduleOn").value;
    const offTime = $("scheduleOff").value;

    const enabled = $(
      "scheduleEnabled"
    ).checked ? 1 : 0;

    const weekdayMask = selectedWeekdays();

    if (
      !onTime ||
      !offTime ||
      onTime === offTime ||
      weekdayMask < 1
    ) {
      setStatus(
        "scheduleStatus",
        "Periksa jam ON/OFF dan hari aktif.",
        "error"
      );
      return;
    }

    const pin = getPin();

    if (pin === null || pin.trim() === "") {
      return;
    }

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
      const data = await request("/api/schedules");

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

        state.className =
          "schedule-state" +
          (Number(schedule.enabled) === 1 ? "" : " off");

        state.textContent =
          Number(schedule.enabled) === 1
            ? "AKTIF"
            : "NONAKTIF";

        top.append(name, state);

        const time = document.createElement("div");
        time.className = "schedule-item-time";

        time.textContent =
          schedule.on_time + " ON  →  " +
          schedule.off_time + " OFF";

        const days = document.createElement("div");
        days.className = "schedule-item-days";

        const mask = Number(schedule.weekday_mask);

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

    loadSchedules();
  }

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
        
