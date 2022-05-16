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
#include "mbed.h" 
#include "Arduino.h" 
#include "Nicla_System.h"                                                                                                                                                             // Main Nicla Sense ME Library >> enables I2C, UART, pinMode etc.
#include "Arduino_BHY2.h"                                                                                                                                                             // Arduino Bosch library >> enables sensor readout and update functionalities
#include "Wire.h"                                                                                                                                                                     // I2C Protocol library (Nicla Sense ME custom)
#include "Adafruit_MLX90614.h"                                                                                                                                                        // Adafruit MLDX90614 Temperature Sensor Library
#include "Adafruit_SI1145.h"                                                                                                                                                          // Adafruit SI1145 UV Index Sensor Library
#include "Ewma.h"                                                                                                                                                                     // Exponentially Weighted Moving Average Library for ADC filtering
#include "Battery.h"                                                                                                                                                                  // Battery Sense library for voltage and percentage measurements
#include "bq25120a_nicla.h"                                                                                                                                                           // Nicla Sense ME Custom Library for BQ25120A Battery Charging IC
#include "I2C_Anything.h"                                                                                                                                                             // library to send any data type through I2C (normally low/high bits) 
#include "DFRobotDFPlayerMini.h"                                                                                                                                                      // modified DFPlayer library for faster and simpler operation in file playing
#include "TimeLib.h"                                                                                                                                                                  // time library to keep track of timedate and sync functionality



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------DECLARATIONS--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define heartrate_pin A0                                                                                                                                                              // heartrate readings pin for the DFRobot Gravity Heartrate sensor
#define battery_sense A1                                                                                                                                                              // battery sense pin for reading the battery voltage (through a voltage divider)
#define busy_pin P0_10                                                                                                                                                                // busy pin definition to check whether the DFPlayer has finished playing
#define slave_address 0x08                                                                                                                                                            // defined slave address for I2C communication with the XIAO Sense Device
#define TIME_HEADER   'T'                                                                                                                                                             // header tag for serial time sync message
#define TIME_REQUEST   7                                                                                                                                                              // ASCII bell character requests a time sync message 

using namespace mbed;
using namespace rtos;

Sensor              humidity(SENSOR_ID_HUM);                                                                                                                                          // create HUMIDITY sensor object
Sensor              temperature(SENSOR_ID_TEMP);                                                                                                                                      // create TEMPERATURE sensor object
Sensor              pressure(SENSOR_ID_BARO);                                                                                                                                         // create PRESSURE sensor object
Sensor              tilt(SENSOR_ID_WRIST_TILT_GESTURE);                                                                                                                               // create WRIST TILT GESTURE sensor object
Sensor              motion_detection(SENSOR_ID_MOTION_DET);
Sensor              steps(SENSOR_ID_STC);                                                                                                                                             // create STEPS sensor object
SensorBSEC          bsec(SENSOR_ID_BSEC);                                                                                                                                             // create BSEC sensor object
SensorOrientation   orientation(SENSOR_ID_ORI);                                                                                                                                       // create ORIENTATION sensor object
SensorXYZ           gravityvector(SENSOR_ID_GRA);
SensorQuaternion    geomagneticrotationvector(SENSOR_ID_GEORV);
DFRobotDFPlayerMini DFPlayer;                                                                                                                                                         // create DFPLAYER object

Adafruit_SI1145 uv    = Adafruit_SI1145();                                                                                                                                            // create SI1145 sensor object >> UV index sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();                                                                                                                                          // create MLX sensor object >> laser temperature sensor
Battery battery       = Battery(3000, 4150, battery_sense);                                                                                                                           // create BATTERY object with specs ranging from 3 to 4.15 volts >> set sense pin for readout to A1

Ewma temperature_BMP390_ewma(0.02);                                                                                                                                                   // less smoothing (alpha value) - faster to detect changes, but more prone to noise
Ewma temperature_BME688_ewma(0.02);                                                                                                                                                   // less smoothing (alpha value) - faster to detect changes, but more prone to noise
Ewma pressure_BMP390_ewma(0.01);                                                                                                                                                      // most smoothing (alpha value) - less prone to noise, but very slow to detect changes
Ewma humidity_BME688_ewma(0.05);                                                                                                                                                      // more smoothing (alpha value) - less prone to noise, but slow to detect changes
Ewma heart_rate(0.02);                                                                                                                                                                // modest smoothing (alpha value) - faster to detect changes, but more prone to noise

bool    condition_flags[9]  = {false, false, false, false, false, false, false, false, false};                                                                                        // array holding flags to not repeat the voice line continously, as soon as the condition is cleared, reset the flag                   
bool    charging_flag    = false;                                                                                                                                                     // charging flag used to output the voice command of charging only once
bool    low_battery_mode = false;                                                                                                                                                     // flag to set the system in low power mode >> the user cannot use voice commands and the environment checks update every 5 minutes

float   temperature_BMP390_filtered, temperature_BME688_filtered, pressure_BMP390_filtered, humidity_BME688_filtered;                                                                 // define global float variables for the filtered sensor outputs
uint_least8_t volume_level  = 25;





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
  gravityvector.begin();
  geomagneticrotationvector.begin();
  motion_detection.begin();

  Serial1.begin(9600);                                                                                                                                                                // start Hardware UART at 9600 bits per second baud rate
  DFPlayer.begin(Serial1, false, false);                                                                                                                                              // start the DFPlayer object with Serial1 (disable debug (1st false), 2nd false to disable the amp reset which creates a pop sound)

  battery.onDemand(8, HIGH);                                                                     
  battery.begin(3300, 2.14, &asigmoidal);                                                                                                                                             // start battery system set with a LiPo battery (asigmoidal draining) on a 3.3v MCU with 2.14 voltage ratio 
                                                                                                                                                                                      // the real voltage divider ratio is 1.3, the set ratio is 2.14 to fix the 10bit ADC library calculations >> Nicla has a 12 bit ADC
  mlx.begin();                                                                                                                                                                        // begin I2C communication with the Laser Temperature >> Adafruit Sensor MLX90614
  uv.begin();                                                                                                                                                                         // begin I2C communication with the UV Sensor >> Adafruit SI1145

  digitalWrite(busy_pin, HIGH);                                                                                                                                                       // set DFPlayer Busy Pin as HIGH >> when the device is playing it outputs LOW (set as HIGH and OUTPUT by nicla::begin())
  pinMode(busy_pin, INPUT);                                                                                                                                                           // set busy pin (GPIO3/P0_10) as INPUT
  pinMode(heartrate_pin, INPUT);
    
  ThisThread::sleep_for(5000);                                                                                                                                                        // delay for the sensors to engage
  DFPlayer.volume(volume_level);                                                                                                                                                      // set default DFPlayer Mini volume as 25 >> maximum is 30
  
  setSyncProvider(RequestSync);                                                                                                                                                       // set function to call when time sync is required
}



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------MAIN ROUTINE--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
//----------------------------------------------------------------------------------------------------------------------------------------ENVIRONMENT CONTAMINATION/QUALITY CHECKS------------------------------------------------------------------------------------------------------------------------------------------
  BHY2.update();                                                                                                                                                                      // refresh Bosch Sensors
  uint_least8_t  discomfort_index = DiscomfortComputation(((1.0095 * bsec.comp_t()) - 4.8051), round((1.4383 * bsec.comp_h()) - 2.5628));                                             // compute discomfort index by using function
  uint_least16_t uv_index         = (uv.readUV()) / 100.0;                                                                                                                            // SI1145 Sensor Output >> Estimated UV Index based on the rapport Infrared and Visible Light
  
  uint_least16_t checks[9]        = {pressure.value(), bq25120_getLevel(), pressure.value(), discomfort_index, uv_index, bsec.iaq(), bsec.co2_eq(), bsec.b_voc_eq(), bsec.b_voc_eq()};    
  uint_least16_t conditions[9]    = {1007, 2, 1023, 32, 8, 225, 2100, 10, 15};                                                                                                        // array holding all conditions for each specific sensor for what would be considered extreme values/health adverse effects
  uint_least16_t voice_outputs[9] = {368, 1135, 369, 375, 1130, 364, 357, 348, 349};                                                                                                  // array containing all voice lines for each sensor based on the specific extreme condition
  
  for (uint_least8_t i = 0; i < 9; i++) {                                                                                                                                                   // for loop to consider all conditions in an elegant manner
    if (i <= 1 && condition_flags[i] == false) {                                                                                                                                      // if the array element is 0 (pressure) and the value is lower, then proceed                                                                                                                                              
        if (checks[i] <= conditions[i] && checks[i] > 0) {                                                                                                                            // if the read value is lower than the threshold but higher than 0 >> helps with empty sensor frames at the start of the system
          condition_flags[i] = true;                                                                                                                                                  // make status flag TRUE
          PlayVoice(voice_outputs[i]);                                                                                                                                                // and play specific condition voice line
          if (i == 1) low_battery_mode = true;
        } else condition_flags[i] = false;
    } else if (i >=- 2 && condition_flags[i] == false) {                                                                                                                              // if sensor condition is met, while the flag is false, and the element is 1 or higher                                                                                                                                                    
        if (checks[i] > conditions[i]) {
          condition_flags[i] = true;                                                                                                                                                  // make status flag TRUE
          PlayVoice(voice_outputs[i]);                                                                                                                                                // and play specific condition voice line
        } else condition_flags[i] = false;                                                                                                                                            // make status flag FALSE    
    }                                                                                                                                                                                                                                                                                                                            
  }

