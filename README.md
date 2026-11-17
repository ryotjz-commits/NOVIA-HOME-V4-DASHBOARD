# NOVIA HOME V4 — Stable UI/UX Release

**Release:** Stable UI/UX 1.0  
**Tanggal:** 17 November 2026  
**Platform:** ESP32 · Blynk IoT · Web Dashboard / PWA

Rilis ini memfinalkan lapisan antarmuka, tema, responsivitas, dan perilaku cache offline pada dashboard NOVIA HOME V4. Fokus rilis adalah kestabilan tampilan tanpa merombak alur kontrol perangkat yang sudah ada.

## Fungsi dasar yang dipertahankan
- Kontrol 4 channel melalui Blynk Virtual Pin V0–V3.
- Sinkronisasi status melalui endpoint Worker yang telah digunakan.
- Monitoring browser, API, waktu respons, dan riwayat koneksi lokal.
- Timer otomatis dan jadwal harian melalui integrasi otomasi yang sudah ada.
- Tema Dark dan Light dengan preferensi lokal.
- PWA dan cache aset untuk penggunaan offline.

## Finalisasi UI/UX
- Tampilan dirapikan untuk ponsel dan desktop.
- Hirarki informasi, kartu relay, monitoring, otomasi, dan navigasi dibuat lebih konsisten.
- Sistem warna Dark/Light disatukan melalui token CSS.
- Status koneksi dan kontrol tema dibuat lebih jelas.
- Fokus keyboard dan preferensi reduced-motion tetap didukung.
- Tema tersimpan diterapkan sebelum stylesheet untuk mengurangi kilatan tema.

## Stabilitas dan cache offline
- Lapisan visual berada di `css/refined.css`, terpisah dari stylesheet dasar.
- Cache Service Worker memakai identitas rilis baru.
- Aktivasi hanya menghapus cache lama dengan prefix `novia-home-`, bukan cache aplikasi lain pada origin yang sama.
- Pencarian aset offline mengabaikan query string versi CSS/JS.
- Fallback `index.html` hanya digunakan untuk navigasi; aset CSS/JS yang tidak tersedia tidak diganti dengan HTML.
- Request API lintas-origin tidak dimasukkan ke cache.

## Deploy melalui GitHub dari HP
1. Backup folder dashboard yang sedang aktif.
2. Ekstrak ZIP rilis ini.
3. Unggah **isi folder** `NOVIA-HOME-V4-DASHBOARD` ke direktori dashboard pada repository/hosting yang sama. Jangan membuat folder bertingkat tambahan jika hosting sebelumnya membaca `index.html` dari root.
4. Pertahankan URL Worker, konfigurasi Blynk, token, serta environment variable yang sudah aktif.
5. Commit perubahan, tunggu proses deploy selesai, lalu buka dashboard dan lakukan refresh.
6. Jika PWA masih menampilkan aset lama, tutup PWA. Buka ulang setelah Service Worker baru aktif; bila perlu hapus cache/data situs dari browser.

## Checklist uji setelah deploy
- [ ] Dashboard terbuka tanpa error tampilan.
- [ ] Relay V0, V1, V2, dan V3 dapat ON/OFF.
- [ ] Status relay tersinkron setelah halaman dimuat ulang.
- [ ] Monitoring dan indikator koneksi tampil normal.
- [ ] Tema Dark dan Light dapat diganti dan tersimpan.
- [ ] Timer dan jadwal harian berjalan sesuai konfigurasi.
- [ ] Wi-Fi terputus lalu tersambung kembali tanpa restart ESP32.
- [ ] Dashboard/PWA dapat dibuka kembali setelah koneksi internet terputus (sesuai batas data yang telah tercache).

## Batas perubahan
- `js/automation.js` tidak diubah.
- `cloudflare-worker/worker.js` tidak diubah; pertahankan Worker yang saat ini sudah berjalan dan jangan menggantinya hanya karena file referensi ikut berada di paket.
- Pemetaan relay, Virtual Pin, endpoint, payload kontrol, serta logika polling tidak dirombak.
- Perubahan JavaScript aplikasi dibatasi pada pembaruan warna meta browser ketika tema diganti.

**Status validasi:** pemeriksaan statis paket dan sintaks JavaScript dilakukan sebelum rilis. Pengujian fisik pasca-deploy pada ESP32, Blynk, dan jaringan rumah tetap perlu dilakukan menggunakan checklist di atas.
