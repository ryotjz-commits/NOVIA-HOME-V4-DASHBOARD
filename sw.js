/* NOVIA HOME V4 — resilient offline cache
 * Strategy: network-first with a short timeout, then cached fallback.
 * API requests and cross-origin requests are never cached.
 */
const CACHE_NAME = "novia-home-v4-ui-20261122-r4";
const CACHE_PREFIX = "novia-home-";
const NETWORK_TIMEOUT_MS = 3500;

// Only include files that exist in the reviewed package.
const APP_FILES = [
  "./",
  "./index.html",
  "./css/style.css",
  "./css/automation.css",
  "./css/refined.css",
  "./css/connection-mode.css",
  "./js/app.js",
  "./js/connection-mode.js",
  "./js/automation.js"
];

self.addEventListener("install", event => {
  event.waitUntil((async () => {
    const cache = await caches.open(CACHE_NAME);
    // Keep installation atomic: the app shell and its local assets must all
    // be available before this worker takes control.
    await cache.addAll(APP_FILES);
    await self.skipWaiting();
  })());
});

self.addEventListener("activate", event => {
  event.waitUntil((async () => {
    const keys = await caches.keys();
    await Promise.all(keys
      .filter(key => key.startsWith(CACHE_PREFIX) && key !== CACHE_NAME)
      .map(key => caches.delete(key)));
    await self.clients.claim();
  })());
});

async function fetchWithTimeout(request) {
  let timeoutId;
  try {
    return await Promise.race([
      fetch(request),
      new Promise((_, reject) => {
        timeoutId = setTimeout(
          () => reject(new Error("Network timeout")),
          NETWORK_TIMEOUT_MS
        );
      })
    ]);
  } finally {
    clearTimeout(timeoutId);
  }
}

self.addEventListener("fetch", event => {
  const request = event.request;
  if (request.method !== "GET") return;

  const url = new URL(request.url);
  // Never cache API traffic, third-party resources, or non-HTTP(S) requests.
  if (url.origin !== self.location.origin || url.pathname.includes("/api/")) return;

  event.respondWith((async () => {
    const cache = await caches.open(CACHE_NAME);
    const isNavigation = request.mode === "navigate";

    try {
      const response = await fetchWithTimeout(request);
      if (response && response.ok && response.type === "basic") {
        // Cache update failure must not turn a successful network response
        // into a failed page/resource load.
        event.waitUntil(cache.put(request, response.clone()).catch(() => {}));
      }
      return response;
    } catch (_) {
      // Query-string changes (e.g. asset version tags) may reuse the same
      // cached local asset when the device is offline.
      const cached = await caches.match(request, { ignoreSearch: true });
      if (cached) return cached;

      if (isNavigation) {
        const shell = await caches.match("./index.html");
        if (shell) return shell;
      }

      return new Response("Aset tidak tersedia saat offline. Hubungkan kembali perangkat ke jaringan lalu coba lagi.", {
        status: 503,
        statusText: "Offline",
        headers: { "Content-Type": "text/plain; charset=utf-8" }
      });
    }
  })());
});
