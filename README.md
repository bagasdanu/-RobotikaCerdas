# -RobotikaCerdas

Kumpulan program Arduino dan Python untuk sistem pan-tilt launcher berbasis PCA9685.

---

## Deskripsi Sistem

Sistem ini menggerakkan mekanisme pan-tilt menggunakan dua servo untuk sumbu tilt dan satu servo untuk sumbu pan, semuanya dikontrol lewat PCA9685 PWM driver. Dua motor DC digunakan sebagai aktuator launcher: satu untuk reload peluru dan satu untuk meluncurkan.

Kontrol dapat dilakukan lewat Serial Monitor Arduino IDE atau lewat program Python yang membaca input keyboard secara real-time.

---

## Struktur Repositori

```
-RobotikaCerdas/
├── ArrowControl/
│   ├── ArrowControl.ino       
│   └── ArrowControl.py          
├── MekanikGabungan/
│   └── MekanikGabungan
├── MekanikMotorDC/
│   └── MekanikMotorDC.ino     
└── MekanikServo/
    └── MekanikServo.ino       
```

---

## Hardware

| Komponen        | Keterangan                         |
|-----------------|------------------------------------|
| PCA9685         | PWM driver, I2C address 0x40       |
| Servo tilt x2   | CH0 dan CH1 pada PCA9685           |
| Servo pan x1    | CH2 pada PCA9685                   |
| Motor A (reload)| PWM CH3, arah pin D6 dan D7        |
| Motor B (launch)| PWM CH4, arah pin D4 dan D5        |
| Arduino         | Komunikasi serial ke Python/Monitor|

---

## Konfigurasi Servo

| Parameter   | CH0 (Tilt) | CH1 (Tilt) | CH2 (Pan) |
|-------------|------------|------------|-----------|
| Pulse MIN   | 150        | 150        | 102       |
| Pulse MAX   | 526        | 526        | 512       |
| Posisi awal | 3°         | 165°       | 90°       |
| Posisi akhir| 43°        | 125°       | -         |
| Range input | 0–40 step  | 0–40 step  | 0–180°    |

Tilt step 0 adalah posisi awal (bawah), step 40 adalah posisi akhir (atas). CH0 dan CH1 bergerak secara bersamaan dengan interpolasi.

---

## Deskripsi Program

### MekanikServo
Kontrol servo pan-tilt tanpa motor. Input perintah lewat Serial Monitor.

### GabunganServoMotor
Kontrol servo pan-tilt dan dua motor DC sekaligus. Input lewat Serial Monitor.

### ArrowControl
Versi lengkap dengan dukungan perintah arrow key dari Python. Motor A digunakan untuk reload, Motor B untuk peluncuran.

---

## Perintah Serial

### Servo

| Perintah      | Fungsi                                      |
|---------------|---------------------------------------------|
| `0` sampai `40` | Set posisi tilt langsung                  |
| `pan <0-180>` | Set posisi pan dalam derajat                |
| `spd <ms>`    | Set kecepatan gerak tilt (delay per step)   |
| `status`      | Tampilkan posisi dan panduan perintah       |

### Motor

| Perintah | Fungsi                                              |
|----------|-----------------------------------------------------|
| `R`      | Motor A maju 1 detik lalu mundur 1 detik (reload)  |
| `O`      | Motor B mundur terus (launch)                       |
| `S`      | Stop semua motor                                    |

### Arrow Key (khusus ArrowControl, dikirim dari Python)

| Kode | Fungsi              |
|------|---------------------|
| `TU` | Tilt naik 1 step    |
| `TD` | Tilt turun 1 step   |
| `PL` | Pan kiri 5°         |
| `PR` | Pan kanan 5°        |

---

## Cara Penggunaan

### Tanpa Python (Serial Monitor)

1. Buka program Arduino yang diinginkan di Arduino IDE.
2. Upload ke board.
3. Buka Serial Monitor, set baud rate ke `9600`.
4. Ketik perintah sesuai tabel di atas, kirim dengan Enter.

### Dengan Python (ArrowControl)

**Kebutuhan:**

```
pip install pyserial pynput
```

**Langkah:**

1. Upload `ArrowControl.ino` ke Arduino.
2. Jalankan `controller.py`:

```
python controller.py
```

3. Program akan mendeteksi port COM secara otomatis jika hanya ada satu port. Jika lebih dari satu, pilih nomor index yang sesuai.
4. Gunakan tombol berikut setelah program berjalan:

| Tombol      | Fungsi                          |
|-------------|---------------------------------|
| Panah Atas  | Tilt naik                       |
| Panah Bawah | Tilt turun                      |
| Panah Kiri  | Pan kiri                        |
| Panah Kanan | Pan kanan                       |
| R           | Reload (Motor A maju-mundur)    |
| O           | Launch (Motor B mundur)         |
| S           | Stop semua motor                |
| ESC         | Keluar dari program             |

---

## Library Arduino yang Dibutuhkan

- `Wire.h` (built-in)
- `Adafruit_PWMServoDriver` (install via Library Manager: "Adafruit PWM Servo Driver Library")

---

## Lisensi

MIT License. Lihat file `LICENSE`.
