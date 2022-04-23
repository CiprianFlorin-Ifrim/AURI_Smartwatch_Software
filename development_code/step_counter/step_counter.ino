#include "Arduino_BHY2.h"
#include "Nicla_System.h"

Sensor steps(SENSOR_ID_STC);

void setup() {
  nicla::begin();
  Serial.begin(9600);
  BHY2.begin();
  steps.begin();

}

void loop() {
  unsigned long last_steps = steps.value();

  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      BHY2.update();
      unsigned long total_steps = steps.value();
      unsigned long current_steps = total_steps - last_steps;
      Serial.println(String("Total Steps: ") + total_steps);
      Serial.println(String("Current Run Steps: ") + String(current_steps));
      delay(10);
      break;
    }

  }
}
