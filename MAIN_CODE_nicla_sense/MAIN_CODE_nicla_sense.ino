//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------LIBRARIES-------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Nicla_System.h"
#include "Arduino_BHY2.h"
#include "Wire.h"
#include <Adafruit_MLX90614.h>
#include "Adafruit_SI1145.h"
#include <Ewma.h>
#include <EwmaT.h>


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------DECLARATIONS------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
SensorBSEC bsec(SENSOR_ID_BSEC);
SensorOrientation orientation(SENSOR_ID_ORI);
Sensor tilt(SENSOR_ID_WRIST_TILT_GESTURE);

Adafruit_SI1145 uv = Adafruit_SI1145();
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

Ewma temperature_ewma(0.2);           // Less smoothing - faster to detect changes, but more prone to noise
Ewma pressure_ewma(0.01);             // Most smoothing - less prone to noise, but very slow to detect changes
EwmaT <int> humidity_ewma(1, 10);     // More smoothing - less prone to noise, but slower to detect changes

uint8_t humidity_filtered;
float temperature_filtered, pressure_filtered, heat_index;
float sea_level_pressure = 1013.25, dry_air_constant = 287.058, water_vapor_constant = 461.495;                              //specific gas constant for dry air equal in J/(kg·K) //specific gas constant for water vapor in J/(kg·K)

String voc_air_quality = "";
String iaq_air_quality = "";
String co2_air_quality = "";
String pressure_effect = "";
String discomfort_condition = "";
String heat_index_condition = "";

