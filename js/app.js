
/*
  NOVIA HOME V4
  Dashboard Starter

  Status:
  - UI only
  - Belum terhubung ke Blynk
  - Tidak ada token atau kredensial
*/

document.addEventListener("DOMContentLoaded", () => {
  console.log("NOVIA HOME V4 Dashboard loaded");

  const status = document.getElementById(
    "connectionStatus"
  );

  if (status) {
    status.textContent = "DEMO MODE";
  }
});
