void setup() {
  Serial.begin(9600);

}

void loop() {
  unsigned long timeBegin = micros();

  uint8_t check[5] = {2 * 2, 3 * 3, 5 * 5, 7 * 7, 11 * 11}; ////x*x is faster than pow(x,2), also requires less flash
  uint8_t labels[4] = {11, 11, 11, 7};

  byte repeat = 0;
  uint8_t product = labels[0] * labels[1] * labels[2] * labels[3];
  for (uint8_t i = 0; i < 5; i++)
  {
    if ((total % check[i]) == 0)
    {
      if (repeat == 0) Serial.println(String(i) + " is the chosen label");
      else if (repeat == 1) Serial.println("There is also the chance of: " + String(i));
      repeat++;
    }
  }
  if (repeat == 0) Serial.println("All labels repeated once. XGBoost has been selected!");

  unsigned long timeEnd = micros();
  unsigned long duration = timeEnd - timeBegin;
  Serial.println("Time to execute: " + String(duration));

  delay(2000);
}
