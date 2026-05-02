#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

#define SERVO_FREQ 50

// Tilt (CH0 & CH1)
#define TILT_MIN 150
#define TILT_MAX 526
const int CH0_START = 3,   CH0_END = 43;
const int CH1_START = 165, CH1_END = 125;

// Pan (CH2)
#define PAN_CH    2
#define PAN_MIN   102
#define PAN_MAX   512
#define PAN_INIT  90

float pos0 = CH0_START;
float pos1 = CH1_START;
int   panPos   = PAN_INIT;
int   stepDelay = 15;

// ── Konversi pulse ────────────────────────────────────────────
uint16_t tiltToPulse(int angle) {
  return map(constrain(angle, 0, 180), 0, 180, TILT_MIN, TILT_MAX);
}

uint16_t panToPulse(int angle) {
  return map(constrain(angle, 0, 180), 0, 180, PAN_MIN, PAN_MAX);
}

// ── Set servo ─────────────────────────────────────────────────
void setTilt(uint8_t ch, float angle) {
  pca.setPWM(ch, 0, tiltToPulse((int)angle));
}

void setPan(int angle) {
  pca.setPWM(PAN_CH, 0, panToPulse(angle));
  panPos = angle;
}

// ── Gerak tilt dengan interpolasi ─────────────────────────────
void moveTo(float target0, float target1) {
  float from0 = pos0, from1 = pos1;
  int steps = max(abs(target0 - from0), abs(target1 - from1));
  if (steps == 0) return;

  for (int i = 1; i <= steps; i++) {
    float ratio = (float)i / steps;
    setTilt(0, from0 + ratio * (target0 - from0));
    setTilt(1, from1 + ratio * (target1 - from1));
    delay(stepDelay);
  }
  pos0 = target0;
  pos1 = target1;
}

// ── Status ────────────────────────────────────────────────────
void printStatus() {
  Serial.println("=================================");
  Serial.print("  CH0 (Tilt): "); Serial.print(pos0, 1); Serial.println("°");
  Serial.print("  CH1 (Tilt): "); Serial.print(pos1, 1); Serial.println("°");
  Serial.print("  CH2 (Pan) : "); Serial.print(panPos);  Serial.println("°");
  Serial.print("  Speed     : "); Serial.print(stepDelay); Serial.println(" ms/step");
  Serial.println("---------------------------------");
  Serial.println("  Perintah:");
  Serial.println("  0-40          = tilt (0=START, 40=END)");
  Serial.println("  pan <0-180>   = gerak pan");
  Serial.println("  spd <ms>      = ubah kecepatan tilt");
  Serial.println("  status        = lihat posisi");
  Serial.println("=================================");
}

// ── Parse command ─────────────────────────────────────────────
void parseCommand(String input) {
  input.trim();
  if (input.length() == 0) return;

  // status
  if (input == "status") {
    printStatus();
    return;
  }

  // spd <ms>
  if (input.startsWith("spd ")) {
    stepDelay = constrain(input.substring(4).toInt(), 1, 1000);
    Serial.print("Speed set ke: "); Serial.print(stepDelay); Serial.println(" ms");
    return;
  }

  // pan <degree>
  if (input.startsWith("pan ")) {
    int deg = input.substring(4).toInt();
    if (deg >= 0 && deg <= 180) {
      setPan(deg);
      Serial.print("[PAN] CH2: "); Serial.print(deg); Serial.println("°");
    } else {
      Serial.println("Pan tidak valid. Masukkan 0-180.");
    }
    return;
  }

  // tilt 0-40 (validasi numerik)
  bool isNumber = true;
  for (int i = 0; i < input.length(); i++) {
    if (!isDigit(input[i])) { isNumber = false; break; }
  }

  if (isNumber) {
    int cmd = input.toInt();
    if (cmd >= 0 && cmd <= 40) {
      float t0 = map(cmd, 0, 40, CH0_START, CH0_END);
      float t1 = map(cmd, 0, 40, CH1_START, CH1_END);
      Serial.print("[TILT "); Serial.print(cmd); Serial.println("]");
      Serial.print("  CH0: "); Serial.print(pos0, 1); Serial.print("° -> "); Serial.print(t0, 1); Serial.println("°");
      Serial.print("  CH1: "); Serial.print(pos1, 1); Serial.print("° -> "); Serial.print(t1, 1); Serial.println("°");
      moveTo(t0, t1);
    } else {
      Serial.println("Tilt tidak valid. Masukkan 0-40.");
    }
    return;
  }

  Serial.println("Perintah tidak dikenal.");
}

// ── Setup & Loop ──────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  pca.begin();
  pca.setOscillatorFrequency(27000000);
  pca.setPWMFreq(SERVO_FREQ);
  delay(100);

  setTilt(0, CH0_START);
  setTilt(1, CH1_START);
  setPan(PAN_INIT);
  delay(500);

  printStatus();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    parseCommand(input);
  }
}