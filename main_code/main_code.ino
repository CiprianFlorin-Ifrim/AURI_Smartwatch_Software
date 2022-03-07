//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------LIBRARIES-------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Arduino_BHY2.h"
#include "Nicla_System.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------DECLARATIONS------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
Sensor gas(SENSOR_ID_GAS);
SensorBSEC bsec(SENSOR_ID_BSEC);
SensorOrientation orientation(SENSOR_ID_ORI);
Sensor tilt(SENSOR_ID_WRIST_TILT_GESTURE);

String voc_air_quality = "";
String iaq_air_quality = "";
String co2_air_quality = "";
String pressure_effect ="";
String discomfort_condition = "";
String heat_index_condition ="";
float sea_level_pressure = 1013.25;
float dry_air_constant = 287.058;                                               //specific gas constant for dry air equal in J/(kg·K)
float water_vapor_constant = 461.495;                                      //specific gas constant for water vapor in J/(kg·K)

String compass_array[13];
int StringCount = 0;
String compass_heading = "";

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------SYSTEM SETUP------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup(){
  nicla::begin();
  Serial.begin(115200);
  BHY2.begin();
  temperature.begin();
  humidity.begin();
  pressure.begin();
  gas.begin();
  bsec.begin();
  orientation.begin();
  tilt.begin();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------MAIN CODE-----------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop(){
  static auto lastCheck= millis();
  BHY2.update();

  // Check sensor values every second  
  if (millis() - lastCheck >= 2000) {
    lastCheck = millis();

    //the word "Temperature" is already taken by the Nicla board, so I use "temp_value"
    float temp_value = temperature.value();
    int hum_value = humidity.value();
    int gas_resistance = gas.value();
    float pressure_value = pressure.value();
    
    int iaq = ((bsec.toString()).substring(25,28)).toInt();
    float voc_eq = ((bsec.toString()).substring(53,57)).toFloat();
    int co2_eq = ((bsec.toString()).substring(68,72)).toInt();
    int accuracy = ((bsec.toString()).substring(84,85)).toInt();
    float comp_temp = ((bsec.toString()).substring(95,101)).toFloat();
    int comp_hum = ((bsec.toString()).substring(112,117)).toInt();
    int comp_gas_res = ((bsec.toString()).substring(128,130)).toInt();

    string_separator(orientation.toString(), compass_array, 0);
    float heading = (compass_array[4]).toFloat() + 28;
    float roll = (compass_array[12]).toFloat();

    float temp_fusion = (temp_value+comp_temp)/2;
    float humidity_fusion = (hum_value+comp_hum)/2;

    int altitude = ((pow((sea_level_pressure/pressure_value), (1/5.257)) -1) * (temp_value + 273.15))/0.0065;           //altitude hypsometric formula
    
    int discomfort_index = temp_value - 0.55 * (1 -0.01 * hum_value) * (temp_value - 14.5);                                        //discomfort index formula

    //the ratio of the actual amount of water vapor in the air to the amount it could hold when saturated
    float relative_humidity = ((hum_value * pressure_value)/(0.378 * hum_value + 0.622))/(6.112 * exp((17.67 * temp_value)/(temp_value+243.5)));     // relative humidity from Bolton 1980 The computation of Equivalent Potential Temperature 
    float air_dew_point = temp_value - ((100-relative_humidity)/5);                                                                                //The dew point is the temperature to which air must be cooled to become saturated with water vapor
    float air_dew_point_accurate = (243.12 * (log(relative_humidity/100) + ((17.62* temp_value)/(243.12+temp_value)))) / (17.62 - (log(relative_humidity/100) + ((17.62* temp_value)/(243.12+temp_value))));
    float saturation_vapor_pressure = 6.1078 * pow(10, ((7.5*temp_value)/(temp_value + 237.3)));                             //from https://www.omnicalculator.com/physics/air-density#how-to-calculate-the-air-density
    float water_vapor_pressure = (saturation_vapor_pressure * relative_humidity)/100;                                                //multiplication of saturation vapor pressure with the relative humidity, divided by 100%
    float dry_air_pressure = pressure_value - water_vapor_pressure;
    float air_density = ((dry_air_pressure/(dry_air_constant * (temp_value+273.15))) + (water_vapor_pressure / ( water_vapor_constant * (temp_value+273.15))))*100;     //air density formula, can be used for air buoyancy 

    //the mass of water vapor in a unit volume of air. It is a measure of the actual water vapor content of the air.
    double absolute_humidity = (((saturation_vapor_pressure * relative_humidity)/10) / (water_vapor_constant * temp_value)) ;   //expressed in kilograms per cubic meter of moist air - https://planetcalc.com/2167/

    //https://ncalculators.com/meteorology/heat-index-calculator.htm
    float heat_index_part1 = -42.379+(2.04901523*temp_value)+(10.14333127*relative_humidity)-(0.22475541*temp_value*relative_humidity)-(6.83783*pow(10,-3)*pow(temp_value,2))-(5.481717*pow(10,-2)*pow(relative_humidity,2)) ;
    float heat_index_part2 = +(1.22874*pow(10, -3)*pow(temp_value,2)*relative_humidity)+(8.5282*pow(10, -4)*temp_value*pow(relative_humidity,2))-(1.99*pow(10, -6) *pow(temp_value, 2)*pow(relative_humidity, 2));
    int heat_index = heat_index_part1 + heat_index_part2;                                                                                                                                     //felt air temperature, does not take into consideration direct sunlight effect

    //https://www.omnicalculator.com/physics/cloud-base#how-to-use-the-cloud-altitude-calculator
    float cloud_base_altitude_method1 = 125 * (temp_value - air_dew_point_accurate) + altitude;
    float cloud_base_altitude_method2 = ((1.8*( temp_value - air_dew_point_accurate)+64) / 4.4) * 1000 + altitude*3.28084;

    int cloud_base_temperature = ((1.8*temp_value + 32) - 5.4*((cloud_base_altitude_method1 - altitude)*3.28084) - 32) * 5/9;
    
    if (voc_eq <= 5.0) voc_air_quality = "Clean Air! No Volatile Detected!";
    else if (5.0 < voc_eq <= 10.0) voc_air_quality = "Ethane Alkane Detected!";
    else if (10.0 < voc_eq <= 15.0) voc_air_quality = "Isoprene/Methyl/Butadiene/Ethanol Detected!";
    else if (15.0 < voc_eq <= 50.0) voc_air_quality = "Carbon Monoxide Detected!";
    else if (voc_eq > 50.0) voc_air_quality = "Acetone Detected!";

    if (iaq <= 50) iaq_air_quality = "Pure air. No measures needed!";
    else if (50 < iaq <= 100) iaq_air_quality = "Good Air! No irritation or impact on well-being. No health measures needed! ";
    else if (100 < iaq <= 150) iaq_air_quality = "Lightly polluted air! Reduction of well-being possible! Fresh air or ventilation suggested!";
    else if (150 < iaq <= 200) iaq_air_quality = "Moderately polluted. Reduction of well-being possible! Increase ventilation with clean air!";
    else if (200 < iaq <= 250) iaq_air_quality = "Heavily polluted. Continous exposition might lead to migraines! Immediately increase ventilation or leave the environment!";
    else if (250 < iaq <= 350) iaq_air_quality = "Severely polluted. Severe health issues possible! Contamination should be identified! Maximise ventilation or leave environment";
    else if (iaq > 350) iaq_air_quality = "Extremely polluted. Neurotoxic effects possible! Contamination must be identified! Avoid the environment!";

    if (co2_eq <= 400) co2_air_quality = "Normal CO2 concentration in outdoor ambient air!";
    else if (400 < co2_eq <= 600) co2_air_quality = "Excellent air! CO2 concentrations typical of occupied indoor spaces with good air exchange!";
    else if (600 < co2_eq <= 900) co2_air_quality = "Good air! CO2 concentrations typical of occupied indoor spaces with decent air exchange!";
    else if (900 < co2_eq <= 1100) co2_air_quality = "Fair air! CO2 concentrations typical of occupied indoor spaces with no air exchange!";
    else if (1100 < co2_eq <= 1600) co2_air_quality = "Mediocre air! High CO2 concentrations! Increase ventilation or leave environment!";
    else if (1600 < co2_eq <= 2000) co2_air_quality = "Bad air! High CO2 concentrations! Ventilation necessary! Leave environment!";
    else if (2000 < co2_eq <= 5000) co2_air_quality = "Headaches, sleepiness, increased heart rate and slight nausea can be caused. Leave environment immediately!";
    else if (5000 < co2_eq < 40000) co2_air_quality = "Environment exposure may lead to oxygen deprivation resulting in adverse health effects. Leave environment immediately!";
    else if (co2_eq > 40000) co2_air_quality = "Environment exposure may lead to oxygen deprivation resulting in permanent brain damage and coma! Leave environment urgently!";

    if (discomfort_index <= 21) discomfort_condition = "No discomfort!";
    else if (21 < discomfort_index <= 25) discomfort_condition = "Under 50% of the population feels discomfort!";
    else if (24 < discomfort_index <= 27) discomfort_condition = "Over 50% of the population feels discomfort!";
    else if (27 < discomfort_index <= 29) discomfort_condition = "Most of the population suffers discomfort!";
    else if (29 < discomfort_index <= 32) discomfort_condition = "Discomfort Index High! The environment will cause severe stress!";
    else if (discomfort_index > 32) discomfort_condition = "State of medical emergency! Please leave environment!";

    if (pressure_value > 1022.678) pressure_effect = "Caution! High pressure which pushes against the body and limits how much the tissue can expand. Potential of joint paint!";
    else if (pressure_value < 1007) pressure_effect = "Caution! Low pressure allowing the body's tissues to expand, affecting the nerves, which can cause migraines!";
    else if (1007 <= pressure_value <= 1022.678) pressure_effect = "Expected pressure range!";

   if  (27 <= heat_index <= 32) heat_index_condition = "Caution Advised. High heat!";
   else if (33 <= heat_index <= 39) heat_index_condition = "Increased Caution Advised. High heat!";
   else if (40 <= heat_index <= 51) heat_index_condition = "Danger! Only stay outdoor if necessary!";
   else if (heat_index > 52)  heat_index_condition = "Extreme Danger! Do not stay oudoor, migraine and negative skin effects possible!";

    if (0 <= heading <= 11.25) compass_heading = "You are heading NORTH!";
    else if (348.75 < heading <= 360) compass_heading = "You are heading NORTH!";
    else if (78.75 < heading <= 101.25) compass_heading = "You are heading EAST!";
    else if (168.75 < heading <= 191.25) compass_heading = "You are heading SOUTH!";
    else if (258.75 < heading <= 281.25) compass_heading = "You are heading WEST!";

    else if (33.75 < heading <= 56.25) compass_heading = "You are heading NORTH-EAST!"; 
    else if (123.75 < heading <= 146.25) compass_heading = "You are heading SOUTH-EAST!";
    else if (213.75 < heading <= 236.25) compass_heading = "You are heading SOUTH-WEST!";
    else if (303.75 < heading <= 326.25) compass_heading = "You are heading NORTH-WEST!";

    else if (11.25 < heading <= 33.75) compass_heading = "You are heading NORTH-NORTH-EAST!"; 
    else if (56.25 < heading <= 78.75) compass_heading = "You are heading EAST-SOUTH-EAST!";
    else if (101.25 < heading <= 123.75) compass_heading = "You are heading EAST-SOUTH-EAST!";
    else if (146.25 < heading <= 168.75) compass_heading = "You are heading SOUTH-SOUTH-EAST!";
    else if (191.25 < heading <= 213.75) compass_heading = "You are heading SOUTH-SOUTH-WEST!";
    else if (236.25 < heading <= 258.75) compass_heading = "You are heading WEST-SOUTH-WEST!";
    else if (281.25 < heading <= 303.75) compass_heading = "You are heading WEST-NORTH-WEST!";
    else if (326.25 < heading <= 348.75) compass_heading = "You are heading NORTH-NORTH-WEST!";

 
    Serial.print("\r\n");
    Serial.println(String("---------------------------------------------------------------------------------------------------------"));

    Serial.println(String("Temperature is: ") + String(temp_value) + String(" C"));
    Serial.println(String("Compensated Temperature is: ") + String(comp_temp) + String(" C"));
    Serial.println(String("Humidity is: ") + String(hum_value) + String(" %"));
    Serial.println(String("Compensated Humidity is: ") + String(comp_hum) + String(" %"));
    Serial.println(String("Pressure: ") + String(pressure_value) + String(" hPa"));

    Serial.println(String("Altitude: ") + String(altitude) + String(" m"));
    Serial.println(String("Relative Humidity: ") + String(relative_humidity) + String(" %"));
    Serial.println(String("Air Dew Point: ") + String(air_dew_point) + String(" C"));
    Serial.println(String("Air Dew Point Accurate: ") + String(air_dew_point_accurate) + String(" C"));
    Serial.println(String("Saturation Vapor Pressure: ") + String(saturation_vapor_pressure));
    Serial.println(String("Water Vapor Pressure: ") + String(water_vapor_pressure) + String(" hPa"));
    Serial.println(String("Dry Air Pressure ") + String(dry_air_pressure) + String(" hPa"));
    Serial.println(String("Air Density: ") + String(air_density) + String(" kg/m^3"));
    Serial.println(String("Absolute Humidity: ") + String(absolute_humidity, 4) + String(" kg/m^3"));

    Serial.println(String("Cloud Base Altitude Method 1: ") + String(cloud_base_altitude_method1, 4) + String(" m"));
    Serial.println(String("Cloud Base Altitude Method 2: ") + String(cloud_base_altitude_method2, 4) + String(" ft"));   
    Serial.println(String("Cloud Base Temperature: ") + String(cloud_base_temperature));
    
    Serial.println(String("Heat Index: ") + String(heat_index));
    Serial.println(String("Discomfort level: ") + String(discomfort_index));
    Serial.println(discomfort_condition);
    Serial.println(pressure_effect);
  
    Serial.println(String("Gas resistance: ") + String(gas_resistance));
    Serial.println(String("Compensated Gas Resistance is: ") + String(comp_gas_res) + String(" Ohms"));

    Serial.println(String("Temperature Fusion is: ") + String(temp_fusion) + String(" C"));
    Serial.println(String("Humidity Fusion is: ") + String(humidity_fusion) + String(" %"));

    Serial.println(String("Volatile Organic: ") + String(voc_eq) + String(" ppm"));
    Serial.println(voc_air_quality);

    Serial.println(String("Estimation of CO2 levels: ") + String(co2_eq) + String(" ppm"));
    Serial.println(co2_air_quality);

    Serial.println(String("Air Quality Index is: ") + String(iaq));
    Serial.println(String("IAQ Accuracy Level: ") + String(accuracy));
    Serial.println(iaq_air_quality);
    
    Serial.println(String("Tilt info: ") + tilt.toString() + String(" - Binary value:") + tilt.value());
    
    Serial.println(String(compass_heading) + String(" Heading Angle: ") + String(heading));
    Serial.println(String("Compass Roll/Tilt: ") + String(roll, 2));
    Serial.println(String("---------------------------------------------------------------------------------------------------------"));

  }
}

void string_separator(String str, String strs[], int result) {                                       //split the string into substrings, second value
  memset(strs, 0, sizeof(strs));                                                                                  //reset char array to empty values
  StringCount = 0;                                                                                                         //reset string index to 0

  while (str.length() > 6)
  {
    int index = str.indexOf(' ');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index + 1);
    }
  }

  if (result == 1)
  {
    for (int i = 0; i < StringCount; i++)
    {
      Serial.print(String("[") + String(i) + String("] = "));
      Serial.print(strs[i]);
      Serial.print("\r\n");
    }
  }
}
