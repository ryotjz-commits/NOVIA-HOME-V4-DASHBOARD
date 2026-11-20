-- NOVIA HOME Worker automation schema (Cloudflare D1)
-- Run once against the D1 database bound to the Worker as DB.

CREATE TABLE IF NOT EXISTS timers (
  channel INTEGER PRIMARY KEY CHECK (channel BETWEEN 0 AND 3),
  deadline_at INTEGER NOT NULL,
  locked_until INTEGER NOT NULL DEFAULT 0,
  updated_at INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS schedules (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  channel INTEGER NOT NULL UNIQUE CHECK (channel BETWEEN 0 AND 3),
  name TEXT NOT NULL,
  enabled INTEGER NOT NULL CHECK (enabled IN (0, 1)),
  on_time TEXT NOT NULL CHECK (length(on_time) = 5),
  off_time TEXT NOT NULL CHECK (length(off_time) = 5),
  weekday_mask INTEGER NOT NULL CHECK (weekday_mask BETWEEN 1 AND 127),
  updated_at INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS schedule_runs (
  schedule_id INTEGER NOT NULL,
  action TEXT NOT NULL CHECK (action IN ('on', 'off')),
  occurrence_date TEXT NOT NULL,
  created_at INTEGER NOT NULL,
  PRIMARY KEY (schedule_id, action, occurrence_date)
);

CREATE INDEX IF NOT EXISTS timers_deadline_idx ON timers (deadline_at, locked_until);
CREATE INDEX IF NOT EXISTS schedules_enabled_idx ON schedules (enabled, channel);
