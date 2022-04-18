// Include Arduino Wire library for I2C
#include <Arduino.h>
#include "Nicla_System.h"

const int button_pin = GPIO3;
void setup() {
  nicla::begin();
  Serial.begin(9600);
  pinMode(button_pin, INPUT);
  digitalWrite(button_pin, LOW);
}

void loop() {
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      Serial.println(digitalRead(button_pin));
    }
  }
}
