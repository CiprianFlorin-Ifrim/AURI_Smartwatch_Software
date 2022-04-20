int numbers[4] = {1, 0, 4, 0};
int count[5];

int maxValue = -1;
int maxCount = 0;
boolean multiple;

void setup ()
{
  Serial.begin(9600);

  unsigned long timeBegin = micros();
  for (int x = 0; x < 4; x++)
  {
    // Keep track of the number of occurences of this value.
    count[numbers[x]]++;

    // Duplicate maximum ?
    if (count[numbers[x]] == maxCount)
    {
      multiple = true;
    }

    // New maximum ?
    if (count[numbers[x]] > maxCount)
    {
      maxValue = numbers[x];
      maxCount = count[numbers[x]];
      multiple = false;
    }
  }

  if (multiple)
  {
    if (maxCount == 1)
      Serial.println("4 values have same count of 1");
    else
      Serial.println("2 values have same count of 2");
  }
  else
  {
    Serial.print(maxValue);
    Serial.print(" occurs ");
    Serial.print(maxCount);
    Serial.println(" times");
  }

  unsigned long timeEnd = micros();
  unsigned long duration = timeEnd - timeBegin;
  Serial.println("Time to execute: " + String(duration));
}

void loop()
{

}
