#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("I2C Scanner starting...");
  Wire.begin(21, 22);
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      Serial.println(address, HEX);
      nDevices++;
    }
  }

  if (nDevices == 0) Serial.println("No I2C devices found");
  Serial.println();
  delay(2000);
}