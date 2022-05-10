/*  This code is for the Nicla Sense ME module used in the AURI Smartwatch. The project represents the dissertation of the Year 2021-2022 at Middlesex University London, for the Robotics Engineering Course. Created by Ciprian-Florin Ifrim.
 *  The code uses publicly available libraries for the Arduino platform, which are also fully compatible with the Nicla Sense ME board. In order for the code to function it needs the second code used on the Seeed Xiao Sense which communicates to the Nicla through I2C.
 *  
 *  All delays are used with the mbed core to put the system to sleep and consume as little battery as possible. The main loop refreshes every 1 second, updating all sensors, and consuming circa 12uA for every refresh, added to the default 3.3mA of the board.
 *  All voice lines use specific rulesets created to simplify the audio output system considering the complexity of the voice lines and the necessity of a smooth and gapless play.
 *  
 *  Final Version: May 4, 2022
 */ 

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------LIBRARIES----------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Nicla_System.h"                                                                                                                                                             // Main Nicla Sense ME Library >> enables I2C, UART, pinMode etc.
#include "Arduino_BHY2.h"                                                                                                                                                             // Arduino Bosch library >> enables sensor readout and update functionalities
#include <Wire.h>                                                                                                                                                                     // I2C Protocol library (Nicla Sense ME custom)
#include <Adafruit_MLX90614.h>                                                                                                                                                        // Adafruit MLDX90614 Temperature Sensor Library
#include <Adafruit_SI1145.h>                                                                                                                                                          // Adafruit SI1145 UV Index Sensor Library
#include <Ewma.h>                                                                                                                                                                     // Exponentially Weighted Moving Average Library for ADC filtering
#include <Battery.h>                                                                                                                                                                  // Battery Sense library for voltage and percentage measurements
#include <bq25120a_nicla.h>                                                                                                                                                           // Nicla Sense ME Custom Library for BQ25120A Battery Charging IC
#include <I2C_Anything.h>                                                                                                                                                             // library to send any data type through I2C (normally low/high bits) 
#include <DFPlayerMini_Fast.h>                                                                                                                                                        // modified DFPlayer library for faster and simpler operation in file playing
#include <TimeLib.h>                                                                                                                                                                  // time library to keep track of timedate and sync functionality
#include <DFRobot_Heartrate.h>                                                                                                                                                        // DFRobot Gravity Heartrate sensor library for BMP output



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------DECLARATIONS--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define heartrate_pin A0                                                                                                                                                              // heartrate readings pin for the DFRobot Gravity Heartrate sensor
#define battery_sense A1                                                                                                                                                              // battery sense pin for reading the battery voltage (through a voltage divider)
#define busy_pin P0_10                                                                                                                                                                // busy pin definition to check whether the DFPlayer has finished playing
#define slave_address 0x08                                                                                                                                                            // defined slave address for I2C communication with the XIAO Sense Device
#define TIME_HEADER  'T'                                                                                                                                                              // header tag for serial time sync message
#define TIME_REQUEST  7                                                                                                                                                               // ASCII bell character requests a time sync message 

Sensor            humidity(SENSOR_ID_HUM);                                                                                                                                            // create HUMIDITY sensor object
Sensor            temperature(SENSOR_ID_TEMP);                                                                                                                                        // create TEMPERATURE sensor object
Sensor            pressure(SENSOR_ID_BARO);                                                                                                                                           // create PRESSURE sensor object
SensorBSEC        bsec(SENSOR_ID_BSEC);                                                                                                                                               // create BSEC sensor object
SensorOrientation orientation(SENSOR_ID_ORI);                                                                                                                                         // create ORIENTATION sensor object
Sensor            tilt(SENSOR_ID_WRIST_TILT_GESTURE);                                                                                                                                 // create WRIST TILT GESTURE sensor object
Sensor            steps(SENSOR_ID_STC);                                                                                                                                               // create STEPS sensor object
DFRobot_Heartrate heartrate(ANALOG_MODE);                                                                                                                                             // create HEARTRATE object as ANALOG_MODE or DIGITAL_MODE

