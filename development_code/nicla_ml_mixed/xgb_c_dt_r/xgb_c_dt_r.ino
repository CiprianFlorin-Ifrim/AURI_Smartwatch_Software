// Arduino sketch
#include <Arduino.h>
//#include "Nicla_System.h"
#include "XGBoost.h"
#include "DecisionTreeRegressor.h"

Eloquent::ML::Port::DecisionTreeRegressor decisiontree_regressor;
Eloquent::ML::Port::XGBClassifier XGBoost_classifier;

void setup() {
  //nicla::begin();
  Serial.begin(9600);
}

void loop() {
    float temp = 5.4;
    float feels_like_temp = 4;
    float dew_point = 3.9;
    float relative_humidity = 90.37;
    float sea_level_pressure = 1029.3;
    float X[] = {temp, feels_like_temp, dew_point, relative_humidity, sea_level_pressure};

    float weather_classification = XGBoost_classifier.predict(X);
    float rainfall_regression = decisiontree_regressor.predict(X);

    Serial.println("The weather has been classified as: " + String(weather_classification));
    Serial.println("The predicted amount of rainfall in millimeters in: " + String(rainfall_regression) + " mm");
    delay(5000);
}
