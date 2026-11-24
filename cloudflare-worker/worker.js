/**
 * NOVIA HOME - Cloudflare Worker status endpoint
 *
 * Required bindings:
 *   BLYNK_AUTH_TOKEN (Secret)
 *   BLYNK_BASE_URL   (Text), e.g. https://sgp1.blynk.cloud
 *
 * Keep the production /api/control route when deploying. This project file
 * contains the status route only; merge it into the existing production Worker
 * if that Worker also handles relay control.
 */
const ALLOWED_ORIGIN = "https://ryotjz-commits.github.io";
const PINS = ["V0", "V1", "V2", "V3"];

function json(data, status = 200) {
  return new Response(JSON.stringify(data), {
    status,
    headers: {
      "Content-Type": "application/json; charset=utf-8",
      "Access-Control-Allow-Origin": ALLOWED_ORIGIN,
      "Access-Control-Allow-Methods": "GET, OPTIONS",
      "Access-Control-Allow-Headers": "Content-Type, Accept",
      "Access-Control-Max-Age": "86400",
      "Cache-Control": "no-store, no-cache, must-revalidate",
      "Pragma": "no-cache",
      "Vary": "Origin"
    }
  });
}

async function blynkGet(baseUrl, token, endpoint, params = {}) {
  const api = new URL(endpoint, baseUrl);
  api.searchParams.set("token", token);
  for (const [key, value] of Object.entries(params)) {
    api.searchParams.set(key, value);
  }

  const response = await fetch(api.toString(), {
    method: "GET",
    headers: { "Accept": "text/plain" },
    signal: AbortSignal.timeout(8000)
  });

  if (!response.ok) throw new Error("Blynk API request failed");
  return (await response.text()).trim();
}

export default {
  async fetch(request, env) {
    const origin = request.headers.get("Origin");
    if (origin !== ALLOWED_ORIGIN) {
      return new Response("Origin not allowed", { status: 403 });
    }

    if (request.method === "OPTIONS") {
      return new Response(null, {
        status: 204,
        headers: {
          "Access-Control-Allow-Origin": ALLOWED_ORIGIN,
          "Access-Control-Allow-Methods": "GET, OPTIONS",
          "Access-Control-Allow-Headers": "Content-Type, Accept",
          "Access-Control-Max-Age": "86400",
          "Vary": "Origin"
        }
      });
    }

    const url = new URL(request.url);
    if (url.pathname !== "/api/status" || request.method !== "GET") {
      return json({ success: false, error: "Not found" }, 404);
    }

    const token = env.BLYNK_AUTH_TOKEN;
    const baseUrl = env.BLYNK_BASE_URL;

    if (!token || !baseUrl) {
      return json({
        success: false,
        error: "Worker configuration missing"
      }, 500);
    }

    try {
      // Check hardware connectivity and read all four datastreams in parallel.
      const [connectedText, ...pinValues] = await Promise.all([
        blynkGet(baseUrl, token, "/external/api/isHardwareConnected"),
        ...PINS.map(pin =>
          blynkGet(baseUrl, token, "/external/api/get", { [pin]: "" })
        )
      ]);

      const connectedNormalized = connectedText.toLowerCase();
      if (!["true", "false", "1", "0"].includes(connectedNormalized)) {
        throw new Error("Invalid hardware connection response");
      }

      const channels = {};
      PINS.forEach((pin, index) => {
        const value = pinValues[index];
        if (value !== "0" && value !== "1") {
          throw new Error("Invalid datastream response");
        }
        channels[pin] = Number(value);
      });

      return json({
        success: true,
        device: "Novia Home",
        deviceOnline: connectedNormalized === "true" || connectedNormalized === "1",
        channels
      });
    } catch (error) {
      // Never return the upstream URL, token, or raw provider error.
      return json({
        success: false,
        error: "Blynk communication failed"
      }, 502);
    }
  }
};
