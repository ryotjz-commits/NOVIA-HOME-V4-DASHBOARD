# Release R3 — Remote ESP32 Monitoring
- Added explicit ESP32 ONLINE/OFFLINE/UNKNOWN monitoring state, separate from Cloud API health.
- Added last-seen display and ESP32 connection history transitions.
- Relay controls are disabled when ESP32 is offline or status cannot be verified.
- Dashboard can monitor remotely over cellular data through Blynk Cloud.
- Added Cloudflare Worker status-route reference; merge into production Worker without removing its existing /api/control route.
- Preserved R2 dashboard cleanup and performance adjustments.

## UI Refresh R2 — Dark Mode correction
- Memperbaiki akar masalah: stylesheet refresh sebelumnya menimpa token warna inti dengan palet Light, sehingga Dark Mode hanya mengganti warna-scheme/background.
- Menambahkan token warna Dark Mode lengkap dan override khusus untuk kartu perangkat, hero, navigasi bawah, panel monitoring, otomasi, jadwal, formulir, dan mode koneksi.
- Menambahkan versi cache stylesheet baru agar browser mengambil CSS R2.
- Tidak mengubah endpoint API, ID elemen, event handler, maupun logika kontrol perangkat.
