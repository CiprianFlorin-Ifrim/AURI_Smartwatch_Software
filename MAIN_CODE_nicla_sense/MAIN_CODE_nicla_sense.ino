//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------LIBRARIES-------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------DECLARATIONS------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
                   
float temperature_BMP390_filtered, temperature_BME688_filtered, pressure_BMP390_filtered, humidity_BME688_filtered;                                                                   // define global float variables for the filtered sensor outputs
bool charging_flag = false;                                                                                                                                                           // charging flag used to output the voice command of charging only once



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------SYSTEM SETUP------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

  Serial1.begin(9600);                                                                                                                                                                // start Hardware UART at 9600 bits per second baud rate
  DFPlayer.begin(Serial1, false);                                                                                                                                                     // start the DFPlayer process with Serial 1 and disable debug mode
  DFPlayer.volume(25);                                                                                                                                                                // set default DFPlayer Mini volume as 25 >> maximum is 30

  battery.onDemand(8, HIGH);                                                                     
  battery.begin(3300, 2.14, &asigmoidal);                                                                                                                                             // start battery system set with a LiPo battery (asigmoidal draining) on a 3.3v MCU with 2.14 voltage ratio 
                                                                                                                                                                                      // the real voltage divider ratio is 1.3, the set ratio is 2.14 to fix the 10bit ADC library calculations >> Nicla has a 12 bit ADC
  mlx.begin();                                                                                                                                                                        // begin I2C communication with the Laser Temperature >> Adafruit Sensor MLX90614
  uv.begin();                                                                                                                                                                         // begin I2C communication with the UV Sensor >> Adafruit SI1145

  setSyncProvider(RequestSync);                                                                                                                                                       // set function to call when time sync is required

  pinMode(busy_pin, INPUT);                                                                                                                                                           // set busy pin (GPIO3/P0_10) as INPUT
  digitalWrite(busy_pin, HIGH);                                                                                                                                                       // set DFPlayer Busy Pin as HIGH >> when the device is playing it outputs LOw, defaults to HIGH

  delay(5000);                                                                                                                                                                        // delay for the sensors to engage
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------MAIN CODE-----------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {

 /*
  if (Serial.available() > 1) {                               // wait for at least two characters
    char c = Serial.read();
    if (c == TIME_HEADER) ProcessSyncMessage(); 
  }


  byte battery_status = bq25120_getStatus();
  if (battery_status == 195 && charging_flag) {
    Serial.println("The battery is charging");
    charging_flag = false;
  } else if (battery_status != 195 && !charging_flag) charging_flag = true;
*/
  int activation = uv.readIR();
  if (activation >= 300) {                                                                                                                                                            // if the gesture event was recognised, then start the computation and inform the user
    //initialise system >> inform user
    nicla::leds.setColor(red);                                                                                                                                                        // set the led colour of the Nicla Sense ME to RED as a visual cue to the user that the system has been activated
    uint8_t random_listening = random(1, 11);                                                                                                                                         // returns a random number between min(1) and max-1(10) >> equivalent to the 10 listening voice lines available
    DFPlayer.play(random_listening + 1101);                                                                                                                                           // play audio to inform user that the system is listening >> instead of the voice function that waits for the file to end first
                                                                                                                                                                                      // use the library play function to have the audio in parallel with the sensor computation

    unsigned long last_steps = steps.value();

    BHY2.update();            //update sensor values
    UpdateMainSensors();

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
    uint16_t    altitude              = ((pow((sea_pressure_constant / pressure_BMP390_regressed), (1 / 5.257)) - 1) * (temperature_regressed_fusion + 273.15)) / 0.0065;             // calculate the altitude by using the hypsometric formula

    //the ratio of the actual amount of water vapor in the air to the amount it could hold when saturated
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

    //heat index decision tree
    float heat_index = 0;                                                                                                                                                             // initialise heat index variable
    if (heat_index_Steadman >= 80) {                                                                                                                                                  // if this heat index value is 80 degrees F or higher, the full regression equation of Rothfusz (along with any adjustment) is applied
      if (relative_humidity < 13 && temperature_fusion_fahrenheit > 80 && temperature_fusion_fahrenheit < 112) {
          heat_index = ((heat_index_Rothfusz - heat_index_lowRH_adjustment) - 32) * 5 / 9;                                                                                            // apply the LOW Relative Humidity adjustement to Rothfusz and convert to Celsius from Fahrenheit
      } else if (relative_humidity > 85 && temperature_fusion_fahrenheit > 80 && temperature_fusion_fahrenheit < 87) {
          heat_index = ((heat_index_Rothfusz + heat_index_highRH_adjustment) - 32) * 5 / 9;                                                                                           // apply the HIGH Relative Humidity adjustement to Rothfusz and convert to Celsius from Fahrenheit
      } else heat_index = (heat_index_Rothfusz - 32) * 5 / 9;
    } else heat_index = (((heat_index_Steadman + temperature_fusion_fahrenheit) / 2) - 32) * 5 / 9;                                                                                   // the Steadman formula is computed first and the result averaged with the temperature for higher accuracy, then converted to Celsius


    
    float X[6] = {temperature_regressed_fusion, heat_index, air_dew_point, relative_humidity, equivalent_sea_pressure, uv_index};                                                     // array to send to the XIAO Sense for Machine Learning classification/regression >> contains the 6 features needed for the ML models

    while (!digitalRead(busy_pin));                                                                                                                                                   // block the code if the computation has been completed before listening voice line 
    nicla::leds.setColor(green);                                                                                                                                                      // change led to green to visually inform the user they can speak >> visual cue
    
    Wire.beginTransmission(slave_address);                                                                                                                                            // transmit to device #8 (SlaveAddress is used)
    for (uint8_t i = 0; i < sizeof(X); i++) I2C_writeAnything(X[i]);                                                                                                                  // transit every element from the features array individually using the I2C_Anything library >> enables the transmission of floats
    Wire.endTransmission();                                                                                                                                                           // stop transmitting


    delay(3000);                                                                                                                                                                      // wait for the processing to end on the Xiao >> data parsing/voice recording/deep learning voice interference/ML weather models
    
    nicla::leds.setColor(off);                                                                                                                                                        // turn the LED off

    float dataReceived[6];                                                                                                                                                            // define float array of 6 elements for the data received (labels) from the Seeed Xiao Sense  
    Wire.requestFrom(slave_address, sizeof(dataReceived));                                                                                                                            // request the voice command label + ML models labels (4) + ML precipitation regression
    while (Wire.available()) for (uint8_t j = 0; j < sizeof(dataReceived); j++) I2C_readAnything(dataReceived[j]);                                                                    // slave may send less than requested >> for the duration 

    switch (int(dataReceived[5])) {
      case 0:  
      {         
        //Serial.println("You've asked for the BATTERY LEVEL");

        //battery checks
        uint16_t battery_voltage_circuit    = battery.voltage();
        uint8_t  battery_percentage_circuit = battery.level();
    
        uint8_t battery_level_bq25120  = bq25120_getLevel();
        byte    battery_fault_bq25120  = bq25120_getFaults();


        PlayVoice(int(battery_level_bq25120 + 7));
        PlayVoice(int(battery_percentage_circuit + 144));
        //Serial.println("nBattery voltage is " + String(battery_voltage_circuit) + " mV");

        if (battery_fault_bq25120 == 0) PlayVoice(54);
        else PlayVoice(54);
        
        break;
      }
      
      case 1:
      {
        //Serial.println("You've asked for the CLOUDS INFO");

        //https://www.omnicalculator.com/physics/cloud-base#how-to-use-the-cloud-altitude-calculator  
        float cloud_base_altitude_groundlevel = (temperature_regressed_fusion - air_dew_point) * 124.6;                                                                                 //https://holfuy.com/en/support/cloud-base-calculations (calibrated to my device and tested against calculators)
        float cloud_base_altitude_sealevel    = cloud_base_altitude_groundlevel + altitude;
        float cloud_base_temperature          = - 0.00182 * cloud_base_altitude_groundlevel + air_dew_point;

        PlayVoice(34);
        if (cloud_base_altitude_groundlevel < 1000) {
            PlayVoice(int(cloud_base_altitude_groundlevel) + 1112);
            PlayVoice(044);
        } else if (cloud_base_altitude_groundlevel >= 1000) {
            uint8_t  cloud_altitute_groundlevel_kilometers = cloud_base_altitude_groundlevel / 1000;
            uint16_t cloud_altitute_groundlevel_meters     = int(round(cloud_base_altitude_groundlevel)) % 1000;
        
            PlayVoice(int(cloud_altitute_groundlevel_kilometers) + 1112);
            if (cloud_altitute_groundlevel_kilometers == 1) PlayVoice(45);                                     //if the altitude is 1 kilometer + x meters, then say "kilometer" voice line, not "kilometers"
            else PlayVoice(46);                                                                             // the altitude is higher than 1 kilometer, so say "kilometers"

            PlayVoice(int(cloud_altitute_groundlevel_meters) + 1112);
            PlayVoice(44);
        }
         
        delay(200);
        PlayVoice(35);
        if (cloud_base_altitude_sealevel < 1000) {
            PlayVoice(int(cloud_base_altitude_groundlevel) + 1112);
            PlayVoice(44);
        } else if (cloud_base_altitude_sealevel >= 1000) {
            uint8_t  cloud_altitute_sealevel_kilometers = cloud_base_altitude_sealevel / 1000;
            uint16_t cloud_altitute_sealevel_meters     = int(round(cloud_base_altitude_sealevel)) % 1000;
        
            PlayVoice(int(cloud_altitute_sealevel_kilometers) + 1112);
            if (cloud_altitute_sealevel_kilometers == 1) PlayVoice(45);                                     //if the altitude is 1 kilometer + x meters, then say "kilometer" voice line, not "kilometers"
            else PlayVoice(46);                                                                             // the altitude is higher than 1 kilometer, so say "kilometers"

            PlayVoice(int(cloud_altitute_sealevel_meters) + 1112);
            PlayVoice(44);
        }

        delay(200);
        PlayVoice(36);
        PlayVoice(int(cloud_base_temperature) + 64);

        break;
      }
      
      case 2:
      {
        //Serial.println("You've asked for the AURI DESCRIPTION");

        PlayVoice(33);
        
        break;
      }
      
      case 3:
      {
        //Serial.println("You've asked for the ENVIRONMENT CONDITIONS");
        uint8_t  gas_kilo_ohms = gas_resistance_BME688 / 1000;
        uint16_t gas_ohms      = gas_resistance_BME688 % 1000;

        
        PlayVoice(1);
      
        PlayVoice(int(round(temperature_regressed_fusion) + 64));
      
        delay(100);
        PlayVoice(53);
      
        PlayVoice(int(round(temperature_fusion_fahrenheit) + 1112));
      
        PlayVoice(2);

        PlayVoice(int(heat_index) + 64);
      
        delay(100);
        PlayVoice(3);
      
        PlayVoice(int(round(air_dew_point + 64)));
      
        delay(200);
        PlayVoice(4);
      
        PlayVoice(int(round(humidity_BME688_regressed + 144)));
      
        delay(100);
        PlayVoice(5);
      
        PlayVoice(int(round(relative_humidity + 144)));
      
        delay(200);
      
        PlayVoice(7);
      
        PlayVoice(int(round(pressure_BMP390_regressed - 949 + 244)));
      
        delay(100);
        PlayVoice(8);
      
        PlayVoice(int(saturation_vapor_pressure) + 1112);
      
        delay(100);
        PlayVoice(9);
      
        PlayVoice(int(round(water_vapor_pressure)) + 1112);
      
        delay(100);
        PlayVoice(10);
      
        PlayVoice(int(round(dry_air_pressure - 949 + 244)));
      
        delay(100);
        PlayVoice(11);
      
        PlayVoice(int(round(equivalent_sea_pressure - 949 + 244)));
      
        delay(300);
        if (gas_resistance_BME688 < 100) PlayVoice(49);
        else if (gas_resistance_BME688 < 1000) {
          PlayVoice(50);
          PlayVoice(int(gas_resistance_BME688) + 1112);
          PlayVoice(52);                                //ohms
        } else {
          PlayVoice(50);
          PlayVoice(int(gas_kilo_ohms) + 1112);
          PlayVoice(51);                                //kilo-ohms
      
          if (gas_ohms != 0) {
            PlayVoice(23);                                //and
            PlayVoice(int(gas_ohms) + 1112);
            PlayVoice(52);
          }
        }
      
        
        break;  
      }
      
      case 4:
      {
        //Serial.println("You've asked for the HEALTH REPORT");

        float   laser_skin_temp  = mlx.readObjectTempC() - 3.5;                                                                              //the sensor has been calibrated to detect through a tempered glass screen the human skin temeperature compared to the regular reading at 5cm ( most accurate)
        uint8_t discomfort_index = temperature_regressed_fusion - 0.55 * (1 - 0.01 * humidity_BME688_regressed) * (temperature_regressed_fusion - 14.5);                                                                  //discomfort index formula

        BHY2.update();
        uint8_t  bsec_accuracy  = bsec.accuracy();
        uint16_t air_quality    = bsec.iaq();
        uint32_t carbon_dioxide = bsec.co2_eq();
        float    volatile_mix   = bsec.b_voc_eq();


        //output skin temperature
        PlayVoice(28);                                                                                                          //laser skin temperature
        PlayVoice(int(round(laser_skin_temp)) + 64);


        uint8_t       heat_index_values[6] = {4, 26, 32, 39, 51, 150};
        uint_least8_t heat_index_output[5] = {1, 2, 3, 4, 5};
        for (uint8_t n = 0; n < 5; n++) {
          if (temperature_regressed_fusion >= 4) {
            if (heat_index_values[n] < heat_index && heat_index <= heat_index_values[n+1]) {
              PlayVoice(29);                                                                                                    //In terms of the effects of the environment on your health. The heat index stands at:
              PlayVoice(int(round(heat_index)) + 1112);                                                                         // heat index number
              PlayVoice(heat_index_output[n] + 375);                                                                            //The Heat Index Condition line
            }
          } //else Serial.println("The Heat Index Condition cannot be calculated as the environment temperature is too low. Please consider the temperature as the absolute truth!");      // VOICE COMMAND NEEDS TO BE CREATED/ADDED
        }


        uint_least8_t discomfort_values[7] = {0, 21, 24, 27, 29, 32, 250};
        uint_least8_t discomfort_output[6] = {1, 2, 3, 4, 5, 6};  
        for (uint8_t l = 0; l < 6; l++) {
          if (discomfort_values[l] < discomfort_index && discomfort_index <= discomfort_values[l+1]) {
            PlayVoice(56);
            PlayVoice(int(discomfort_index) + 1112);
            PlayVoice(discomfort_output[l] + 369);
          }
        }


        if (pressure_BMP390_regressed > 1022.678) PlayVoice(369);                                                 // "Caution! High pressure which pushes against the body and limits how much the tissue can expand. Potential of joint paint!"
        else if (pressure_BMP390_regressed < 1007) PlayVoice(368);                                                // "Caution! Low pressure allowing the body's tissues to expand, affecting the nerves, which can cause migraines!"
        else PlayVoice(367);                                                                                      // "Expected pressure range!" 


        PlayVoice(63);                                                                                            // "Checking the system status. The BSEC sensor accuracy is at the level:" 
        PlayVoice(bsec_accuracy + 1112);                                                                          // output the accuracy level
        
        uint16_t      iaq_values[8] = {0, 50, 100, 150, 200, 250, 350, 500};
        uint_least8_t iaq_output[7] = {1, 2, 3, 4, 5, 6, 7};  
        for (uint8_t j = 0; j < 7; j++) {
          if (iaq_values[j] < air_quality && air_quality <= iaq_values[j+1]) {
            PlayVoice(30);                                                                                        // "The air quality of your environment resides at:"
            PlayVoice(air_quality + 1112);
            PlayVoice(37);
            PlayVoice(iaq_output[j] + 359);
          }
        }


        uint16_t      co2_values[10] = {0, 400, 600, 900, 1100, 1600, 2000, 5000, 40000, 65500};
        uint_least8_t co2_output[9]  = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
        for (uint8_t k = 0; k < 9; k++) {
          if (co2_values[k] < carbon_dioxide && carbon_dioxide <= co2_values[k+1]) {
            PlayVoice(31);
            PlayVoice(co2_output[k] + 350);
          }
        }


        uint8_t       voc_values[11] = {0, 5, 10, 15, 50, 255}; 
        uint_least8_t voc_output[5]  = {1, 2, 3, 4, 5};
        for (uint8_t h = 0; h < 5; h++) {
          if (voc_values[h] < volatile_mix && volatile_mix <= voc_values[h+1]) {
            PlayVoice(32);                                                                                      // "In addition, I have also detected"
            if (volatile_mix <= 50) PlayVoice(int(round(volatile_mix)) + 1007);                                 // 0-50 ppm VOC
            else PlayVoice(1058);                                                                               // over 50 ppm VOC
            PlayVoice(voc_output[h] + 345);                                                                     //VOC condition output
          } 
        }

        break;
      }
      
      case 5:
      {
        //Serial.println("You've asked for the LOCATION DETAILS");   

        BHY2.update();
        float heading = orientation.heading();
        float pitch   = orientation.pitch();
        float roll    = orientation.roll();

        float   compass_points[18] = {0, 11.25, 33.75, 56.25, 78.75, 101.25, 123.75, 146.25, 
                                      168.75, 191.25, 213.75, 236.25, 258.75, 281.25, 303.75, 326.25, 348.75, 360};
        uint8_t compass_output[17] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};  
        for (uint8_t m = 0; m < 17; m++) {
          if (compass_points[m] <= heading && heading <= compass_points[m+1]) {
            PlayVoice(13);
            PlayVoice(758 + compass_output[m]);
          }
        }

        PlayVoice(14);                                                                 //"based on an angle of"
        PlayVoice(int(heading) + 381); 
        PlayVoice(15);                                                                 //"with a compass roll of"
        PlayVoice(int(roll) + 381);
        PlayVoice(16);                                                                 //"and a compass pitch of"
        PlayVoice(int(pitch) + 381);

        PlayVoice(12);
        if (altitude < 0) {
            PlayVoice(int(-altitude) + 1112);
            PlayVoice(44);
        } else if (altitude >= 0 && altitude < 1000) {
            PlayVoice(int(altitude) + 1112);
            PlayVoice(44);
        } else if (altitude >= 1000) {
            uint8_t  altitude_kilometers = altitude / 1000;
            uint16_t altitude_meters     = altitude % 1000;
        
            PlayVoice(altitude_kilometers + 1112);
            if (altitude_kilometers == 1) PlayVoice(45);                                                    //if the altitude is 1 kilometer + x meters, then say "kilometer" voice line, not "kilometers"
            else PlayVoice(46);                                                                             // the altitude is higher than 1 kilometer, so say "kilometers"

            PlayVoice(int(altitude_meters) + 1112);
            PlayVoice(44);
        }

        break;
      }
      
      case 6:
      {
        //Serial.println("You've asked for the STEPS COUNTER");
  
        //steps counting
        BHY2.update();
        unsigned long total_steps   = steps.value();
        unsigned long current_steps = total_steps - last_steps;

        //total steps
        PlayVoice(17);
        if (total_steps < 1000) {
            PlayVoice(int(total_steps) + 1112);
        } else if (altitude >= 1000) {
            uint8_t  total_steps_thousands = total_steps / 1000;
            uint16_t total_steps_reminder  = total_steps % 1000;
        
            PlayVoice(total_steps_thousands + 1112);
            if (total_steps_thousands == 1) PlayVoice(45);                                                    //NEEDS TO BE CHANGED TO THOUSAND
            else PlayVoice(46);                                                                             //NEEDS TO BE CHANGED TO THOUSANDS

            PlayVoice(int(total_steps_reminder) + 1112);                                                   
        }

        //current steps
        if (current_steps < 1000) {
            PlayVoice(int(current_steps) + 1112);
        } else if (altitude >= 1000) {
            uint8_t  current_steps_thousands = current_steps / 1000;
            uint16_t current_steps_reminder  = current_steps % 1000;
        
            PlayVoice(current_steps_thousands + 1112);
            PlayVoice(45);                                                    //NEEDS TO BE CHANGED TO THOUSAND
            
            PlayVoice(int(current_steps_reminder) + 1112);                                                   
        }

        //conversion of steps to kilocalories
        unsigned long kilocalories = current_steps * 0.03344;
        
        PlayVoice(19);
        if (kilocalories < 1000) {
            PlayVoice(int(kilocalories) + 1112);
            PlayVoice(47);                                                                               //"kilocalories"
        } else if (altitude >= 1000) {
            uint8_t  kilocalories_thousands = kilocalories / 1000;
            uint16_t kilocalories_reminder  = kilocalories % 1000;
        
            PlayVoice(kilocalories_thousands + 1112);
            PlayVoice(45);                                                                               //NEEDS TO BE CHANGED TO THOUSAND
            PlayVoice(int(kilocalories_reminder) + 1112);          
            PlayVoice(47);                                  
        }
        

        //conversion of kilocalories to kilojouls (energy)
        unsigned long kilojouls = kilocalories * 4.184;
        
        PlayVoice(20);
        if (kilocalories < 1000) {
            PlayVoice(int(kilojouls) + 1112);
            PlayVoice(48);                                                                               //"kilocalories"
        } else if (altitude >= 1000) {
            uint8_t  kilojouls_thousands = kilojouls / 1000;
            uint16_t kilojouls_reminder  = kilojouls % 1000;
        
            PlayVoice(kilojouls_thousands + 1112);
            PlayVoice(45);                                                                               //NEEDS TO BE CHANGED TO THOUSAND
            PlayVoice(int(kilojouls_reminder) + 1112);          
            PlayVoice(48);                                  
        }

        //conversion of current_steps to meters
        uint32_t distance_travelled = (current_steps / 1179) * 1000;                                       //convert from steps to kilometers, and then from kilometers to meters
       
        PlayVoice(21);
        if (distance_travelled < 1000) {
            PlayVoice(int(distance_travelled) + 1112);
            PlayVoice(44);
        } else if (distance_travelled >= 1000) {
            uint8_t  distance_travelled_kilometers = distance_travelled / 1000;
            uint16_t distance_travelled_meters     = distance_travelled % 1000;
        
            PlayVoice(distance_travelled_kilometers + 1112);
            if (distance_travelled_kilometers == 1) PlayVoice(45);                                                    //if the distance_travelled is 1 kilometer + x meters, then say "kilometer" voice line, not "kilometers"
            else PlayVoice(46);                                                                                     // the distance_travelled is higher than 1 kilometer, so say "kilometers"

            PlayVoice(int(distance_travelled_meters) + 1112);
            PlayVoice(44);
        }
        
        
        break;
      }
      
      case 7:
      {
        //Serial.println("You've asked for the THANKS OUTPUT");
        uint8_t random_thanks = random(1, 7);                                                                    //returns a random number between min(1) and max-1(10) >> equivalent to the 6 thanks voice lines available
        PlayVoice(random_thanks + 752);
        
        break;
      }
      
      case 8:
      {
        //Serial.println("You've asked for the TIME AND DATE");

        if (timeStatus() != timeNotSet) {
          //output time
          PlayVoice(22);
          PlayVoice(int(hour()) + 1112);
          PlayVoice(int(minute()) + 1112);
          
          delay(200);
          PlayVoice(24);
          PlayVoice(int(day()) + 1058);
          PlayVoice(int(month()) + 1089);

          uint8_t year_first_half = year() / 100;
          uint8_t year_second_half = year() % (year_first_half * 100);

          PlayVoice(year_first_half + 1112);
          if (year_second_half == 0) PlayVoice(44);                                                                                  //NEEDS TO BE CHANGED TO HUNDRED
          else PlayVoice(year_second_half + 1112);
          
        } else PlayVoice(62);
       
        break;
      }
      
      case 9: 
      {
        //Serial.println("You've asked for the WEATHER");

        //find the correct weather classification output and say it
        uint8_t allLabels[4], bestLabels[2] = {5, 5}, primes[5] = {2, 3, 5, 7, 11};
        int product = 1;
        
        for (uint8_t l = 0; l < 4; l++) {
          allLabels[l] = dataReceived[l];
          product *= primes[allLabels[l]];
        }
        for (uint8_t k = 0; k < 4; k++) if (!fmod(product, sq(primes[allLabels[k]]))) bestLabels[k / 2] = allLabels[k];

        if (bestLabels[0] == 5) PlayVoice(allLabels[1] + 742);                                                                            //Everything repeats once. XGBoost has been selected for ML Classification as the most robust model
        else {  
          PlayVoice(bestLabels[0] + 742);                                                                                                 //the most common classification out of all 4 models
          delay(200);
          if (bestLabels[0] != 5 && bestLabels[0] != bestLabels[1]) PlayVoice(bestLabels[1] + 747);                                       //if 2 outputs repeat twice with all ML models, then also output the chance of the 2nd classification
        }

        //rainfall forecasting output
        delay(200);
        PlayVoice(752);
        if (int(dataReceived[4] * 100) == 0) PlayVoice(1112);
        else PlayVoice(int(dataReceived[4] * 100) + 2111);
        
        break;
      }
      
      case 10:
      {
        //Serial.println("VOICE VOLUME TOO LOW OR TOO MUCH NOISE. PLEASE REPEAT THE COMMAND");

        PlayVoice(57);                                                                                                                                                           //You spoke too quietly, or there was too much noise in your surroundings. PLEASE REPEAT THE COMMAND!

        break;
      }
      
      case 11:
      {
        //Serial.println("SOFTWARE ISSUE DETECTED. FAILED TO SETUP AUDIO SAMPLING");

        PlayVoice(58);
       
        break;
      }
      
      case 12:
      {
        //Serial.println("SOFTWARE OR HARDWARE ISSUE DETECTED. FAILED TO RECORD AUDIO");

        PlayVoice(59);
        
        break;
      }
      
      case 13:
      {
        //Serial.println("DEEP LEARNING ISSUE DETECTED. FAILED TO RUN THE CLASSIFIER");

        PlayVoice(60);
        
        break;
      }
    }
  }
  delay(100);
}

