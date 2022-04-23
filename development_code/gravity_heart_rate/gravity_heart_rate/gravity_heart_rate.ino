#include "Nicla_System.h"
#include "DFRobot_Heartrate.h"

#define heartratePin A0
DFRobot_Heartrate heartrate(ANALOG_MODE); ///< ANALOG_MODE or DIGITAL_MODE

void setup() {
  Serial.begin(9600);
}

void loop() {
  HeartMonitor();
}

void HeartMonitor() {
  heartrate.getValue(heartratePin); ///< A1 foot sampled values
  uint8_t rate_value = heartrate.getRate(); ///< Get heart rate value 
  if(rate_value)  {
    Serial.println(rate_value);
  }
  delay(10);
}
