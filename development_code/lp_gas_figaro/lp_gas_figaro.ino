#define gas A0

void setup() {
  Serial.begin(115200);
  pinMode(gas, INPUT);
}

void loop() {
  float value = analogRead(gas);
  Serial.println("Gas value: " + String(value));

  delay(1000);

}
