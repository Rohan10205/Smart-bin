#include <CheapStepper.h>
#include <Servo.h>

#define IR_SENSOR_PIN 5
#define PROXIMITY_SENSOR_PIN 6
#define BUZZER_PIN 12
#define SOIL_SENSOR_PIN A0

const int SOIL_MIN = 485;
const int SOIL_MAX = 1023;
const int MOISTURE_THRESHOLD = 20;

Servo servo1;
CheapStepper stepper(8, 9, 10, 11);

void setup() {
  Serial.begin(9600);

  pinMode(PROXIMITY_SENSOR_PIN, INPUT_PULLUP);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  servo1.attach(7);
  stepper.setRpm(17);

  // Initialize servo position
  servo1.write(180);
  delay(1000);
  servo1.write(70);
  delay(1000);
}

void loop() {
  // Check proximity sensor
  if (digitalRead(PROXIMITY_SENSOR_PIN) == LOW) {
    Serial.println("Proximity Detected");
    tone(BUZZER_PIN, 1000, 1000);

    stepper.moveDegreesCW(240);
    delay(1000);
    openAndCloseLid();
    stepper.moveDegreesCCW(240);
    delay(1000);
  }

  // Check IR sensor
  if (digitalRead(IR_SENSOR_PIN) == LOW) {
    Serial.println("IR Triggered");
    tone(BUZZER_PIN, 1000, 500);
    delay(1000);

    int averageMoisture = readAverageSoilMoisture();
    Serial.print("Soil Moisture: ");
    Serial.print(averageMoisture);
    Serial.println("%");

    if (averageMoisture > MOISTURE_THRESHOLD) {
      // Wet soil
      stepper.moveDegreesCW(120);
      delay(1000);
      openAndCloseLid();
      stepper.moveDegreesCCW(120);
      delay(1000);
    } else {
      // Dry soil
      tone(BUZZER_PIN, 1000, 500);
      delay(1000);
      openAndCloseLid();
    }
  }
}

void openAndCloseLid() {
  servo1.write(180);
  delay(1000);
  servo1.write(70);
  delay(1000);
}

int readAverageSoilMoisture() {
  int total = 0;
  for (int i = 0; i < 3; i++) {
    int raw = analogRead(SOIL_SENSOR_PIN);
    raw = constrain(raw, SOIL_MIN, SOIL_MAX);
    total += map(raw, SOIL_MIN, SOIL_MAX, 100, 0);
    delay(75);
  }
  return total / 3;
}
