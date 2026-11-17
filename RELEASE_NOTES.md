# NOVIA HOME V5 — REALISTIC SOFT CERAMIC

## Perubahan visual
- Palet Soft Pastel Earth untuk Light Mode dan Dark Mode.
- Permukaan kartu dengan highlight dan bayangan lembut bergaya soft ceramic.
- Tombol perangkat memiliki kedalaman visual yang lebih terasa.
- Bottom navigation dan panel monitoring mengikuti token warna yang sama.
- File CSS dimuat satu kali; duplikasi stylesheet pada index.html dihapus.
- Cache-busting stylesheet diseragamkan ke versi V5.

## Batas perubahan
- Tidak mengubah Worker API, D1, endpoint, kontrol relay, timer, jadwal, atau firmware.
- JavaScript aplikasi hanya mengubah warna meta theme-color agar sesuai dengan tema baru.
- Struktur HTML, ID, data attributes, dan event handler dipertahankan.

## File utama
- css/style.css: token warna dan layout inti.
- css/automation.css: komponen otomasi.
- css/refined.css: monitoring dan riwayat.
- css/uiux-r2.css: lapisan visual Realistic Soft Ceramic.
- css/theme-reference.css: file kompatibilitas, tidak mendefinisikan ulang palet.
