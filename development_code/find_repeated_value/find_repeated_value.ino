void setup() {
  Serial.begin(250000);

  byte numbers[4] = {1,0,4,0};
  byte temp;

  // Sort the first pair
  if (numbers[0] > numbers[1]) {
    temp = numbers[0];
    numbers[0] = numbers[1];
    numbers[1] = temp;
  }
  // Sort the second pair
  if (numbers[2] > numbers[3]) {
    temp = numbers[2];
    numbers[2] = numbers[3];
    numbers[3] = temp;
  }
  // Set the minimum value
  if (numbers[0] > numbers[2]) {
    temp = numbers[0];
    numbers[0] = numbers[2];
    numbers[2] = temp;
  }
  // Set the maximum value
  if (numbers[1] > numbers[3]) {
    temp = numbers[1];
    numbers[1] = numbers[3];
    numbers[3] = temp;
  }
  // Ensure the middle two are in the correct order
  if (numbers[1] > numbers[2]) {
    temp = numbers[1];
    numbers[1] = numbers[2];
    numbers[2] = temp;
  }

  // numbers is sorted from minimum to maximum
  if (numbers[1] == numbers[2]) {
    // There are three possibilities...
    // a b b c
    // b b b c
    // a b b b
    // We don't care which one.  In all three cases b is the winner.  It's just a question of how much
    // of a winner.
    Serial.print(numbers[1]);
    Serial.println(F(" is the winner"));
  }
  else {
    // There are four possibilities...
    // a b c d
    // b b c d
    // a b c c
    // b b c c
    if (numbers[0] == numbers[1]) {
      // Two left...
      // b b c d
      // b b c c
      if (numbers[2] == numbers[3]) {
        // There can be only one: b b c c
        Serial.print(numbers[1]);
        Serial.println(F(" and "));
        Serial.print(numbers[2]);
        Serial.println(F(" are tied"));
      }
      else {
        // There can be only one: b b c d
        Serial.print(numbers[0]);
        Serial.println(F(" is the winner"));
      }
    }
    else {
      // Two left...
      // a b c d
      // a b c c
      if (numbers[2] == numbers[3]) {
        // There can be only one: a b c c
        Serial.print(numbers[2]);
        Serial.println(F(" is the winner"));
      }
      else {
        // There can be only one: a b c d
        Serial.println(F("No winners"));
      }
    }
  }
}

void loop() {
}
