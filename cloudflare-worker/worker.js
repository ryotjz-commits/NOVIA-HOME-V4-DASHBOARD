const ALLOWED_ORIGIN = "https://ryotjz-commits.github.io";
const CHANNEL_PINS = ["V0", "V1", "V2", "V3"];
const CHANNEL_NAMES = ["Lampu Depan", "Lampu Tengah", "Lampu Kamar", "Kipas Angin"];
const MAX_TIMER_MINUTES = 10080;
const BLYNK_TIMEOUT_MS = 8000;

function json(data, status, corsHeaders) {
  return Response.json(data, {
    status,
    headers: { ...corsHeaders, "Cache-Control": "no-store" }
  });
}

function corsFor(request) {
  const origin = request.headers.get("Origin");
  if (origin !== ALLOWED_ORIGIN) return null;

  return {
    "Access-Control-Allow-Origin": ALLOWED_ORIGIN,
    "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
    "Access-Control-Allow-Headers": "Content-Type",
    "Access-Control-Max-Age": "86400",
    "Vary": "Origin"
  };
}

function validChannel(value) {
  return Number.isInteger(value) && value >= 0 && value < CHANNEL_PINS.length;
}

function validTime(value) {
  return typeof value === "string" && /^([01]\d|2[0-3]):[0-5]\d$/.test(value);
}

async function readJson(request) {
  if (!request.headers.get("Content-Type")?.toLowerCase().includes("application/json")) {
    throw new Error("Content-Type harus application/json");
  }
  try {
    return await request.json();
  } catch {
    throw new Error("Body JSON tidak valid");
  }
}

function requirePin(body, env) {
  if (!env.CONTROL_PIN) throw new Error("Secret CONTROL_PIN belum dikonfigurasi");
  if (typeof body.pin !== "string" || body.pin !== env.CONTROL_PIN) {
    const error = new Error("PIN kontrol salah");
    error.status = 401;
    throw error;
  }
}

function requireDatabase(env) {
  if (!env.DB) {
    const error = new Error("Binding D1 DB belum dikonfigurasi");
    error.status = 503;
    throw error;
  }
  return env.DB;
}

function blynkBase(env) {
  if (!env.BLYNK_AUTH_TOKEN || !env.BLYNK_BASE_URL) {
    const error = new Error("Konfigurasi Blynk Worker belum lengkap");
    error.status = 500;
    throw error;
  }
  try {
    const base = new URL(env.BLYNK_BASE_URL);
    if (base.protocol !== "https:") throw new Error();
    return base;
  } catch {
    const error = new Error("BLYNK_BASE_URL harus URL HTTPS yang valid");
    error.status = 500;
    throw error;
  }
}

async function blynkRequest(env, endpoint, params = {}) {
  const url = new URL(endpoint, blynkBase(env));
  url.searchParams.set("token", env.BLYNK_AUTH_TOKEN);
  for (const [key, value] of Object.entries(params)) {
    url.searchParams.set(key, String(value));
  }

  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), BLYNK_TIMEOUT_MS);
  try {
    const response = await fetch(url, { signal: controller.signal });
    const text = await response.text();
    if (!response.ok) throw new Error("Blynk menolak permintaan");
    return text.trim();
  } catch (error) {
    if (error?.name === "AbortError") throw new Error("Blynk timeout");
    throw error;
  } finally {
    clearTimeout(timeout);
  }
}

async function setRelay(env, channel, value) {
  const result = await blynkRequest(env, "/external/api/update", {
    [CHANNEL_PINS[channel]]: value
  });
  // Blynk biasanya mengembalikan "" atau "1" pada update sukses.
  if (result.toLowerCase() === "false" || result.toLowerCase() === "error") {
    throw new Error("Blynk gagal memperbarui relay");
  }
}

async function getStatus(env) {
  const pins = CHANNEL_PINS.map(pin => blynkRequest(env, "/external/api/get", { [pin]: "" }));
  const [hardwareResult, ...values] = await Promise.all([
    blynkRequest(env, "/external/api/isHardwareConnected"),
    ...pins
  ]);
  const hardware = hardwareResult.toLowerCase();
  if (hardware !== "true" && hardware !== "false") {
    throw new Error("Respons status hardware Blynk tidak valid");
  }

  const channels = {};
  values.forEach((value, index) => {
    const numeric = Number(value);
    if (numeric !== 0 && numeric !== 1) throw new Error("Nilai datastream Blynk tidak valid");
    channels[CHANNEL_PINS[index]] = numeric;
  });
  return { success: true, device: "Novia Home", deviceOnline: hardware === "true", channels };
}

async function handleControl(request, env, corsHeaders) {
  const body = await readJson(request);
  requirePin(body, env);
  if (!validChannel(body.channel) || (body.value !== 0 && body.value !== 1)) {
    return json({ success: false, error: "Channel atau nilai relay tidak valid" }, 400, corsHeaders);
  }

  await setRelay(env, body.channel, body.value);
  // Perintah OFF manual membatalkan timer agar tidak ada timer lama yang tertinggal.
  if (body.value === 0 && env.DB) {
    await env.DB.prepare("DELETE FROM timers WHERE channel = ?1").bind(body.channel).run();
  }
  return json({ success: true, channel: body.channel, value: body.value }, 200, corsHeaders);
}

