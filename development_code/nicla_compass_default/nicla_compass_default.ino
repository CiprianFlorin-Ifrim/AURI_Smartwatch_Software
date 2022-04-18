#include "Arduino_BHY2.h"
#include "Nicla_System.h"

SensorOrientation orientation(SENSOR_ID_ORI);

void setup() {
  nicla::begin();
  Serial.begin(115200);
  BHY2.begin();
  orientation.begin();

}

void loop() {
  BHY2.update();
  Serial.println(orientation.toString());
  delay(200);

}
