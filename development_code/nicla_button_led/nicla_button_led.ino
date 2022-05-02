// Include Arduino Wire library for I2C
#include <Arduino.h>
#include "Nicla_System.h"

const int button_pin = P0_29;
int ledflag=0;                   // LED status flag

void setup() {
  nicla::begin();
  nicla::leds.begin();
  
  pinMode(button_pin, INPUT);
  digitalWrite(button_pin, HIGH);
}

void loop() {
  if (digitalRead(button_pin)==LOW){ // if button is pressed
    if (ledflag==0) {             // and the status flag is LOW
      ledflag=1;                  // make status flag HIGH
      nicla::leds.setColor(green);
      }                           // 
    else {                        // otherwise...
      ledflag=0;                  // make status flag LOW
      nicla::leds.setColor(off);
    }
  delay(1000);                    // wait a sec for the 
  }                               // hardware to stabilize
}                                 // begin again 
