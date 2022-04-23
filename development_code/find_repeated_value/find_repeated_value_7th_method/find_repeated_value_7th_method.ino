uint8_t dataReceived = { 2, 2, 0 ,4}
uint8_t vals[4] = { 0, 0, 0, 0 };
uint8_t primes[5] = { 2, 3, 5, 7, 11 };

uint8_t values[2] = {5, 5};

void setup() {
  Serial.begin(9600);

}

void loop() {
  unsigned long timeBegin = micros();

  int total = primes[vals[0]]*primes[vals[1]]*primes[vals[2]]*primes[vals[3]];
  for (uint8_t i = 0; i < 4; i++) {
    
    if (!fmod(total, sq(primes[vals[i]])))
      values[i / 2] = vals[i];
  }

  if (values[0] == 5) Serial.println("Everything repeats once. XGBoost has been selected!");
  else {
    Serial.println("The value that repeats the most is: " + String(values[0]));
    if (values[0] != 5 && values[0] != values[1]) Serial.println("There is also chance of: " + String(values[1]));
  }
  
  unsigned long timeEnd = micros();
  unsigned long duration = timeEnd - timeBegin;
  Serial.println("Time to execute: " + String(duration));

  delay(3000);
}
