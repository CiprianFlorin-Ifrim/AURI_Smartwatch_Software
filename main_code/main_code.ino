//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------LIBRARIES--------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Arduino_BHY2.h"
#include "Nicla_System.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------DECLARATIONS------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
Sensor gas(SENSOR_ID_GAS);
SensorBSEC bsec(SENSOR_ID_BSEC);
SensorXYZ magnetometer(SENSOR_ID_MAG);
SensorOrientation orientation(SENSOR_ID_ORI);
Sensor tilt(SENSOR_ID_WRIST_TILT_GESTURE);

String voc_air_quality = "";
String iaq_air_quality = "";
String co2_air_quality = "";
String discomfort_condition = "";
String pressure_effect ="";
float sea_level_pressure = 1013.25;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------SYSTEM SETUP------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------MAIN CODE-------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
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

    float temp_fusion = (temp_value+comp_temp)/2;
    float humidity_fusion = (hum_value+comp_hum)/2;

    int altitude = ((pow((sea_level_pressure/pressure_value), (1/5.257)) -1) * (temp_value + 273.15))/0.0065;           //altitude hypsometric formula
    int discomfort_index = temp_value - 0.55 * (1 -0.01 * hum_value) * (temp_value - 14.5);                             //discomfort index formula

    if (voc_eq <= 5.0) voc_air_quality = "Clean Air! No Volatile voc_air_qualitys Detected!";
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

    Serial.println(String("Temperature is: ") + String(temp_value) + String(" C"));
    Serial.println(String("Compensated Temperature is: ") + String(comp_temp) + String(" C"));
    Serial.println(String("Humidity is: ") + String(hum_value) + String(" %"));
    Serial.println(String("Compensated Humidity is: ") + String(comp_hum) + String(" %"));
    Serial.println(String("Pressure: ") + String(pressure_value) + String(" hPa"));

    Serial.println(pressure_effect);
    Serial.println(String("Altitude: ") + String(altitude) + String(" m"));
    Serial.println(String("Discomfort level: ") + String(discomfort_index));

    
    Serial.println(String("Gas resistance: ") + String(gas_resistance));
    Serial.println(String("Compensated Gas Resistance is: ") + String(comp_gas_res) + String(" Ohms"));

    Serial.println(String("Temperature Fusion is: ") + String(temp_fusion) + String(" C"));
    Serial.println(String("Humidity Fusion is: ") + String(humidity_fusion) + String(" %"));


    Serial.println(String("Volatile Organic voc_air_qualitys: ") + String(voc_eq) + String(" ppm"));
    Serial.println(voc_air_quality);

    Serial.println(String("Estimation of CO2 levels: ") + String(co2_eq) + String(" ppm"));
    Serial.println(co2_air_quality);

    Serial.println(String("Air Quality Index is: ") + String(iaq));
    Serial.println(String("IAQ Accuracy Level: ") + String(accuracy));
    Serial.println(iaq_air_quality);
    
    Serial.println(String("Tilt info: ") + tilt.toString() + String(" - Binary value:") + tilt.value());

    Serial.println(orientation.toString());
    Serial.println((orientation.toString()).substring(62, 70));
  }
}
