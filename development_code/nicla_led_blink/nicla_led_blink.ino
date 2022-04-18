#include "Nicla_System.h"

void setup() {
  nicla::begin();
  nicla::leds.begin();

}

void loop() {
  nicla::leds.setColor(green);
  delay(1000);
  nicla::leds.setColor(blue);
  delay(1000);  
}
