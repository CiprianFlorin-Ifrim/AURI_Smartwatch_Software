// Arduino sketch
#include <Arduino.h>
//#include "Nicla_System.h"
#include "SVMClassifier.h"
#include "XGBoostClassifier.h"
#include "DecisionTreeClassifier.h"
#include "GaussianNaiveBayesClassifier.h"
#include "DecisionTreeRegressor.h"

Eloquent::ML::Port::SVM SVM_classifier;
Eloquent::ML::Port::XGBClassifier XGBoost_classifier;
Eloquent::ML::Port::DecisionTree DecisionTree_classifier;
Eloquent::ML::Port::GaussianNB GaussianNB_classifier;
Eloquent::ML::Port::DecisionTreeRegressor decisiontree_regressor;

void setup() {
  //nicla::begin();
  Serial.begin(9600);
}

void loop() {
    float temp = 24.5;
    float feels_like_temp = 24.3;
    float dew_point = 14.6;
    float relative_humidity = 57.27;
    float sea_level_pressure = 1014.2;
    float uv_index = 7;
    float X[] = {temp, feels_like_temp, dew_point, relative_humidity, sea_level_pressure};
    float X_SVM[] = {temp, feels_like_temp, dew_point, relative_humidity, sea_level_pressure, uv_index};
    
    int weather_svm = SVM_classifier.predict(X);
    int weather_xgb = XGBoost_classifier.predict(X);
    int weather_dtc = DecisionTree_classifier.predict(X);
    int weather_gnb = GaussianNB_classifier.predict(X);
    
    float rainfall_regression = decisiontree_regressor.predict(X);

    Serial.println("\nThe weather has been identified by the Support Vector Machines Classifier as: " + String(weather_svm));
    Serial.println("The weather has been identified by the XGBoost Classifier as: " + String(weather_xgb));
    Serial.println("The weather has been identified by the Decision Tree Classifier as: " + String(weather_dtc));
    Serial.println("The weather has been identified by the Gaussian Naive Bayes Classifier as: " + String(weather_gnb));
    Serial.println("The predicted amount of rainfall in millimeters in: " + String(rainfall_regression) + " mm");
    delay(5000);
}
