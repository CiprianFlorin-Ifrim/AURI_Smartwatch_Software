int count[5];
int bestLabel = -1;
int maxCount = 0;
boolean multiple;

void setup() {
  Serial.begin(9600);

}

void loop() {
  unsigned long timeBegin = micros();

  int labels[4] = {0, 1, 1, 0};
  for (int x = 0; x < 4; x++)
  {
    count[labels[x]]++;                                                 // Keep track of the number of occurences of this value
    if (count[labels[x]] == maxCount) {
      multiple = true; // Duplicate maximum ?
    }
    if (count[labels[x]] > maxCount)                                    // New maximum ?
    {
      bestLabel = labels[x];
      maxCount = count[labels[x]];
      multiple = false;
    }
  }

  if (multiple)
  {
    if (maxCount == 1) Serial.println("The weather has been classified as: " + String(labels[1]));
    else
    {
      Serial.println("The weather has been identified as: " + String(labels[1]));
      if (labels[0] != labels[1]) Serial.println("With chances of: " + String(labels[0]));
      else Serial.println("With chances of: " + String(labels[2]));
    }
  }
  else Serial.println("The weather has been classified as: " + String(bestLabel));

  unsigned long timeEnd = micros();
  unsigned long duration = timeEnd - timeBegin;
  Serial.println("Time to execute: " + String(duration));

  delay(2000);
}
