// Include Arduino Wire library for I2C
#include <Arduino.h>
#include "Nicla_System.h"
#include <Wire.h>

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);
}

void loop() {
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write(5);              // sends one byte
      Wire.endTransmission();    // stop transmitting

      Wire.requestFrom(8, 1);    // request 1 byte from slave device #8
      while (Wire.available()) { // slave may send less than requested
        int c = Wire.read(); // receive a byte as character

        Serial.print("Slave value : ");
        Serial.println(c);         // print the character
      }

      delay(100);
    }
  }
}