String compass_array[3];
String compass_heading = "";

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------SYSTEM SETUP------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  delay(2000);
  
  nicla::begin();
  BHY2.begin();
  temperature.begin();
  humidity.begin();
  pressure.begin();
  bsec.begin();
  orientation.begin();

  if (!mlx.begin()) Serial.println("Error connecting to MLX sensor. Check wiring.");
  if (!uv.begin()) Serial.println("Didn't find Si1145");

  Serial.begin(9600);

  delay(3000);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------MAIN CODE-----------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
 
  //UpdateNiclaSensors();
  UpdateMainSensors();            //update sensor values
  
  float temperature_calibrated = RoundToHalfInteger((1.0095 * temperature_filtered) - 4.8051);
  float temperature_fahrenheit = (temperature_calibrated * 9 / 5) + 32;
  float pressure_calibrated = RoundToHalfInteger(1.00718 * pressure_filtered);
  uint8_t humidity_calibrated = round((1.4383 * humidity_filtered) - 2.5628);

  uint8_t iaq = ((bsec.toString()).substring(25, 28)).toInt();
  float voc_eq = ((bsec.toString()).substring(53, 57)).toFloat();
  int co2_eq = ((bsec.toString()).substring(68, 72)).toInt();
  uint8_t accuracy = ((bsec.toString()).substring(84, 85)).toInt();

  StringSeparator(orientation.toString(), compass_array);
  float heading = (compass_array[0]).toFloat();
  float pitch = (compass_array[1]).toFloat();
  float roll = (compass_array[2]).toFloat();

  float visible_light = uv.readVisible();
  float infrared_light = uv.readIR();
  uint16_t uv_index = uv.readUV() / 100.0;

  float laser_object_temp = mlx.readObjectTempC() - 3.5;                                                                              //the sensor has been calibrated to detect through a tempered glass screen the human skin temeperature compared to the regular reading at 5cm ( most accurate)

  uint16_t altitude = ((pow((sea_level_pressure / pressure_calibrated), (1 / 5.257)) - 1) * (temperature_calibrated + 273.15)) / 0.0065;                                                                                                                                                          //altitude hypsometric formula

  uint8_t discomfort_index = temperature_calibrated - 0.55 * (1 - 0.01 * humidity_calibrated) * (temperature_calibrated - 14.5);                                                                                                                                                                                 //discomfort index formula

  //the ratio of the actual amount of water vapor in the air to the amount it could hold when saturated
  float relative_humidity = (((humidity_calibrated * pressure_calibrated) / (0.378 * humidity_calibrated + 0.622))                                                                                                          // relative humidity from Bolton 1980 The computation of Equivalent Potential Temperature
                            /(6.112 * exp((17.67 * temperature_calibrated) / (temperature_calibrated + 243.5))));                                                                                                               //The dew point is the temperature to which air must be cooled to become saturated with water vapor
  float air_dew_point = ((243.12 * (log(relative_humidity / 100) + ((17.62 * temperature_calibrated) / (243.12 + temperature_calibrated)))) 
                        /(17.62 - (log(relative_humidity / 100) + ((17.62 * temperature_calibrated) / (243.12 + temperature_calibrated)))));
  float saturation_vapor_pressure = 6.1078 * pow(10, ((7.5 * temperature_calibrated) / (temperature_calibrated + 237.3)));                                                                                                                                                             //from https://www.omnicalculator.com/physics/air-density#how-to-calculate-the-air-density
  float water_vapor_pressure = (saturation_vapor_pressure * relative_humidity) / 100;                                                                                                                                                                          //multiplication of saturation vapor pressure with the relative humidity, divided by 100%
  float dry_air_pressure = pressure_calibrated - water_vapor_pressure;
  float air_density = 100 * ((dry_air_pressure / (dry_air_constant * (temperature_calibrated + 273.15)))                                                                                       //air density formula, can be used for air buoyancy
                      +(water_vapor_pressure / ( water_vapor_constant * (temperature_calibrated + 273.15))));

  //the mass of water vapor in a unit volume of air. It is a measure of the actual water vapor content of the air.
  float absolute_humidity = (((saturation_vapor_pressure * relative_humidity) / 10) / (water_vapor_constant * temperature_calibrated));                                                                                                                                         //expressed in kilograms per cubic meter of moist air - https://planetcalc.com/2167/

  //the regression equation of Rothfusz used by the US National Weather Service: https://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
  float heat_index_Rothfusz = -42.379 + (2.04901523 * temperature_fahrenheit) - (0.00000199 * pow(temperature_fahrenheit, 2) * pow(relative_humidity, 2))
                              -(0.22475541 * temperature_fahrenheit * relative_humidity) - (0.00683783 * pow(temperature_fahrenheit, 2))
                              -(0.05481717 * pow(relative_humidity, 2)) + (0.00122874 * pow(temperature_fahrenheit, 2) * relative_humidity)
                              +(0.00085282 * temperature_fahrenheit * pow(relative_humidity, 2)) + (10.14333127 * relative_humidity);                                                                                      //felt air temperature, does not take into consideration direct sunlight effect
  float heat_index_Steadman = 0.5 * (temperature_fahrenheit + 61.0 + ((temperature_fahrenheit - 68.0) * 1.2) + (relative_humidity * 0.094));

  float heat_index_lowRH_adjustment = ((13 - relative_humidity) / 4) * sqrt((17 - abs(temperature_fahrenheit - 95)) / 17);                                  //if the RH is less than 13% and the temperature is between 80 and 112 degrees F, then the following adjustment is subtracted from HI
  float heat_index_highRH_adjustment = ((relative_humidity - 85) / 10) * ((87 - temperature_fahrenheit) / 5);                                               //if the RH is greater than 85% and the temperature is between 80 and 87 degrees F, then the following adjustment is added to HI

  if (heat_index_Steadman >= 80) {                                                                                                                          //If this heat index value is 80 degrees F or higher, the full regression equation along with any adjustment as described above is applied.
    if (relative_humidity < 13 && temperature_fahrenheit > 80 && temperature_fahrenheit < 112) {
      heat_index = ((heat_index_Rothfusz - heat_index_lowRH_adjustment) - 32) * 5 / 9;
    } else if (relative_humidity > 85 && temperature_fahrenheit > 80 && temperature_fahrenheit < 87) {
      heat_index = ((heat_index_Rothfusz + heat_index_highRH_adjustment) - 32) * 5 / 9;
    } else heat_index = (heat_index_Rothfusz - 32) * 5 / 9;
  } else heat_index = (((heat_index_Steadman + temperature_fahrenheit) / 2) - 32) * 5 / 9;                                                               //the Steadman formula is computed first and the result averaged with the temperature


  //https://www.omnicalculator.com/physics/cloud-base#how-to-use-the-cloud-altitude-calculator
  float cloud_base_altitude_groundlevel = (temperature_calibrated - air_dew_point) * 124.6;             //https://holfuy.com/en/support/cloud-base-calculations (calibrated to my device and tested against calculators)
  float cloud_base_altitude_sealevel = cloud_base_altitude_groundlevel + altitude;

  float cloud_base_temperature = - 0.00182 * cloud_base_altitude_groundlevel + air_dew_point;

  if (voc_eq <= 5.0) voc_air_quality = "Clean Air! No Volatile Detected!";
  else if (5.0 < voc_eq && voc_eq <= 10.0) voc_air_quality = "Ethane Alkane Detected!";
  else if (10.0 < voc_eq && voc_eq <= 15.0) voc_air_quality = "Isoprene/Methyl/Butadiene/Ethanol Detected!";
  else if (15.0 < voc_eq && voc_eq <= 50.0) voc_air_quality = "Carbon Monoxide Detected!";
  else if (voc_eq > 50.0) voc_air_quality = "Acetone Detected!";

  if (iaq <= 50) iaq_air_quality = "Pure air. No measures needed!";
  else if (50 < iaq && iaq <= 100) iaq_air_quality = "Good Air! No irritation or impact on well-being. No health measures needed! ";
  else if (100 < iaq && iaq <= 150) iaq_air_quality = "Lightly polluted air! Reduction of well-being possible! Fresh air or ventilation suggested!";
  else if (150 < iaq && iaq <= 200) iaq_air_quality = "Moderately polluted. Reduction of well-being possible! Increase ventilation with clean air!";
  else if (200 < iaq && iaq <= 250) iaq_air_quality = "Heavily polluted. Continous exposition might lead to migraines! Immediately increase ventilation or leave the environment!";
  else if (250 < iaq && iaq <= 350) iaq_air_quality = "Severely polluted. Severe health issues possible! Contamination should be identified! Maximise ventilation or leave environment";
  else if (iaq > 350) iaq_air_quality = "Extremely polluted. Neurotoxic effects possible! Contamination must be identified! Avoid the environment!";

  if (co2_eq <= 400) co2_air_quality = "Normal CO2 concentration in outdoor ambient air!";
  else if (400 < co2_eq && co2_eq <= 600) co2_air_quality = "Excellent air! CO2 concentrations typical of occupied indoor spaces with good air exchange!";
  else if (600 < co2_eq && co2_eq <= 900) co2_air_quality = "Good air! CO2 concentrations typical of occupied indoor spaces with decent air exchange!";
  else if (900 < co2_eq && co2_eq <= 1100) co2_air_quality = "Fair air! CO2 concentrations typical of occupied indoor spaces with no air exchange!";
  else if (1100 < co2_eq && co2_eq <= 1600) co2_air_quality = "Mediocre air! High CO2 concentrations! Increase ventilation or leave environment!";
  else if (1600 < co2_eq && co2_eq <= 2000) co2_air_quality = "Bad air! High CO2 concentrations! Ventilation necessary! Leave environment!";
  else if (2000 < co2_eq && co2_eq <= 5000) co2_air_quality = "Headaches, sleepiness, increased heart rate and slight nausea can be caused. Leave environment immediately!";
  else if (5000 < co2_eq && co2_eq < 40000) co2_air_quality = "Environment exposure may lead to oxygen deprivation resulting in adverse health effects. Leave environment immediately!";
  else if (co2_eq > 40000) co2_air_quality = "Environment exposure may lead to oxygen deprivation resulting in permanent brain damage and coma! Leave environment urgently!";

  if (discomfort_index <= 21) discomfort_condition = "No discomfort!";
  else if (21 < discomfort_index && discomfort_index <= 25) discomfort_condition = "Under 50% of the population feels discomfort!";
  else if (24 < discomfort_index && discomfort_index <= 27) discomfort_condition = "Over 50% of the population feels discomfort!";
  else if (27 < discomfort_index && discomfort_index <= 29) discomfort_condition = "Most of the population suffers discomfort!";
  else if (29 < discomfort_index && discomfort_index <= 32) discomfort_condition = "Discomfort Index High! The environment will cause severe stress!";
  else if (discomfort_index > 32) discomfort_condition = "State of medical emergency! Please leave environment!";

  if (pressure_calibrated > 1022.678) pressure_effect = "Caution! High pressure which pushes against the body and limits how much the tissue can expand. Potential of joint paint!";
  else if (pressure_calibrated < 1007) pressure_effect = "Caution! Low pressure allowing the body's tissues to expand, affecting the nerves, which can cause migraines!";
  else if (1007 <= pressure_calibrated && pressure_calibrated <= 1022.678) pressure_effect = "Expected pressure range!";

  if  (27 <= heat_index && heat_index <= 32) heat_index_condition = "Caution Advised. High heat!";
  else if (33 <= heat_index && heat_index <= 39) heat_index_condition = "Increased Caution Advised. High heat!";
  else if (40 <= heat_index && heat_index <= 51) heat_index_condition = "Danger! Only stay outdoor if necessary!";
  else if (heat_index > 52)  heat_index_condition = "Extreme Danger! Do not stay outdoors, migraine and negative skin effects possible!";

  if (0 <= heading && heading <= 11.25) compass_heading = "You are heading NORTH!";
  else if (348.75 < heading && heading <= 360) compass_heading = "You are heading NORTH!";
  else if (78.75 < heading && heading <= 101.25) compass_heading = "You are heading EAST!";
  else if (168.75 < heading && heading <= 191.25) compass_heading = "You are heading SOUTH!";
  else if (258.75 < heading && heading <= 281.25) compass_heading = "You are heading WEST!";

  else if (33.75 < heading && heading <= 56.25) compass_heading = "You are heading NORTH-EAST!";
  else if (123.75 < heading && heading <= 146.25) compass_heading = "You are heading SOUTH-EAST!";
  else if (213.75 < heading && heading <= 236.25) compass_heading = "You are heading SOUTH-WEST!";
  else if (303.75 < heading && heading <= 326.25) compass_heading = "You are heading NORTH-WEST!";

  else if (11.25 < heading && heading <= 33.75) compass_heading = "You are heading NORTH-NORTH-EAST!";
  else if (56.25 < heading && heading <= 78.75) compass_heading = "You are heading EAST-SOUTH-EAST!";
  else if (101.25 < heading && heading <= 123.75) compass_heading = "You are heading EAST-SOUTH-EAST!";
  else if (146.25 < heading && heading <= 168.75) compass_heading = "You are heading SOUTH-SOUTH-EAST!";
  else if (191.25 < heading && heading <= 213.75) compass_heading = "You are heading SOUTH-SOUTH-WEST!";
  else if (236.25 < heading && heading <= 258.75) compass_heading = "You are heading WEST-SOUTH-WEST!";
  else if (281.25 < heading && heading <= 303.75) compass_heading = "You are heading WEST-NORTH-WEST!";
  else if (326.25 < heading && heading <= 348.75) compass_heading = "You are heading NORTH-NORTH-WEST!";


  Serial.print("\r\n");
  Serial.println(String("---------------------------------------------------------------------------------------------------------"));

  Serial.println(String("Temperature is: ") + String(temperature_calibrated) + String(" C"));
  Serial.println(String("Temperature in Fahrenheit is: ") + String(temperature_fahrenheit) + String(" F"));
  Serial.println(String("Humidity is: ") + String(humidity_calibrated) + String(" %"));
  Serial.println(String("Pressure: ") + String(pressure_calibrated) + String(" hPa"));

  Serial.println("Visible Light: " + String(visible_light));
  Serial.println("Infrared Light: " + String(infrared_light));
  Serial.println("UV Index: " + String(uv_index));

  Serial.println("Laser Object Temp: " + String(laser_object_temp) + " C");

  Serial.println(String("Altitude: ") + String(altitude) + String(" m"));
  Serial.println(String("Relative Humidity: ") + String(relative_humidity) + String(" %"));
  Serial.println(String("Air Dew Point Accurate: ") + String(air_dew_point) + String(" C"));
  Serial.println(String("Saturation Vapor Pressure: ") + String(saturation_vapor_pressure));
  Serial.println(String("Water Vapor Pressure: ") + String(water_vapor_pressure) + String(" hPa"));
  Serial.println(String("Dry Air Pressure ") + String(dry_air_pressure) + String(" hPa"));
  Serial.println(String("Air Density: ") + String(air_density) + String(" kg/m^3"));
  Serial.println(String("Absolute Humidity: ") + String(absolute_humidity, 4) + String(" %"));

  Serial.println(String("Cloud Base Altitude Ground Level: ") + String(cloud_base_altitude_groundlevel, 4) + " m");
  Serial.println(String("Cloud Base Altitude Sea Level: ") + String(cloud_base_altitude_sealevel, 4) + " m");
  Serial.println(String("Cloud Base Temperature: ") + String(cloud_base_temperature) + " C");

  Serial.println(String("Heat Index: ") + String(heat_index, 4) + " C");
  Serial.println(String("Discomfort level: ") + String(discomfort_index));
  Serial.println(discomfort_condition);
  Serial.println(pressure_effect);

  Serial.println(String("Volatile Organic Gasses: ") + String(voc_eq) + String(" ppm"));
  Serial.println(voc_air_quality);

  Serial.println(String("Estimation of CO2 levels: ") + String(co2_eq) + String(" ppm"));
  Serial.println(co2_air_quality);

  Serial.println(String("Air Quality Index is: ") + String(iaq));
  Serial.println(String("IAQ Sensor Accuracy Level: ") + String(accuracy));
  Serial.println(iaq_air_quality);

  //Serial.println(String("Tilt info: ") + tilt.toString() + String(" - Binary value:") + tilt.value());

  Serial.println(String(compass_heading) + String(" Heading Angle: ") + String(heading));
  Serial.println(String("Compass Roll/Tilt: ") + String(pitch, 2));
  Serial.println(String("Compass Roll/Tilt: ") + String(roll, 2));
  Serial.println(String("---------------------------------------------------------------------------------------------------------"));

  delay(2000);
}

void UpdateNiclaSensors() {
  for (uint8_t i = 0; i < 10; i++) {
    BHY2.update();                                                                //necessary to update sensor 
    delay(10);
  }
}


void UpdateMainSensors() {
  float temperature_readings = 0, pressure_readings = 0;                            //declare and initialize variable for the sum of 10 readings
  uint16_t humidity_readings = 0;                                                   //16bits are required to hold 10 values that can reach 10000

  for (uint8_t i = 0; i < 10; i++) {
    BHY2.update();                                                                  //necessary to update sensor classes
    temperature_readings += temperature.value();                                    //sum all the values from the array to create the threshold value
    humidity_readings += humidity.value();
    pressure_readings += pressure.value();
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

void StringSeparator(String str, String strs[]) {                                       //split the string into substrings, second value
  memset(strs, 0, sizeof(strs));                                                                     //reset char array to empty values
  int StringCount = 0;                                                                               //reset string index to 0

  while (str.length() > 6)
  {
    int index = str.indexOf(' ');
    if (index == -1)                                                                     // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      if (isDigit(str.charAt(StringCount + 1))) strs[StringCount++] = str.substring(0, index);
      str = str.substring(index + 1);
    }
  }
}
