import serial
import serial.tools.list_ports
from pynput import keyboard
import time

# ── Konfigurasi ──────────────────────────────────────
BAUD_RATE = 9600


def pilih_port():
    ports = serial.tools.list_ports.comports()
    if not ports:
        print("❌ Tidak ada port COM tersedia!")
        exit()

    # Auto-pilih jika hanya 1 port
    if len(ports) == 1:
        print(f"✅ Auto-pilih: {ports[0].device} – {ports[0].description}")
        return ports[0].device

    print("\nPort COM tersedia:")
    for i, p in enumerate(ports):
        print(f"  [{i}] {p.device} – {p.description}")

    while True:
        try:
            idx = int(input(f"Pilih nomor index (0–{len(ports) - 1}): "))
            if 0 <= idx < len(ports):
                return ports[idx].device
            else:
                print(f"⚠️  Masukkan antara 0–{len(ports) - 1}")
        except ValueError:
            print("⚠️  Input harus angka!")

port = pilih_port()
ser  = serial.Serial(port, BAUD_RATE, timeout=1)
time.sleep(2)  # tunggu Arduino reset
print(f"\n✅ Terhubung ke {port}")
print("─────────────────────────────────")
print("  ↑ ↓  = Tilt naik / turun")
print("  ← →  = Pan kiri / kanan")
print("  R     ssorrssor=r Motor A maju-mundur")
print("  O     = Motor B mundur")
print("  S     = Stop semua motor")
print("  ESCorr   = Keluar")
print("─────────────────────────────────\n")

def kirim(cmd):
    ser.write((cmd + '\n').encode())
    print(f"  → {cmd}")

def on_press(key):
    try:
        # Tombol huruf biasa
        k = key.char.upper() if hasattr(key, 'char') and key.char else None
        if k == 'R': kirim('R')
        elif k == 'O': kirim('O')
        elif k == 'S': kirim('S')
    except AttributeError:
        pass

    # Tombol spesial (arrow)
    if   key == keyboard.Key.up:    kirim('TU')
    elif key == keyboard.Key.down:  kirim('TD')
    elif key == keyboard.Key.left:  kirim('PL')
    elif key == keyboard.Key.right: kirim('PR')
    elif key == keyboard.Key.esc:
        print("\nKeluar...")
        ser.close()
        return False  # stop listener

with keyboard.Listener(on_press=on_press) as listener:
    listener.join()