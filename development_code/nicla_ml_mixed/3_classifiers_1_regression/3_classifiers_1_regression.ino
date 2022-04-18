// Arduino sketch
#include <Arduino.h>
//#include "Nicla_System.h"
#include "XGBoostClassifier.h"
#include "DecisionTreeClassifier.h"
#include "GaussianNaiveBayesClassifier.h"
#include "DecisionTreeRegressor.h"

Eloquent::ML::Port::XGBClassifier XGBoost_classifier;
Eloquent::ML::Port::DecisionTree DecisionTree_classifier;
Eloquent::ML::Port::GaussianNB GaussianNB_classifier;
Eloquent::ML::Port::DecisionTreeRegressor decisiontree_regressor;

void setup() {
  //nicla::begin();
  Serial.begin(9600);
}

void loop() {
    float temp = 11.6;
    float feels_like_temp = 11.3;
    float dew_point = 10.2;
    float relative_humidity = 91.49;
    float sea_level_pressure = 1006.3;
    float X[] = {temp, feels_like_temp, dew_point, relative_humidity, sea_level_pressure};

    int weather_xgb = XGBoost_classifier.predict(X);
    int weather_dtc = DecisionTree_classifier.predict(X);
    int weather_gnb = GaussianNB_classifier.predict(X);
    float rainfall_regression = decisiontree_regressor.predict(X);

    Serial.println("\nThe weather has been identified by the XGBoost Classifier as: " + String(weather_xgb));
    Serial.println("The weather has been identified by the Decision Tree Classifier as: " + String(weather_dtc));
    Serial.println("The weather has been identified by the Gaussian Naive Bayes Classifier as: " + String(weather_gnb));
    Serial.println("The predicted amount of rainfall in millimeters in: " + String(rainfall_regression) + " mm");
    delay(5000);
}