DFPlayerMini_Fast DFPlayer;                                                                                                                                                           // create DFPLAYER object
Adafruit_SI1145 uv    = Adafruit_SI1145();                                                                                                                                            // create SI1145 sensor object >> UV index sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();                                                                                                                                          // create MLX sensor object >> laser temperature sensor
Battery battery       = Battery(3000, 4150, battery_sense);                                                                                                                           // create BATTERY object with specs ranging from 3 to 4.15 volts >> set sense pin for readout to A1

Ewma temperature_BMP390_ewma(0.2);                                                                                                                                                    // less smoothing (alpha value) - faster to detect changes, but more prone to noise
Ewma temperature_BME688_ewma(0.2);                                                                                                                                                    // less smoothing (alpha value) - faster to detect changes, but more prone to noise
Ewma pressure_BMP390_ewma(0.01);                                                                                                                                                      // most smoothing (alpha value) - less prone to noise, but very slow to detect changes
Ewma humidity_BME688_ewma(0.05);                                                                                                                                                      // more smoothing (alpha value) - less prone to noise, but slow to detect changes
                   
float   temperature_BMP390_filtered, temperature_BME688_filtered, pressure_BMP390_filtered, humidity_BME688_filtered;                                                                 // define global float variables for the filtered sensor outputs
bool    charging_flag = false;                                                                                                                                                        // charging flag used to output the voice command of charging only once
uint8_t volume_level  = 25;



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------SYSTEM SETUP--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  nicla::begin();                                                                                                                                                                     // initialise Nicla Sense ME internal protocols (I2C, drivers, UART etc.) >> without this everything stops after 48 seconds
  nicla::leds.begin();                                                                                                                                                                // initialise LED object (I2C driver) under NICLA namespace 
  Wire.begin();                                                                                                                                                                       // join the I2C bus (address optional for master)

  BHY2.begin();                                                                                                                                                                       // initialise Bosch Library for sensor definitions and updates
  temperature.begin();                                                                                                                                                                // initialise TEMPERATURE object (BMP390 sensor) from Nicla Sense ME 
  pressure.begin();                                                                                                                                                                   // initialise PRESSURE object (BMP390 sensor) from Nicla Sense ME 
  bsec.begin();                                                                                                                                                                       // initialise BSEC object (BME688 sensor) from Nicla Sense ME  
  orientation.begin();                                                                                                                                                                // initialise ORIENTATION object (compass) from Nicla Sense ME 
  steps.begin();                                                                                                                                                                      // initialise STEPS object (pedometer) from Nicla Sense ME 

  
  Serial1.begin(9600);
  Serial1.begin(9600);                                                                                                                                                                // start Hardware UART at 9600 bits per second baud rate
  DFPlayer.begin(Serial1, true);                                                                                                                                                     // start the DFPlayer process with Serial 1 and disable debug mode
  DFPlayer.volume(volume_level);                                                                                                                                                      // set default DFPlayer Mini volume as 25 >> maximum is 30

  battery.onDemand(8, HIGH);                                                                     
  battery.begin(3300, 2.14, &asigmoidal);                                                                                                                                             // start battery system set with a LiPo battery (asigmoidal draining) on a 3.3v MCU with 2.14 voltage ratio 
                                                                                                                                                                                      // the real voltage divider ratio is 1.3, the set ratio is 2.14 to fix the 10bit ADC library calculations >> Nicla has a 12 bit ADC
  mlx.begin();                                                                                                                                                                        // begin I2C communication with the Laser Temperature >> Adafruit Sensor MLX90614
  uv.begin();                                                                                                                                                                         // begin I2C communication with the UV Sensor >> Adafruit SI1145

  setSyncProvider(RequestSync);                                                                                                                                                       // set function to call when time sync is required

  digitalWrite(busy_pin, HIGH);                                                                                                                                                       // set DFPlayer Busy Pin as HIGH >> when the device is playing it outputs LOW (set as HIGH and OUTPUT by nicla::begin())
  pinMode(busy_pin, INPUT);                                                                                                                                                           // set busy pin (GPIO3/P0_10) as INPUT

  delay(1000);                                                                                                                                                                        // delay for the sensors to engage
}



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------MAIN ROUTINE--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
//------------------------------------------------------------------------------------------------------------------------------------MAIN CODE - SENSOR COMPUTATION - OUTPUT SELECTION-------------------------------------------------------------------------------------------------------------------------------------
  int activation = uv.readIR();
  if (activation >= 280) {                                                                                                                                                            // if the gesture event was recognised, then start the computation and inform the user
    // initialise system >> inform user
    nicla::leds.setColor(red);                                                                                                                                                        // set the led colour of the Nicla Sense ME to RED as a visual cue to the user that the system has been activated
    uint8_t random_listening = random(1, 11);                                                                                                                                         // returns a random number between min(1) and max-1(10) >> equivalent to the 10 listening voice lines available
    DFPlayer.play(random_listening + 1101);                                                                                                                                           // play audio to inform user that the system is listening >> instead of the voice function that waits for the file to end first                                                                                                                                                                                      

    unsigned long last_steps = steps.value();                                                                                                                                         // set the last steps in a variable for future usage in per command calculation

    BHY2.update();                                                                                                                                                                    // update Bosch Nicla sensors
    UpdateMainSensors();                                                                                                                                                              // filter the main sensors (temperature, humidity, pressure)

    float temperature_BMP390_regressed  = (1.0095 * temperature_BMP390_filtered) - 4.8051;                                                                                            // linearly regressed temperature output from BMP390 once filtered with an Exponentially Weighted Moving Average Low-Pass Filter
    float temperature_BME688_regressed  = (1.0095 * temperature_BME688_filtered) - 4.8051;                                                                                            // linearly regressed temperature output from BME688 once filtered with an Exponentially Weighted Moving Average Low-Pass Filter
    float temperature_regressed_fusion  = (temperature_BMP390_regressed + temperature_BME688_regressed) / 2;                                                                          // sensor fusion between the two temeperature outputs after filtering and regression
    float temperature_fusion_fahrenheit = (temperature_regressed_fusion * 9 / 5) + 32;                                                                                                // conversion of the temperature fusion value to Fahrenheit to be used in following equations
    float pressure_BMP390_regressed     = 1.00718 * pressure_BMP390_filtered;                                                                                                         // linearly regressed pressure output from BMP390 once filtered with an Exponentially Weighted Moving Average Low-Pass Filter
    uint8_t humidity_BME688_regressed   = round((1.4383 * humidity_BME688_filtered) - 2.5628);                                                                                        // linearly regressed humidity output from BME688 once filtered with an Exponentially Weighted Moving Average Low-Pass Filter
    uint32_t gas_resistance_BME688      = bsec.comp_g();                                                                                                                              // air/gas resistance measure in Ohms
    
    float    visible_light  = uv.readVisible();                                                                                                                                       // SI1145 Sensor Output >> Human Visible Light (wavelengths from 380 to 700 nanometers)
    float    infrared_light = uv.readIR();                                                                                                                                            // SI1145 Sensor Output >> Infrared Light Amount (wavelengths from 780 nanometers and 1 millimeters)
    uint16_t uv_index       = (uv.readUV()) / 100.0;                                                                                                                                  // SI1145 Sensor Output >> Estimated UV Index based on the rapport Infrared and Visible Light

    const float sea_pressure_constant = 1013.25;                                                                                                                                      // sea level pressure constant >> 1013.25 hectoPascals
    int16_t    altitude               = ((pow((sea_pressure_constant / pressure_BMP390_regressed), (1 / 5.257)) - 1) * (temperature_regressed_fusion + 273.15)) / 0.0065;             // calculate the altitude by using the hypsometric formula

    float relative_humidity         = (((humidity_BME688_regressed * pressure_BMP390_regressed) / (0.378 * humidity_BME688_regressed + 0.622))                                        // relative humidity from Bolton 1980 The computation of Equivalent Potential Temperature
                                      /(6.112 * exp((17.67 * temperature_regressed_fusion) / (temperature_regressed_fusion + 243.5))));                                               
    float air_dew_point             = ((243.12 * (log(relative_humidity / 100) + ((17.62 * temperature_regressed_fusion) / (243.12 + temperature_regressed_fusion))))                 // the dew point is the temperature to which air must be cooled to become saturated with water vapor
                                      /(17.62 - (log(relative_humidity / 100) + ((17.62 * temperature_regressed_fusion) / (243.12 + temperature_regressed_fusion)))));
    float saturation_vapor_pressure = 6.1078 * pow(10, ((7.5 * temperature_regressed_fusion) / (temperature_regressed_fusion + 237.3)));                                              // saturation vapor used for air density >> https://www.omnicalculator.com/physics/air-density#how-to-calculate-the-air-density
    float water_vapor_pressure      = (saturation_vapor_pressure * relative_humidity) / 100;                                                                                          // multiplication of saturation vapor pressure with the relative humidity, divided by 100%
    float dry_air_pressure          = pressure_BMP390_regressed - water_vapor_pressure;
    float equivalent_sea_pressure   = pressure_BMP390_regressed * pow((1 - ((0.0065 * altitude)/(temperature_regressed_fusion + 0.0065 * altitude + 273.15))), -5.257);
    const float dry_air_constant    = 287.058, water_vapor_constant = 461.495;                                                                                                        // specific gas constant for dry air equal in J/(kg·K)        //specific gas constant for water vapor in J/(kg·K)
    float air_density               = 100 * ((dry_air_pressure / (dry_air_constant * (temperature_regressed_fusion + 273.15)))                                                        // air density formula, can be used for air buoyancy
                                      +(water_vapor_pressure / ( water_vapor_constant * (temperature_regressed_fusion + 273.15))));
    float absolute_humidity         = (((saturation_vapor_pressure * relative_humidity) / 10) / (water_vapor_constant * temperature_regressed_fusion));                               // absolute humidity formula expressed in kilograms per cubic meter of moist air - https://planetcalc.com/2167/
                                                                                                                                                   
                               
    float heat_index_Rothfusz          = -42.379 + (2.04901523 * temperature_fusion_fahrenheit) - (0.00000199 * pow(temperature_fusion_fahrenheit, 2) * pow(relative_humidity, 2))    // felt air temperature, does not take into consideration direct sunlight effect or shade
                                         -(0.22475541 * temperature_fusion_fahrenheit * relative_humidity) - (0.00683783 * pow(temperature_fusion_fahrenheit, 2))                     // the regression equation of Rothfusz used by the US National Weather Service: 
                                         -(0.05481717 * pow(relative_humidity, 2)) + (0.00122874 * pow(temperature_fusion_fahrenheit, 2) * relative_humidity)                         // https://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml  
                                         +(0.00085282 * temperature_fusion_fahrenheit * pow(relative_humidity, 2)) + (10.14333127 * relative_humidity);                               
    float heat_index_Steadman          = 0.5 * (temperature_fusion_fahrenheit + 61.0 + ((temperature_fusion_fahrenheit - 68.0) * 1.2) + (relative_humidity * 0.094));                 // the simple heat index/felt temperature equation, used when the Fahrenheit Temperature is lower or equal to 80

    float heat_index_lowRH_adjustment  = ((13 - relative_humidity) / 4) * sqrt((17 - abs(temperature_fusion_fahrenheit - 95)) / 17);                                                  // if the RH is less than 13% and the temperature is between 80 and 112 degrees F, then the following adjustment is subtracted from HI
    float heat_index_highRH_adjustment = ((relative_humidity - 85) / 10) * ((87 - temperature_fusion_fahrenheit) / 5);                                                                // if the RH is greater than 85% and the temperature is between 80 and 87 degrees F, then the following adjustment is added to HI

    // heat index decision tree
    float heat_index = 0;                                                                                                                                                             // initialise heat index variable
    if (heat_index_Steadman >= 80) {                                                                                                                                                  // if this heat index value is 80 degrees F or higher, the full regression equation of Rothfusz (along with any adjustment) is applied
      if (relative_humidity < 13 && temperature_fusion_fahrenheit > 80 && temperature_fusion_fahrenheit < 112) {
          heat_index = ((heat_index_Rothfusz - heat_index_lowRH_adjustment) - 32) * 5 / 9;                                                                                            // apply the LOW Relative Humidity adjustement to Rothfusz and convert to Celsius from Fahrenheit
      } else if (relative_humidity > 85 && temperature_fusion_fahrenheit > 80 && temperature_fusion_fahrenheit < 87) {
          heat_index = ((heat_index_Rothfusz + heat_index_highRH_adjustment) - 32) * 5 / 9;                                                                                           // apply the HIGH Relative Humidity adjustement to Rothfusz and convert to Celsius from Fahrenheit
      } else heat_index = (heat_index_Rothfusz - 32) * 5 / 9;
    } else heat_index = (((heat_index_Steadman + temperature_fusion_fahrenheit) / 2) - 32) * 5 / 9;                                                                                   // the Steadman formula is computed first and the result averaged with the temperature for higher accuracy, then converted to Celsius

    // features array for machine learning weather classification and rainfall regression
    float X[6] = {temperature_regressed_fusion, heat_index, air_dew_point, relative_humidity, equivalent_sea_pressure, uv_index};                                                     // array to send to the XIAO Sense for Machine Learning classification/regression >> contains the 6 features needed for the ML models

    // computation ended
    while (!digitalRead(busy_pin));                                                                                                                                                   // block the code if the computation has been completed before listening voice line 

    // begin transfer from Nicla Sense ME to Seeed Xiao Sense through I2C
    Wire.beginTransmission(slave_address);                                                                                                                                            // transmit to device #8 (SlaveAddress is used)
    for (uint8_t i = 0; i < sizeof(X); i++) I2C_writeAnything(X[i]);                                                                                                                  // transit every element from the features array individually using the I2C_Anything library >> enables the transmission of floats
    Wire.endTransmission();                                                                                                                                                           // stop transmitting

    nicla::leds.setColor(green);                                                                                                                                                      // change led to green to visually inform the user they can speak >> visual cue
    
    // wait for the Xiao Sense to finish recording the audio and performing the classification
    delay(3500);                                                                                                                                                                      // delay for time to send everything & time to unpack on the Xiao Sense & 350ms reaction delay & recording (1650ms)  >> circa 2050ms
    nicla::leds.setColor(off);                                                                                                                                                        // turn the LED off
    //PlayVoice(int(round(random_listening/2)) + 1135);                                                                                                                                 // play random voice line betwee 
    
    // receive data from the Seeed Xiao Sense to Nicla Sense ME through I2C
    float dataReceived[6];                                                                                                                                                            // define float array of 6 elements for the data received (labels) 
    Wire.requestFrom(slave_address, sizeof(dataReceived));                                                                                                                            // request the voice command label + ML models labels (4) + ML precipitation regression
    while (Wire.available()) { 
      for (uint8_t j = 0; j < sizeof(dataReceived); j++) I2C_readAnything(dataReceived[j]);                                                                                           // slave may send less than requested >> for the duration 
    }
    delay(200);

    Serial.println("The received label is: " + String(dataReceived[5]);
  }
  delay(1000);                                                                                                                                                                        // delay to reduce the main loop update rate and reduce battery usage
}



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------SUBROUTINES---------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------DISCOMFORT COMPUTATION---------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t DiscomfortComputation(float temperature, uint8_t humidity) {
  uint8_t index = temperature - 0.55 * (1 - 0.01 * humidity) * (temperature - 14.5);                                                                                                  // discomfort index formula
  return index;                                                                                                                                                                       // return discomfort index value as 8 bit integer
}

