#include <Arduino.h>
#include "Nicla_System.h"


void setup()
{
  nicla::begin();
  Serial.begin(115200);
  Serial1.begin(9600);

  delay(1000);
}

void loop() {
  if(Serial.available()) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      Serial.println("Command activated");
    }
    delay(100);                    // wait a sec for the
  }
}
