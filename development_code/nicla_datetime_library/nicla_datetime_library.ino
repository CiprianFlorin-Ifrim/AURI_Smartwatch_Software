#include <TimeLib.h>
#include <Arduino.h>
#include "Nicla_System.h"

// single character message tags
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7     // ASCII bell character requests a time sync message 

static boolean isLongFormat = true;

void setup()  {
  nicla::begin();
  Serial.begin(9600);
  setSyncProvider(requestSync);  //set function to call when sync required
}

void loop() {
  if (Serial.available() > 1) { // wait for at least two characters
    char c = Serial.read();
    if ( c == TIME_HEADER) {
      processSyncMessage();
    }
  }
    if (timeStatus() != timeNotSet) {
      Serial.println(String(hour()) + " " + String(minute()));
      Serial.println(String(day()) + " " + String(month()) + " " + String(year()));
    }
  delay(1000);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  pctime = Serial.parseInt();
  if ( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
    setTime(pctime); // Sync Arduino clock to the time received on the serial port
  }
}

time_t requestSync() {
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}
