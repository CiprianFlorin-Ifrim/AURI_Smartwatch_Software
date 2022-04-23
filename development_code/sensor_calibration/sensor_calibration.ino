// coding scheme based on Google's C++ style: https://google.github.io/styleguide/cppguide.html

#include <Arduino.h>
#include <Ewma.h>
#include <EwmaT.h>
#include "Arduino_BHY2.h"
#include "Nicla_System.h"

Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
SensorBSEC bsec(SENSOR_ID_BSEC);

//filter of 0.01 too slow, 0.1 too fast
Ewma temperature_ewma(0.2);           // Less smoothing - faster to detect changes, but more prone to noise
Ewma pressure_ewma(0.01);             // Most smoothing - less prone to noise, but very slow to detect changes
EwmaT <int> humidity_ewma(1, 10);     // More smoothing - less prone to noise, but slower to detect changes

float temperature_filtered;
float pressure_filtered;
uint8_t humidity_filtered;

void setup() {
  nicla::begin();
  Serial.begin(115200);
  BHY2.begin();
  temperature.begin();
  humidity.begin();
  pressure.begin();
  bsec.begin();

  delay(5000);
}


void loop() {
  UpdateMainSensors();            //update sensor values

  //values have been calibrated using output equations from linear regression systems
  float temperature_calibrated = RoundToHalfInteger((1.0095 * temperature_filtered) - 4.8051);
  float pressure_calibrated = RoundToHalfInteger(1.00718 * pressure_filtered);
  uint8_t humidity_calibrated = round((1.4383 * humidity_filtered) - 2.5628);

  float temperature_calibrated_fahrenheit = (temperature_calibrated * 9/5) + 32;

  //print values to the user
  Serial.println(String("\n----------------------------------------------------------------------------"));
  Serial.println(String("Final Temperature is: ") + String(temperature_calibrated) + String(" C"));
  Serial.println(String("Final Temperature in F is: ") + String(temperature_calibrated_fahrenheit) + String(" F"));
  Serial.println(String("Final Pressure is: ") + String(pressure_calibrated) + String(" hPa"));
  Serial.println(String("Final Humidity is: ") + String(humidity_calibrated) + String(" %"));
  Serial.println(String("-----------------------------------------------------------------------------\n"));

  delay(3000);
}

void UpdateMainSensors(void) {
  float temperature_readings = 0, pressure_readings = 0;                            //declare and initialize variable for the sum of 10 readings
  uint16_t humidity_readings = 0;                                                   //16bits are required to hold 10 values that can reach 10000

  for (uint8_t i = 0; i < 10; i++) {
    BHY2.update();                                                                  //necessary to update sensor classes
    temperature_readings += temperature.value();                                    //sum all the values from the array to create the threshold value
    humidity_readings += humidity.value();
    pressure_readings += pressure.value();
    Serial.println(String(humidity.value()) + " " + String(humidity_readings));
    delay(50);                                                                      //get a new value every 50ms => 500ms + 12ms total runtime of loop
  }

  float temperature_outlier_higher_bound = 1.20 * (temperature_readings / 10);        //120% of the average of the last 10 values
  float temperature_outlier_lower_bound = 0.80 * (temperature_readings / 10);         //80% of the average of the last 10 values

  float pressure_outlier_higher_bound = 1.20 * (pressure_readings / 10);              //120% of the average of the last 10 values
  float pressure_outlier_lower_bound = 0.80 * (pressure_readings / 10);               //80% of the average of the last 10 values

  uint8_t humidity_outlier_higher_bound = 1.20 * (humidity_readings / 10);            //120% of the average of the last 10 values
  uint8_t humidity_outlier_lower_bound = 0.80 * (humidity_readings / 10);             //80% of the average of the last 10 values

  for (uint8_t enough_t, enough_p, enough_h; enough_t < 10 && enough_p < 10 && enough_h < 10;) { 
    BHY2.update();
    if (temperature.value() >= temperature_outlier_lower_bound && temperature.value() <= temperature_outlier_higher_bound && enough_t < 10) {
        temperature_filtered = temperature_ewma.filter(temperature.value());
        enough_t++;
    } if (pressure.value() >= pressure_outlier_lower_bound && pressure.value() <= pressure_outlier_higher_bound && enough_p < 10) {
        pressure_filtered = pressure_ewma.filter(pressure.value());
        enough_p++;
    } if (humidity.value() >= humidity_outlier_lower_bound && humidity.value() <= humidity_outlier_higher_bound && enough_h < 10) {
        humidity_filtered = humidity_ewma.filter(humidity.value());
        enough_h++;
    } 
    
   delay(100);
  }
}


float RoundToHalfInteger(float x) {
  return 0.5 * round(2.0 * x) ;
}

// https://www.ctrl-alt-test.fr/2018/making-floating-point-numbers-smaller/
// roundb(f, 15) => keep 15 bits in the float, set the other bits to zero
float RoundB(float f, int bits) {
  union { int i; float f; } num;
 
  bits = 32 - bits; // assuming sizeof(int) == sizeof(float) == 4
  num.f = f;
  num.i = num.i + (1 << (bits - 1)); // round instead of truncate
  num.i = num.i & (-1 << bits);
  return num.f;
}
