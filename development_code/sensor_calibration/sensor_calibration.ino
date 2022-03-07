#include <Arduino.h>
#include <Ewma.h>
#include "Arduino_BHY2.h"
#include "Nicla_System.h"

Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
SensorBSEC bsec(SENSOR_ID_BSEC);

//filter of 0.01 too slow, 0.1 too fast
Ewma temp_ewma_filter(0.05);   // Less smoothing - faster to detect changes, but more prone to noise
Ewma hum_ewma_filter(0.01);   // Less smoothing - faster to detect changes, but more prone to noise
Ewma press_ewma_filter(0.01);   // Less smoothing - faster to detect changes, but more prone to noise

float temp_filtered;
float humidity_filtered;
float pressure_filtered;

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

  float temp_value = temperature.value();
  int hum_value = humidity.value();
  float pressure_value = pressure.value();


  if (millis() - lastCheck >= 5000) {
    lastCheck = millis();


    float final_temp = filter(temp_value, 1);
    float regressed_temp = round_to_half_integer(0.6990 * final_temp + 4.210);                                                   //https://www.graphpad.com/quickcalcs/linear1/
    float final_humidity = filter(hum_value, 2);
    //float final_pressure = filter(pressure_value, pressure_filtered, false);

    Serial.print("\r\n");
    Serial.println(String("----------------------------------------------------------------------------"));
    Serial.println("Original Temperature value: " + String(temperature.value()));
    Serial.println("Last EWMA value: " + String(temp_filtered));
    Serial.println(String("Final Temperature is: ") + String(final_temp) + String(" C"));
    Serial.println(String("Regressed Temperature is: ") + String(regressed_temp) + String(" C"));
    Serial.println(String("-----------------------------------------------------------------------------"));
    Serial.println(String("----------------------------------------------------------------------------"));
    Serial.println(String("Original Humidity is: ") + String(hum_value) + String(" %"));
    Serial.println("Last EWMA value: " + String(humidity_filtered));
    Serial.println(String("Final Humidity is: ") + String(final_humidity) + String(" %"));
    Serial.println(String("-----------------------------------------------------------------------------"));

    Serial.print("\r\n");
  }
}

//first attribute is the data to use for EWMA and MA, second is for rounding to the nearest 0.5
float filter(float data, int x)
{
  float store_var;

  for (int i  = 0; i < 10;  i++)
  {
    if (x == 1) {
      store_var =  temp_ewma_filter.filter(data);
      temp_filtered = store_var;
    }
    else if (x == 2) {
      store_var =  hum_ewma_filter.filter(data);
      humidity_filtered = store_var;
    }
    delay(50);
  }
  return ((store_var + data) / 2);
}


float round_to_half_integer (float x)
{
  return 0.5 * round(2.0 * x) ;
}
