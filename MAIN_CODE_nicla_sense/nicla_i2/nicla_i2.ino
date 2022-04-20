// Include Arduino Wire library for I2C
#include <Arduino.h>
#include "Nicla_System.h"
#include <Wire.h>
#include <DFPlayerMini_Fast.h>
#include <I2C_Anything.h>

#define slaveAddress 0x08

//Sensor tilt(SENSOR_ID_WRIST_TILT_GESTURE);
float dataReceived[6];
DFPlayerMini_Fast myMP3;

void setup() {
  nicla::begin();
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);

  Serial1.begin(9600);
  myMP3.begin(Serial1, true, false);
  myMP3.volume(30);
  delay(1000);
}

void loop() {

  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      float temp = 5.8;
      float feels_like_temp = 1.6;
      float dew_point = 2.4;
      float relative_humidity = 79.15;
      float sea_level_pressure = 1000.4;
      float uv_index = 3;
      float X[6] = {temp, feels_like_temp, dew_point, relative_humidity, sea_level_pressure, uv_index};

      Serial.println("Command received");

      Wire.beginTransmission(slaveAddress); // transmit to device #8
      for (uint8_t i = 0; i < sizeof(X); i++) I2C_writeAnything(X[i]);
      Wire.endTransmission();    // stop transmitting

      delay(3000);
      Wire.requestFrom(8, sizeof(dataReceived));
      while (Wire.available()) {                                // slave may send less than requested
        for (uint8_t j = 0; j < sizeof(dataReceived); j++) I2C_readAnything(dataReceived[j]);
      }

      switch (int(dataReceived[5])) {
        case 0:
          Serial.println("You've asked for the BATTERY LEVEL");
          break;
        case 1:
          Serial.println("You've asked for the CLOUDS INFO");
          break;
        case 2:
          Serial.println("You've asked for the AURI DESCRIPTION");
          break;
        case 3:
          Serial.println("You've asked for the ENVIRONMENT CONDITIONS");
          break;
        case 4:
          Serial.println("You've asked for the HEALTH REPORT");
          break;
        case 5:
          Serial.println("You've asked for the LOCATION DETAILS");
          break;
        case 6:
          Serial.println("You've asked for the STEPS COUNTER");
          break;
        case 7:
          Serial.println("You've asked for the THANKS OUTPUT");
          break;
        case 8:
          Serial.println("You've asked for the TIME AND DATE");
          break;
        case 9:
          {
            Serial.println("You've asked for the WEATHER");
            uint8_t allLabels[4], bestLabels[2] = {5, 5}, primes[5] = {2, 3, 5, 7, 11};
            int product = 1;
            
            for (uint8_t l = 0; l < 4; l++) {
              allLabels[l] = dataReceived[l];
              product *= primes[allLabels[l]];
            }
            for (uint8_t k = 0; k < 4; k++) if (!fmod(product, sq(primes[allLabels[k]]))) bestLabels[k / 2] = allLabels[k];

            if (bestLabels[0] == 5) Serial.println("Everything repeats once. XGBoost has been selected!" + String(allLabels[1]));
            else {
              Serial.println("The value that repeats the most is: " + String(bestLabels[0]));
              if (bestLabels[0] != 5 && bestLabels[0] != bestLabels[1]) Serial.println("There is also chance of: " + String(bestLabels[1]));
            }
            Serial.println("The precipitation amount is equal to: " + String(dataReceived[4]) + " mm!");
            break;
          }
        case 10:
          Serial.println("VOICE VOLUME TOO LOW OR TOO MUCH NOISE. PLEASE REPEAT THE COMMAND");
          break;
        case 11:
          Serial.println("SOFTWARE ISSUE DETECTED. FAILED TO SETUP AUDIO SAMPLING");
          break;
        case 12:
          Serial.println("SOFTWARE OR HARDWARE ISSUE DETECTED. FAILED TO RECORD AUDIO");
          break;
        case 13:
          Serial.println("DEEP LEARNING ISSUE DETECTED. FAILED TO RUN THE CLASSIFIER");
          break;
      }
    }
    delay(100);
  }
}

void playVoice(int folder, int file) {
  myMP3.playFolder(folder, file);
  delay(150);
  while (!digitalRead(GPIO3));
}
