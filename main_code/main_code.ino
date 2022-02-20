//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------LIBRARIES--------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Arduino.h"
#include "Arduino_BHY2.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------DECLARATIONS------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
Sensor humidity(SENSOR_ID_HUM);
Sensor temperature(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
Sensor gas(SENSOR_ID_GAS);
SensorBSEC bsec(SENSOR_ID_BSEC);;
SensorXYZ magnetometer(SENSOR_ID_MAG);
SensorXYZ gravity_vector(SENSOR_ID_GRA);
SensorOrientation orientation(SENSOR_ID_ORI);
SensorOrientation device_orientation(SENSOR_ID_DEVICE_ORI);
Sensor tilt(SENSOR_ID_WRIST_TILT_GESTURE);

String compound = "";
String air_quality = "";
float sea_level_pressure = 1013.25;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------SYSTEM SETUP------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup(){
  Serial.begin(115200);
  BHY2.begin();
  temperature.begin();
  humidity.begin();
  pressure.begin();
  gas.begin();
  bsec.begin();
  magnetometer.begin();
  gravity_vector.begin();
  orientation.begin();
  tilt.begin();
  device_orientation.begin();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------MAIN CODE-------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop(){
  static auto lastCheck= millis();
  BHY2.update();

  // Check sensor values every second  
  if (millis() - lastCheck >= 1000) {
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
    int altitude = ((pow((sea_level_pressure/pressure_value), (1/5.257)) -1) * (temp_fusion + 273.15))/0.0065;           //altitude hypsometric formula
    
    if (voc_eq < 5.0) compound = "Clean Air! No Volatile Compounds Detected!";
    else if (5.0 < voc_eq < 10.0) compound = "Ethane Alkane Detected!";
    else if (10.0 < voc_eq < 15.0) compound = "Isoprene/Methyl/Butadiene/Ethanol Detected!";
    else if (15.0 < voc_eq < 40.0) compound = "Carbon Monoxide Detected!";
    else if (voc_eq > 50.0) compound = "Acetone Detected!";

    if (iaq < 50) air_quality = "Pure air. No mmeasures needed!";
    else if (50 < iaq < 100) air_quality = "Good Air! No irritation or impact on well-being. No health measures needed! ";
    else if (100 < iaq < 150) air_quality = "Lightly polluted air! Reduction of well-being possible! Fresh air or ventilation suggested!";
    else if (150 < iaq < 200) air_quality = "Moderately polluted. Reduction of well-being possible! Increase ventilation with clean air!";
    else if (200 < iaq < 250) air_quality = "Heavily polluted. Continous exposition might lead to migraines! Immediately increase ventilation or leave the environment!";
    else if (250 < iaq < 350) air_quality = "Severely polluted. Severe health issues possible! Contamination should be identified! Maximise ventilation or leave environment";
    else if (iaq > 350) air_quality = "Extremely polluted. Neurotoxic effects possible! Contamination must be identified! Avoid the environment!";

    Serial.println(String("Temperature is: ") + String(temp_value) + String(" C"));
    Serial.println(String("Compensated Temperature is: ") + String(comp_temp) + String(" C"));
    Serial.println(String("Humidity is: ") + String(hum_value) + String(" %"));
    Serial.println(String("Compensated Humidity is: ") + String(comp_hum) + String(" %"));
    Serial.println(String("Pressure: ") + String(pressure_value) + String(" hPa"));
    Serial.println(String("Altitude: ") + String(altitude) + String(" m"));
    
    Serial.println(String("Gas resistance: ") + String(gas_resistance));
    Serial.println(String("Compensated Gas Resistance is: ") + String(comp_gas_res) + String(" Ohms"));

    Serial.println(String("Temperature Fusion is: ") + String(temp_fusion) + String(" C"));
    Serial.println(String("Humidity Fusion is: ") + String(humidity_fusion) + String(" %"));

    Serial.println(String("Air Quality Index is: ") + String(iaq));
    Serial.println(String("Volatile Organic Compounds: ") + String(voc_eq) + String(" ppm"));
    Serial.println(compound);
    Serial.println(String("Estimation of CO2 levels: ") + String(co2_eq) + String(" ppm"));
    Serial.println(String("IAQ Accuracy Level: ") + String(accuracy));
    Serial.println(air_quality);
    
    Serial.println(String("Magnetometer info: ") + magnetometer.toString());
    Serial.println(String("Gravity Vector info: ") + gravity_vector.toString());
    Serial.println(String("Tilt info: ") + tilt.toString() + String(" - Binary value:") + tilt.value());
    Serial.println(String(orientation.toString()));

  }
}