//----------------------------------------------------------------------------------------------------------------------------------------------------SPEECH FUNCTION-------------------------------------------------------------------------------------------------------------------------------------------------------
void PlayVoice(int file) {
  DFPlayer.play(file);                                                                                                                                                                // play the wav file specified with the play function of the DFPlayer object
  delay(500);                                                                                                                                                                         // delay 400ms necessary to compute the command >> any delay higher than 150ms works 
  while (!digitalRead(busy_pin));                                                                                                                                                     // block the code for the duration of the file playing >> busy pin will be LOW
} 

//----------------------------------------------------------------------------------------------------------------------------------------------TEMPERATURE OUTPUT FORMATTED------------------------------------------------------------------------------------------------------------------------------------------------
void TemperatureFormatted(float temperature) {
  if (temperature < 0) PlayVoice(43);                                                                                                                                                 // if the given temperature is negative, then play the "negative" voice line
  
  temperature = RoundToHalfInteger(temperature);                                                                                                                                      // round temperature to .5 or .0
  if (int(temperature * 10) % 10 == 0) PlayVoice(int(temperature) + 64);                                                                                                                 // if it has been rounded up to 0, play voice lines for the temperature from a specific first set based on the rule
  else if (int(temperature * 10) % 10 == 5) PlayVoice(int(temperature + 0.5) + 926);                                                                                                     // if it has been rounded up to 5, play voice lines for the temperature from the second set based on the rule
}

