#include "Arduino.h"
#include "Arduino_BHY2.h"

Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
Sensor gas(SENSOR_ID_GAS);
//SensorBSEC bsec(uint8_t(171));
//SensorBSEC bsec(uint8_t(115));
SensorBSEC bsec(SENSOR_ID_BSEC);;
SensorXYZ magnetometer(SENSOR_ID_MAG);
SensorXYZ gravity_vector(SENSOR_ID_GRA);
SensorOrientation orientation(SENSOR_ID_ORI);
SensorOrientation device_orientation(SENSOR_ID_DEVICE_ORI);
Sensor tilt(SENSOR_ID_WRIST_TILT_GESTURE);

//SensorActivity activity(uint8_t(63));
//SensorActivity activity(SENOSR_ID_AR);

void setup(){
  Serial.begin(115200);
  BHY2.begin();
  temperature.begin();
  humidity.begin();
  pressure.begin();
  gas.begin();
  bsec.begin();
  magnetometer.begin();
  gravity_vector.begin();
  orientation.begin();
  tilt.begin();
  device_orientation.begin();
  
  //activity.configure(100,1);
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
    Serial.println(String("Magnetometer info: ") + magnetometer.toString());
    Serial.println(String("Gravity Vector info: ") + gravity_vector.toString());
    Serial.println(String("Tilt info: ") + tilt.toString());
    Serial.println(String(orientation.toString()));
    Serial.println(bsec.toString());
    
    //Serial.println(String("Activity info: ") + activity.toString());
  }
}
