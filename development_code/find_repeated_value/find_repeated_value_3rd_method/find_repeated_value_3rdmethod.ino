void setup() {
  Serial.begin(9600);

  unsigned long timeBegin = micros();
  uint8_t a = 3;
  uint8_t b = 1;
  uint8_t c = 4;
  uint8_t d = 4;


  // 3 or more the same (4 combinations).
  if (a == b && b == c ||
      a == b && b == d ||
      a == c && c == d)
  {
    Serial.print(a);
    Serial.println(" wins");
  }
  else if (b == c && c == d)
  {
    Serial.print(b);
    Serial.println(" wins");
  }
  // 2 the same (6 pairs of numbers to consider ab, ac, ad, bc, bd, cd... need to check the opposite pair in each case)
  else if ( a == b)
  {
    if (c == d)
    {
      Serial.print(a);
      Serial.print(" ties with ");
      Serial.print(c);
    }
    else
    {
      Serial.print(a);
      Serial.println(" wins");
    }
  }
  else if (a == c)
  {
    if (b == d)
    {
      Serial.print(a);
      Serial.print(" ties with ");
      Serial.print(b);
    }
    else
    {
      Serial.print(a);
      Serial.println(" wins");
    }
  }
  else if (a == d)
  {
    if (b == c)
    {
      Serial.print(a);
      Serial.print(" ties with ");
      Serial.print(b);
    }
    else
    {
      Serial.print(a);
      Serial.println(" wins");
    }
  }
  else if (b == c)
  {
    Serial.print(b);
    Serial.println(" wins");
  }
  else if (b == d)
  {
    Serial.print(b);
    Serial.println(" wins");
  }
  else if (c == d)
  {
    Serial.print(c);
    Serial.println(" wins");
  }
  else
  {
    Serial.println("no winner");
  }

  unsigned long timeEnd = micros();
  unsigned long duration = timeEnd - timeBegin;
  Serial.println("Time to execute: " + String(duration));
}

void loop()
{
}
