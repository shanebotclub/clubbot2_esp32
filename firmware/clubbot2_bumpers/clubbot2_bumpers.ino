#include <Arduino.h>

// Bumper set up
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

// Encoder pins
const int LEFT_A = 34;
const int LEFT_B = 35;
const int RIGHT_A = 36;
const int RIGHT_B = 39;

// Encoder counters
volatile long left_forward = 0;
volatile long left_backward = 0;
volatile long right_forward = 0;
volatile long right_backward = 0;


void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM; i++) {
    pinMode(bumpers[i].pin, INPUT_PULLUP);
    bumpers[i].lastState = digitalRead(bumpers[i].pin);
  }
}

void loop() {
  bool changed = false;

  for (int i = 0; i < NUM; i++) {
    int state = digitalRead(bumpers[i].pin);
    if (state != bumpers[i].lastState) {
      bumpers[i].lastState = state;
      changed = true;
    }
  }

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

  delay(10);
}
