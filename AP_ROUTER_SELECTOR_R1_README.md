# NOVIA HOME Dashboard — AP / Router Selector (R1)

## Perubahan
- Menambahkan menu pilihan **MODE AP** dan **MODE ROUTER** pada dashboard online.
- AP membuka `http://192.168.4.1/` (HP perlu tersambung ke Wi-Fi AP NOVIA HOME terlebih dahulu).
- Router meminta IP lokal ESP32 dan membuka `http://IP-ESP32/`.
- IP Router terakhir disimpan lokal di browser.
- Tidak mengubah firmware, Cloud API, Blynk, tab Kontrol/Monitoring/Otomasi, atau kontrol relay cloud.

## Penting
Mode lokal membuka dashboard yang di-host oleh ESP32, bukan mengubah dashboard GitHub Pages menjadi API lokal. Ini sengaja dipilih agar tidak memerlukan CORS atau SSL/API bridge tambahan.

## Deploy ke GitHub Pages
1. Ekstrak ZIP.
2. Upload/commit isi folder `NOVIA-HOME-V4-DASHBOARD-main` ke repo dashboard yang saat ini dipakai, dengan struktur file tetap.
3. Jangan upload folder ZIP-nya sebagai folder tambahan di dalam repo; file `index.html` harus tetap di root Pages.
4. Setelah Pages deploy, buka dashboard online dan tes menu AP/Router.

## Tes
- AP: sambungkan HP ke Wi-Fi ESP32 NOVIA HOME (jika Android memberi pilihan, pilih tetap tersambung tanpa internet), lalu dari dashboard online tekan MODE AP.
- Router: pastikan HP dan ESP32 di Wi-Fi router yang sama; masukkan IP ESP32 yang diketahui dari router atau Serial Monitor; tekan BUKA DASHBOARD LOKAL.
- Tes tampilan lokal dan kontrol relay di dashboard ESP32.

## Batasan R1
- Router mode menggunakan IP manual; pencarian IP otomatis belum ditambahkan karena browser dari GitHub Pages tidak dapat melakukan scan jaringan lokal secara andal.
- Saat membuka dashboard ESP32, halaman akan berpindah dari GitHub Pages ke alamat lokal. Untuk kembali ke dashboard online, gunakan tombol Back browser.
- Versi ini adalah perubahan UI/navigasi saja. Firmware tidak disentuh.
