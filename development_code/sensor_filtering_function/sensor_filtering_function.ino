void UpdateMainSensors() {
  float temperature_BMP390_readings, temperature_BME688_readings, pressure_BMP390_readings;        //declare and initialize variable for the sum of 10 readings
  uint16_t humidity_BME688_readings;                                                               //16bits are required to hold 10 values that can reach 10000
  uint32_t gas_resistance_BME688_readings;

  for (uint8_t i = 0; i < 10; i++) {
    BHY2.update();                                                                            //necessary to update sensor classes
    temperature_BMP390_readings    += temperature.value();                                    //sum all the values from the array to create the threshold value
    temperature_BME688_readings    += bsec.comp_t();
    pressure_BMP390_readings       += pressure.value();
    humidity_BME688_readings       += bsec.comp_h();
    gas_resistance_BME688_readings += bsec.comp_g();
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

  uint32_t gas_BME688_outlier_higher_bound = 1.20 * (gas_resistance_BME688_readings / 10);     //120% of the average of the last 10 values
  uint32_t gas_BME688_outlier_lower_bound  = 0.80 * (gas_resistance_BME688_readings / 10);      //80% of the average of the last 10 values 

  for (uint8_t t_BMP390, t_BMP688, p_BMP390, h_BME688, g_BME688; t_BMP390 < 10 && t_BMP688 < 10 && p_BMP390 < 10 && h_BME688 < 10 && g_BME688 < 10;) {
  }
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
    } if (bsec.comp_g() >= gas_BME688_outlier_lower_bound && bsec.comp_g() <= gas_BME688_outlier_higher_bound && g_BME688 < 10) {
        gas_resistance_BME688_filtered = gas_resistance_BME688_ewma.filter(bsec.comp_g());
        g_BME688++;
    delay(100);
  }
}