//-------------------------------------------------------------------------------------------------------------------------------------------------BATTERY CHARGING CHECK---------------------------------------------------------------------------------------------------------------------------------------------------
  byte battery_status = bq25120_getStatus();                                                                                                                                          // get the status of the battery and save it as a byte
  if (battery_status == 195 && charging_flag == false) {                                                                                                                              // if the watch is charging and the flag is false, then proceed
    charging_flag = true;                                                                                                                                                             // set the flag to true
    PlayVoice(5648);                                                                                                                                                                  // inform the user the watch is charging
  } else if (battery_status != 195 && charging_flag == true) charging_flag = false;                                                                                                   // is the battery status is different from 195 representing "charging" and the flag is true, then set it to false
 
//-------------------------------------------------------------------------------------------------------------------------------------------------TIMESYNC MESSAGE CHECK---------------------------------------------------------------------------------------------------------------------------------------------------
  if (Serial.available() > 1) {                                                                                                                                                       // wait for at least two characters
    char c = Serial.read();                                                                                                                                                           // read the charater from the serial
    if (c == TIME_HEADER) ProcessSyncMessage();                                                                                                                                       // if the received character matches the time header definition, then start the processing of the message
  }

//------------------------------------------------------------------------------------------------------------------------------------MAIN CODE - SENSOR COMPUTATION - OUTPUT SELECTION-------------------------------------------------------------------------------------------------------------------------------------
  // using the IR sensor for testing when the system is on the desk
  int activation = uv.readIR();
  if (activation >= 330 && low_battery_mode == false) {                                                                                                                               // if the gesture event was recognised, with the battery level higher than 10%, then start the computation and inform the user
    // initialise system >> inform user
    nicla::leds.setColor(red);                                                                                                                                                        // set the led colour of the Nicla Sense ME to RED as a visual cue to the user that the system has been activated
    uint_least8_t random_listening = random(1, 11);                                                                                                                                   // returns a random number between min(1) and max-1(10) >> equivalent to the 10 listening voice lines available
    DFPlayer.play(random_listening + 1101);                                                                                                                                           // play audio to inform user that the system is listening >> instead of the voice function that waits for the file to end first                                                                                                                                                                                      

    unsigned long last_steps = steps.value();                                                                                                                                         // set the last steps in a variable for future usage in per command calculation

    BHY2.update();                                                                                                                                                                    // update Bosch Nicla sensors
    UpdateMainSensors();                                                                                                                                                              // filter the main sensors (temperature, humidity, pressure)

    float temperature_BMP390_regressed  = (0.6990 * temperature_BMP390_filtered) + 4.210;                                                                                             // linearly regressed temperature output from BMP390 once filtered with an Exponentially Weighted Moving Average Low-Pass Filter
    float temperature_BME688_regressed  = (0.6990 * temperature_BME688_filtered) + 4.210;                                                                                             // linearly regressed temperature output from BME688 once filtered with an Exponentially Weighted Moving Average Low-Pass Filter
    float temperature_regressed_fusion  = (temperature_BMP390_regressed + temperature_BME688_regressed) / 2;                                                                          // sensor fusion between the two temeperature outputs after filtering and regression
    float temperature_fusion_fahrenheit = (temperature_regressed_fusion * 9 / 5) + 32;                                                                                                // conversion of the temperature fusion value to Fahrenheit to be used in following equations
    float pressure_BMP390_regressed     = 1.00718 * pressure_BMP390_filtered;                                                                                                         // linearly regressed pressure output from BMP390 once filtered with an Exponentially Weighted Moving Average Low-Pass Filter
   
    uint_least8_t  humidity_BME688_regressed = round((1.4383 * humidity_BME688_filtered) - 2.5628);                                                                                   // linearly regressed humidity output from BME688 once filtered with an Exponentially Weighted Moving Average Low-Pass Filter
    uint_least32_t gas_resistance_BME688     = bsec.comp_g();                                                                                                                         // air/gas resistance measure in Ohms
    
    float    visible_light  = uv.readVisible();                                                                                                                                       // SI1145 Sensor Output >> Human Visible Light (wavelengths from 380 to 700 nanometers)
    float    infrared_light = uv.readIR();                                                                                                                                            // SI1145 Sensor Output >> Infrared Light Amount (wavelengths from 780 nanometers and 1 millimeters)
    uint_least16_t uv_index = (uv.readUV()) / 100.0;                                                                                                                                  // SI1145 Sensor Output >> Estimated UV Index based on the rapport Infrared and Visible Light

    const float sea_pressure_constant = 1013.25;                                                                                                                                      // sea level pressure constant >> 1013.25 hectoPascals
    int16_t     altitude              = ((pow((sea_pressure_constant / pressure_BMP390_regressed), (1 / 5.257)) - 1) * (temperature_regressed_fusion + 273.15)) / 0.0065;             // calculate the altitude by using the hypsometric formula

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
    nicla::leds.setColor(green);                                                                                                                                                      // change led to green to visually inform the user they can speak >> visual cue
    ThisThread::sleep_for(10);                                                                                                                                                        // adds stability to the system 
    
    // begin transfer from Nicla Sense ME to Seeed Xiao Sense through I2C
    Wire.beginTransmission(slave_address);                                                                                                                                            // transmit to device #8 (SlaveAddress is used)
    for (uint_least8_t i = 0; i < sizeof(X); i++) I2C_writeAnything(X[i]);                                                                                                            // transit every element from the features array individually using the I2C_Anything library >> enables the transmission of floats
    Wire.endTransmission();                                                                                                                                                           // stop transmitting

    // wait for the Xiao Sense to finish recording the audio and performing the classification
    ThisThread::sleep_for(2500);                                                                                                                                                      // delay for time to send everything & time to unpack on the Xiao Sense & 350ms reaction delay & recording (1650ms)  >> circa 2050ms
    nicla::leds.setColor(off);                                                                                                                                                        // turn the LED off
    uint_least8_t random_computing = random(1, 6);                                                                                                                                    // returns a random number between min(1) and max-1(5) >> equivalent to the 5 computation voice lines available
    PlayVoice(random_computing + 1141);                                                                                                                                               // play random voice line between the 5 available
    ThisThread::sleep_for(100);                                                                                                                                                       // adds stability to the system 
    
    // receive data from the Seeed Xiao Sense to Nicla Sense ME through I2C
    float dataReceived[6];                                                                                                                                                            // define float array of 6 elements for the data received (labels) 
    dataReceived[5] = 14;
    Wire.requestFrom(slave_address, sizeof(dataReceived));                                                                                                                            // request the voice command label + ML models labels (4) + ML precipitation regression
    while (Wire.available()) for (uint_least8_t j = 0; j < sizeof(dataReceived); j++) I2C_readAnything(dataReceived[j]);                                                              // slave may send less than requested >> for the duration 
    ThisThread::sleep_for(300);                                                                                                                                                       // adds stability to the system 



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------SWITCH OUTPUT BASED ON VOICE COMMAND---------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    switch (int(dataReceived[5])) {
//--------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 0 >> Command: "Update me on the battery level."-----------------------------------------------------------------------------------------------------------------
      case 0:  
      {         
        // battery checks
        uint_least16_t battery_voltage_circuit    = battery.voltage();
        uint_least8_t  battery_percentage_circuit = battery.level();
        uint_least8_t  battery_level_bq25120      = bq25120_getLevel();
        byte     battery_fault_bq25120      = bq25120_getFaults();

        PlayVoice(int(battery_level_bq25120 + 37));                                                                                                                                   // 1 out of 5 voice lines representing the battery level as full, high, medium, low, empty based on the BQ25120 IC and custom library
        PlayVoice(int(battery_percentage_circuit + 144));                                                                                                                             // Voice Line: 1-100% battery charge percentage
        
        ThisThread::sleep_for(200);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(1112);                                                                                                                                                              // Voice Line: "The battery voltage is:"
        OutputFormatted(battery_voltage_circuit);                                                                                                                                     // formatted in thousands & remainder
        PlayVoice(1119);                                                                                                                                                              // Voice Line: "millivolts"

        ThisThread::sleep_for(200);                                                                                                                                                   // delay between voice lines for smoother output
        if (battery_fault_bq25120 == 0) PlayVoice(54);                                                                                                                                // Voice Line: "The battery is healthy and with no faults!"
        else PlayVoice(55);                                                                                                                                                           // Voice Line: "Attention! Faults regarding the battery have been detected!"
        
        break;
      }

//-----------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 1 >> Command: "Do you know anything about the clouds?"-------------------------------------------------------------------------------------------------------------
      case 1:
      {
        float cloud_base_altitude_groundlevel = (temperature_regressed_fusion - air_dew_point) * 124.6;                                                                               // https://holfuy.com/en/support/cloud-base-calculations (calibrated to my device and tested against calculators)
        float cloud_base_altitude_sealevel    = cloud_base_altitude_groundlevel + altitude;                                                                                           // cloud base altitude + user altitude = the altitude of the cloud base from the sea level
        float cloud_base_temperature          = - 0.00182 * cloud_base_altitude_groundlevel + air_dew_point;                                                                          // cloud base temperature formula for Celsius computation (for Fahrenheit there is a different formula)

        PlayVoice(34);                                                                                                                                                                // Voice Line: "The cloud base forms at a computed altitude of:"
        DistanceFormatted(cloud_base_altitude_groundlevel);                                                                                                                           // altitude output formatted as kilometers and meters
         
        ThisThread::sleep_for(200);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(35);                                                                                                                                                                // Voice Line: "Converted to the sea level, the cloud base altitude is:"
        DistanceFormatted(cloud_base_altitude_sealevel);                                                                                                                              // altitude output formatted as kilometers and meters

        ThisThread::sleep_for(200);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(36);                                                                                                                                                                // Voice Line: "Furthermore, the approximated cloud temperature is:"
        TemperatureFormatted(cloud_base_temperature);                                                                                                                                 // temperature formatted output                                        

        break;
      }

//-------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 2 >> Command: "AURI describe yourself!"--------------------------------------------------------------------------------------------------------------------     
      case 2:
      {
        PlayVoice(33);                                                                                                                                                                // Voice Line: "My name is AURI, I am the assistant that will always be here for you. (...) " 
        break;                                                                                                                                                                                     
      }

//-------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 3 >> Command: "What are the environment conditions?"-------------------------------------------------------------------------------------------------------------
      case 3:
      {
        //temperature output
        PlayVoice(1);                                                                                                                                                                 // Voice Line: "After analysing your surroundings, I can confirm the measured temperature is:"
        TemperatureFormatted(temperature_regressed_fusion);                                                                                                                           // air temperature formatted output    

        //feels like temperature
        ThisThread::sleep_for(200);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(2);                                                                                                                                                                 // Voice Line: "Furthermore, the calculated feels like temperature equals to:"
        TemperatureFormatted(heat_index);                                                                                                                                             // heat index temperature formatted output  

        // air dew point temperature output
        ThisThread::sleep_for(200);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(3);                                                                                                                                                                 // Voice Line: "Additionally, the dew point is:"
        TemperatureFormatted(air_dew_point);                                                                                                                                          // air dew point temperature formatted output  
      
        //environment conditions outputs in a single array to save code footprint through a for loop
        uint_least16_t environment_output[15] = {0, 53, int(round(temperature_fusion_fahrenheit) + 1147), 0, 4, int(round(humidity_BME688_regressed + 144)), 
                                           0, 5, int(round(relative_humidity + 144)), 0, 7, int(round(pressure_BMP390_regressed - 949 + 244)), 0, 11, int(round(equivalent_sea_pressure - 949 + 244))};                        
                                                                                                          
        for (uint_least8_t i = 0; i < 15; i++) {                                                                                                                                      // loop through all array elements
          if (environment_output[i] == 0) ThisThread::sleep_for(200);                                                                                                                 // if the array element in position "i" is a 0, perform a 200ms delay
          else PlayVoice(environment_output[i]);                                                                                                                                      // otherwise play the specific voice line
        }

        break;  
      }

//------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 4 >> Command: "Present the health report!"------------------------------------------------------------------------------------------------------------------
      case 4:
      {
        uint_least8_t discomfort_index = DiscomfortComputation(temperature_regressed_fusion, humidity_BME688_regressed);                                                              // discomfort index formula
        float         laser_skin_temp  = mlx.readObjectTempC() - 3.5;                                                                                                                 // sensor calibrated for skin temeperature detection through a tempered glass screen compared to the regular reading  

        // output heart rate
        HeartMonitor();

        // output skin temperature
        ThisThread::sleep_for(500);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(28);                                                                                                                                                                // Voice Line: "Moreover, the skin temperature is:"
        TemperatureFormatted(laser_skin_temp);                                                                                                                                        // skin temperature formatted output  
        
        // output heat index health effect
        ThisThread::sleep_for(500);                                                                                                                                                   // delay between voice lines for smoother output
        uint_least8_t heat_index_values[6] = {4, 26, 32, 39, 51, 150};                                                                                                                // array containing heat index conditions for the correct health effect output
        uint_least8_t heat_index_output[5] = {1, 2, 3, 4, 5};                                                                                                                         // voice lines for the specific health effect caused by heat index
       
        for (uint_least8_t n = 0; n < 5; n++) {                                                                                                                                       // loop through the first 5 elements of the values array, always comparing i and i+1
          if (temperature_regressed_fusion >= 4) {                                                                                                                                    // only output the heat index condition if it is higher than 4 degrees Celsius
            if (heat_index_values[n] < heat_index && heat_index <= heat_index_values[n+1]) {                                                                                          // compare condition from element i with element i+1 for the correct output
              PlayVoice(29);                                                                                                                                                          // Voice Line: "In terms of the effects of the environment on your health. The heat index stands at:"
              PlayVoice(int(round(heat_index)) + 1147);                                                                                                                               // Voice Line: heat index value (as number)
              PlayVoice(heat_index_output[n] + 375);                                                                                                                                  // output the specific heat index condition voice line
            }
          } else PlayVoice(1125);                                                                                                                                                     // Voice Line: "The Heat Index Condition cannot be calculated as the environment temperature is too low. Please consider the temperature as the absolute truth!"
        }

        // output discomfort index health effect
        ThisThread::sleep_for(500);                                                                                                                                                   // delay between voice lines for smoother output
        uint_least8_t discomfort_values[7] = {0, 21, 24, 27, 29, 32, 250};                                                                                                            // array containing discomfort index conditions for the correct health effect output                                                                                                            
        uint_least8_t discomfort_output[6] = {1, 2, 3, 4, 5, 6};                                                                                                                      // voice lines for the specific health effect caused by the discomfort 
        
        for (uint_least8_t l = 0; l < 6; l++) {                                                                                                                                       // loop through the first 6 elements of the values array, always comparing i and i+1
          if (discomfort_values[l] < discomfort_index && discomfort_index <= discomfort_values[l+1]) {                                                                                // compare condition from element i with element i+1 for the correct output
            PlayVoice(56);                                                                                                                                                            // Voice Line: "The discomfort index is currently at:" 
            PlayVoice(int(abs(discomfort_index)) + 1147);                                                                                                                             // output as a voice line the discomfort index number
            PlayVoice(discomfort_output[l] + 369);                                                                                                                                    // output the specific discomfort index condition voice line
          }
        }

        // output pressure health effect
        ThisThread::sleep_for(500);                                                                                                                                                   // delay between voice lines for smoother output
        if (pressure_BMP390_regressed > 1022.678) PlayVoice(369);                                                                                                                     // Voice Line: "Caution! High pressure which pushes against the body and limits how much the tissue can expand. Potential of joint paint!"
        else if (pressure_BMP390_regressed < 1007) PlayVoice(368);                                                                                                                    // Voice Line: "Caution! Low pressure allowing the body's tissues to expand, affecting the nerves, which can cause migraines!"
        else PlayVoice(367);                                                                                                                                                          // Voice Line: "Expected pressure range!"

        // output uv index health effect
        ThisThread::sleep_for(500);
        uint_least8_t uv_index_values[6] = {0, 3, 6, 8, 11, 20};                                                                                                                      // array containing uv index conditions for the correct health effect output 
        uint_least8_t uv_index_output[5] = {1, 2, 3, 4, 5};                                                                                                                           // voice lines for the specific health effect caused by the uv index       
        
        for (uint_least8_t m = 0; m < 5; m++) {                                                                                                                                       // loop through the first 5 elements of the values array, always comparing i and i+1
          if (uv_index_values[m] <= uv_index && uv_index < uv_index_values[m+1]) {                                                                                                    // compare condition from element i with element i+1 for the correct output                                                                                             
            PlayVoice(uv_index_output[m] + 1126);                                                                                                                                     // output the specific uv index condition voice line
          }
        }

        break;
      }
      
//------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 5 >> Command: "How about the air quality?"------------------------------------------------------------------------------------------------------------------
      case 5:
      {
        BHY2.update();                                                                                                                                                                // update Bosch Sensors
        uint_least8_t  bsec_accuracy  = bsec.accuracy();                                                                                                                              // BSEC sensor accuracy - informes the user how accurate the outputs are >> it improves the more it is used
        uint_least16_t air_quality    = bsec.iaq();                                                                                                                                   // BSEC Air Quality Index >> 0-500
        uint_least32_t carbon_dioxide = bsec.co2_eq();                                                                                                                                // BSEC Carbon Dioxide extimated value
        float          volatile_mix   = bsec.b_voc_eq();                                                                                                                              // BSEC Volatile Organic Compounds detected
        
        // output BSEC sensor accuracy before proceeding with the BSEC readouts and health effects
        PlayVoice(63);                                                                                                                                                                // Voice Line: "Checking the system status. The BSEC sensor accuracy is at the level:" 
        PlayVoice(bsec_accuracy + 1147);                                                                                                                                              // output the BSEC accuracy level

        // output iaq health effect
        ThisThread::sleep_for(300);                                                                                                                                                   // delay between voice lines for smoother output
        uint_least16_t iaq_values[8] = {0, 50, 100, 150, 200, 250, 350, 500};                                                                                                         // array containing air quality index conditions for the correct health effect output 
        uint_least8_t  iaq_output[7] = {1, 2, 3, 4, 5, 6, 7};                                                                                                                         // voice lines for the specific health effect caused by the discomfort
        
        for (uint_least8_t j = 0; j < 7; j++) {                                                                                                                                       // loop through the first 7 elements of the values array, always comparing i and i+1
          if (iaq_values[j] < air_quality && air_quality <= iaq_values[j+1]) {                                                                                                        // compare condition from element i with element i+1 for the correct output                                                                                             
            PlayVoice(30);                                                                                                                                                            // Voice Line: "The air quality of your environment resides at:"
            PlayVoice(air_quality + 1147);                                                                                                                                            // output air quality index as a number between 0 and 500
            PlayVoice(37);                                                                                                                                                            // Voice Line: "Which means the air is:"
            PlayVoice(iaq_output[j] + 359);                                                                                                                                           // output the specific air quality index condition voice line
          }
        }

        // output carbon dioxide health effect
        ThisThread::sleep_for(300);                                                                                                                                                   // delay between voice lines for smoother output
        uint_least16_t co2_values[10] = {0, 400, 600, 900, 1100, 1600, 2000, 5000, 40000, 65500};                                                                                     // array containing carbon dioxide conditions for the correct health effect output 
        uint_least8_t  co2_output[9]  = {1, 2, 3, 4, 5, 6, 7, 8, 9};                                                                                                                  // voice lines for the specific health effect caused by the carbon dioxide 
        
        for (uint_least8_t k = 0; k < 9; k++) {                                                                                                                                       // loop through the first 9 elements of the values array, always comparing i and i+1
          if (co2_values[k] < carbon_dioxide && carbon_dioxide <= co2_values[k+1]) {                                                                                                  // compare condition from element i with element i+1 for the correct output
            PlayVoice(31);                                                                                                                                                            // Voice Line: "In terms of Carbon Dioxide I have detected:"
            PlayVoice(co2_output[k] + 350);                                                                                                                                           // output the specific carbon dioxide condition voice line 
          }
        }

        // output volatile organic compounds health effect
        ThisThread::sleep_for(300);                                                                                                                                                   // delay between voice lines for smoother output
        uint_least8_t voc_values[6] = {0, 5, 10, 15, 50, 255};                                                                                                                        // array containing the VOC conditions for specific voice lines
        uint_least8_t voc_output[5] = {1, 2, 3, 4, 5};                                                                                                                                // array containing all output voice lines
        
        for (uint_least8_t h = 0; h < 5; h++) {                                                                                                                                       // for loop to simplify a multitude of if/else if
          if (voc_values[h] < volatile_mix && volatile_mix <= voc_values[h+1]) {                                                                                                      // if statement to check the corrent output based on the voc from the sensor
            PlayVoice(32);                                                                                                                                                            // Voice Line: "In addition, I have also detected"
            if (volatile_mix <= 50) PlayVoice(int(round(volatile_mix)) + 1007);                                                                                                       // Voice Line: between 0-50 "parts per million volatile organic compounds"
            else PlayVoice(1058);                                                                                                                                                     // Voice Line: "Over 50 volatile organic compounds"
            PlayVoice(voc_output[h] + 345);                                                                                                                                           // VOC condition output between low and extreme
          } 
        }

        break;
      }

//------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 6 >> Command: "Details about my location."------------------------------------------------------------------------------------------------------------------
      case 6:
      {
        BHY2.update();                                                                                                                                                                // update Bosch sensor values

        // altitude output in kilometers and meters
        ThisThread::sleep_for(300);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(12);                                                                                                                                                                // Voice Line: "Based on your location, the estimated altitude is:"
        DistanceFormatted(altitude);                                                                                                                                                  // altitude output formatted as kilometers and meters
        
        //compass 16-point heading output
        ThisThread::sleep_for(300);
        float         compass_points[18] = {0, 11.25, 33.75, 56.25, 78.75, 101.25, 123.75, 146.25, 168.75, 191.25, 213.75, 236.25, 258.75, 281.25, 303.75, 326.25, 348.75, 360};      // array containing all conditions for the different 16-point headings                                                                                                                                                       
        uint_least8_t compass_output[17] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};                                                                               // compass heading outputs >> represents 1 of the 16 points on the compass, the 17th element is North repeated for 349.75 to 360 deg
        
        for (uint_least8_t m = 0; m < 17; m++) {
          if (compass_points[m] <= orientation.heading() && orientation.heading() <= compass_points[m+1]) {                                                                           // if the heading angle is between the element i and i+1 then continue
            PlayVoice(13);                                                                                                                                                            // Voice Line: "Your magnetic heading at the time of computation is towards:" 
            PlayVoice(758 + compass_output[m]);                                                                                                                                       // Voice Line: one of the 16 points on the compass (example: south-south-west
          }
        }

        // compass heading angle in degrees
        ThisThread::sleep_for(300);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(14);                                                                                                                                                                // Voice Line: "Based on the angle of:"
        DegreesFormatted(orientation.heading());                                                                                                                                      // heading degrees formatted in function of a positive or negative value, with added "degrees" voice line

        // compass roll angle in degrees
        ThisThread::sleep_for(300);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(15);                                                                                                                                                                // Voice Line: "With a compass roll of:"
        DegreesFormatted(orientation.pitch());                                                                                                                                        // roll degrees formatted in function of a positive or negative value, with added "degrees" voice line

        // compass pitch angle in degrees
        ThisThread::sleep_for(300);                                                                                                                                                   // delay between voice lines for smoother output
        PlayVoice(16);                                                                                                                                                                // Voice Line: "And a compass pitch of:"
        DegreesFormatted(orientation.roll());                                                                                                                                         // pitch degrees formatted in function of a positive or negative value, with added "degrees" voice line
        
        break;
      }
      
//--------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 7 >> Command: "Identify the atmosphere physics!"----------------------------------------------------------------------------------------------------------------
      case 7:
      {
        uint_least16_t physics_output[18] = {8, int(round(saturation_vapor_pressure + 1147)), 0, 9, int(round(water_vapor_pressure + 1147)), 0, 10, int(round(dry_air_pressure - 949 + 244)), 
                                       0, 1132, int((air_density * 100) + 2146), 1121, 0, 1113, int(round(visible_light) + 1147), 0, 1114, int(round(infrared_light) + 1147)};
                                                                             
        for (uint_least8_t i = 0; i < 18; i++) {                                                                                                                                      // loop through all array elements
          if (physics_output[i] == 0) ThisThread::sleep_for(200);                                                                                                                     // if the array element in position "i" is a 0, perform a 200ms delay
          else PlayVoice(physics_output[i]);                                                                                                                                          // otherwise play the specific voice line
        }

        //gas resistance output
        ThisThread::sleep_for(300);                                                                                                                                                   // delay between voice lines for smoother output
        if (gas_resistance_BME688 < 100) PlayVoice(49);                                                                                                                               // Voice Line: "The sensor needs to calibrate to the current environment before outputting an accurate gas resistance reading!"
        else if (gas_resistance_BME688 < 1000) {
          PlayVoice(50);                                                                                                                                                              // Voice Line: "The Gas Resistance subsists at a level of"                                                                                                                                                              // Voice Line: "The Gas Resistance subsists at a level of"
          PlayVoice(gas_resistance_BME688 + 1147);                                                                                                                                    // Voice Line: number between 0 to 999 representing the gas resistance in ohms
          PlayVoice(52);                                                                                                                                                              // Voice Line: "ohms"                              
        } else {
          uint_least8_t  gas_kilo_ohms = gas_resistance_BME688 / 1000;                                                                                                                // get the kiloohms part of the gas resistance
          uint_least16_t gas_ohms      = gas_resistance_BME688 % 1000;                                                                                                                // get the ohms part of the gas resistance
        
          PlayVoice(50);                                                                                                                                                              // Voice Line: "The Gas Resistance subsists at a level of"
          PlayVoice(gas_kilo_ohms + 1147);                                                                                                                                            // Voice Line: number between 0 to 999 representing the gas kiloohms part
          PlayVoice(51);                                                                                                                                                              // Voice Line: "kilo-ohms"
      
          if (gas_ohms != 0) {
            ThisThread::sleep_for(100);                                                                                                                                               // delay between voice lines for smoother output
            PlayVoice(23);                                                                                                                                                            // Voice Line: "and"
            PlayVoice(gas_ohms + 1147);                                                                                                                                               // Voice Line: number between 1 and 999
            if (gas_ohms == 1) PlayVoice(1120);                                                                                                                                       // Voice Line: "ohm"  
            else PlayVoice(52);                                                                                                                                                       // Voice Line: "ohms"
          }
        }
        
        break;
      }
      
//----------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 8 >> Command: "Count the number of steps done!"---------------------------------------------------------------------------------------------------------------
      case 8:
      {
        //steps counting
        BHY2.update();                                                                                                                                                                // Update all Bosch Sensors
        unsigned long  total_steps        = steps.value();                                                                                                                            // compute the total steps since the device has been turned on (if the battery dies, the total steps gets reset to 0)
        unsigned long  current_steps      = total_steps - last_steps;                                                                                                                 // compute the steps since the last time the command has been activated
        uint_least16_t kilocalories       = current_steps * 0.03344;                                                                                                                  // conversion of current steps to kilocalories                                                  
        uint_least16_t kilojouls          = kilocalories * 4.184;                                                                                                                     // conversion of kilocalories to kilojouls (energy)
        uint_least32_t distance_travelled = (current_steps / 1179) * 1000;                                                                                                            // convert from steps to kilometers, and then from kilometers to meters

        //total steps
        PlayVoice(17);                                                                                                                                                                // Voice Line: "The total aggregate of steps adds up to:"
        OutputFormatted(total_steps);

        //current steps
        PlayVoice(18);                                                                                                                                                                // Voice Line: "Since the last command execution, the number of steps performed equals to:"
        OutputFormatted(current_steps);

        //kilocalories output
        PlayVoice(19);                                                                                                                                                                // Voice Line: "Considering an average speed of 4.8 kilometres per hour, with the world body average weight of 62 kilograms, 
        OutputFormatted(kilocalories);                                                                                                                                                //              and height of 171 centimetres, you have burnt circa:"
        PlayVoice(47);                                                                                                                                                                // Voice Line: "kilocalories"  

        //kilojouls (energy) output
        PlayVoice(20);                                                                                                                                                                // Voice Line: "Converting the kilocalories burnt to energy, it results in:"
        OutputFormatted(kilojouls);
        PlayVoice(48);                                                                                                                                                                // Voice Line: "kilojouls"

        //output of the distance travelled once converted from steps
        PlayVoice(21);                                                                                                                                                                // Voice Line: "In addition, by using the averages aforementioned, you have travelled a total distance of:"
        DistanceFormatted(distance_travelled);
        
        break;
      }

//---------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 9 >> Command: "Thanks for the info provided!"------------------------------------------------------------------------------------------------------------------
      case 9:
      {
        uint_least8_t random_thanks = random(1, 7);                                                                                                                                   // returns a random number between min(1) and max-1(10) >> equivalent to the 6 thanks voice lines available
        PlayVoice(random_thanks + 752);                                                                                                                                               // Voice Line: randomly chosen "you are welcome" voice line       
        break;
      }

//-------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 10 >> Command: "Tell me the current time and date."---------------------------------------------------------------------------------------------------------------
      case 10:
      {
        if (timeStatus() != timeNotSet) {
          //output time
          PlayVoice(22);                                                                                                                                                              // Voice Line: "The current time is:"
          PlayVoice(int(hour()) + 1141);                                                                                                                                              // play the voice line between 0 and 23 representing the hour
          PlayVoice(int(minute()) + 1141);                                                                                                                                            // play a number between 0 and 59 representing the minutes
          if (int(minute()) == 0) PlayVoice(int(minute()) + 1141);                                                                                                                    // if the minute is equal to 00, then play 0 twice (example: thirteen zero zero >> 13:00)
          
          //output date
          delay(200);                                                                                                                                                                 // delay between voice lines for smoother output
          PlayVoice(24);                                                                                                                                                              // Voice Line: "Meanwhile, today's date is:"
          PlayVoice(int(day()) + 1058);                                                                                                                                               // play the voice line representing the day (example: "the fifth")
          PlayVoice(int(month()) + 1089);                                                                                                                                             // play the voice line representing the month (example: "of February")

          uint_least8_t year_first_half = year() / 100;                                                                                                                               // find the first part of the year number (example 2022 = 20)
          uint_least8_t year_second_half = year() % (year_first_half * 100);                                                                                                          // find the second part of the year number (example 2022 = 22)

          PlayVoice(year_first_half + 1141);                                                                                                                                          // Play the number 0 - 99 for the first part of the year
          if (year_second_half == 0) PlayVoice(1124);                                                                                                                                 // Voice Line: "hundred"
          else PlayVoice(year_second_half + 1141);                                                                                                                                    // Play the number 0 - 99 for the second part of the year
        } else PlayVoice(62);  
       
        break;
      }

//------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 11 >> Command: "Reduce the volume by 10!"---------------------------------------------------------------------------------------------------------------------
      case 11:
      {
        if (volume_level > 5) {
          volume_level = volume_level - 10;                                                                                                                                           // reduce the volume variable by 10
          DFPlayer.volume(volume_level);                                                                                                                                              // set DFPlayer Mini volume to the specific variable value 
          ThisThread::sleep_for(200);
          PlayVoice(61);                                                                                                                                                              // Voice Line: "The volume has been changed to:" 
          PlayVoice(volume_level + 1147);                                                                                                                                             // play the voice line between 0 and 30 representing the volume level
        } else if (volume_level <= 5) PlayVoice(1133);                                                                                                                                // Voice Line: "Volume is at the highest level of 30! It cannot be increased further!"
        break;
      }

//-----------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 12 >> Command: "Increase the volume by 5!"--------------------------------------------------------------------------------------------------------------------

      case 12:
      {
        if (volume_level <= 25) {
          volume_level = volume_level + 5;                                                                                                                                            // increase the volume variable by 5
          DFPlayer.volume(volume_level);                                                                                                                                              // set DFPlayer Mini volume to the specific variable value    
          ThisThread::sleep_for(200);
          PlayVoice(61);                                                                                                                                                              // Voice Line: "The volume has been changed to:" 
          PlayVoice(volume_level + 1147);                                                                                                                                             // play the voice line between 0 and 30 representing the volume level
        } else if (volume_level == 30) PlayVoice(1134);                                                                                                                               // Voice Line: "Volume is at the lowest level of 5! It cannot be decreased further!"
        break;
      }

//----------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 13 >> Command: "How is it going to be today?"------------------------------------------------------------------------------------------------------------------
      case 13: 
      {
        uint_least8_t allLabels[4], bestLabels[2] = {5, 5}, primes[5] = {2, 3, 5, 7, 11};                                                                                             // declare and initialise arrays for computations
        uint_least16_t product = 1;
        for (uint_least8_t l = 0; l < 4; l++) {
          allLabels[l] = dataReceived[l];                                                                                                                                             // pass the value of the labels from the main I2C array to the ML array
          product *= primes[allLabels[l]];                                                                                                                                            // create the product of 4 primes >> depending on the 4 labels received
        }
        for (uint_least8_t k = 0; k < 4; k++) if (!fmod(product, sq(primes[allLabels[k]]))) bestLabels[k / 2] = allLabels[k];                                                         // divide the product of the primes (from the 4 labels) with all 5 labels to find which value repeats

        // machine learning weather classification output
        if (bestLabels[0] == 5) PlayVoice(allLabels[1] + 742);                                                                                                                        // every label repeats once. XGBoost has been selected for ML Classification as the most robust model
        else {  
          PlayVoice(bestLabels[0] + 742);                                                                                                                                             // the most common classification out of all 4 models
          ThisThread::sleep_for(200);                                                                                                                                                 // delay between voice lines for smoother output
          if (bestLabels[0] != 5 && bestLabels[0] != bestLabels[1]) PlayVoice(bestLabels[1] + 747);                                                                                   // if 2 outputs repeat twice with all ML models, then also output the chance of the 2nd classification
        }

        // machine learning rainfall regression output
        ThisThread::sleep_for(200);                                                                                                                                                   // delay between voice lines for smoother output
        if (int(dataReceived[4] * 100) == 0) PlayVoice(1115);                                                                                                                         // Voice Line: "In view of the weather classification, I can also forecast that there will be no rainfall"
        else if (int(dataReceived[4]) > 35) {                                                                                                                                         // if the predicted rainfall is higher than 35 millimeters (last voice line in numbers)
            PlayVoice(752);                                                                                                                                                           // Voice Line: "In view of the weather classification, I can forecast the precipitation amount to:"
            PlayVoice(1123);                                                                                                                                                          // Voice Line: "A computed value higher than 35"
        } else {
            PlayVoice(752);                                                                                                                                                           // Voice Line: "In view of the weather classification, I can forecast the precipitation amount to:"
            PlayVoice(int(dataReceived[4] * 100) + 2146);                                                                                                                             // Voice Line: number between 0.01 and 35.00
            PlayVoice(1118);                                                                                                                                                          // Voice Line: "millimeters"
        }
        
        break;
      }

//------------------------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 14----------------------------------------------------------------------------------------------------------------------------------------  
      case 14:
      {
        PlayVoice(57);                                                                                                                                                                // Voice Line: "You spoke too quietly, or there was too much noise in your surroundings. PLEASE REPEAT THE COMMAND!"
        break;
      }
      
//------------------------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 15----------------------------------------------------------------------------------------------------------------------------------------    
      case 15:
      {
        PlayVoice(58);                                                                                                                                                                // Voice Line: "Software issue detected. Failed to setup audio sampling!"
        break;
      }
//------------------------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 16---------------------------------------------------------------------------------------------------------------------------------------- 
      case 16:
      {
        PlayVoice(59);                                                                                                                                                                // Voice Line: "Software or Hardware issue detected. Failed to record audio!"
        break;
      }
      
//------------------------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 17---------------------------------------------------------------------------------------------------------------------------------------- 
      case 17: 
      {      
        PlayVoice(60);                                                                                                                                                                // Voice Line: "Deep Learning issue detected. Failed to run the classifier!"
        break;
      }

//------------------------------------------------------------------------------------------------------------------VOICE INTERFERENCE SYSTEM - LABEL RECEIVED: 18 >> Command: "Relevant location vectors!"----------------------------------------------------------------------------------------------------------------- 
      case 18:
      {
        BHY2.update();                                                                                                                                                                // update Bosch Sensors

        // gravity vector output                                                                                                                                                      
        PlayVoice(1136);                                                                                                                                                              // Voice Line: "The gravity vector quaternion values are as follows"
        PlayVoice(1138);                                                                                                                                                              // Voice Line: "X"
        CoordinateFormatted(gravityvector.x());                                                                                                                                       // compute the value accordingly for voice output

        ThisThread::sleep_for(200);                                                                                                                                                   // delay for a pause in the outputs to not have voice lines play one after the other
        PlayVoice(1139);                                                                                                                                                              // Voice Line: "Y"
        CoordinateFormatted(gravityvector.y());                                                                                                                                       // compute the value accordingly for voice output

        ThisThread::sleep_for(200);                                                                                                                                                   // delay for a pause in the outputs to not have voice lines play one after the other
        PlayVoice(1140);                                                                                                                                                              // Voice Line: "Z"
        CoordinateFormatted(gravityvector.z());                                                                                                                                       // compute the value accordingly for voice output

        // geomagnetic rotation vector output
        ThisThread::sleep_for(300);                                                                                                                                                   // delay for a pause in the outputs to not have voice lines play one after the other
        PlayVoice(1137);                                                                                                                                                              // Voice Line: "Meanwhile, the geomagnetic rotation vector has the following coordinates"
        PlayVoice(1138);                                                                                                                                                              // Voice Line: "X"
        CoordinateFormatted(geomagneticrotationvector.x());                                                                                                                           // compute the value accordingly for voice output

        ThisThread::sleep_for(200);                                                                                                                                                   // delay for a pause in the outputs to not have voice lines play one after the other
        PlayVoice(1139);                                                                                                                                                              // Voice Line: "Y"
        CoordinateFormatted(geomagneticrotationvector.x());                                                                                                                           // compute the value accordingly for voice output

        ThisThread::sleep_for(200);                                                                                                                                                   // delay for a pause in the outputs to not have voice lines play one after the other
        PlayVoice(1140);                                                                                                                                                              // Voice Line: "Z"
        CoordinateFormatted(geomagneticrotationvector.x());                                                                                                                           // compute the value accordingly for voice output

        ThisThread::sleep_for(200);                                                                                                                                                   // delay for a pause in the outputs to not have voice lines play one after the other
        PlayVoice(1141);                                                                                                                                                              // Voice Line: "W"
        CoordinateFormatted(geomagneticrotationvector.x());                                                                                                                           // compute the value accordingly for voice output
        
      }
    }
  }
  if (!low_battery_mode) ThisThread::sleep_for(1000);                                                                                                                                 // if the low power mode flag is false, then continue updating the sensors every 1 second
  else ThisThread::sleep_for(300000);                                                                                                                                                 // else if the flag is high, then update the main loop and sensors every 5 minutes
}



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------SUBROUTINES---------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------DISCOMFORT COMPUTATION---------------------------------------------------------------------------------------------------------------------------------------------------
uint_least8_t DiscomfortComputation(float temperature, uint_least8_t humidity) {
  uint_least8_t index = temperature - 0.55 * (1 - 0.01 * humidity) * (temperature - 14.5);                                                                                            // discomfort index formula
  return index;                                                                                                                                                                       // return discomfort index value as 8 bit integer
}

