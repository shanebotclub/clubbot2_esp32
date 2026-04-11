#include <Arduino.h>
#include "driver/pcnt.h"

// -----------------------------
// BUMPER SETUP
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

const int NUM = sizeof(bumpers) / sizeof(bumpers[0]);

// -----------------------------
// ENCODER PINS
// -----------------------------
const int LEFT_A  = 34;
const int LEFT_B  = 35;
const int RIGHT_A = 36;
const int RIGHT_B = 39;

// -----------------------------
// PCNT CONFIGURATION FUNCTION
// -----------------------------
void setupPCNT(pcnt_unit_t unit, int pinA, int pinB) {
    pcnt_config_t pcnt_config;

    pcnt_config.pulse_gpio_num = pinA;     // Encoder A
    pcnt_config.ctrl_gpio_num  = pinB;     // Encoder B
    pcnt_config.channel        = PCNT_CHANNEL_0;
    pcnt_config.unit           = unit;

    pcnt_config.pos_mode       = PCNT_COUNT_INC;   // Rising edge counts up
    pcnt_config.neg_mode       = PCNT_COUNT_DIS;   // Ignore falling edge
    pcnt_config.lctrl_mode     = PCNT_MODE_REVERSE; // B = LOW → reverse
    pcnt_config.hctrl_mode     = PCNT_MODE_KEEP;    // B = HIGH → forward

    pcnt_config.counter_h_lim  = 32767;
    pcnt_config.counter_l_lim  = -32768;

    pcnt_unit_config(&pcnt_config);

    // Optional noise filter (recommended)
    pcnt_set_filter_value(unit, 100);
    pcnt_filter_enable(unit);

    pcnt_counter_pause(unit);
    pcnt_counter_clear(unit);
    pcnt_counter_resume(unit);
}

// -----------------------------
// SETUP
// -----------------------------
void setup() {
  Serial.begin(115200);

  // Bumpers
  for (int i = 0; i < NUM; i++) {
    pinMode(bumpers[i].pin, INPUT_PULLUP);
    bumpers[i].lastState = digitalRead(bumpers[i].pin);
  }

  // Encoders using PCNT hardware
  setupPCNT(PCNT_UNIT_0, LEFT_A, LEFT_B);
  setupPCNT(PCNT_UNIT_1, RIGHT_A, RIGHT_B);
}

// -----------------------------
// LOOP
// -----------------------------
void loop() {
  bool changed = false;

  // Check bumpers
  for (int i = 0; i < NUM; i++) {
    int state = digitalRead(bumpers[i].pin);
    if (state != bumpers[i].lastState) {
      bumpers[i].lastState = state;
      changed = true;
    }
  }

  // Print bumper message
  if (changed) {
    Serial.print("BUMP ");
    for (int i = 0; i < NUM; i++) {
      Serial.print(bumpers[i].name);
      Serial.print("=");
      Serial.print(bumpers[i].lastState == LOW ? 1 : 0);
      if (i < NUM - 1) Serial.print(" ");
    }
    Serial.println();
  }

  // Read PCNT encoder values every 100ms
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 100) {
    lastPrint = millis();

    int16_t left_count = 0;
    int16_t right_count = 0;

    pcnt_get_counter_value(PCNT_UNIT_0, &left_count);
    pcnt_get_counter_value(PCNT_UNIT_1, &right_count);

    Serial.print("ENC left=");
    Serial.print(left_count);
    Serial.print(" right=");
    Serial.println(right_count);
  }

  delay(10);
}
