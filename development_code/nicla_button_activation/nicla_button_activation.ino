#include "Nicla_System.h"

void setup() {
  nicla::begin();
  Serial.begin(115200);
  
  pinMode(P0_29, INPUT);
  digitalWrite(P0_29, HIGH);
}

void loop() {
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      Serial.println(digitalRead(P0_29));
    }
  }
}
