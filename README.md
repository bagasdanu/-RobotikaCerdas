Kumpulan program untuk sistem pan-tilt launcher.

---

## Hardware

| Komponen        | Keterangan                              |
|-----------------|-----------------------------------------|
| Arduino Mega    | Mikrokontroler utama                    |
| PCA9685         | PWM driver, I2C address 0x40            |
| SG90 x2         | Servo tilt, CH0 dan CH1 pada PCA9685    |
| MG996R x1       | Servo pan, CH2 pada PCA9685             |
| L298N           | Driver motor DC                         |
| Motor A (reload)| PWM CH3 PCA9685, arah pin D7 dan D6    |
| Motor B (launch)| PWM CH4 PCA9685, arah pin D4 dan D5    |

---

## Konfigurasi Servo

| Parameter    | CH0 (Tilt) | CH1 (Tilt) | CH2 (Pan) |
|--------------|------------|------------|-----------|
| Pulse MIN    | 150        | 150        | 102       |
| Pulse MAX    | 526        | 526        | 512       |
| Posisi awal  | 3°         | 165°       | 90°       |
| Posisi akhir | 43°        | 125°       | -         |
| Range input  | 0–40 step  | 0–40 step  | 0–180°    |

Tilt step 0 adalah posisi awal, step 40 adalah posisi akhir. CH0 dan CH1 bergerak bersamaan dengan interpolasi linier.

---

## Struktur Repositori

```
-RobotikaCerdas/
├── ArrowControl/
│   ├── ArrowControl.ino
│   └── ArrowControl.py
├── MekanikGabungan/
│   └── MekanikGabungan.ino
├── MekanikMotorDC/
│   └── MekanikMotorDC.ino
└── MekanikServo/
    └── MekanikServo.ino
```

---

## Wiring

### Koneksi Umum (Berlaku untuk Semua Program)

**PCA9685 ke Arduino Mega**

| PCA9685 | Arduino Mega                              |
|---------|-------------------------------------------|
| VCC     | 5V                                        |
| GND     | GND                                       |
| SDA     | Pin 20 (SDA)                              |
| SCL     | Pin 21 (SCL)                              |
| V+      | Power supply eksternal 5–6V (untuk servo) |

**Servo ke PCA9685**

| Servo           | Channel PCA9685 | VCC        | GND |
|-----------------|-----------------|------------|-----|
| SG90 #1 (Tilt)  | CH0             | V+ PCA9685 | GND |
| SG90 #2 (Tilt)  | CH1             | V+ PCA9685 | GND |
| MG996R (Pan)    | CH2             | V+ PCA9685 | GND |

> MG996R disarankan menggunakan power supply terpisah 5–6V langsung ke pin V+ PCA9685. Jangan mengandalkan 5V dari Arduino untuk MG996R.

---

### MekanikServo

Hanya menggunakan servo. Tidak ada koneksi motor atau L298N. Gunakan tabel koneksi umum di atas.

---

### MekanikMotorDC

Hanya menggunakan motor DC. Servo tidak digunakan.

**L298N ke Arduino Mega dan PCA9685**

| L298N  | Terhubung ke       | Keterangan                    |
|--------|--------------------|-------------------------------|
| ENA    | PCA9685 CH3 output | PWM kecepatan Motor A         |
| IN1    | Arduino Pin D6     | Arah Motor A                  |
| IN2    | Arduino Pin D7     | Arah Motor A                  |
| ENB    | PCA9685 CH4 output | PWM kecepatan Motor B         |
| IN3    | Arduino Pin D4     | Arah Motor B                  |
| IN4    | Arduino Pin D5     | Arah Motor B                  |
| VCC    | Power supply 12V   | Sumber daya motor             |
| GND    | GND (common)       | Ground bersama dengan Arduino |

---

### MekanikGabungan

Gabungan servo dan motor. Koneksi servo sama dengan tabel koneksi umum.

**L298N ke Arduino Mega dan PCA9685**

