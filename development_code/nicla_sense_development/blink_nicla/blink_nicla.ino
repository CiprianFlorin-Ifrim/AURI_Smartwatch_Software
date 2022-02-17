#include "Nicla_System.h"

void setup() {
  nicla::begin();
  nicla::leds.begin();
}

void loop() {
  int red = 234;
  int green = 72;
  int blue = 122;

  nicla::leds.setColor(red, green, blue);
  delay(1000);
  nicla::leds.setColor(off);
  delay(1000); 
}