//----------------------------------------------------------------------------------------------------------------------------------------------------SPEECH FUNCTION-------------------------------------------------------------------------------------------------------------------------------------------------------
void PlayVoice(int file) {
  DFPlayer.play(file);                                                                                                                                                                // play the wav file specified with the play function of the DFPlayer object
  ThisThread::sleep_for(500);                                                                                                                                                         // delay 500ms necessary to compute the command >> any delay higher than 150ms works 
  while (!digitalRead(busy_pin));                                                                                                                                                     // block the code for the duration of the file playing >> busy pin will be LOW
} 

//----------------------------------------------------------------------------------------------------------------------------------------------TEMPERATURE OUTPUT FORMATTED------------------------------------------------------------------------------------------------------------------------------------------------
void TemperatureFormatted(float temperature) {
  if (temperature < 0) PlayVoice(43);                                                                                                                                                 // if the given temperature is negative, then play the "negative" voice line
  
  temperature = RoundToHalfInteger(temperature);                                                                                                                                      // round temperature to .5 or .0
  if (int(temperature * 10) % 10 == 0) PlayVoice(int(temperature) + 64);                                                                                                              // if it has been rounded up to 0, play voice lines for the temperature from a specific first set based on the rule
  else if (int(temperature * 10) % 10 == 5) PlayVoice(int(temperature + 0.5) + 926);                                                                                                  // if it has been rounded up to 5, play voice lines for the temperature from the second set based on the rule
}

