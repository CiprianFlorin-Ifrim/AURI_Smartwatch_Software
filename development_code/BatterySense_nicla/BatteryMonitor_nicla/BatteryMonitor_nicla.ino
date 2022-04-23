#include "Nicla_System.h"
#include <Battery.h>

#define battery_pin A1
Battery battery = Battery(3000, 4150, battery_pin);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  battery.onDemand(8, HIGH);
  battery.begin(3300, 2.14, &asigmoidal);
  
}

void loop() {
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      Serial.print("Battery voltage is ");
      Serial.print(battery.voltage());
      Serial.print(" (");
      Serial.print(battery.level());
      Serial.println("%)");
    }
    delay(50);
  }
}