//-------------------------------------------------------------------------------------------------------------------------------------------DISTANCE/ALTITUDE OUTPUT FORMATTED---------------------------------------------------------------------------------------------------------------------------------------------
void DistanceFormatted(uint32_t distance_value) {
  if (distance_value < 0) {                                                                                                                                                           // if the distance is negative then proceed
      PlayVoice(43);                                                                                                                                                                  // Voice Line: "negative"
      PlayVoice(-(distance_value) + 1141);                                                                                                                                            // Voice Line: Absolute value of the distance as a number between 0 and 999
      MetersFormatted(-(distance_value));                                                                                                                                             // Formatting Meters or Meter output depending on the value
  } else if (distance_value >= 0 && distance_value < 1000) {
      PlayVoice(distance_value + 1141);                                                                                                                                               // Voice Line: number between 0 and 999
      MetersFormatted(distance_value);                                                                                                                                                // Formatting Meters or Meter output depending on the value
  } else {
      uint8_t  distance_value_kilometers = distance_value / 1000;                                                                                                                     // find from the given value the number of kilometers
      uint16_t distance_value_meters     = distance_value % 1000;                                                                                                                     // find from the given value the number of meters
  
      PlayVoice(distance_value_kilometers + 1141);                                                                                                                                    // Voice Line: number between 0 and 999 
      if (distance_value_kilometers == 1) PlayVoice(45);                                                                                                                              // if the distance_travelled is 1 kilometer + x meters, then say "kilometer" voice line, not "kilometers"
      else PlayVoice(46);                                                                                                                                                             // the distance_travelled is higher than 1 kilometer, so say "kilometers"

      if (distance_value_meters != 0) {
        delay(100);                                                                                                                                                                   // helps with voice smoothness
        PlayVoice(23);                                                                                                                                                                // Voice Line: "and"
        PlayVoice(int(distance_value_meters) + 1141);                                                                                                                                 // Voice Line: number between 0 and 999
        MetersFormatted(distance_value_meters);                                                                                                                                       // Formatting Meters or Meter output depending on the value
      }
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------METERS OUTPUT FORMATTED--------------------------------------------------------------------------------------------------------------------------------------------------
void MetersFormatted(uint16_t meters) {
  if (meters == 1) PlayVoice(1117);                                                                                                                                                   // if the given value is equal to 1 then output "meter"          
  else PlayVoice(44);                                                                                                                                                                 // Voice Line: "meters"                       
}

//------------------------------------------------------------------------------------------------------------------------------TOTAL/CURRENT STEPS & KILOCALORIES/KILOJOULS OUTPUT FORMATTED-------------------------------------------------------------------------------------------------------------------------------
void OutputFormatted(unsigned long output) { 
  if (output < 1000) {                                                                                                                                                                // if the output is lower than 1 thousand, then use the voice lines available for the number speech
      PlayVoice(int(output) + 1141);                                                                                                                                                  // Voice Line: number between 0 and 999
  } else if (output >= 1000) {                                                                                                                                                        // if the output is higher or equal to 1000, then convert the output to thousand + remainder
      uint8_t  output_thousands = output / 1000;                                                                                                                                      // divide the value by 1000 to get the number of thousands
      uint16_t output_remainder = output % 1000;                                                                                                                                      // use Arduino Modulo function to get the remaining amount
  
      PlayVoice(output_thousands + 1141);                                                                                                                                             // Voice Line: number between 0 and 999
      PlayVoice(1116);                                                                                                                                                                // Voice Line: "thousand"
      
      if (output_remainder != 0) {
        delay(100);                                                                                                                                                                   // helps with voice smoothness
        PlayVoice(23);                                                                                                                                                                // Voice Line: "and"
        PlayVoice(int(output_remainder) + 1141);                                                                                                                                      // Voice Line: number between 0 and 999
      }                                              
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------COMPASS DEGREES OUTPUT FORMATTED---------------------------------------------------------------------------------------------------------------------------------------------
void DegreesFormatted(float deg_value) {
  if (deg_value < 0) PlayVoice(43);                                                                                                                                                   // if the given degrees value is negative, then play the "negative" voice line
  PlayVoice(int(round(abs(deg_value)) + 381));                                                                                                                                        // Voice Line: between 0 to 360 degrees 
}

//-----------------------------------------------------------------------------------------------------------------------------------------MAIN SENSORS THRESHOLDING/EWMA FILTERING-----------------------------------------------------------------------------------------------------------------------------------------
void UpdateMainSensors(void) {
  float temperature_BMP390_readings = 0, temperature_BME688_readings = 0, pressure_BMP390_readings = 0;                                                                               // declare and initialize variable for the sum of 10 readings
  uint16_t humidity_BME688_readings = 0;                                                                                                                                              // 16 bits/2 bytes are required to hold 10 values that can reach 10000

  for (uint8_t i = 0; i < 10; i++) {                                                                                                                                                  // 10 total values updated with a loop
    BHY2.update();                                                                                                                                                                    // necessary to update sensor classes
    temperature_BMP390_readings += temperature.value();                                                                                                                               // sum all the values from the array to create the temperature threshold value
    temperature_BME688_readings += bsec.comp_t();                                                                                                                                     // sum all the values from the array to create the temperature 2 threshold value
    pressure_BMP390_readings    += pressure.value();                                                                                                                                  // sum all the values from the array to create the pressure threshold value
    humidity_BME688_readings    += bsec.comp_h();                                                                                                                                     // sum all the values from the array to create the humidity threshold value
    delay(50);                                                                                                                                                                        // get a new value every 50ms => 500ms + 12ms total runtime of loop
  }

  float temp_BMP390_outlier_higher_bound   = 1.20 * (temperature_BMP390_readings / 10);                                                                                               // 120% of the average of the last 10 values
  float temp_BMP390_outlier_lower_bound    = 0.80 * (temperature_BMP390_readings / 10);                                                                                               // 80% of the average of the last 10 values

  float temp_BME688_outlier_higher_bound   = 1.20 * (temperature_BME688_readings / 10);                                                                                               // 120% of the average of the last 10 values
  float temp_BME688_outlier_lower_bound    = 0.80 * (temperature_BME688_readings / 10);                                                                                               // 80% of the average of the last 10 values

  float pres_BMP390_outlier_higher_bound   = 1.20 * (pressure_BMP390_readings / 10);                                                                                                  // 120% of the average of the last 10 values
  float pres_BMP390_outlier_lower_bound    = 0.80 * (pressure_BMP390_readings / 10);                                                                                                  // 80% of the average of the last 10 values
  
  uint8_t hum_BME688_outlier_higher_bound  = 1.20 * (humidity_BME688_readings / 10);                                                                                                  // 120% of the average of the last 10 values
  uint8_t hum_BME688_outlier_lower_bound   = 0.80 * (humidity_BME688_readings / 10);                                                                                                  // 80% of the average of the last 10 values

  for (uint8_t t_BMP390, t_BMP688, p_BMP390, h_BME688; t_BMP390 < 10 && t_BMP688 < 10 && p_BMP390 < 10 && h_BME688 < 10;) {                                                           // for loop to get all values updated 10 times through EWMA
    BHY2.update();                                                                                                                                                                    // update sensor values each loop
    if (temperature.value() >= temp_BMP390_outlier_lower_bound && temperature.value() <= temp_BMP390_outlier_higher_bound && t_BMP390 < 10) {                                         // if the BMP390 temperature reading is between the lower and higher bounds of the threshold, and there have been less than 10 updates
        temperature_BMP390_filtered = temperature_BMP390_ewma.filter(temperature.value());                                                                                            // update BMP390 temperature filter variable 
        t_BMP390++;                                                                                                                                                                   // add +1 every time the BMP390 temperature filter has been updated >> maximum 10
    } if (bsec.comp_t() >= temp_BME688_outlier_lower_bound && bsec.comp_t() <= temp_BME688_outlier_higher_bound && t_BMP688 < 10) {                                                   // if the BME688 temperature reading is between the lower and higher bounds of the threshold, and there have been less than 10 updates
        temperature_BME688_filtered = temperature_BMP390_ewma.filter(bsec.comp_t());                                                                                                  // update BME688 temperature filter variable 
        t_BMP688++;                                                                                                                                                                   // add +1 every time the BME688 temperature filter has been updated >> maximum 10
    } if (pressure.value() >= pres_BMP390_outlier_lower_bound && pressure.value() <= pres_BMP390_outlier_higher_bound && p_BMP390 < 10) {                                             // if the BMP390 pressure reading is between the lower and higher bounds of the threshold, and there have been less than 10 updates
        pressure_BMP390_filtered = pressure_BMP390_ewma.filter(pressure.value());                                                                                                     // update BMP390 pressure filter variable 
        p_BMP390++;                                                                                                                                                                   // add +1 every time the pressure filter has been updated >> maximum 10
    } if (bsec.comp_h() >= hum_BME688_outlier_lower_bound && bsec.comp_h() <= hum_BME688_outlier_higher_bound && h_BME688 < 10) {                                                     // if the BME688 humidity reading is between the lower and higher bounds of the threshold, and there have been less than 10 updates
        humidity_BME688_filtered = humidity_BME688_ewma.filter(bsec.comp_h());                                                                                                        // update BME688 humidity filter variable 
        h_BME688++;                                                                                                                                                                   // add +1 every time the humidity filter has been updated >> maximum 10
    }
    delay(100);
 }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------HEART MONITOR OUTPUT--------------------------------------------------------------------------------------------------------------------------------------------------
void HeartMonitor(void) {
  heartrate.getValue(heartrate_pin);                                                                                                                                                  // analog heart rate sampled values
  uint8_t rate_value = heartrate.getRate();                                                                                                                                           // get heart rate value 
  if(rate_value)  {                                                                                                                                                                   // if the sensor has been positioned properly
    //Serial.println("The heart rate is: " + String(rate_value));                                                                                                                     // play voice for the heart rate value
  }
  delay(10);
}

//----------------------------------------------------------------------------------------------------------------------------------------------VARIABLE ROUNDING TO HALF INTEGER-------------------------------------------------------------------------------------------------------------------------------------------
float RoundToHalfInteger(float x) {
  return 0.5 * round(2.0 * x) ;                                                                                                                                                       // equation to round any value to .0 or .5 >> used for temperature throghout the code
}

//---------------------------------------------------------------------------------------------------------------------------------------------------TIMEDATE SYNC PROCESSING-----------------------------------------------------------------------------------------------------------------------------------------------
void ProcessSyncMessage(void) {
  unsigned long pctime;                                                                                                                                                               // variable to store the correct timedate received from the PC Serial
  const unsigned long DEFAULT_TIME = 1357041600;                                                                                                                                      // Jan 1 2013

  pctime = Serial.parseInt();
  if ( pctime >= DEFAULT_TIME) {                                                                                                                                                      // check the integer is a valid time (greater than Jan 1 2013)
    setTime(pctime);                                                                                                                                                                  // sync Arduino clock to the time received on the serial port
  }
}

//------------------------------------------------------------------------------------------------------------------------------------------------REQUEST TIMEDATE SYNC FUNCTION------------------------------------------------------------------------------------------------------------------------------------------------
time_t RequestSync(void) {
  Serial.write(TIME_REQUEST);                                                                                                                                                         // serial write the sync request header
  return 0;                                                                                                                                                                           // the time will be sent later in response to serial mesg
}