| L298N  | Terhubung ke       | Keterangan                    |
|--------|--------------------|-------------------------------|
| ENA    | PCA9685 CH3 output | PWM kecepatan Motor A (reload)|
| IN1    | Arduino Pin D6     | Arah Motor A                  |
| IN2    | Arduino Pin D7     | Arah Motor A                  |
| ENB    | PCA9685 CH4 output | PWM kecepatan Motor B (launch)|
| IN3    | Arduino Pin D4     | Arah Motor B                  |
| IN4    | Arduino Pin D5     | Arah Motor B                  |
| VCC    | Power supply 12V   | Sumber daya motor             |
| GND    | GND (common)       | Ground bersama dengan Arduino |

---

### ArrowControl

Koneksi servo sama dengan tabel koneksi umum. Koneksi motor berbeda dari MekanikGabungan pada pin IN1 dan IN2 Motor A.

**L298N ke Arduino Mega dan PCA9685**

| L298N  | Terhubung ke       | Keterangan                    |
|--------|--------------------|-------------------------------|
| ENA    | PCA9685 CH3 output | PWM kecepatan Motor A (reload)|
| IN1    | Arduino Pin D7     | Arah Motor A                  |
| IN2    | Arduino Pin D6     | Arah Motor A                  |
| ENB    | PCA9685 CH4 output | PWM kecepatan Motor B (launch)|
| IN3    | Arduino Pin D4     | Arah Motor B                  |
| IN4    | Arduino Pin D5     | Arah Motor B                  |
| VCC    | Power supply 12V   | Sumber daya motor             |
| GND    | GND (common)       | Ground bersama dengan Arduino |

---

### Catatan Ground

Semua komponen (Arduino, PCA9685, L298N, power supply eksternal) harus memiliki GND yang tersambung ke titik yang sama. Jika tidak, sinyal PWM dan arah motor tidak akan terbaca dengan benar.

---

## Deskripsi Program

### MekanikServo
Kontrol servo pan-tilt tanpa motor. Input perintah lewat Serial Monitor Arduino IDE.

### MekanikMotorDC
Kontrol dua motor DC tanpa servo. Input perintah lewat Serial Monitor Arduino IDE.

### MekanikGabungan
Kontrol servo pan-tilt dan dua motor DC sekaligus. Input lewat Serial Monitor Arduino IDE.

### ArrowControl
Versi lengkap dengan dukungan input keyboard real-time dari Python lewat komunikasi serial. Motor A untuk reload, Motor B untuk peluncuran.

---

## Perintah Serial

### Servo

| Perintah       | Fungsi                                    |
|----------------|-------------------------------------------|
| `0` sampai `40`| Set posisi tilt langsung                  |
| `pan <0-180>`  | Set posisi pan dalam derajat              |
| `spd <ms>`     | Set kecepatan gerak tilt (delay per step) |
| `status`       | Tampilkan posisi dan panduan perintah     |

### Motor

| Perintah | Fungsi                                            |
|----------|---------------------------------------------------|
| `R`      | Motor A maju 1 detik lalu mundur 1 detik (reload) |
| `O`      | Motor B mundur terus (launch)                     |
| `S`      | Stop semua motor                                  |

### Arrow Key (khusus ArrowControl, dikirim dari Python)

| Kode | Fungsi           |
|------|------------------|
| `TU` | Tilt naik 2 step |
| `TD` | Tilt turun 2 step|
| `PL` | Pan kiri 2°      |
| `PR` | Pan kanan 2°     |

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
2. Jalankan `ArrowControl.py`:

```
python ArrowControl.py
```

3. Program akan mendeteksi port COM secara otomatis jika hanya ada satu port. Jika lebih dari satu, pilih nomor index yang sesuai.
4. Gunakan tombol berikut setelah program berjalan:

| Tombol      | Fungsi                              |
|-------------|-------------------------------------|
| Panah Atas  | Tilt naik                           |
| Panah Bawah | Tilt turun                          |
| Panah Kiri  | Pan kiri                            |
| Panah Kanan | Pan kanan                           |
| R           | Reload (Motor A maju lalu mundur)   |
| O           | Launch (Motor B mundur)             |
| S           | Stop semua motor                    |
| ESC         | Keluar dari program                 |

---

## Library Arduino yang Dibutuhkan

- `Wire.h` (built-in)
- `Adafruit_PWMServoDriver` (install via Library Manager: "Adafruit PWM Servo Driver Library")

---

## Lisensi

MIT License. Lihat file `LICENSE`.
