(() => {
  const AP_URL = "http://192.168.4.1/";
  const IP_KEY = "novia-home-router-ip-v1";
  const apButton = document.getElementById("openApMode");
  const routerButton = document.getElementById("openRouterMode");
  const ipInput = document.getElementById("routerDeviceIp");
  const note = document.getElementById("connectionModeNote");
  if (!apButton || !routerButton || !ipInput) return;

  try { ipInput.value = localStorage.getItem(IP_KEY) || ""; } catch (_) {}

  apButton.addEventListener("click", () => {
    if (note) note.textContent = "Membuka dashboard ESP32 melalui mode AP. Pastikan HP sudah terhubung ke Wi-Fi NOVIA HOME.";
    window.location.assign(AP_URL);
  });

  routerButton.addEventListener("click", () => {
    const raw = ipInput.value.trim().replace(/^https?:\/\//i, "").replace(/\/$/, "");
    const ip = raw.split("/")[0].split(":")[0];
    const valid = /^(?:(?:25[0-5]|2[0-4]\d|1?\d?\d)\.){3}(?:25[0-5]|2[0-4]\d|1?\d?\d)$/.test(ip);
    if (!valid) {
      if (note) note.textContent = "Masukkan IPv4 ESP32 yang benar, misalnya 192.168.1.25.";
      ipInput.focus();
      return;
    }
    try { localStorage.setItem(IP_KEY, ip); } catch (_) {}
    if (note) note.textContent = "Membuka dashboard lokal ESP32 di " + ip + ". Pastikan HP dan ESP32 berada di router yang sama.";
    window.location.assign("http://" + ip + "/");
  });
})();