void PlayVoice(int file) {
  DFPlayer.play(file);                                                                                                                                                                // play the wav file specified with the play function of the DFPlayer object
  delay(300);                                                                                                                                                                         // delay 300ms necessary to compute the command >> any delay higher than 150ms works 
  while (!digitalRead(busy_pin));                                                                                                                                                     // block the code for the duration of the file playing >> busy pin will be LOW
} 

void UpdateMainSensors() {
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


void HeartMonitor() {
  heartrate.getValue(heartrate_pin);                                                                                                                                                  // analog heart rate sampled values
  uint8_t rate_value = heartrate.getRate();                                                                                                                                           // get heart rate value 
  if(rate_value)  {                                                                                                                                                                   // if the sensor has been positioned properly
    //Serial.println("The heart rate is: " + String(rate_value));                                                                                                                     // play voice for the heart rate value
  }
  delay(10);
}


float RoundToHalfInteger(float x) {
  return 0.5 * round(2.0 * x) ;                                                                                                                                                       // equation to round any value to .0 or .5 >> used for temperature throghout the code
}


void ProcessSyncMessage() {
  unsigned long pctime;                                                                                                                                                               // variable to store the correct timedate received from the PC Serial
  const unsigned long DEFAULT_TIME = 1357041600;                                                                                                                                      // Jan 1 2013

  pctime = Serial.parseInt();
  if ( pctime >= DEFAULT_TIME) {                                                                                                                                                      // check the integer is a valid time (greater than Jan 1 2013)
    setTime(pctime);                                                                                                                                                                  // sync Arduino clock to the time received on the serial port
  }
}


time_t RequestSync() {
  Serial.write(TIME_REQUEST);                                                                                                                                                         // serial write the sync request header
  return 0;                                                                                                                                                                           // the time will be sent later in response to serial mesg
}
