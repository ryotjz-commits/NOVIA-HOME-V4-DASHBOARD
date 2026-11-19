# NOVIA HOME V4.4 - MODULAR EDITION

## Isi paket
- `NOVIA_HOME_V4_4.ino` — entry point (`setup()` dan `loop()`).
- `NOVIA_Config.h` — library, konfigurasi Blynk/hardware/network, variabel global, deklarasi fungsi.
- `NOVIA_Core.h` — watchdog, kontrol relay, sinkronisasi tombol fisik dan Blynk.
- `NOVIA_Automation.h` — timer, jadwal harian, mode AUTO/MANUAL, NTP, statistik.
- `NOVIA_Network.h` — penyimpanan WiFi/AP, koneksi dan reconnect WiFi/Blynk.
- `NOVIA_Dashboard.h` — HTML/CSS/JavaScript dashboard.
- `NOVIA_Web.h` — API, kontrol akses klien AP/LAN, WiFi scanner, dan routing web.
- `NOVIA_Auth.h` — PIN lokal, hash PIN, sesi akses dashboard, dan pembatasan percobaan login.

## Cara membuka
1. Ekstrak ZIP.
2. Pastikan semua file berada dalam satu folder bernama `NOVIA_HOME_V4_4`.
3. Buka `NOVIA_HOME_V4_4.ino` dari Arduino IDE/ArduinoDroid.
4. Pilih board ESP32 Dev Module / ESP32 DevKit V1.
5. Pasang library yang dipakai source: Blynk, ESP32 core (WiFi/WebServer/Preferences), serta dependensi bawaan ESP32.
6. Isi `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, dan `BLYNK_AUTH_TOKEN` di `NOVIA_Config.h`.
7. Hubungkan HP ke AP NOVIA HOME (default awal mengikuti nilai `DEFAULT_AP_SSID` / `DEFAULT_AP_PASS` di `NOVIA_Config.h`).
8. Buka Chrome/browser secara manual, lalu ketik `http://192.168.4.1/` untuk membuka dashboard.
9. Dari dashboard, ubah SSID dan password AP. Password wajib 8-63 karakter. ESP32 akan menyimpan konfigurasi lalu restart; sambungkan HP ke SSID baru.
10. Compile terlebih dahulu, baru upload.

## Hardware sesuai source
Relay ACTIVE HIGH:
- CH1 GPIO26
- CH2 GPIO27
- CH3 GPIO13
- CH4 GPIO14

Tombol INPUT_PULLUP:
- CH1 GPIO32
- CH2 GPIO33
- CH3 GPIO25
- CH4 GPIO23

Blynk:
- V0=CH1, V1=CH2, V2=CH3, V3=CH4

## Catatan refactor
- Pemisahan dilakukan dari file `NOVIA_HOME_V4_3_AUTOMATION.ino` yang diberikan pada percakapan ini.
- Alur dan implementasi fitur sumber dipertahankan saat dipisahkan ke beberapa header.
- HTTP Basic Authentication diganti PIN lokal 6–8 digit. Dashboard dan API menerima klien dari subnet SoftAP NOVIA HOME maupun subnet LAN router yang sama dengan koneksi STA ESP32. Akses dari jaringan lain tetap ditolak.
- Saat pertama kali membuka V4.4, buat PIN lokal. Semua endpoint API dan konfigurasi WiFi/AP memerlukan sesi setelah PIN benar. Gunakan PIN yang tidak mudah ditebak dan password AP unik.
- Mode akses adalah browser biasa: tidak ada DNS wildcard, captive portal, pop-up Sign in to Wi-Fi, atau redirect otomatis. Buka `http://192.168.4.1/` secara manual dari browser setelah tersambung ke AP.
- SSID/password AP disimpan dengan Preferences/NVS pada key `ap_ssid` dan `ap_pass`. Kredensial WiFi router, jadwal, mode, dan statistik yang sudah ada tidak dihapus saat AP diubah. Restart/power-cycle biasa tidak menghapus data; hanya penghapusan NVS/factory erase yang mengembalikan default.
- Kredensial Blynk pada file sumber masih berupa placeholder `0000-` dan `0000`; isi dengan kredensial perangkat sebenarnya.
- Versi ini belum dikompilasi dengan Arduino CLI/IDE dan belum diuji pada perangkat ESP32 fisik.
- Jangan mengandalkan inisialisasi software saja untuk mencegah relay aktif saat boot; gunakan pull-down hardware yang sesuai pada input relay ACTIVE HIGH dan pastikan rangkaian aman.


