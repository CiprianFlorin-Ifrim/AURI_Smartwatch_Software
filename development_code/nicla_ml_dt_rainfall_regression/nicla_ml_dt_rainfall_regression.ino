// Arduino sketch
#include <Arduino.h>
#include "Nicla_System.h"
#include "DecisionTreeRegressor.h"

Eloquent::ML::Port::DecisionTreeRegressor decisiontree_regressor;

void setup() {
  nicla::begin();
  Serial.begin(9600);
}

void loop() {
    float temp = 5.4;
    float feels_like_temp = 4;
    float dew_point = 3.9;
    float relative_humidity = 90.37;
    float sea_level_pressure = 1029.3;
    float X[] = {temp, feels_like_temp, dew_point, relative_humidity, sea_level_pressure};
    
    float y_pred = decisiontree_regressor.predict(X);
    Serial.println("The predicted amount of rainfall in millimeters in: " + String(y_pred) + " mm");
    delay(5000);
}
