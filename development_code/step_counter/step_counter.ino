#include "Arduino_BHY2.h"
#include "Nicla_System.h"

Sensor steps(SENSOR_ID_STC);
float last_steps = 0;

void setup() {
  nicla::begin();
  Serial.begin(115200);
  BHY2.begin();
}

void loop() {
  steps.begin();
  float last_steps = steps.value();

  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '2') {
      BHY2.update();
      Serial.println(String("Total Steps: ") + steps.value());
      Serial.println(String("Current Run Steps: ") + (steps.value() - last_steps));
      delay(10);
      break;
    }

  }
}
