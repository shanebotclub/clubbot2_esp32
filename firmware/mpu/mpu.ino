THIS_SHOULD_FAIL
#include <Wire.h>
#include <MPU6050_light.h>

MPU6050* mpu;   // pointer, not object

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("SETUP START");

  Wire.begin(21, 22);
  Serial.println("I2C STARTED");

  mpu = new MPU6050(Wire);   // create AFTER I2C is ready
  Serial.println("MPU object created");

  Serial.println("Calling mpu->begin()...");
  byte status = mpu->begin();
  Serial.print("mpu->begin() returned: ");
  Serial.println(status);

  Serial.println("SETUP DONE");
}

void loop() {
  Serial.println("LOOP");
  delay(1000);
}
