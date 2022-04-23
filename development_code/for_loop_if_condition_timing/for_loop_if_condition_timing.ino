uint8_t vals[4] = { 0, 0, 0, 0 };
uint8_t primes[6] = { 2, 3, 5, 7, 11, 13 };

void setup() {
  Serial.begin(9600);

}

void loop() {
  unsigned long timeBegin = micros();
  function();
  unsigned long timeEnd = micros();
  unsigned long duration = timeEnd - timeBegin;
  Serial.println("Time to execute: " + String(duration, 5));

  delay(5000);

}

void function() {
  for (uint8_t i = 0; i < 4; i++) {
    vals[i] = primes[i];
  }
}
