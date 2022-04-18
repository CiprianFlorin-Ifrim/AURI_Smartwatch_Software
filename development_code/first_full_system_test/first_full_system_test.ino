// Include Arduino Wire library for I2C
#include <Arduino.h>
#include "Nicla_System.h"
#include <Wire.h>
#include <DFPlayerMini_Fast.h>

//Sensor tilt(SENSOR_ID_WRIST_TILT_GESTURE);
int command = 10;
DFPlayerMini_Fast myMP3;

void setup()
{
  nicla::begin();
  Serial.begin(9600);
  Serial1.begin(9600);
  myMP3.begin(Serial1, true);

  Wire.begin(); // join i2c bus (address optional for master)
  delay(1000);
  
  myMP3.volume(30);
}

void loop() {
  
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      Serial.println("Command received");
      
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write(5);              // sends one byte
      Wire.endTransmission();    // stop transmitting

      delay(3000);
      Wire.requestFrom(8, 1);    // request 1 byte from slave device #8
      while (Wire.available()) { // slave may send less than requested
        command = Wire.read(); // receive a byte as character
      }

      switch (command) {
        case 0:
          Serial.println("You've asked for the BATTERY LEVEL");
          break;
        case 1:
          Serial.println("You've asked for the CLOUDS INFO");
          break;
        case 2:
          Serial.println("You've asked for the AURI DESCRIPTION");
          myMP3.playFolder(1, 10);
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
          Serial.println("You've asked for the WEATHER");
          break;
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
