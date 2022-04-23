/*
   This example reads ADC input and filters it with two separate EWMA filters, each one
   with a different smoothing factor.

   You can observe how the first one is faster to detect changes, but more prone to noise,
   while the last one is less prone to noise, but slower to detect changes.

   The selection of the best alpha value depends on your actual application, the amount of
   noise and sampling frequency.
*/

#include <Arduino.h>
#include <Ewma.h>
#include "Arduino_BHY2.h"
#include "Nicla_System.h"

Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
SensorBSEC bsec(SENSOR_ID_BSEC);

Ewma adcFilter1(0.1);   // Less smoothing - faster to detect changes, but more prone to noise
Ewma adcFilter2(0.01);  // More smoothing - less prone to noise, but slower to detect changes

void setup() {
  nicla::begin();
  Serial.begin(115200);
  BHY2.begin();
  temperature.begin();
  humidity.begin();
  pressure.begin();
  bsec.begin();
}

void loop()
{
  BHY2.update();
  float temperature_value = temperature.value();

  
  float filtered1 = adcFilter1.filter(temperature_value);
  float filtered2 = adcFilter2.filter(temperature_value);
  Serial.println("Temp value is: " + String(temperature_value));
  Serial.println("Filtered 1 is: " + String(filtered1));
  Serial.println("Filtered 2 is: " + String(filtered2));

  delay(2000);
}
