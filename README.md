# NOVIA HOME — Refined Dashboard

Dashboard smart home berbasis ESP32 dan Blynk IoT. Tampilan diperbarui menjadi lebih minimalis, rapi, dan responsif tanpa mengubah alur kontrol maupun integrasi yang sudah ada.

## Fitur
- Kontrol 4 channel melalui Blynk Virtual Pin V0–V3.
- Monitoring koneksi browser dan API.
- Riwayat koneksi lokal.
- Timer otomatis dan jadwal harian.
- Tema gelap/terang.
- PWA dan dukungan offline melalui Service Worker.

## Deploy
Unggah seluruh isi folder ini ke GitHub Pages atau hosting statis yang digunakan sebelumnya. Pastikan `cloudflare-worker/worker.js` dan endpoint API tetap menggunakan konfigurasi yang sama.

## Catatan perubahan
- Penyesuaian visual dipisahkan di `css/refined.css` agar stylesheet dasar tetap utuh.
- Elemen dan ID yang dipakai JavaScript dipertahankan.
- Service Worker diperbarui agar stylesheet baru tersedia saat offline dan cache lama diganti.
- `js/app.js`, `js/automation.js`, serta endpoint kontrol tidak diubah.
