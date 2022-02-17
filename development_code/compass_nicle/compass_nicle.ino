#include "Arduino.h"
#include "Arduino_BHY2.h"

SensorXYZ magnetometer(SENSOR_ID_MAG);

void setup() {
  Serial.begin(115200);
  BHY2.begin();
  magnetometer.begin();
}

void loop() {
  static auto lastCheck= millis();
  BHY2.update();

  // Check sensor values every second  
  if (millis() - lastCheck >= 1000) {
    lastCheck = millis();
    String all_values = magnetometer.toString();
    x_val = all_values.substring(17, 20)
    Serial.println("Value X is:" + String(x_val))
    
  }
}
