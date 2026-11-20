# NOVIA HOME — Dashboard Online

Paket ini hanya berisi dashboard web/PWA untuk GitHub Pages. Firmware ESP32, proyek Android, workflow build, dan source Cloudflare Worker tidak disertakan. Dashboard tetap menggunakan API Worker yang sudah dikonfigurasi di JavaScript.

## Deploy
1. Upload seluruh isi folder ini ke root repository GitHub Pages.
2. Pastikan `index.html` berada langsung di root repository.
3. Buka URL GitHub Pages dan uji Kontrol, Monitoring, Otomasi, serta akses MODE AP/ROUTER.

## Catatan
- API online: `https://novia-home-api.ryo-tjz.workers.dev` (dipakai untuk status, kontrol, dan otomasi).
- MODE AP membuka `http://192.168.4.1/`. MODE ROUTER memakai IP ESP32 yang dimasukkan pengguna.
- Service Worker menyimpan aset dashboard untuk akses ulang/offline; permintaan API tetap memerlukan internet.
- Jangan menaruh PIN kontrol atau token rahasia di repository publik.
