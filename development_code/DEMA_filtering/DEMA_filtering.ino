//https://www.norwegiancreations.com/2016/08/double-exponential-moving-average-filter-speeding-up-the-ema/
#include <Arduino.h>
#include <Ewma.h>
#include "Arduino_BHY2.h"
#include "Nicla_System.h"

Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
SensorBSEC bsec(SENSOR_ID_BSEC);

float EMA_function(float alpha, float latest, float stored);


float ema_a = 0.06;
float ema_ema = 0;
float ema = 0;

void setup() {
  nicla::begin();
  Serial.begin(115200);
  BHY2.begin();
  temperature.begin();
  humidity.begin();
  pressure.begin();
  bsec.begin();
}

void loop() {
  static auto lastCheck = millis();
  BHY2.update();


  ema = EMA_function(ema_a, temperature.value(), ema);
  ema_ema = EMA_function(ema_a, ema, ema_ema);

  float DEMA = 2 * ema - ema_ema;
  delay(10);

  if (millis() - lastCheck >= 5000) {
    lastCheck = millis();
    Serial.println("Original Temperature value: " + String(temperature.value()));
    Serial.println("DEMA Temp is: " + String(DEMA));
  }
}

float EMA_function(float alpha, float latest, float stored) {
  return round(alpha * latest) + round((1 - alpha) * stored);
}
