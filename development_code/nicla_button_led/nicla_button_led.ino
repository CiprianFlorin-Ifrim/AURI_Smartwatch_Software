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
  if (digitalRead(button_pin) == LOW) { // if button is pressed
    Serial.println("Button on");
    //digitalWrite(button_pin, LOW);
  }                           //
  
  delay(500);                    // wait a sec for the
}                               // hardware to stabilize
