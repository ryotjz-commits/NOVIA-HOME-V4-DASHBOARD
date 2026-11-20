# NOVIA HOME V4 — Stable UI/UX Release 1.0

**Tanggal rilis:** 2026-11-17

## Ringkasan
Rilis stabil untuk penyempurnaan tampilan dashboard NOVIA HOME V4, tema Dark/Light, responsivitas, dan cache PWA/offline.

## Yang difinalkan
- Konsistensi desain dan komponen pada layar ponsel maupun desktop.
- Penanganan tema tersimpan agar tampilan awal lebih konsisten.
- Warna browser mengikuti tema aktif.
- Strategi cache Service Worker lebih aman terhadap cache aplikasi lain pada origin yang sama.
- Fallback offline dibatasi pada navigasi dokumen.
- Versi aset dan cache diperbarui untuk memaksa browser mengambil paket rilis terbaru.

## Kompatibilitas
Rilis ini ditujukan sebagai pembaruan dashboard. Firmware ESP32, konfigurasi Blynk, dan Worker aktif tidak perlu diganti untuk menerapkan perubahan UI/UX ini.

## Validasi
Pemeriksaan sintaks, struktur HTML, referensi elemen, dan integritas ZIP dapat dilakukan secara statis. Status stabil pada perangkat nyata baru dapat dipastikan setelah checklist pasca-deploy dijalankan.
