#include "Arduino_BHY2.h"
#include "Nicla_System.h"

Sensor motion_detection(SENSOR_ID_MOTION_DET);

void setup() {
  nicla::begin();
  BHY2.begin();

  motion_detection.begin();
  
  Serial.begin(115200);
}

void loop() {
  static auto lastCheck = millis();
  BHY2.update();
  
  String motion_event = motion_detection.toString();

  if (motion_event.equals("Event detected\n")){
    Serial.println("Gesture Triggered");
  }
  delay(2000);
}
