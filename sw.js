/* NOVIA HOME V4 — resilient offline cache */
const CACHE_NAME = "novia-home-v4-ap-router-20261120-r2";
const CACHE_PREFIX = "novia-home-";

const APP_FILES = [
  "./",
  "./index.html",
  "./css/style.css",
  "./css/automation.css",
  "./css/refined.css",
  "./css/connection-mode.css",
  "./js/app.js",
  "./js/connection-mode.js",
  "./js/automation.js",
  "./manifest.json",
  "./assets/icon.svg"
];

self.addEventListener("install", event => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then(cache => cache.addAll(APP_FILES))
      .then(() => self.skipWaiting())
  );
});

self.addEventListener("activate", event => {
  event.waitUntil(
    caches.keys()
      .then(keys => Promise.all(
        keys
          .filter(key => key.startsWith(CACHE_PREFIX) && key !== CACHE_NAME)
          .map(key => caches.delete(key))
      ))
      .then(() => self.clients.claim())
  );
});

self.addEventListener("fetch", event => {
  const request = event.request;
  if (request.method !== "GET") return;

  const url = new URL(request.url);
  if (url.origin !== self.location.origin || url.pathname.includes("/api/")) return;

  event.respondWith((async () => {
    const cache = await caches.open(CACHE_NAME);
    const isNavigation = request.mode === "navigate";

    try {
      const response = await fetch(request);
      if (response && response.ok) {
        await cache.put(request, response.clone());
      }
      return response;
    } catch (error) {
      // Ignore query-string version changes when resolving cached static assets.
      const cached = await caches.match(request, { ignoreSearch: true });
      if (cached) return cached;

      // Only navigation requests may fall back to the app shell.
      if (isNavigation) {
        const shell = await caches.match("./index.html");
        if (shell) return shell;
      }

      return new Response("Aset tidak tersedia saat offline.", {
        status: 503,
        statusText: "Offline",
        headers: { "Content-Type": "text/plain; charset=utf-8" }
      });
    }
  })());
});
