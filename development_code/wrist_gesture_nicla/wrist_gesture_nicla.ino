#include "Nicla_System.h"
#include "Arduino_BHY2.h"

Sensor gesture(SENSOR_ID_WRIST_TILT_GESTURE);

void setup() {
  nicla::begin();
  BHY2.begin();
  gesture.begin();

  Serial.begin(9600);
}

void loop() {
  UpdateNiclaSensors();
  String gesture_event = gesture.toString();
  if (gesture_event.equals("Event detected\n")){
    Serial.println("Gesture Triggered");
  }
  delay(2000);

}

void UpdateNiclaSensors() {
  for (uint8_t i = 0; i < 5; i++) {
    BHY2.update();                                                                //necessary to update sensor
    delay(10);
  }
}
