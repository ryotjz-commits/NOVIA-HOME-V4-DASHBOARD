# NOVIA HOME — Dashboard Online R3

Dashboard online mobile-first dengan monitoring ESP32 jarak jauh melalui Blynk Cloud. Paket mempertahankan optimasi dan pembersihan dari R2.

## Deploy dashboard
1. Upload isi folder `NOVIA-HOME-V4-DASHBOARD-main` ke root repository GitHub Pages.
2. Pastikan alamat Pages tetap `https://ryotjz-commits.github.io`.
3. Buka ulang dashboard / hard refresh. Service worker memakai cache R3.

## Update Cloudflare Worker (WAJIB untuk status ESP32)
File `cloudflare-worker/worker-status-route-reference.js` adalah referensi route status tambahan, BUKAN Worker produksi lengkap. Worker produksi yang saat ini berjalan juga melayani `/api/control`; jangan menggantinya dengan file referensi ini karena kontrol relay akan hilang.

Gabungkan logika endpoint `GET /api/status` dari file referensi ke Worker produksi yang sudah ada. Pastikan CORS mengizinkan origin GitHub Pages dan method GET/OPTIONS. Worker memerlukan:
- Secret `BLYNK_AUTH_TOKEN` (token device Blynk)
- Variable `BLYNK_BASE_URL` (server regional, misalnya `https://sgp1.blynk.cloud`; gunakan region persis dari Blynk Console)

Route status menggunakan API resmi Blynk `isHardwareConnected` dan membaca V0–V3 secara paralel. Query token dibentuk sebagai parameter `token`, bukan nama parameter acak.

## Arti status
- `ESP32 ONLINE`: Blynk Cloud menyatakan hardware tersambung. Dapat dipantau dari HP melalui 4G/5G.
- `ESP32 OFFLINE`: Blynk Cloud tidak mendeteksi hardware. Kemungkinan penyebab: ESP32 mati, Wi-Fi/router/internet rumah putus, atau koneksi Blynk terputus.
- `Tidak dapat diverifikasi`: API/Internet dashboard sedang tidak dapat diakses; bukan bukti bahwa ESP32 mati.
- Status relay saat perangkat offline adalah nilai terakhir yang berhasil dibaca. Tombol kontrol otomatis dinonaktifkan sampai ESP32 online kembali.

Status Blynk tidak dapat membedakan secara pasti antara ESP32 kehilangan daya dan ESP32 hidup tetapi Wi-Fi/router/internet putus. Untuk diagnosis terpisah tersebut dibutuhkan telemetry tambahan dari firmware.

## Tes wajib
1. ESP32 online: tampil `ESP32 ONLINE`.
2. Matikan ESP32: tampil `ESP32 OFFLINE` setelah polling berikutnya.
3. Hidupkan kembali: status pulih otomatis.
4. Ulangi saat HP memakai data seluler.
5. Pastikan kontrol relay `/api/control` tetap berfungsi setelah route status digabungkan.
