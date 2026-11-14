
export default {
  async fetch(request, env) {
    const allowedOrigin =
      "https://ryotjz-commits.github.io";

    const origin = request.headers.get("Origin");

    const corsHeaders = {
      "Access-Control-Allow-Origin": allowedOrigin,
      "Access-Control-Allow-Methods": "GET, OPTIONS",
      "Access-Control-Allow-Headers": "Content-Type",
      "Vary": "Origin"
    };

    if (origin !== allowedOrigin) {
      return new Response("Origin not allowed", {
        status: 403
      });
    }

    if (request.method === "OPTIONS") {
      return new Response(null, {
        headers: corsHeaders
      });
    }

    const url = new URL(request.url);

    if (url.pathname !== "/api/status" ||
        request.method !== "GET") {
      return new Response("Not found", {
        status: 404,
        headers: corsHeaders
      });
    }

    if (!env.BLYNK_AUTH_TOKEN ||
        !env.BLYNK_BASE_URL) {
      return Response.json(
        { error: "Worker configuration missing" },
        { status: 500, headers: corsHeaders }
      );
    }

    try {
      const pins = ["v0", "v1", "v2", "v3"];
      const values = {};

      for (const pin of pins) {
        const api = new URL(
          "/external/api/get",
          env.BLYNK_BASE_URL
        );

        api.searchParams.set(
          "uUhUdnlB22cqJfKinHQu8yGMXLRbgzf1",
          env.BLYNK_AUTH_TOKEN
        );

        api.searchParams.append(pin, "");

        const response = await fetch(api);

        if (!response.ok) {
          throw new Error("Blynk request failed");
        }

        const result = await response.text();
        const value = Number(result.trim());

        if (!Number.isInteger(value) ||
            (value !== 0 && value !== 1)) {
          throw new Error("Invalid datastream value");
        }

        values[pin.toUpperCase()] = value;
      }

      return Response.json(
        {
          success: true,
          device: "Novia Home",
          channels: values
        },
        { headers: corsHeaders }
      );

    } catch (error) {
      return Response.json(
        { success: false, error: "Blynk communication failed" },
        { status: 502, headers: corsHeaders }
      );
    }
  }
};

