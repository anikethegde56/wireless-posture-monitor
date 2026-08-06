#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;
const int BUZZER_PIN = 8;
const float THRESHOLD = 15.0;
const unsigned long HOLD_TIME = 5000;
float basePitch;
unsigned long badPostureStart = 0;
void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);
  Serial.println("Warming up sensor...");
  delay(3000);
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;
  basePitch = atan2(ay_g, sqrt(ax_g * ax_g + az_g * az_g)) * 180 / PI;
  Serial.println("Baseline set. Sit upright.");
  Serial.print("Base Vertical Pitch: ");
  Serial.println(basePitch);
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;
  float pitch = atan2(ay_g, sqrt(ax_g * ax_g + az_g * az_g)) * 180 / PI;
  float deviation = abs(pitch - basePitch);
  Serial.print("Vertical Pitch: ");
  Serial.print(pitch);
  Serial.print(" | Deviation: ");
  Serial.print(deviation);
  if (deviation > THRESHOLD) {
    if (badPostureStart == 0) {
      badPostureStart = millis();
      Serial.print(" -- countdown started");
    }
    unsigned long elapsed = millis() - badPostureStart;
    Serial.print(" -- bad posture time: ");
    Serial.print(elapsed / 1000);
    Serial.println("s");
    if (elapsed >= HOLD_TIME) {
      tone(BUZZER_PIN, 2000);
      Serial.println("BUZZER ON");
    }
  } else {
    badPostureStart = 0;
    noTone(BUZZER_PIN);
    Serial.println(" -- BUZZER OFF");
  }
  delay(100);
}