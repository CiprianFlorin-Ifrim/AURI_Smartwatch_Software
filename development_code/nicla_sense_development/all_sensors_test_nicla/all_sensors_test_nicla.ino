#include "Arduino.h"
#include "Arduino_BHY2.h"

Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
Sensor gas(SENSOR_ID_GAS);
//SensorBSEC bsec(uint8_t(171));
//SensorBSEC bsec(uint8_t(115));
SensorBSEC bsec(SENSOR_ID_BSEC);
//SensorActivity activity(SENSOR_ID_AR);

void setup(){
  Serial.begin(115200);
  BHY2.begin();
  temperature.configure(1,0);
  humidity.configure(1,0);
  pressure.configure(200,0);
  gas.configure(1,0);
  bsec.configure(100,1);
  //activity.begin();
}

void loop(){
  static auto lastCheck= millis();
  BHY2.update();

  // Check sensor values every second  
  if (millis() - lastCheck >= 1000) {
    lastCheck = millis();
    Serial.println(String("Temperature is: ") + String(temperature.value()) + String("C"));
    Serial.println(String("Humidity is: ") + String(humidity.value()) + String("%"));
    Serial.println(String("Gas resistance: ") + String(gas.value()));
    Serial.println(String("Pressure: ") + String(pressure.value()) + String("hPa"));
    Serial.println(bsec.toString());
    //Serial.println(String("Activity info: ") + activity.toString());
  }
}
