#include <Arduino.h>
#include "driver/pcnt.h"

// -------------------------
// BUMPER SETUP
// -------------------------

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

// -------------------------
// ENCODER PINS (ESP32)
// -------------------------
// Left encoder:  A = 35, B = 34
// Right encoder: A = 36, B = 39

const int LEFT_A  = 35;
const int LEFT_B  = 34;
const int RIGHT_A = 36;
const int RIGHT_B = 39;

// Use PCNT unit 0 for left, unit 1 for right
const pcnt_unit_t PCNT_LEFT_UNIT  = PCNT_UNIT_0;
const pcnt_unit_t PCNT_RIGHT_UNIT = PCNT_UNIT_1;

// Cumulative 32-bit counts (what you care about for ROS)
volatile int32_t left_total  = 0;
volatile int32_t right_total = 0;

// -------------------------
// PCNT CONFIG
// -------------------------

void setupPCNT(pcnt_unit_t unit, int pinA, int pinB) {
  pcnt_config_t config = {
    .pulse_gpio_num = pinA,          // encoder A
    .ctrl_gpio_num  = pinB,          // encoder B
    .channel        = PCNT_CHANNEL_0,
    .unit           = unit,
    // Quadrature: count up on one direction, down on the other
    .pos_mode       = PCNT_COUNT_INC,   // rising edge
    .neg_mode       = PCNT_COUNT_DEC,   // falling edge
    .lctrl_mode     = PCNT_MODE_REVERSE,
    .hctrl_mode     = PCNT_MODE_KEEP,
    .counter_h_lim  = 32767,
    .counter_l_lim  = -32768
  };

  pcnt_unit_config(&config);

  // Optional: input filter to ignore glitches (in APB clock cycles)
  pcnt_set_filter_value(unit, 100);
  pcnt_filter_enable(unit);

  pcnt_counter_pause(unit);
  pcnt_counter_clear(unit);
  pcnt_counter_resume(unit);
}

// -------------------------
// SETUP
// -------------------------

void setup() {
  Serial.begin(115200);
  delay(100);

  // Bumpers as INPUT_PULLUP
  for (int i = 0; i < NUM_BUMPERS; i++) {
    pinMode(bumpers[i].pin, INPUT_PULLUP);
    bumpers[i].lastState = digitalRead(bumpers[i].pin);
  }

  // Encoder pins as inputs (PCNT will use them)
  pinMode(LEFT_A,  INPUT);
  pinMode(LEFT_B,  INPUT);
  pinMode(RIGHT_A, INPUT);
  pinMode(RIGHT_B, INPUT);

  // Configure PCNT units
  setupPCNT(PCNT_LEFT_UNIT,  LEFT_A,  LEFT_B);
  setupPCNT(PCNT_RIGHT_UNIT, RIGHT_A, RIGHT_B);

  Serial.println("ESP32 bumpers + PCNT encoders started");
}

// -------------------------
// LOOP
// -------------------------

void loop() {
  static unsigned long lastEncPrint = 0;
  bool bump_changed = false;

  // -------------------------
  // BUMPERS: detect changes
  // -------------------------
  for (int i = 0; i < NUM_BUMPERS; i++) {
    int state = digitalRead(bumpers[i].pin);
    if (state != bumpers[i].lastState) {
      bumpers[i].lastState = state;
      bump_changed = true;
    }
  }

  if (bump_changed) {
    Serial.print("BUMP ");
    for (int i = 0; i < NUM_BUMPERS; i++) {
      Serial.print(bumpers[i].name);
      Serial.print("=");
      // Active LOW → pressed = 1
      Serial.print(bumpers[i].lastState == LOW ? 1 : 0);
      if (i < NUM_BUMPERS - 1) Serial.print(" ");
    }
    Serial.println();
  }

  // -------------------------
  // ENCODERS: read PCNT every 100 ms
  // -------------------------
  unsigned long now = millis();
  if (now - lastEncPrint >= 100) {
    lastEncPrint = now;

    int16_t left_raw  = 0;
    int16_t right_raw = 0;

    // Read current PCNT values
    pcnt_get_counter_value(PCNT_LEFT_UNIT,  &left_raw);
    pcnt_get_counter_value(PCNT_RIGHT_UNIT, &right_raw);

    // Accumulate into 32-bit totals
    left_total  += left_raw;
    right_total += right_raw;

    // Clear hardware counters so next read is delta
    pcnt_counter_clear(PCNT_LEFT_UNIT);
    pcnt_counter_clear(PCNT_RIGHT_UNIT);

    // Serial output: cumulative counts (int32)
    Serial.print("ENC ");
    Serial.print("left=");
    Serial.print(left_total);
    Serial.print(" right=");
    Serial.print(right_total);
    Serial.println();
  }

  delay(10);
}
