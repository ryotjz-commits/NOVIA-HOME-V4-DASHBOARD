# NOVIA HOME Cloudflare Worker

## Required Cloudflare configuration

Bind a D1 database to the Worker as `DB`, then apply `schema.sql` once (for example with `wrangler d1 execute <database-name> --remote --file=cloudflare-worker/schema.sql`). Configure these Worker variables/secrets:

- `BLYNK_BASE_URL`: regional Blynk HTTPS base URL, e.g. `https://sgp1.blynk.cloud`.
- `BLYNK_AUTH_TOKEN`: device token; store as a Worker secret.
- `CONTROL_PIN`: control PIN; store as a Worker secret.
- `TIME_ZONE`: optional IANA timezone for daily schedules; defaults to `Asia/Jakarta`.
- D1 binding named `DB`.

Add a Cron Trigger that runs every minute (`* * * * *`). The scheduled Worker expires timers and evaluates daily schedules. Without the trigger, endpoints still save and return data, but automated relay actions will not run.

## Routes

| Method | Route | Purpose |
| --- | --- | --- |
| GET | `/api/status` | Read ESP32 state and V0–V3 from Blynk |
| POST | `/api/control` | Set a relay (`{ pin, channel, value }`) |
| POST | `/api/timer` | Set/cancel timer (`{ pin, channel, minutes }`; `minutes: 0` cancels) |
| GET | `/api/timers` | List active timer deadlines and server time |
| POST | `/api/schedule` | Upsert one daily schedule per channel |
| GET | `/api/schedules` | List saved schedules |

Schedule weekday bits match the dashboard: bit 0 Sunday, bit 1 Monday, through bit 6 Saturday. `on_time` and `off_time` use 24-hour `HH:MM`. For an overnight schedule, the selected weekday is the day the ON action occurs, and its OFF action is run the following day. Schedules and timer writes require the configured PIN. Reads are restricted by CORS to the GitHub Pages origin; CORS is not a substitute for authentication.

## Deploy and verify

Deploy `cloudflare-worker/worker.js` to the existing Worker name used by `js/app.js` and `js/automation.js`. Configure the D1 binding, secrets, variable, and cron trigger before deploying. Test relay control only with the ESP32 and connected appliances in a safe state. Verify timer expiry and schedule ON/OFF behavior at a short test time, including an overnight schedule and a disabled schedule.
