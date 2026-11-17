
# NOVIA HOME V4

Hybrid Smart Home Dashboard.

## Platform
- ESP32 DevKit V1
- GitHub Pages
- Blynk IoT
- HTML
- CSS
- JavaScript

## Features Planned
- Four Channel Control
- Online Device Monitoring
- Blynk Integration
- Responsive Dashboard
- Local Web Server Compatibility

## Development Status

Phase A: Repository and Dashboard Starter.

Current mode: DEMO ONLY.

No ESP32 connection is active yet.

## Important

NOVIA HOME V3 remains the stable firmware backup.

Dashboard development is separated from the firmware project.

Never store Blynk Auth Token or WiFi passwords
in this public repository.


## NOVIA HOME V5 Phase 2 — Connection History

Phase 2 menambahkan riwayat perubahan status API dan jaringan browser pada dashboard. Riwayat disimpan lokal di browser/perangkat menggunakan `localStorage`, dibatasi hingga 50 kejadian terbaru, dan dapat dihapus melalui tombol Hapus Riwayat. Data tidak dikirim ke Cloudflare Worker atau Blynk.

Riwayat mencatat perubahan status (misalnya API terputus dan pulih), bukan setiap polling berkala. Jika penyimpanan browser diblokir atau penuh, dashboard tetap dapat digunakan tetapi riwayat mungkin tidak tersimpan.
