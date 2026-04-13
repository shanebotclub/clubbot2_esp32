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

const int LEFT_A  = 35;
const int LEFT_B  = 34;
const int RIGHT_A = 36;
const int RIGHT_B = 39;

const pcnt_unit_t PCNT_LEFT_UNIT  = PCNT_UNIT_0;
const pcnt_unit_t PCNT_RIGHT_UNIT = PCNT_UNIT_1;

volatile int32_t left_total  = 0;
volatile int32_t right_total = 0;

// -------------------------
// PCNT CONFIG (fixed for Arduino 3.3.7)
// -------------------------

void setupPCNT(pcnt_unit_t unit, int pinA, int pinB) {
    pcnt_config_t config;
    memset(&config, 0, sizeof(pcnt_config_t));   // IMPORTANT for Arduino 3.x

    config.pulse_gpio_num = pinA;
    config.ctrl_gpio_num  = pinB;
    config.channel        = PCNT_CHANNEL_0;
    config.unit           = unit;

    config.pos_mode       = PCNT_COUNT_INC;
    config.neg_mode       = PCNT_COUNT_DEC;
    config.lctrl_mode     = PCNT_MODE_REVERSE;
    config.hctrl_mode     = PCNT_MODE_KEEP;

    config.counter_h_lim  = 32767;
    config.counter_l_lim  = -32768;

    pcnt_unit_config(&config);

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

  for (int i = 0; i < NUM_BUMPERS; i++) {
    pinMode(bumpers[i].pin, INPUT_PULLUP);
    bumpers[i].lastState = digitalRead(bumpers[i].pin);
  }

  pinMode(LEFT_A,  INPUT);
  pinMode(LEFT_B,  INPUT);
  pinMode(RIGHT_A, INPUT);
  pinMode(RIGHT_B, INPUT);

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
      Serial.print(bumpers[i].lastState == LOW ? 1 : 0);
      if (i < NUM_BUMPERS - 1) Serial.print(" ");
    }
    Serial.println();
  }

  unsigned long now = millis();
  if (now - lastEncPrint >= 100) {
    lastEncPrint = now;

    int16_t left_raw  = 0;
    int16_t right_raw = 0;

    pcnt_get_counter_value(PCNT_LEFT_UNIT,  &left_raw);
    pcnt_get_counter_value(PCNT_RIGHT_UNIT, &right_raw);

    left_total  += left_raw;
    right_total += right_raw;

    pcnt_counter_clear(PCNT_LEFT_UNIT);
    pcnt_counter_clear(PCNT_RIGHT_UNIT);

    Serial.print("ENC left=");
    Serial.print(left_total);
    Serial.print(" right=");
    Serial.print(right_total);
    Serial.println();
  }

  delay(10);
}
