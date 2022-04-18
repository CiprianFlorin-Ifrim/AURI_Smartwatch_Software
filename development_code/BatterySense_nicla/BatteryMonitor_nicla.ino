#include <Arduino.h>
#include "Nicla_System.h"
#include <Battery.h>

Battery battery = Battery(3000, 4200, A0);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  float ratio = (2.99 + 10.02) / 10.02;
	battery.begin(4900, 1.3, &sigmoidal);
}

void loop() {
  digitalWrite(GPIO3, HIGH);
	Serial.print("Battery voltage is ");
	Serial.print(battery.voltage());
	Serial.print(" (");
	Serial.print(battery.level());
	Serial.println("%)");
 delay(100);
}
