
/*
  NOVIA HOME V4
  DASHBOARD PREVIEW

  DEMO ONLY:
  Tombol hanya mengubah tampilan browser.
  Belum terhubung ke Blynk atau ESP32.
*/

document.addEventListener("DOMContentLoaded", () => {

  const status = document.getElementById("connectionStatus");
  const cards = document.querySelectorAll(".device-card");

  if (status) {
    status.textContent = "DEMO MODE";
  }

  cards.forEach((card) => {

    const button = card.querySelector(".toggle-button");
    const badge = card.querySelector(".state-badge");
    const label = card.querySelector(".toggle-label");

    if (!button) return;

    button.addEventListener("click", () => {

      const isOn =
        button.getAttribute("aria-pressed") !== "true";

      button.setAttribute("aria-pressed", String(isOn));
      card.classList.toggle("is-on", isOn);

      if (badge) {
        badge.textContent = isOn ? "DEMO ON" : "DEMO OFF";
      }

      if (label) {
        label.textContent = isOn
          ? "SIMULASI ON"
          : "SIMULASI OFF";
      }

    });

  });

  console.log("NOVIA HOME V4 - Local demo only");

});
