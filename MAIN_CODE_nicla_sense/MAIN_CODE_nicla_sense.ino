//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------LIBRARIES-------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Nicla_System.h"
#include "Arduino_BHY2.h"
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_SI1145.h>
#include <Ewma.h>
#include <Battery.h>
#include <bq25120a_nicla.h>                              // NICLA library for BQ25120A
#include <I2C_Anything.h>
#include <DFPlayerMini_Fast.h>
#include <TimeLib.h>
#include <DFRobot_Heartrate.h>


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------DECLARATIONS------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define heartrate_pin A0
#define battery_sense A1
#define slave_address 0x08
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7     // ASCII bell character requests a time sync message 

Sensor            humidity(SENSOR_ID_HUM);
Sensor            temperature(SENSOR_ID_TEMP);
Sensor            pressure(SENSOR_ID_BARO);
SensorBSEC        bsec(SENSOR_ID_BSEC);
SensorOrientation orientation(SENSOR_ID_ORI);
Sensor            tilt(SENSOR_ID_WRIST_TILT_GESTURE);
Sensor            steps(SENSOR_ID_STC);
DFRobot_Heartrate heartrate(ANALOG_MODE);                                           // ANALOG_MODE or DIGITAL_MODE

DFPlayerMini_Fast myMP3;
Adafruit_SI1145 uv = Adafruit_SI1145();
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Battery battery = Battery(3000, 4150, battery_sense);

Ewma temperature_BMP390_ewma(0.2);                      // Less smoothing - faster to detect changes, but more prone to noise
Ewma temperature_BME688_ewma(0.2);                      // Less smoothing - faster to detect changes, but more prone to noise
Ewma pressure_BMP390_ewma(0.01);                        // Most smoothing - less prone to noise, but very slow to detect changes
Ewma humidity_BME688_ewma(0.05);

static boolean isLongFormat = true;

float temperature_BMP390_filtered, temperature_BME688_filtered, pressure_BMP390_filtered, humidity_BME688_filtered, heat_index;
float dataReceived[6];

