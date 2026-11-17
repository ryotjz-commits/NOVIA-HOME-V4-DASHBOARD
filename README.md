# NOVIA HOME V4 — UI/UX & Performance Refinement

Dashboard smart home ESP32 + Blynk IoT. Paket ini mempertahankan alur kontrol yang ada dan memperbarui lapisan antarmuka, tema, serta strategi cache offline.

## Fitur yang dipertahankan
- Kontrol 4 channel melalui Blynk Virtual Pin V0–V3.
- Status perangkat disinkronkan melalui endpoint Worker `/api/status`.
- Perintah kontrol melalui endpoint Worker `/api/control`.
- Monitoring browser, API, waktu respons, dan riwayat koneksi lokal.
- Timer otomatis dan jadwal harian melalui Worker API.
- Tema Dark dan Light dengan preferensi lokal.
- PWA dan cache aset untuk penggunaan offline.

## Perubahan UI/UX
- Tata letak dirapikan untuk layar ponsel dan desktop.
- Hirarki judul, status, kartu relay, monitoring, dan otomasi disederhanakan.
- Warna Dark/Light disatukan melalui token CSS.
- Status koneksi lebih mudah dibedakan.
- Fokus keyboard dan reduced-motion tetap diperhatikan.
- Tema tersimpan diterapkan sebelum stylesheet untuk mengurangi kilatan tema.

## Perubahan efisiensi dan offline
- Lapisan desain tetap terpisah di `css/refined.css`.
- Service Worker memakai cache bernama versi baru.
- Cache lama hanya dibersihkan jika memakai prefix `novia-home-`; cache aplikasi lain pada origin yang sama tidak dihapus.
- Pencarian aset offline mengabaikan query string versi CSS/JS, sehingga URL seperti `style.css?v=...` tetap dapat memakai aset yang sudah dicache.
- Fallback dokumen utama hanya berlaku untuk navigasi; file CSS/JS yang hilang tidak diganti dengan HTML.
- Request API lintas-origin tidak dimasukkan ke cache.

## Deploy
1. Backup dashboard yang sedang berjalan.
2. Ekstrak ZIP ini.
3. Unggah seluruh isi folder `NOVIA-HOME-V4-DASHBOARD-main` ke lokasi hosting dashboard yang sama.
4. Pastikan endpoint Worker dan konfigurasi Blynk yang sudah digunakan tetap sama.
5. Buka dashboard, lakukan hard refresh, lalu uji kontrol setiap relay, monitoring, timer, jadwal, tema, dan pemulihan koneksi.
6. Jika dashboard dipasang sebagai PWA, tutup aplikasi lalu buka kembali setelah Service Worker baru aktif. Jika masih memakai aset lama, hapus data/cache situs dari browser lalu buka kembali.

## Batas perubahan
`js/automation.js`, `cloudflare-worker/worker.js`, endpoint API, nama channel, dan payload kontrol tidak diubah. Perubahan JavaScript aplikasi hanya menyesuaikan warna status browser saat tema diganti; alur kontrol dan polling tidak diubah.
