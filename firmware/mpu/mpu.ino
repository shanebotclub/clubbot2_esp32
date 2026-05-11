#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("BOOT OK");

  // I2C setup
  Wire.begin(21, 22);  // SDA=21, SCL=22

  Serial.println("Starting MPU6050...");

  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU6050 init failed with code: ");
    Serial.println(status);
    while (1);
  }

  Serial.println("MPU6050 ready. Calibrating...");
  delay(1000);
  mpu.calcOffsets();   // gyro + accel calibration
  Serial.println("Calibration done.");
}

void loop() {
  Serial.println("RUNNING");
  delay(1000);
  mpu.update();

  unsigned long now = millis();
  if (now - lastPrint >= 100) {   // 10 Hz output
    lastPrint = now;

    float ax = mpu.getAccX();
    float ay = mpu.getAccY();
    float az = mpu.getAccZ();

    float gx = mpu.getGyroX();
    float gy = mpu.getGyroY();
    float gz = mpu.getGyroZ();

    float roll  = mpu.getAngleX();
    float pitch = mpu.getAngleY();
    float yaw   = mpu.getAngleZ();

    // Human-readable debug
    Serial.print("MPU ");
    Serial.print("ax="); Serial.print(ax);
    Serial.print(" ay="); Serial.print(ay);
    Serial.print(" az="); Serial.print(az);

    Serial.print(" gx="); Serial.print(gx);
    Serial.print(" gy="); Serial.print(gy);
    Serial.print(" gz="); Serial.print(gz);

    Serial.print(" roll=");  Serial.print(roll);
    Serial.print(" pitch="); Serial.print(pitch);
    Serial.print(" yaw=");   Serial.println(yaw);

    // ROS-friendly line (easy to parse)
    Serial.print("IMU ");
    Serial.print("ax="); Serial.print(ax); Serial.print(" ");
    Serial.print("ay="); Serial.print(ay); Serial.print(" ");
    Serial.print("az="); Serial.print(az); Serial.print(" ");
    Serial.print("gx="); Serial.print(gx); Serial.print(" ");
    Serial.print("gy="); Serial.print(gy); Serial.print(" ");
    Serial.print("gz="); Serial.print(gz); Serial.print(" ");
    Serial.print("roll=");  Serial.print(roll); Serial.print(" ");
    Serial.print("pitch="); Serial.print(pitch); Serial.print(" ");
    Serial.print("yaw=");   Serial.println(yaw);
  }
}
