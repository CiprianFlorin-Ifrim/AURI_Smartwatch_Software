void setup()
{
  Serial.begin(115200);
  int anInt1 = 250;
  int anInt2 = 275;
  Serial.println(anInt1 % 10);
  Serial.println(anInt2 % 10);

  float temp = 26.5;
  Serial.println(int(temp));
}

void loop()
{
}