async function handleTimer(request, env, corsHeaders) {
  const body = await readJson(request);
  requirePin(body, env);
  if (!validChannel(body.channel) || !Number.isInteger(body.minutes) || body.minutes < 0 || body.minutes > MAX_TIMER_MINUTES) {
    return json({ success: false, error: "Channel atau durasi timer tidak valid (0–10080 menit)" }, 400, corsHeaders);
  }

  const db = requireDatabase(env);
  if (body.minutes === 0) {
    await db.prepare("DELETE FROM timers WHERE channel = ?1").bind(body.channel).run();
    return json({ success: true, channel: body.channel, deadline_at: null, cancelled: true }, 200, corsHeaders);
  }

  const deadlineAt = Math.floor(Date.now() / 1000) + body.minutes * 60;
  await db.prepare(`
    INSERT INTO timers (channel, deadline_at, locked_until, updated_at)
    VALUES (?1, ?2, 0, ?3)
    ON CONFLICT(channel) DO UPDATE SET
      deadline_at = excluded.deadline_at,
      locked_until = 0,
      updated_at = excluded.updated_at
  `).bind(body.channel, deadlineAt, Math.floor(Date.now() / 1000)).run();

  return json({ success: true, channel: body.channel, deadline_at: deadlineAt }, 200, corsHeaders);
}

async function handleTimers(env, corsHeaders) {
  const db = requireDatabase(env);
  const now = Math.floor(Date.now() / 1000);
  const result = await db.prepare(
    "SELECT channel, deadline_at FROM timers WHERE deadline_at > ?1 ORDER BY channel"
  ).bind(now).all();
  return json({ success: true, server_now: now, timers: result.results || [] }, 200, corsHeaders);
}

async function handleSchedule(request, env, corsHeaders) {
  const body = await readJson(request);
  requirePin(body, env);
  if (!validChannel(body.channel) ||
      (body.enabled !== 0 && body.enabled !== 1) ||
      !validTime(body.on_time) || !validTime(body.off_time) ||
      !Number.isInteger(body.weekday_mask) || body.weekday_mask < 1 || body.weekday_mask > 127) {
    return json({ success: false, error: "Data jadwal tidak valid" }, 400, corsHeaders);
  }

  const db = requireDatabase(env);
  await db.prepare(`
    INSERT INTO schedules (channel, name, enabled, on_time, off_time, weekday_mask, updated_at)
    VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)
    ON CONFLICT(channel) DO UPDATE SET
      name = excluded.name,
      enabled = excluded.enabled,
      on_time = excluded.on_time,
      off_time = excluded.off_time,
      weekday_mask = excluded.weekday_mask,
      updated_at = excluded.updated_at
  `).bind(
    body.channel,
    CHANNEL_NAMES[body.channel],
    body.enabled,
    body.on_time,
    body.off_time,
    body.weekday_mask,
    Math.floor(Date.now() / 1000)
  ).run();

  return json({ success: true, channel: body.channel }, 200, corsHeaders);
}

async function handleSchedules(env, corsHeaders) {
  const db = requireDatabase(env);
  const result = await db.prepare(`
    SELECT channel, name, enabled, on_time, off_time, weekday_mask
    FROM schedules ORDER BY channel
  `).all();
  return json({ success: true, schedules: result.results || [] }, 200, corsHeaders);
}

function localClock(epochSeconds, timeZone) {
  const parts = new Intl.DateTimeFormat("en-CA", {
    timeZone,
    year: "numeric", month: "2-digit", day: "2-digit",
    weekday: "short", hour: "2-digit", minute: "2-digit", hourCycle: "h23"
  }).formatToParts(new Date(epochSeconds * 1000));
  const values = Object.fromEntries(parts.map(part => [part.type, part.value]));
  const weekdayIndex = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"].indexOf(values.weekday);
  return {
    date: `${values.year}-${values.month}-${values.day}`,
    time: `${values.hour}:${values.minute}`,
    weekday: weekdayIndex
  };
}

function previousDate(date) {
  const [year, month, day] = date.split("-").map(Number);
  const previous = new Date(Date.UTC(year, month - 1, day - 1));
  return previous.toISOString().slice(0, 10);
}

async function claimScheduleRun(db, scheduleId, action, date) {
  const result = await db.prepare(`
    INSERT OR IGNORE INTO schedule_runs (schedule_id, action, occurrence_date, created_at)
    VALUES (?1, ?2, ?3, ?4)
  `).bind(scheduleId, action, date, Math.floor(Date.now() / 1000)).run();
  return result.meta?.changes === 1;
}

