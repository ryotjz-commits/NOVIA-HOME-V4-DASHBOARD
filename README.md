# NOVIA HOME - Single Repository

Repository ini menggabungkan dashboard web, aplikasi Android, dan firmware ESP32.

## Struktur
- `android/` — aplikasi Android WebView (auto-connect).
- `firmware/NOVIA_HOME_V4_4/` — sketch ESP32 DevKit V1 beserta header.
- `.github/workflows/build-all.yml` — satu workflow untuk membangun APK dan BIN secara paralel.

## Build dari GitHub Actions
1. Upload seluruh isi folder ini ke root repository `NOVIA-HOME-V4-DASHBOARD` (branch `main`).
2. Di GitHub buka **Settings → Secrets and variables → Actions → New repository secret**.
3. Tambahkan secret `BLYNK_AUTH_TOKEN` dengan token perangkat Blynk yang baru/aktif. Jangan simpan token di source publik.
4. Buka **Actions → NOVIA HOME - Build APK + ESP32 BIN → Run workflow**.
5. Unduh artifact `NOVIA-HOME-APK` dan `NOVIA-HOME-ESP32-FIRMWARE-BIN`.

## Catatan firmware
Target kompilasi dikonfigurasi untuk **ESP32 DevKit V1** (`esp32:esp32:esp32`) menggunakan Arduino-ESP32 core 3.0.7 dan library Blynk. Pastikan board fisik dan library cocok sebelum flashing. BIN hasil build adalah firmware aplikasi; untuk flashing melalui tool yang meminta image gabungan bootloader/partition/app, gunakan metode upload Arduino IDE/arduino-cli atau periksa kebutuhan image board.

## Keamanan penting
Token Blynk pada arsip firmware asli berada langsung di source. Token tersebut sengaja dihapus dari source gabungan dan diganti dengan secret GitHub `BLYNK_AUTH_TOKEN`. Jika token lama pernah berada di repository publik, segera regenerate/rotate token di Blynk Console.
