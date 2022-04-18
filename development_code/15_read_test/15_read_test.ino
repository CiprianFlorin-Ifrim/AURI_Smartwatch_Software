#include "Nicla_System.h"

void setup() {
  Serial.begin(115200);

}

void loop() {
  digitalWrite(GPIO3, LOW);
  Serial.println(digitalRead(GPIO3));
  delay(2000);
  digitalWrite(GPIO3, HIGH);
  Serial.println(digitalRead(GPIO3));
  delay(2000);

}
