NOVIA HOME — paket hasil review
Tanggal: 21 November 2026

STRUKTUR
- index.html
- css/style.css
- css/automation.css
- css/refined.css
- css/connection-mode.css
- js/app.js
- js/automation.js
- js/connection-mode.js
- sw.js

PERUBAHAN
1. Memperbaiki struktur folder agar sesuai dengan path asset pada index.html.
   Pada arsip awal, file CSS dan JavaScript berada sejajar dengan index.html,
   sedangkan HTML memanggilnya dari folder css/ dan js/. Kondisi itu membuat
   stylesheet dan script tidak termuat jika struktur arsip dipakai apa adanya.
2. Menambah keterangan singkat untuk Mode AP dan Mode Router.
3. Menjelaskan arti status Browser, Cloud API, ESP32, waktu respons, serta batas
   interpretasi monitoring.
4. Menambahkan penjelasan perilaku Timer dan pembatalannya.
5. Menambahkan petunjuk jadwal harian.
6. Memperbaiki urutan nama hari pada tampilan jadwal tersimpan agar sesuai
   dengan bitmask checkbox Senin–Minggu.
7. Memperbaiki indikator saat jaringan HP/browser terputus: status Cloud API
   terakhir tidak lagi langsung ditandai offline, karena kedua kondisi itu
   tidak selalu sama.
8. Menambahkan sw.js dan menyelaraskan daftar cache dengan aset yang benar-benar
   tersedia di paket. Referensi manifest.json dan assets/icon.svg dihapus dari
   daftar precache karena kedua file tersebut tidak ada pada sumber yang diberikan.
9. Menambahkan batas tunggu jaringan 3,5 detik sebelum fallback cache agar akses
   offline/lemah tidak menunggu koneksi tanpa batas. Cache API dan lintas-origin
   tetap dikecualikan; kegagalan memperbarui cache tidak menggagalkan respons online.

CATATAN VERIFIKASI
- Sintaks ketiga file JavaScript lolos pemeriksaan `node --check`.
- Path stylesheet dan script di index.html sudah cocok dengan struktur paket.
- Integrasi API/Worker, PIN, Blynk, dan perilaku perangkat fisik belum dapat
  diuji dari arsip frontend saja; perlu pengujian langsung dengan backend dan ESP32.
- Service Worker sekarang disertakan sebagai `sw.js` di root paket, sesuai dengan
  lokasi pendaftaran pada index.html. Cache precache mencakup file lokal yang ada
  dalam paket ini.
- Paket sumber tidak menyertakan `manifest.json` maupun `assets/icon.svg`; karena
  itu keduanya tidak direferensikan dalam daftar precache. Instalasi PWA sebagai
  aplikasi terpasang masih memerlukan manifest dan ikon bila fitur tersebut diinginkan.
- Offline fallback bergantung pada setidaknya satu kali pemuatan online yang
  berhasil agar Service Worker dapat terpasang dan mengisi cache.
- Jangan menganggap status online Cloud sebagai bukti kondisi relay secara fisik.