//-------------------------------------------------------------------------------------------------------------------------------------------DISTANCE/ALTITUDE OUTPUT FORMATTED---------------------------------------------------------------------------------------------------------------------------------------------
void DistanceFormatted(int32_t distance_value) {
  if (distance_value < 0) {                                                                                                                                                           // if the distance is negative then proceed
      PlayVoice(43);                                                                                                                                                                  // Voice Line: "negative"
      PlayVoice(abs(distance_value) + 1147);                                                                                                                                          // Voice Line: Absolute value of the distance as a number between 0 and 999
      MetersFormatted(abs(distance_value));                                                                                                                                           // Formatting Meters or Meter output depending on the value
  } else if (distance_value >= 0 && distance_value < 1000) {
      PlayVoice(distance_value + 1147);                                                                                                                                               // Voice Line: number between 0 and 999
      MetersFormatted(distance_value);                                                                                                                                                // Formatting Meters or Meter output depending on the value
  } else {
      uint_least8_t  distance_value_kilometers = distance_value / 1000;                                                                                                               // find from the given value the number of kilometers
      uint_least16_t distance_value_meters     = distance_value % 1000;                                                                                                               // find from the given value the number of meters
  
      PlayVoice(distance_value_kilometers + 1147);                                                                                                                                    // Voice Line: number between 0 and 999 
      if (distance_value_kilometers == 1) PlayVoice(45);                                                                                                                              // if the distance_travelled is 1 kilometer + x meters, then say "kilometer" voice line, not "kilometers"
      else PlayVoice(46);                                                                                                                                                             // the distance_travelled is higher than 1 kilometer, so say "kilometers"

      if (distance_value_meters != 0) {
        ThisThread::sleep_for(100);                                                                                                                                                   // helps with voice smoothness
        PlayVoice(23);                                                                                                                                                                // Voice Line: "and"
        PlayVoice(int(distance_value_meters) + 1147);                                                                                                                                 // Voice Line: number between 0 and 999
        MetersFormatted(distance_value_meters);                                                                                                                                       // Formatting Meters or Meter output depending on the value
      }
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------METERS OUTPUT FORMATTED--------------------------------------------------------------------------------------------------------------------------------------------------
void MetersFormatted(uint_least16_t meters) {
  if (meters == 1) PlayVoice(1117);                                                                                                                                                   // if the given value is equal to 1 then output "meter"          
  else PlayVoice(44);                                                                                                                                                                 // Voice Line: "meters"                       
}

//------------------------------------------------------------------------------------------------------------------------------TOTAL/CURRENT STEPS & KILOCALORIES/KILOJOULS OUTPUT FORMATTED-------------------------------------------------------------------------------------------------------------------------------
void OutputFormatted(unsigned long output) { 
  if (output < 1000) {                                                                                                                                                                // if the output is lower than 1 thousand, then use the voice lines available for the number speech
      PlayVoice(int(output) + 1147);                                                                                                                                                  // Voice Line: number between 0 and 999
  } else if (output >= 1000) {                                                                                                                                                        // if the output is higher or equal to 1000, then convert the output to thousand + remainder
      uint_least8_t  output_thousands = output / 1000;                                                                                                                                // divide the value by 1000 to get the number of thousands
      uint_least16_t output_remainder = output % 1000;                                                                                                                                // use Arduino Modulo function to get the remaining amount
  
      PlayVoice(output_thousands + 1147);                                                                                                                                             // Voice Line: number between 0 and 999
      PlayVoice(1116);                                                                                                                                                                // Voice Line: "thousand"
      
      if (output_remainder != 0) {
        ThisThread::sleep_for(100);                                                                                                                                                   // helps with voice smoothness
        PlayVoice(23);                                                                                                                                                                // Voice Line: "and"
        PlayVoice(int(output_remainder) + 1147);                                                                                                                                      // Voice Line: number between 0 and 999
      }                                              
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------COMPASS DEGREES OUTPUT FORMATTED---------------------------------------------------------------------------------------------------------------------------------------------
void DegreesFormatted(float deg_value) {
  if (deg_value < 0) PlayVoice(43);                                                                                                                                                   // if the given degrees value is negative, then play the "negative" voice line
  PlayVoice(int(round(abs(deg_value)) + 381));                                                                                                                                        // Voice Line: between 0 to 360 degrees 
}

//-----------------------------------------------------------------------------------------------------------------------------------------------COORDINATES OUTPUT FORMATTED-----------------------------------------------------------------------------------------------------------------------------------------------
void CoordinateFormatted(float coord_given) {
  if (coord_given < 0) PlayVoice(43);                                                                                                                                                 // Voice Line: "negative"                                                                                                                                      
  
  uint_least16_t absolute_coord_value = abs(coord_given);                                                                                                                             // absolute of the given coordiante value                                                                                                                             
  if (absolute_coord_value == 0) PlayVoice(1147);                                                                                                                                     // if the value is equal to 0, play the 0 voice line
  else if (absolute_coord_value > 0 && absolute_coord_value <= 35) {                                                                                                                  // if the value is between 0 and 35 then use the 0.01-35.00 voice lines to output value
      uint_least16_t processed_coord = int(round((absolute_coord_value * 1000) / 10));                                                                                                // rule to use the 0.01 - 35.00 voice lines
      PlayVoice(processed_coord + 2146);                                                                                                                                              // output number
  } else OutputFormatted(absolute_coord_value);                                                                                                                                       // if the value is higher than 35, then use the OutputFormatted function which can output hundreds and thousands
  
}

//-----------------------------------------------------------------------------------------------------------------------------------------MAIN SENSORS THRESHOLDING/EWMA FILTERING-----------------------------------------------------------------------------------------------------------------------------------------
void UpdateMainSensors(void) {
  float temperature_BMP390_readings = 0, temperature_BME688_readings = 0, pressure_BMP390_readings = 0;                                                                               // declare and initialize variable for the sum of 10 readings
  uint_least16_t humidity_BME688_readings = 0;                                                                                                                                        // 16 bits/2 bytes are required to hold 10 values that can reach 10000

  for (uint_least8_t i = 0; i < 10; i++) {                                                                                                                                            // 10 total values updated with a loop
    BHY2.update();                                                                                                                                                                    // necessary to update sensor classes
    temperature_BMP390_readings += temperature.value();                                                                                                                               // sum all the values from the array to create the temperature threshold value
    temperature_BME688_readings += bsec.comp_t();                                                                                                                                     // sum all the values from the array to create the temperature 2 threshold value
    pressure_BMP390_readings    += pressure.value();                                                                                                                                  // sum all the values from the array to create the pressure threshold value
    humidity_BME688_readings    += bsec.comp_h();                                                                                                                                     // sum all the values from the array to create the humidity threshold value
    ThisThread::sleep_for(50);                                                                                                                                                        // get a new value every 50ms => 500ms + 12ms total runtime of loop
  }

  float temp_BMP390_outlier_higher_bound = 1.20 * (temperature_BMP390_readings / 10);                                                                                                 // 120% of the average of the last 10 values
  float temp_BMP390_outlier_lower_bound  = 0.80 * (temperature_BMP390_readings / 10);                                                                                                 // 80% of the average of the last 10 values

  float temp_BME688_outlier_higher_bound = 1.20 * (temperature_BME688_readings / 10);                                                                                                 // 120% of the average of the last 10 values
  float temp_BME688_outlier_lower_bound  = 0.80 * (temperature_BME688_readings / 10);                                                                                                 // 80% of the average of the last 10 values
  
  float pres_BMP390_outlier_higher_bound = 1.20 * (pressure_BMP390_readings / 10);                                                                                                    // 120% of the average of the last 10 values
  float pres_BMP390_outlier_lower_bound  = 0.80 * (pressure_BMP390_readings / 10);                                                                                                    // 80% of the average of the last 10 values
  
  uint_least8_t hum_BME688_outlier_higher_bound = 1.20 * (humidity_BME688_readings / 10);                                                                                             // 120% of the average of the last 10 values
  uint_least8_t hum_BME688_outlier_lower_bound  = 0.80 * (humidity_BME688_readings / 10);                                                                                             // 80% of the average of the last 10 values

  for (uint_least8_t t_BMP390, t_BMP688, p_BMP390, h_BME688; t_BMP390 < 10 && t_BMP688 < 10 && p_BMP390 < 10 && h_BME688 < 10;) {                                                     // for loop to get all values updated 10 times through EWMA
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
    ThisThread::sleep_for(100);
 }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------HEART MONITOR OUTPUT--------------------------------------------------------------------------------------------------------------------------------------------------
void HeartMonitor(void) {
  PlayVoice(25);                                                                                                                                                                      // Voice Line: "Please stand down and relax your arm during the heart rate measurement."

  BHY2.update();                                                                                                                                                                      // update Bosch sensors
  String motion_event = motion_detection.toString();                                                                                                                                  // save the string
  
  if (motion_event.equals("Event detected\n")) PlayVoice(26);                                                                                                                         // if motion then play the Voice Line: "My sensors are telling me that you are active, please stand still for an accurate reading!"
  else {                                                                                                                                                                              // else perform the heart rate processing
    uint_least16_t heart_value = 0, last_beat = 0;
    
    for (uint_least8_t i = 0; i < 8;) {                                                                                                                                               // start a for loop with 8 iterations
      heart_value = analogRead(heartrate_pin);                                                                                                                                        // read the analog value
      if (heart_value > last_beat) {                                                                                                                                                  // if the new value is higher than the last then:
        last_beat = heart_value;                                                                                                                                                      // store it for the use in the next iteration
        i++;                                                                                                                                                                          // increase the i by 1 >> perform this until it reaches 7 and the loop breaks
      }                                                                                                                                                                               // explanation: if the value has increase 8 times it a row, then the readings can be considered as heart rate values
    }
  
    ThisThread::sleep_for(300);                                                                                                                                                       // the voice lines play immediately one after the other, the delay adds a pause
    uint_least8_t bpm_filtered = 0;                                                                                                                                                   // set the filtered variable as 0
    for (uint_least8_t i = 0; i < 10; i++) {                                                                                                                                          // perform 10 iterations
      heart_value = analogRead(heartrate_pin);                                                                                                                                        // read the analog value of the heart rate sensor SEN0203
      bpm_filtered = (heart_rate.filter(heart_value))/10 + 15;                                                                                                                        // pass it to the exponentially weighted moving average, but divide by 10 and add 15 which represents the formula for output
    }
 
    if (bpm_filtered >= 50 && bpm_filtered < 120) {                                                                                                                                   // if the filtered BMP value is between those 2 values then proceed
      PlayVoice(27);                                                                                                                                                                  // Voice Line: "I can report the heart rate is at:"
      PlayVoice(bpm_filtered - 50 + 776);                                                                                                                                             // play voice for the heart rate value
    } else if (bpm_filtered >= 120) PlayVoice(5647);                                                                                                                                  // Voice Line: "The heart rate is over 120 BMP!"
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------VARIABLE ROUNDING TO HALF INTEGER-------------------------------------------------------------------------------------------------------------------------------------------
float RoundToHalfInteger(float x) {
  return 0.5 * round(2.0 * x) ;                                                                                                                                                       // equation to round any value to .0 or .5 >> used for temperature throghout the code
}

//---------------------------------------------------------------------------------------------------------------------------------------------------TIMEDATE SYNC PROCESSING-----------------------------------------------------------------------------------------------------------------------------------------------
void ProcessSyncMessage() {
  unsigned long pctime;                                                                                                                                                               // variable to store the correct timedate received from the PC Serial
  const unsigned long DEFAULT_TIME = 1357041600;                                                                                                                                      // Jan 1 2013

  pctime = Serial.parseInt();
  if ( pctime >= DEFAULT_TIME) setTime(pctime);                                                                                                                                       // check the integer is a valid time (greater than Jan 1 2013) and set the time
}

//------------------------------------------------------------------------------------------------------------------------------------------------REQUEST TIMEDATE SYNC FUNCTION------------------------------------------------------------------------------------------------------------------------------------------------
time_t RequestSync() {
  Serial.write(TIME_REQUEST);                                                                                                                                                         // serial write the sync request header
  return 0;                                                                                                                                                                           // the time will be sent later in response to serial mesg
}
