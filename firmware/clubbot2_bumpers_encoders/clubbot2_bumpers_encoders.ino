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

// Interrupt Service Routine (ISR) for left encoder 
void IRAM_ATTR leftEncoderISR() {
  int a = digitalRead(LEFT_A);
  int b = digitalRead(LEFT_B);

  if (a == b)
    left_forward++;
  else
    left_backward++;
}

// ISR for right encoder
void IRAM_ATTR rightEncoderISR() {
  int a = digitalRead(RIGHT_A);
  int b = digitalRead(RIGHT_B);

  if (a == b)
    right_forward++;
  else
    right_backward++;
}



void setup() {
  Serial.begin(115200);
  // Bumpers
  for (int i = 0; i < NUM; i++) {
    pinMode(bumpers[i].pin, INPUT_PULLUP);
    bumpers[i].lastState = digitalRead(bumpers[i].pin);
  }

  // Encoders
  pinMode(LEFT_A, INPUT);
  pinMode(LEFT_B, INPUT);
  pinMode(RIGHT_A, INPUT);
  pinMode(RIGHT_B, INPUT);

  attachInterrupt(digitalPinToInterrupt(LEFT_A), leftEncoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_A), rightEncoderISR, CHANGE);
}

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

  // Print encoder counts every 100ms
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 100) {
    lastPrint = millis();

    Serial.print("ENC ");
    Serial.print("left_forward=");
    Serial.print(left_forward);
    Serial.print(" left_backward=");
    Serial.print(left_backward);
    Serial.print(" right_forward=");
    Serial.print(right_forward);
    Serial.print(" right_backward=");
    Serial.print(right_backward);
    Serial.println();
  }

  delay(10);
}
