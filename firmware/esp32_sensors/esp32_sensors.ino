#include <Wire.h>
#include <MPU6050_light.h>
#include "driver/pcnt.h"

// -----------------------------
// BUMPERS
// -----------------------------
struct Bumper {
  const char* name;
  uint8_t pin;
  int lastState;
};

Bumper bumpers[] = {
  {"lf", 13, HIGH},
  {"mf", 33, HIGH},
  {"rf", 32, HIGH},
  {"lb", 18, HIGH},
  {"mb", 19, HIGH},
  {"rb", 21, HIGH}
};

const int NUM_BUMPERS = sizeof(bumpers) / sizeof(bumpers[0]);

// -----------------------------
// ENCODERS (PCNT)
// -----------------------------
#define PCNT_LF PCNT_UNIT_0
#define PCNT_LB PCNT_UNIT_1
#define PCNT_RF PCNT_UNIT_2
#define PCNT_RB PCNT_UNIT_3

// Left encoder pins
#define LEFT_ENC_A 34
#define LEFT_ENC_B 35

// Right encoder pins
#define RIGHT_ENC_A 36
#define RIGHT_ENC_B 39

int16_t cntLF = 0, cntLB = 0, cntRF = 0, cntRB = 0;

void setupPCNT(pcnt_unit_t unit, int pinA, int pinB) {
  pcnt_config_t pcnt_config = {};
  pcnt_config.pulse_gpio_num = pinA;
  pcnt_config.ctrl_gpio_num = pinB;
  pcnt_config.channel = PCNT_CHANNEL_0;
  pcnt_config.unit = unit;
  pcnt_config.pos_mode = PCNT_COUNT_INC;
  pcnt_config.neg_mode = PCNT_COUNT_DEC;
  pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
  pcnt_config.counter_h_lim = 32767;
  pcnt_config.counter_l_lim = -32768;

  pcnt_unit_config(&pcnt_config);
  pcnt_counter_pause(unit);
  pcnt_counter_clear(unit);
  pcnt_counter_resume(unit);
}

// -----------------------------
// MPU6050
// -----------------------------
MPU6050* mpu;

// -----------------------------
// SETUP
// -----------------------------
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("SETUP START");

  // Bumpers
  for (int i = 0; i < NUM_BUMPERS; i++) {
    pinMode(bumpers[i].pin, INPUT_PULLUP);
    bumpers[i].lastState = digitalRead(bumpers[i].pin);
  }

  // Encoders
  setupPCNT(PCNT_LF, LEFT_ENC_A, LEFT_ENC_B);
  setupPCNT(PCNT_LB, LEFT_ENC_B, LEFT_ENC_A);
  setupPCNT(PCNT_RF, RIGHT_ENC_A, RIGHT_ENC_B);
  setupPCNT(PCNT_RB, RIGHT_ENC_B, RIGHT_ENC_A);

  // IMU
  Wire.begin(21, 22);
  mpu = new MPU6050(Wire);
  byte status = mpu->begin();
  Serial.print("MPU status: ");
  Serial.println(status);

  Serial.println("SETUP DONE");
}

// -----------------------------
// LOOP
// -----------------------------
unsigned long lastEnc = 0;
unsigned long lastIMU = 0;

void loop() {

  // -----------------------------
  // BUMPERS (change only)
  // -----------------------------
  bool changed = false;
  for (int i = 0; i < NUM_BUMPERS; i++) {
    int state = digitalRead(bumpers[i].pin);
    if (state != bumpers[i].lastState) {
      bumpers[i].lastState = state;
      changed = true;
    }
  }

  if (changed) {
    Serial.print("BUMP ");
    for (int i = 0; i < NUM_BUMPERS; i++) {
      Serial.print(bumpers[i].name);
      Serial.print("=");
      Serial.print(bumpers[i].lastState == LOW ? 1 : 0);
      if (i < NUM_BUMPERS - 1) Serial.print(" ");
    }
    Serial.println();
  }

  // ----------------