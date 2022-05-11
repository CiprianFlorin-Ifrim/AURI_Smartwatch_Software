
#define heartPin A0
#include "Nicla_System.h"
#include <Ewma.h> 

Ewma heart_rate(0.03); 

void setup() {
  nicla::begin();
  Serial.begin(115200);
}

void loop() {
  uint16_t heart_value = 0;
  uint16_t last_beat = 0;
  
  for (uint8_t i = 0; i < 8;) {
    heart_value = analogRead(heartPin);
    if (heart_value > last_beat) {
      last_beat = heart_value;
      i++;
    }
  }

  uint8_t bpm_filtered = 0;
  for (uint8_t i = 0; i < 10; i++) {
    heart_value = analogRead(heartPin);
    bpm_filtered = (heart_rate.filter(heart_value))/10;
  }
  
  Serial.println(bpm_filtered + 15);
  delay(2000);
}     