## Akses browser & pengaturan AP
- AP memakai IP lokal `192.168.4.1`; dashboard dibuka langsung melalui browser. Tidak ada DNS wildcard.
- Setelah perangkat tersambung ke AP NOVIA HOME, buka browser (misalnya Chrome) dan ketik `http://192.168.4.1/`. Dashboard tidak dibuka otomatis. Jika ponsel memperingatkan bahwa WiFi tidak memiliki internet, pilih opsi tetap terhubung / gunakan jaringan ini.
- Form Access Point di dashboard dapat mengubah SSID dan password. Setelah simpan, ESP32 restart dan ponsel harus menyambung ulang ke SSID baru.
- Akses browser dibatasi ke klien subnet AP atau LAN router yang sama, lalu memerlukan PIN lokal untuk API dan perubahan konfigurasi.

## Perbaikan WiFi Scanner
- Scan berjalan dalam mode asynchronous agar web server tetap responsif.
- Pemindaian meminta daftar SSID tersembunyi untuk kebutuhan diagnostik; SSID tersembunyi ditampilkan tetapi tidak bisa dipilih otomatis.
- Dashboard melakukan polling status secara berurutan (tanpa request tumpang tindih).
- Serial Monitor menampilkan hasil mulai dan jumlah jaringan yang ditemukan (`[WIFI SCAN]`).
- Jika daftar tetap kosong, periksa Serial Monitor dan pastikan router memancarkan SSID 2.4 GHz (ESP32 tidak mendukung WiFi 5 GHz).


## Akses dashboard melalui router (STA)
- Saat HP/PC berada pada WiFi router yang sama dengan ESP32, buka alamat IP STA ESP32 yang diberikan router, misalnya `http://192.168.1.xxx/`.
- Alamat `http://192.168.4.1/` hanya dapat digunakan saat perangkat terhubung ke hotspot NOVIA HOME.
- IP STA ditampilkan di Serial Monitor setelah ESP32 berhasil tersambung ke router.


## V4.4 upgrade notes

- Added a local 6-8 digit PIN gate. On first boot after flashing, open the dashboard from the NOVIA AP or the same home LAN and create the PIN. The 6-8 digit PIN is stored as a salted SHA-256 digest in the existing `novia` Preferences namespace (`authSalt`, `authHash`).
- Dashboard API and WiFi/AP configuration endpoints now require a short-lived in-memory session token. The browser stores the token locally; the ESP32 session expires after 12 hours or on reboot. Five failed PIN attempts trigger a 60-second lockout.
- Existing NVS namespace and existing configuration keys are unchanged. New timer keys `tmR0`–`tmR3` (remaining seconds) and `tmX0`–`tmX3` (NTP expiry epoch) are additive.
- Timer OFF continues across ESP32 reboot using an NTP epoch deadline when time is available. Remaining time is checkpointed about once per minute. If NTP is unavailable for 90 seconds after boot, it safely falls back to the last saved remaining duration (power-off time is paused in this fallback mode). On expiry or manual clear, saved timer keys are removed.
- Existing relay startup safety remains unchanged: all relays initialize OFF. Schedule settings, WiFi credentials, AP credentials, usage statistics, Blynk, and physical buttons are retained.

Security note: this is local-network PIN protection over HTTP, not encrypted transport. Keep NOVIA HOME on a trusted LAN, use a strong AP password, and do not expose TCP port 80 to the internet.
