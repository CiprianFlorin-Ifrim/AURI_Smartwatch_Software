#include "Arduino.h"
#include "Arduino_BHY2.h"

SensorBSEC bsec(SENSOR_ID_BSEC);

void setup(){
  Serial.begin(115200);
  BHY2.begin();
  bsec.begin();
}

void loop(){
  static auto lastCheck= millis();
  bsec.begin();
  BHY2.update();

  // Check sensor values every second  
  if (millis() - lastCheck >= 1000) {
    lastCheck = millis();
    Serial.println(bsec.toString());

    int iaq = ((bsec.toString()).substring(25,28)).toInt();
    float voc_eq = ((bsec.toString()).substring(53,57)).toFloat();
    int co2_eq = ((bsec.toString()).substring(68,72)).toInt();
    int accuracy = ((bsec.toString()).substring(84,85)).toInt();
    float comp_temp = ((bsec.toString()).substring(95,101)).toFloat();
    float comp_hum = ((bsec.toString()).substring(112,117)).toFloat();
    int comp_gasr = ((bsec.toString()).substring(128,130)).toInt();

    Serial.println(iaq);
    Serial.println(voc_eq);
    Serial.println(co2_eq);
    Serial.println(accuracy);
    Serial.println(comp_temp);
    Serial.println(comp_hum);
    Serial.println(comp_gasr);
  }
}
