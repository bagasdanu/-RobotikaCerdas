#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// ═══════════════════════════════════════════════════════
//  SERVO CONFIG
// ═══════════════════════════════════════════════════════
#define SERVO_FREQ  50

// Tilt (CH0 & CH1)
#define TILT_MIN    150
#define TILT_MAX    526
const int CH0_START = 3,   CH0_END = 43;
const int CH1_START = 165, CH1_END = 125;

// Pan (CH2)
#define PAN_CH      2
#define PAN_MIN     102
#define PAN_MAX     512
#define PAN_INIT    90

float pos0    = CH0_START;
float pos1    = CH1_START;
int   panPos  = PAN_INIT;
int   stepDelay = 15;

// ═══════════════════════════════════════════════════════
//  MOTOR CONFIG  ← dipindah ke CH3 & CH4 (hindari konflik)
// ═══════════════════════════════════════════════════════
#define PWM_FREQ    1000
#define PWM_MAX     4095

// Motor A – PCA CH3 | arah D6, D7
#define CH_A    3
#define IN1_A   6
#define IN2_A   7

// Motor B – PCA CH4 | arah D4, D5
#define CH_B    4
#define IN1_B   4
#define IN2_B   5

// ═══════════════════════════════════════════════════════
//  SERVO – FUNGSI
// ═══════════════════════════════════════════════════════
uint16_t tiltToPulse(int angle) {
  return map(constrain(angle, 0, 180), 0, 180, TILT_MIN, TILT_MAX);
}

uint16_t panToPulse(int angle) {
  return map(constrain(angle, 0, 180), 0, 180, PAN_MIN, PAN_MAX);
}

void setTilt(uint8_t ch, float angle) {
  pca.setPWM(ch, 0, tiltToPulse((int)angle));
}

void setPan(int angle) {
  pca.setPWM(PAN_CH, 0, panToPulse(angle));
  panPos = angle;
}

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

// ═══════════════════════════════════════════════════════
//  MOTOR – FUNGSI
// ═══════════════════════════════════════════════════════
void motorA_Maju(int speed) {
  digitalWrite(IN1_A, HIGH); digitalWrite(IN2_A, LOW);
  pca.setPWM(CH_A, 0, map(speed, 0, 100, 0, PWM_MAX));
}

void motorA_Mundur(int speed) {
  digitalWrite(IN1_A, LOW); digitalWrite(IN2_A, HIGH);
  pca.setPWM(CH_A, 0, map(speed, 0, 100, 0, PWM_MAX));
}

void motorA_Stop() {
  digitalWrite(IN1_A, LOW); digitalWrite(IN2_A, LOW);
  pca.setPWM(CH_A, 0, 0);
}

void motorB_Mundur(int speed) {
  digitalWrite(IN1_B, LOW); digitalWrite(IN2_B, HIGH);
  pca.setPWM(CH_B, 0, map(speed, 0, 100, 0, PWM_MAX));
}

void motorB_Stop() {
  digitalWrite(IN1_B, LOW); digitalWrite(IN2_B, LOW);
  pca.setPWM(CH_B, 0, 0);
}

void stopSemua() {
  motorA_Stop();
  motorB_Stop();
  Serial.println("[MOTOR] Semua motor berhenti.");
}

// ═══════════════════════════════════════════════════════
//  STATUS
// ═══════════════════════════════════════════════════════
void printStatus() {
  Serial.println("===========================================");
  Serial.print("  CH0 (Tilt) : "); Serial.print(pos0, 1); Serial.println("°");
  Serial.print("  CH1 (Tilt) : "); Serial.print(pos1, 1); Serial.println("°");
  Serial.print("  CH2 (Pan)  : "); Serial.print(panPos);  Serial.println("°");
  Serial.print("  Speed Servo: "); Serial.print(stepDelay); Serial.println(" ms/step");
  Serial.println("-------------------------------------------");
  Serial.println("  [SERVO]");
  Serial.println("  0-40          = tilt (0=START, 40=END)");
  Serial.println("  pan <0-180>   = gerak pan");
  Serial.println("  spd <ms>      = ubah kecepatan tilt");
  Serial.println("  status        = lihat posisi");
  Serial.println("  [MOTOR]");
  Serial.println("  R             = Motor A maju 1s → mundur 1s");
  Serial.println("  O             = Motor B mundur terus");
  Serial.println("  S             = Stop semua motor");
  Serial.println("===========================================");
}

// ═══════════════════════════════════════════════════════
//  PARSE COMMAND
// ═══════════════════════════════════════════════════════
void parseCommand(String input) {
  input.trim();
  if (input.length() == 0) return;

  // ── status ──
  if (input == "status") { printStatus(); return; }

  // ── Motor: R, O, S ──
  if (input == "R" || input == "r") {
    Serial.println("[MOTOR A] MAJU...");
    motorA_Maju(75);
    delay(1000);
    Serial.println("[MOTOR A] MUNDUR...");
    motorA_Mundur(75);
    delay(1000);
    motorA_Stop();
    Serial.println("[MOTOR A] Selesai.");
    return;
  }

  if (input == "O" || input == "o") {
    Serial.println("[MOTOR B] MUNDUR...");
    motorB_Mundur(75);
    return;
  }

  if (input == "S" || input == "s") {
    stopSemua();
    return;
  }

  // ── spd <ms> ──
  if (input.startsWith("spd ")) {
    stepDelay = constrain(input.substring(4).toInt(), 1, 1000);
    Serial.print("Speed servo: "); Serial.print(stepDelay); Serial.println(" ms");
    return;
  }

  // ── pan <degree> ──
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

  // ── tilt 0-40 ──
  bool isNumber = true;
  for (int i = 0; i < (int)input.length(); i++) {
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

// ═══════════════════════════════════════════════════════
//  SETUP & LOOP
// ═══════════════════════════════════════════════════════
void setup() {
  Serial.begin(9600);

  // Pin motor
  pinMode(IN1_A, OUTPUT); pinMode(IN2_A, OUTPUT);
  pinMode(IN1_B, OUTPUT); pinMode(IN2_B, OUTPUT);

  // PCA init – servo dulu (50Hz), motor pakai setPWM langsung
  pca.begin();
  pca.setOscillatorFrequency(27000000);
  pca.setPWMFreq(SERVO_FREQ);  // 50Hz untuk servo
  delay(100);

  // Posisi awal servo
  setTilt(0, CH0_START);
  setTilt(1, CH1_START);
  setPan(PAN_INIT);
  delay(500);

  // Matikan semua motor
  stopSemua();

  printStatus();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    parseCommand(input);
  }
}