bool charging_flag = true;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------SYSTEM SETUP------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  nicla::begin();
  Wire.begin(); // join i2c bus (address optional for master)
  
  BHY2.begin();
  temperature.begin();
  pressure.begin();
  bsec.begin();
  orientation.begin();
  steps.begin();

  battery.onDemand(8, HIGH);
  battery.begin(3300, 2.14, &asigmoidal);

  Serial.begin(9600);
  Serial1.begin(9600);
  
  if (!mlx.begin()) Serial.println("Error connecting to MLX sensor. Check wiring.");
  if (!uv.begin()) Serial.println("Didn't find Si1145");

  myMP3.begin(Serial1, true);
  myMP3.volume(30);

  setSyncProvider(RequestSync);                                                                      //set function to call when sync required

  delay(5000);                                                                                       //delay for the sensors to engage
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------MAIN CODE-----------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {
  if (Serial.available() > 1) {                               // wait for at least two characters
    char c = Serial.read();
    if (c == TIME_HEADER) ProcessSyncMessage(); 
  }

/*
  byte battery_status = bq25120_getStatus();
  if (battery_status == 195 && charging_flag) {
    Serial.println("The battery is charging");
    charging_flag = false;
  } else if (battery_status != 195 && !charging_flag) charging_flag = true;
*/

  if (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {

      unsigned long last_steps = steps.value();
  
      UpdateMainSensors();            //update sensor values
  
      float temperature_BMP390_regressed  = (1.0095 * temperature_BMP390_filtered) - 4.8051;
      float temperature_BME688_regressed  = (1.0095 * temperature_BME688_filtered) - 4.8051;
      float temperature_regressed_fusion  = (temperature_BMP390_regressed + temperature_BME688_regressed) / 2;
      float temperature_fusion_fahrenheit = (temperature_regressed_fusion * 9 / 5) + 32;
      float pressure_BMP390_regressed     = 1.00718 * pressure_BMP390_filtered;
      uint8_t humidity_BME688_regressed   = round((1.4383 * humidity_BME688_filtered) - 2.5628);
      
      float    visible_light  = uv.readVisible();
      float    infrared_light = uv.readIR();
      uint16_t uv_index       = (uv.readUV()) / 100.0;
  
      const float sea_pressure_constant = 1013.25;    
      uint16_t    altitude              = ((pow((sea_pressure_constant / pressure_BMP390_regressed), (1 / 5.257)) - 1) * (temperature_regressed_fusion + 273.15)) / 0.0065;  

      //the ratio of the actual amount of water vapor in the air to the amount it could hold when saturated
      float relative_humidity         = (((humidity_BME688_regressed * pressure_BMP390_regressed) / (0.378 * humidity_BME688_regressed + 0.622))                                                                                                          // relative humidity from Bolton 1980 The computation of Equivalent Potential Temperature
                                        /(6.112 * exp((17.67 * temperature_regressed_fusion) / (temperature_regressed_fusion + 243.5))));                                                                                                              //The dew point is the temperature to which air must be cooled to become saturated with water vapor
      float air_dew_point             = ((243.12 * (log(relative_humidity / 100) + ((17.62 * temperature_regressed_fusion) / (243.12 + temperature_regressed_fusion))))
                                        /(17.62 - (log(relative_humidity / 100) + ((17.62 * temperature_regressed_fusion) / (243.12 + temperature_regressed_fusion)))));
      float saturation_vapor_pressure = 6.1078 * pow(10, ((7.5 * temperature_regressed_fusion) / (temperature_regressed_fusion + 237.3)));                                                                                                                 //from https://www.omnicalculator.com/physics/air-density#how-to-calculate-the-air-density
      float water_vapor_pressure      = (saturation_vapor_pressure * relative_humidity) / 100;                                                                                                                                                                          //multiplication of saturation vapor pressure with the relative humidity, divided by 100%
      float dry_air_pressure          = pressure_BMP390_regressed - water_vapor_pressure;
      float equivalent_sea_pressure   = pressure_BMP390_regressed * pow((1 - ((0.0065 * altitude)/(temperature_regressed_fusion + 0.0065 * altitude + 273.15))), -5.257);
      const float dry_air_constant    = 287.058, water_vapor_constant = 461.495;                                                                                                               //specific gas constant for dry air equal in J/(kg·K) //specific gas constant for water vapor in J/(kg·K)
      float air_density               = 100 * ((dry_air_pressure / (dry_air_constant * (temperature_regressed_fusion + 273.15)))                                                                                       //air density formula, can be used for air buoyancy
                                        +(water_vapor_pressure / ( water_vapor_constant * (temperature_regressed_fusion + 273.15))));
      float absolute_humidity         = (((saturation_vapor_pressure * relative_humidity) / 10) / (water_vapor_constant * temperature_regressed_fusion));                                        //expressed in kilograms per cubic meter of moist air - https://planetcalc.com/2167/
                                                                                                                                                     
      //the regression equation of Rothfusz used by the US National Weather Service: https://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
      float heat_index_Rothfusz          = -42.379 + (2.04901523 * temperature_fusion_fahrenheit) - (0.00000199 * pow(temperature_fusion_fahrenheit, 2) * pow(relative_humidity, 2))
                                           -(0.22475541 * temperature_fusion_fahrenheit * relative_humidity) - (0.00683783 * pow(temperature_fusion_fahrenheit, 2))
                                           -(0.05481717 * pow(relative_humidity, 2)) + (0.00122874 * pow(temperature_fusion_fahrenheit, 2) * relative_humidity)
                                           +(0.00085282 * temperature_fusion_fahrenheit * pow(relative_humidity, 2)) + (10.14333127 * relative_humidity);                                                                                     //felt air temperature, does not take into consideration direct sunlight effect
      float heat_index_Steadman          = 0.5 * (temperature_fusion_fahrenheit + 61.0 + ((temperature_fusion_fahrenheit - 68.0) * 1.2) + (relative_humidity * 0.094));
  
      float heat_index_lowRH_adjustment  = ((13 - relative_humidity) / 4) * sqrt((17 - abs(temperature_fusion_fahrenheit - 95)) / 17);                                  //if the RH is less than 13% and the temperature is between 80 and 112 degrees F, then the following adjustment is subtracted from HI
      float heat_index_highRH_adjustment = ((relative_humidity - 85) / 10) * ((87 - temperature_fusion_fahrenheit) / 5);                                               //if the RH is greater than 85% and the temperature is between 80 and 87 degrees F, then the following adjustment is added to HI
  
      //heat index decision tree
      if (heat_index_Steadman >= 80) {                                                                                                                          //If this heat index value is 80 degrees F or higher, the full regression equation along with any adjustment as described above is applied.
        if (relative_humidity < 13 && temperature_fusion_fahrenheit > 80 && temperature_fusion_fahrenheit < 112) {
            heat_index = ((heat_index_Rothfusz - heat_index_lowRH_adjustment) - 32) * 5 / 9;
        } else if (relative_humidity > 85 && temperature_fusion_fahrenheit > 80 && temperature_fusion_fahrenheit < 87) {
            heat_index = ((heat_index_Rothfusz + heat_index_highRH_adjustment) - 32) * 5 / 9;
        } else heat_index = (heat_index_Rothfusz - 32) * 5 / 9;
      } else heat_index = (((heat_index_Steadman + temperature_fusion_fahrenheit) / 2) - 32) * 5 / 9;                                                               //the Steadman formula is computed first and the result averaged with the temperature

      //array to send to the XIAO device for Machine Learning classification/regression
      float X[6] = {temperature_regressed_fusion, heat_index, air_dew_point, relative_humidity, equivalent_sea_pressure, uv_index};


      Serial.println("Command received");

      Wire.beginTransmission(slave_address);                                                                          // transmit to device #8
      for (uint8_t i = 0; i < sizeof(X); i++) I2C_writeAnything(X[i]);
      Wire.endTransmission();                                                                                         // stop transmitting

      delay(3000);                                                                                                    //wait for the processing to end on the Xiao Sense
      
      Wire.requestFrom(slave_address, sizeof(dataReceived));
      while (Wire.available()) for (uint8_t j = 0; j < sizeof(dataReceived); j++) I2C_readAnything(dataReceived[j]);                                                       // slave may send less than requested

      switch (int(dataReceived[5])) {
        case 0:  
        {         
          Serial.println("You've asked for the BATTERY LEVEL");

          //battery checks
          uint16_t battery_voltage_circuit    = battery.voltage();
          uint8_t  battery_percentage_circuit = battery.level();
      
          uint8_t battery_level_bq25120  = bq25120_getLevel();
          byte    battery_fault_bq25120  = bq25120_getFaults();

          Serial.println("\nBattery level is " + String(battery_level_bq25120));
          Serial.println("Battery percentage is " + String(battery_percentage_circuit) + "%");
          Serial.println("nBattery voltage is " + String(battery_voltage_circuit) + " mV");

          if (battery_fault_bq25120 == 0) Serial.println("The battery is health");
          else Serial.println("The battery has no faults");
          
          break;
        }
        
        case 1:
        {
          Serial.println("You've asked for the CLOUDS INFO");

          //https://www.omnicalculator.com/physics/cloud-base#how-to-use-the-cloud-altitude-calculator
          float cloud_base_altitude_groundlevel = (temperature_regressed_fusion - air_dew_point) * 124.6;                                                              //https://holfuy.com/en/support/cloud-base-calculations (calibrated to my device and tested against calculators)
          float cloud_base_altitude_sealevel    = cloud_base_altitude_groundlevel + altitude;
          float cloud_base_temperature          = - 0.00182 * cloud_base_altitude_groundlevel + air_dew_point;

          Serial.println("Cloud Base Altitude Ground Level: " + String(cloud_base_altitude_groundlevel, 4) + " m");
          Serial.println("Cloud Base Altitude Sea Level: " + String(cloud_base_altitude_sealevel, 4) + " m");
          Serial.println("Cloud Base Temperature: " + String(cloud_base_temperature) + " C");

          break;
        }
        
        case 2:
        {
          Serial.println("You've asked for the AURI DESCRIPTION");

          Serial.println("My name is AURI, I am the assistant that will always be here for you."); 
          Serial.println("I am able to continuously monitor your health and the effect of the environment on it.");
          Serial.println("If you ever wonder about the conditions of your surroundings, just ask, I’ll be glad to answer!");
          
          break;
        }
        
        case 3:
        {
          Serial.println("You've asked for the ENVIRONMENT CONDITIONS");

          Serial.println("Temperature is: " + String(temperature_regressed_fusion) + " C");
          Serial.println("Temperature in Fahrenheit is: " + String(temperature_fusion_fahrenheit) + " F");
          Serial.println("The Feels Like Temperature is: " + String(heat_index) + " C");
          Serial.println("Air Dew Point Temperature: " + String(air_dew_point) + " C");
          
          Serial.println("Humidity is: " + String(humidity_BME688_regressed) + " %");
          Serial.println("Relative Humidity: " + String(relative_humidity) + " %");
          Serial.println("Absolute Humidity: " + String(absolute_humidity, 4) + " kg/m3");
          
          Serial.println("Barometric Pressure: " + String(pressure_BMP390_regressed) + " hPa");
          Serial.println("Barometric to Equivalent Sea Level Pressure: " + String(equivalent_sea_pressure) + " hPa");
          Serial.println("Water Vapor Pressure: " + String(water_vapor_pressure) + " hPa");
          Serial.println("Dry Air Pressure " + String(dry_air_pressure) + " hPa");

          if (bsec.comp_g() <= 100) Serial.println("The sensor needs to calibrate to the current environment before outputting an accurate gas resistance reading!");
          else if (bsec.comp_g() > 100 && bsec.comp_g() < 10000) {
              Serial.print("The Gas Resistance is: ");
              Serial.print(bsec.comp_g());                          //file MP3 from 1 to 10k 
              Serial.println("ohm");                                //ohms
          } else {
              Serial.print("The Gas Resistance is: ");
              Serial.print(round(bsec.comp_g()/1000));                          //file MP3 from 1 to 10k 
              Serial.println("kiloohm");                                //ohms    
          }
          
          break;
        }
        
        case 4:
        {
          Serial.println("You've asked for the HEALTH REPORT");

          float   laser_skin_temp  = mlx.readObjectTempC() - 3.5;                                                                              //the sensor has been calibrated to detect through a tempered glass screen the human skin temeperature compared to the regular reading at 5cm ( most accurate)
          uint8_t discomfort_index = temperature_regressed_fusion - 0.55 * (1 - 0.01 * humidity_BME688_regressed) * (temperature_regressed_fusion - 14.5);                                                                  //discomfort index formula

          UpdateNiclaSensors();
          uint8_t  bsec_accuracy  = bsec.accuracy();
          uint16_t air_quality    = bsec.iaq();
          uint32_t carbon_dioxide = bsec.co2_eq();
          float    volatile_mix   = bsec.b_voc_eq();

          uint8_t       voc_values[11] = {0, 5, 10, 15, 50, 255}; 
          uint_least8_t voc_output[5]  = {1, 2, 3, 4, 5};
          for (uint8_t h = 0; h < 5; h++) {
            if (voc_values[h] < volatile_mix && volatile_mix <= voc_values[h+1]) {
              Serial.println("Volatile Organic Gasses Amount: " + String(volatile_mix) + " ppm");
              Serial.println("The VOC condition is: " + String(voc_output[h]));
            } 
          }

          uint16_t      iaq_values[8] = {0, 50, 100, 150, 200, 250, 350, 500};
          uint_least8_t iaq_output[7] = {1, 2, 3, 4, 5, 6, 7};  
          for (uint8_t j = 0; j < 7; j++) {
            if (iaq_values[j] < air_quality && air_quality <= iaq_values[j+1]) {
              Serial.println("Air Quality Index is: " + String(air_quality));
              Serial.println("The Air Quality is: " + String(iaq_output[j]));
            }
          }

          uint16_t      co2_values[10] = {0, 400, 600, 900, 1100, 1600, 2000, 5000, 40000, 65500};
          uint_least8_t co2_output[9]  = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
          for (uint8_t k = 0; k < 9; k++) {
            if (co2_values[k] < carbon_dioxide && carbon_dioxide <= co2_values[k+1]) {
              Serial.println("Estimation of CO2 levels: " + String(carbon_dioxide) + " ppm");
              Serial.println("The Carbon Diooxide condition is: " + String(co2_output[k]));
            }
          }

          uint_least8_t discomfort_values[7] = {0, 21, 24, 27, 29, 32, 250};
          uint_least8_t discomfort_output[6] = {1, 2, 3, 4, 5, 6};  
          for (uint8_t l = 0; l < 6; l++) {
            if (discomfort_values[l] < discomfort_index && discomfort_index <= discomfort_values[l+1]) {
              Serial.println("Discomfort level: " + String(discomfort_index));
              Serial.println("The Discomfort Index Condition is: " + String(discomfort_output[l]));
            }
          }

          uint8_t       heat_index_values[6] = {4, 26, 32, 39, 51, 150};
          uint_least8_t heat_index_output[5] = {1, 2, 3, 4, 5};
          for (uint8_t n = 0; n < 5; n++) {
            if (temperature_regressed_fusion >= 4) {
              if (heat_index_values[n] < heat_index && heat_index <= heat_index_values[n+1]) {
                Serial.println("Heat Index: " + String(heat_index) + " C");
                Serial.println("The Heat Index Condition is: " + String(heat_index_output[n]));
              }
            } else Serial.println("The Heat Index Condition cannot be calculated as the environment temperature is too low. Please consider the temperature as the absolute truth!");
          }

          Serial.println("BSEC Sensor Accuracy is: " + String(bsec_accuracy));
      
          if (pressure_BMP390_regressed > 1022.678) Serial.println("Caution! High pressure which pushes against the body and limits how much the tissue can expand. Potential of joint paint!");
          else if (pressure_BMP390_regressed < 1007) Serial.println("Caution! Low pressure allowing the body's tissues to expand, affecting the nerves, which can cause migraines!");
          else Serial.println("Expected pressure range!");

          Serial.println("Skin Temperature: " + String(laser_skin_temp) + " C");
            
          break;
        }
        
        case 5:
        {
          Serial.println("You've asked for the LOCATION DETAILS");   

          UpdateNiclaSensors();
          float heading = orientation.heading();
          float pitch   = orientation.pitch();
          float roll    = orientation.roll();

          float   compass_points[18] = {0, 11.25, 33.75, 56.25, 78.75, 101.25, 123.75, 146.25, 
                                        168.75, 191.25, 213.75, 236.25, 258.75, 281.25, 303.75, 326.25, 348.75, 360};
          uint8_t compass_output[17] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};  
          for (uint8_t m = 0; m < 17; m++) {
            if (compass_points[m] <= heading && heading <= compass_points[m+1]) Serial.println("You are heading: " + String(compass_output[m]));
          }

          Serial.println("Compass Pitch: " + String(pitch, 2));
          Serial.println("Compass Roll/Tilt: " + String(roll, 2));
          Serial.println("Altitude: " + String(altitude) + " m");

          break;
        }
        
        case 6:
        {
          Serial.println("You've asked for the STEPS COUNTER");
    
          //steps counting
          UpdateNiclaSensors();
          unsigned long total_steps   = steps.value();
          unsigned long current_steps = total_steps - last_steps;

          Serial.println("\nTotal Steps: " + String(total_steps));
          Serial.println("Current Run Steps: " + String(current_steps));
          
          break;
        }
        
        case 7:
        {
          Serial.println("You've asked for the THANKS OUTPUT");

          Serial.println("You are, as always, very welcome!");
          
          break;
        }
        
        case 8:
        {
          Serial.println("You've asked for the TIME AND DATE");

          if (timeStatus() != timeNotSet) {
            Serial.println(String(hour()) + " " + String(minute()));
            Serial.println(String(day()) + " " + String(month()) + " " + String(year()));
          } else Serial.println("The watch has not been setup. Please dock the watch and use the PC executable to sync the time and date!");
         
          break;
        }
        
        case 9: 
        {
          Serial.println("You've asked for the WEATHER");
          
          uint8_t allLabels[4], bestLabels[2] = {5, 5}, primes[5] = {2, 3, 5, 7, 11};
          int product = 1;
          
          for (uint8_t l = 0; l < 4; l++) {
            allLabels[l] = dataReceived[l];
            product *= primes[allLabels[l]];
          }
          for (uint8_t k = 0; k < 4; k++) if (!fmod(product, sq(primes[allLabels[k]]))) bestLabels[k / 2] = allLabels[k];

          if (bestLabels[0] == 5) Serial.println("Everything repeats once. XGBoost has been selected!" + String(allLabels[1]));
          else {
            Serial.println("The value that repeats the most is: " + String(bestLabels[0]));
            if (bestLabels[0] != 5 && bestLabels[0] != bestLabels[1]) Serial.println("There is also chance of: " + String(bestLabels[1]));
          }
          Serial.println("The precipitation amount is equal to: " + String(dataReceived[4]) + " mm!");
          
          break;
        }
        
        case 10:
        {
          Serial.println("VOICE VOLUME TOO LOW OR TOO MUCH NOISE. PLEASE REPEAT THE COMMAND");
          break;
        }
        
        case 11:
        {
          Serial.println("SOFTWARE ISSUE DETECTED. FAILED TO SETUP AUDIO SAMPLING");
          break;
        }
        
        case 12:
        {
          Serial.println("SOFTWARE OR HARDWARE ISSUE DETECTED. FAILED TO RECORD AUDIO");
          break;
        }
        
        case 13:
        {
          Serial.println("DEEP LEARNING ISSUE DETECTED. FAILED TO RUN THE CLASSIFIER");
          break;
        }
      }
    }
    delay(100);
  }
}

