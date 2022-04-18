#include "Arduino_BHY2.h"


SensorQuaternion gravityvector(SENSOR_ID_GRA);
SensorQuaternion rotationvector(SENSOR_ID_RV);
SensorQuaternion gamerotationvector(SENSOR_ID_GAMERV);
SensorQuaternion geomagneticrotationvector(SENSOR_ID_GEORV);

void setup() {
  Serial.begin(115200);
  BHY2.begin();

  gravityvector.begin();
  rotationvector.begin();
  gamerotationvector.begin();
  geomagneticrotationvector.begin();

}

void loop() {
  static auto lastCheck = millis();

  // Update function should be continuously polled
  BHY2.update();

  // Check sensor values every second
  if (millis() - lastCheck >= 5000) {
    lastCheck = millis();

    Serial.println("\n-------------------------------------------------------------");
    Serial.println(String("Gravity Vector: ") + gravityvector.toString());
    Serial.println(String("Rotation Vector: ") + rotationvector.toString());
    Serial.println(String("Game Rotation Vector: ") + gamerotationvector.toString());
    Serial.println(String("Geomagnetic Rotation Vector: ") + geomagneticrotationvector.toString());
  }

}
