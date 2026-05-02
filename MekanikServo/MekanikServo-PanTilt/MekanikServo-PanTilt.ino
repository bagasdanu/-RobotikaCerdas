#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

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

float pos0      = CH0_START;
float pos1      = CH1_START;
int   panPos    = PAN_INIT;
int   stepDelay = 15;

// ── Tracker posisi step ──────────────────────────────
int   tiltStep  = 0;    // 0–40
int   panStep   = PAN_INIT; // 0–180

// ── Motor ────────────────────────────────────────────
#define PWM_FREQ  1000
#define PWM_MAX   4095
#define CH_A      3
#define IN1_A     6
#define IN2_A     7
#define CH_B      4
#define IN1_B     4
#define IN2_B     5

// ══════════════════════════════════════════════════════
//  SERVO FUNGSI
// ══════════════════════════════════════════════════════
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
  panStep = constrain(angle, 0, 180);
  pca.setPWM(PAN_CH, 0, panToPulse(panStep));
  panPos = panStep;
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

// ── Gerak step tilt ───────────────────────────────────
void doTilt(int cmd) {
  tiltStep = constrain(cmd, 0, 40);
  float t0 = map(tiltStep, 0, 40, CH0_START, CH0_END);
  float t1 = map(tiltStep, 0, 40, CH1_START, CH1_END);
  moveTo(t0, t1);
  Serial.print("[TILT] step="); Serial.print(tiltStep);
  Serial.print(" | pan="); Serial.println(panPos);
}

// ══════════════════════════════════════════════════════
//  MOTOR FUNGSI
// ══════════════════════════════════════════════════════
void motorA_Maju(int speed)   { digitalWrite(IN1_A,HIGH); digitalWrite(IN2_A,LOW);  pca.setPWM(CH_A,0,map(speed,0,100,0,PWM_MAX)); }
void motorA_Mundur(int speed) { digitalWrite(IN1_A,LOW);  digitalWrite(IN2_A,HIGH); pca.setPWM(CH_A,0,map(speed,0,100,0,PWM_MAX)); }
void motorA_Stop()            { digitalWrite(IN1_A,LOW);  digitalWrite(IN2_A,LOW);  pca.setPWM(CH_A,0,0); }
void motorB_Mundur(int speed) { digitalWrite(IN1_B,LOW);  digitalWrite(IN2_B,HIGH); pca.setPWM(CH_B,0,map(speed,0,100,0,PWM_MAX)); }
void motorB_Stop()            { digitalWrite(IN1_B,LOW);  digitalWrite(IN2_B,LOW);  pca.setPWM(CH_B,0,0); }
void stopSemua()              { motorA_Stop(); motorB_Stop(); Serial.println("[MOTOR] Stop."); }

// ══════════════════════════════════════════════════════
//  STATUS & PARSE
// ══════════════════════════════════════════════════════
void printStatus() {
  Serial.println("===========================================");
  Serial.print("  Tilt Step : "); Serial.print(tiltStep); Serial.println(" (0-40)");
  Serial.print("  CH0 angle : "); Serial.print(pos0, 1);  Serial.println("°");
  Serial.print("  CH1 angle : "); Serial.print(pos1, 1);  Serial.println("°");
  Serial.print("  Pan (CH2) : "); Serial.print(panPos);   Serial.println("°");
  Serial.print("  Speed     : "); Serial.print(stepDelay); Serial.println(" ms/step");
  Serial.println("-------------------------------------------");
  Serial.println("  [SERVO via arrow key / manual]");
  Serial.println("  UP / DOWN    = tilt naik / turun (step 1)");
  Serial.println("  LEFT / RIGHT = pan kiri / kanan (step 5°)");
  Serial.println("  0-40         = set tilt langsung");
  Serial.println("  pan <0-180>  = set pan langsung");
  Serial.println("  spd <ms>     = ubah kecepatan tilt");
  Serial.println("  [MOTOR]");
  Serial.println("  R = Motor A maju-mundur | O = Motor B mundur | S = Stop");
  Serial.println("===========================================");
}

void parseCommand(String input) {
  input.trim();
  if (input.length() == 0) return;

  if (input == "status") { printStatus(); return; }

  // ── Arrow key commands (dikirim Python) ─────────────
  if (input == "TU") { doTilt(tiltStep + 1); return; }   // Tilt Up
  if (input == "TD") { doTilt(tiltStep - 1); return; }   // Tilt Down
  if (input == "PL") { setPan(panStep - 5);  Serial.print("[PAN] "); Serial.println(panPos); return; }  // Pan Left
  if (input == "PR") { setPan(panStep + 5);  Serial.print("[PAN] "); Serial.println(panPos); return; }  // Pan Right

  // ── Motor ────────────────────────────────────────────
  if (input == "R" || input == "r") {
    Serial.println("[MOTOR A] MAJU...");
    motorA_Maju(75); delay(1000);
    Serial.println("[MOTOR A] MUNDUR...");
    motorA_Mundur(75); delay(1000);
    motorA_Stop(); Serial.println("[MOTOR A] Selesai.");
    return;
  }
  if (input == "O" || input == "o") { motorB_Mundur(75); Serial.println("[MOTOR B] MUNDUR..."); return; }
  if (input == "S" || input == "s") { stopSemua(); return; }

  // ── spd ──────────────────────────────────────────────
  if (input.startsWith("spd ")) {
    stepDelay = constrain(input.substring(4).toInt(), 1, 1000);
    Serial.print("Speed: "); Serial.print(stepDelay); Serial.println(" ms");
    return;
  }

  // ── pan <deg> ─────────────────────────────────────────
  if (input.startsWith("pan ")) {
    int deg = input.substring(4).toInt();
    if (deg >= 0 && deg <= 180) { setPan(deg); Serial.print("[PAN] "); Serial.println(panPos); }
    else Serial.println("Pan tidak valid (0-180).");
    return;
  }

  // ── tilt 0-40 ─────────────────────────────────────────
  bool isNum = true;
  for (int i = 0; i < (int)input.length(); i++) if (!isDigit(input[i])) { isNum = false; break; }
  if (isNum) {
    int cmd = input.toInt();
    if (cmd >= 0 && cmd <= 40) doTilt(cmd);
    else Serial.println("Tilt tidak valid (0-40).");
    return;
  }

  Serial.println("Perintah tidak dikenal.");
}

void setup() {
  Serial.begin(9600);
  pinMode(IN1_A,OUTPUT); pinMode(IN2_A,OUTPUT);
  pinMode(IN1_B,OUTPUT); pinMode(IN2_B,OUTPUT);

  pca.begin();
  pca.setOscillatorFrequency(27000000);
  pca.setPWMFreq(SERVO_FREQ);
  delay(100);

  setTilt(0, CH0_START);
  setTilt(1, CH1_START);
  setPan(PAN_INIT);
  delay(500);
  stopSemua();
  printStatus();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    parseCommand(input);
  }
}