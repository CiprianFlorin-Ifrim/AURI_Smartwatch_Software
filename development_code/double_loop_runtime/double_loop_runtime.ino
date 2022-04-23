void setup() {
  Serial.begin(9600);

}

void loop() {
  unsigned long timeBegin = micros();

  uint8_t labels[4] = {2, 0, 0, 3};
  uint8_t oldLabels[5] = {0, 1, 2, 3, 4};
  uint8_t newLabels[5] = {2, 3, 5, 7, 11};

  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 5; j++) {
      if (labels[i] == oldLabels[j]) {
        labels[i] = newLabels[j];
        break;
      }  
    }
  }

  unsigned long timeEnd = micros();
  unsigned long duration = timeEnd - timeBegin;
  Serial.println("Time to execute: " + String(duration));

  for (uint8_t k = 0; k < 4; k++) {
    Serial.println(labels[k]);
  }

  delay(2000);

}