void PlayVoice(int folder, int file) {
  myMP3.playFolder(folder, file);
  delay(150);
  while (!digitalRead(GPIO3));
}

void UpdateNiclaSensors() {
  for (uint8_t i = 0; i < 5; i++) {
    BHY2.update();                                                                //necessary to update sensor
    delay(10);
  }
}


void UpdateMainSensors() {
  float temperature_BMP390_readings, temperature_BME688_readings, pressure_BMP390_readings;        //declare and initialize variable for the sum of 10 readings
  uint16_t humidity_BME688_readings;                                                               //16bits are required to hold 10 values that can reach 10000

  for (uint8_t i = 0; i < 10; i++) {
    BHY2.update();                                                                            //necessary to update sensor classes
    temperature_BMP390_readings    += temperature.value();                                    //sum all the values from the array to create the threshold value
    temperature_BME688_readings    += bsec.comp_t();
    pressure_BMP390_readings       += pressure.value();
    humidity_BME688_readings       += bsec.comp_h();
    delay(50);                                                                      //get a new value every 50ms => 500ms + 12ms total runtime of loop
  }

  float temp_BMP390_outlier_higher_bound   = 1.20 * (temperature_BMP390_readings / 10);        //120% of the average of the last 10 values
  float temp_BMP390_outlier_lower_bound    = 0.80 * (temperature_BMP390_readings / 10);         //80% of the average of the last 10 values

  float temp_BME688_outlier_higher_bound   = 1.20 * (temperature_BME688_readings / 10);        //120% of the average of the last 10 values
  float temp_BME688_outlier_lower_bound    = 0.80 * (temperature_BME688_readings / 10);         //80% of the average of the last 10 values

  float pres_BMP390_outlier_higher_bound   = 1.20 * (pressure_BMP390_readings / 10);              //120% of the average of the last 10 values
  float pres_BMP390_outlier_lower_bound    = 0.80 * (pressure_BMP390_readings / 10);               //80% of the average of the last 10 values
  
  uint8_t hum_BME688_outlier_higher_bound  = 1.20 * (humidity_BME688_readings / 10);            //120% of the average of the last 10 values
  uint8_t hum_BME688_outlier_lower_bound   = 0.80 * (humidity_BME688_readings / 10);             //80% of the average of the last 10 values

  for (uint8_t t_BMP390, t_BMP688, p_BMP390, h_BME688; t_BMP390 < 10 && t_BMP688 < 10 && p_BMP390 < 10 && h_BME688 < 10;) {
    BHY2.update(); 
    if (temperature.value() >= temp_BMP390_outlier_lower_bound && temperature.value() <= temp_BMP390_outlier_higher_bound && t_BMP390 < 10) {
        temperature_BMP390_filtered = temperature_BMP390_ewma.filter(temperature.value());
        t_BMP390++;
    } if (bsec.comp_t() >= temp_BME688_outlier_lower_bound && bsec.comp_t() <= temp_BME688_outlier_higher_bound && t_BMP688 < 10) {
        temperature_BME688_filtered = temperature_BMP390_ewma.filter(bsec.comp_t());
        t_BMP688++;
    } if (pressure.value() >= pres_BMP390_outlier_lower_bound && pressure.value() <= pres_BMP390_outlier_higher_bound && p_BMP390 < 10) {
        pressure_BMP390_filtered = pressure_BMP390_ewma.filter(pressure.value());
        p_BMP390++;
    } if (bsec.comp_h() >= hum_BME688_outlier_lower_bound && bsec.comp_h() <= hum_BME688_outlier_higher_bound && h_BME688 < 10) {
        humidity_BME688_filtered = humidity_BME688_ewma.filter(bsec.comp_h());
        h_BME688++;
    }
    delay(100);
 }
}


void HeartMonitor() {
  heartrate.getValue(heartrate_pin); ///< A1 foot sampled values
  uint8_t rate_value = heartrate.getRate(); ///< Get heart rate value 
  if(rate_value)  {
    Serial.println("The heart rate is: " + String(rate_value));
  }
  delay(10);
}


float RoundToHalfInteger(float x) {
  return 0.5 * round(2.0 * x) ;
}


void ProcessSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600;                                          // Jan 1 2013

  pctime = Serial.parseInt();
  if ( pctime >= DEFAULT_TIME) {                                                          // check the integer is a valid time (greater than Jan 1 2013)
    setTime(pctime);                                                                      // Sync Arduino clock to the time received on the serial port
  }
}


time_t RequestSync() {
  Serial.write(TIME_REQUEST);
  return 0;                                                                                // the time will be sent later in response to serial mesg
}