async function runScheduleAction(db, env, schedule, action, occurrenceDate, value) {
  const claimed = await claimScheduleRun(db, schedule.id, action, occurrenceDate);
  if (!claimed) return;
  try {
    await setRelay(env, Number(schedule.channel), value);
  } catch (error) {
    await db.prepare(`
      DELETE FROM schedule_runs WHERE schedule_id = ?1 AND action = ?2 AND occurrence_date = ?3
    `).bind(schedule.id, action, occurrenceDate).run();
    throw error;
  }
}

async function processTimers(db, env, now) {
  const expired = await db.prepare(`
    SELECT channel, deadline_at FROM timers
    WHERE deadline_at <= ?1 AND locked_until <= ?1
    ORDER BY deadline_at LIMIT 50
  `).bind(now).all();

  for (const timer of expired.results || []) {
    const channel = Number(timer.channel);
    const deadline = Number(timer.deadline_at);
    const claim = await db.prepare(`
      UPDATE timers SET locked_until = ?1
      WHERE channel = ?2 AND deadline_at = ?3 AND deadline_at <= ?4 AND locked_until <= ?4
    `).bind(now + 30, channel, deadline, now).run();
    if (claim.meta?.changes !== 1) continue;

    try {
      // Do not execute an expired timer if it was replaced while being claimed.
      const current = await db.prepare(
        "SELECT deadline_at FROM timers WHERE channel = ?1"
      ).bind(channel).first();
      if (!current || Number(current.deadline_at) !== deadline) continue;

      await setRelay(env, channel, 0);
      await db.prepare(
        "DELETE FROM timers WHERE channel = ?1 AND deadline_at = ?2"
      ).bind(channel, deadline).run();
    } catch (error) {
      await db.prepare(`
        UPDATE timers SET locked_until = 0
        WHERE channel = ?1 AND deadline_at = ?2
      `).bind(channel, deadline).run();
      console.error("Timer relay OFF gagal", { channel, error: String(error) });
    }
  }
}

async function processSchedules(db, env, now) {
  const timeZone = env.TIME_ZONE || "Asia/Jakarta";
  const current = localClock(now, timeZone);
  const yesterday = localClock(now - 86400, timeZone);
  const schedules = await db.prepare(
    "SELECT id, channel, enabled, on_time, off_time, weekday_mask FROM schedules WHERE enabled = 1"
  ).all();

  for (const schedule of schedules.results || []) {
    const mask = Number(schedule.weekday_mask);
    const channel = Number(schedule.channel);
    try {
      if (schedule.on_time === current.time && (mask & (1 << current.weekday))) {
        await runScheduleAction(db, env, schedule, "on", current.date, 1);
      }

      // For an overnight schedule (e.g. 22:00–06:00), its OFF day is
      // associated with the weekday on which that schedule started.
      const overnight = schedule.off_time <= schedule.on_time;
      const offOccurrence = overnight ? yesterday : current;
      const offDate = overnight ? previousDate(current.date) : current.date;
      const offWeekday = overnight ? yesterday.weekday : current.weekday;
      if (schedule.off_time === current.time && (mask & (1 << offWeekday))) {
        await runScheduleAction(db, env, schedule, "off", offDate, 0);
      }
    } catch (error) {
      console.error("Jadwal relay gagal", { channel, error: String(error) });
    }
  }
}

async function runAutomation(env) {
  const db = requireDatabase(env);
  const now = Math.floor(Date.now() / 1000);
  await processTimers(db, env, now);
  await processSchedules(db, env, now);
}

export default {
  async fetch(request, env) {
    const corsHeaders = corsFor(request);
    if (!corsHeaders) return new Response("Origin not allowed", { status: 403 });
    if (request.method === "OPTIONS") return new Response(null, { status: 204, headers: corsHeaders });

    const url = new URL(request.url);
    try {
      if (url.pathname === "/api/status" && request.method === "GET") {
        return json(await getStatus(env), 200, corsHeaders);
      }
      if (url.pathname === "/api/control" && request.method === "POST") {
        return await handleControl(request, env, corsHeaders);
      }
      if (url.pathname === "/api/timer" && request.method === "POST") {
        return await handleTimer(request, env, corsHeaders);
      }
      if (url.pathname === "/api/timers" && request.method === "GET") {
        return await handleTimers(env, corsHeaders);
      }
      if (url.pathname === "/api/schedule" && request.method === "POST") {
        return await handleSchedule(request, env, corsHeaders);
      }
      if (url.pathname === "/api/schedules" && request.method === "GET") {
        return await handleSchedules(env, corsHeaders);
      }
      return json({ success: false, error: "Route tidak ditemukan" }, 404, corsHeaders);
    } catch (error) {
      const status = error.status || (error.message?.includes("Blynk") ? 502 : 400);
      return json({ success: false, error: error.message || "Permintaan gagal" }, status, corsHeaders);
    }
  },

  async scheduled(_controller, env, context) {
    context.waitUntil(runAutomation(env));
  }
};
