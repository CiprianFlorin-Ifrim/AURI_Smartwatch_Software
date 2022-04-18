// Arduino sketch
#include <Arduino.h>
#include "Nicla_System.h"
#include "XGBoost.h"

Eloquent::ML::Port::XGBClassifier XGBoost_classifier;

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
    
    float y_pred = XGBoost_classifier.predict(X);
    Serial.println("The weather has been classified as: " + String(y_pred));
    delay(5000);
}
