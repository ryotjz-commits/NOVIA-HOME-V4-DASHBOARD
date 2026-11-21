# NOVIA HOME V4 — UI Refresh R2

## Perubahan UI
- Bottom navigation menjadi empat tab: Kontrol, Perangkat, Monitoring, dan Otomasi.
- Selector Mode AP dan Mode Router dipindahkan dari halaman utama ke tab Perangkat.
- Tab bar fixed di bagian bawah dengan safe-area spacing untuk perangkat mobile.
- Kartu kontrol, panel akses perangkat, monitoring, dan otomasi diselaraskan dengan referensi UI mobile.
- Light Mode menggunakan permukaan putih/pale blue; Dark Mode menggunakan navy/charcoal dengan teks terang dan aksen hijau.
- Versi aset dan cache Service Worker diperbarui untuk mencegah penggunaan CSS lama.

## Bagian fungsional yang dipertahankan
- ID elemen koneksi AP/Router dan handler `connection-mode.js`.
- ID panel kontrol, monitoring, dan otomasi.
- Logika kontrol Blynk/ESP32, polling status, timer, dan jadwal pada JavaScript yang sudah ada.
- Cloudflare Worker tidak diubah.

## Pemeriksaan
- Sintaks JavaScript diperiksa dengan `node --check`.
- Pemeriksaan struktur HTML dan ID duplikat dilakukan.
- Komunikasi fisik dengan ESP32/Blynk belum diuji pada perangkat.


## R3 — Bottom navigation fit fix
- Fixed the four-item bottom navigation to remain in a single row on narrow mobile screens.
- Uses four equal grid columns with minmax(0, 1fr), min-width: 0, and compact responsive labels.
- Prevents long labels such as “Monitoring” and “Perangkat” from forcing tab overlap or stacking.
- Bumped CSS asset URLs and service-worker cache version to ensure clients receive the corrected stylesheet.
