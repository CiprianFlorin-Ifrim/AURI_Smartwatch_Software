#include "Arduino_BHY2.h"
#include "Nicla_System.h"

SensorOrientation orientation(SENSOR_ID_ORI);

String compass_array[13];
int StringCount = 0;

String compass_heading = "";


void setup() {
  nicla::begin();
  Serial.begin(115200);
  BHY2.begin();
  orientation.begin();
}

void loop() {
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      BHY2.update();
      Serial.println(orientation.toString());

      string_separator(orientation.toString(), compass_array, 0);
      float heading = (compass_array[4]).toFloat();
      float roll = (compass_array[12]).toFloat();
      compass_heading = "";

      Serial.println(heading);
      Serial.println(roll);

      if (0 <= heading && heading <= 11.25) compass_heading = "You are heading NORTH!";
      else if (348.75 < heading && heading <= 360) compass_heading = "You are heading NORTH!";
      else if (78.75 < heading && heading <= 101.25) compass_heading = "You are heading EAST!";
      else if (168.75 < heading && heading <= 191.25) compass_heading = "You are heading SOUTH!";
      else if (258.75 < heading && heading <= 281.25) compass_heading = "You are heading WEST!";

      else if (33.75 < heading && heading <= 56.25) compass_heading = "You are heading NORTH-EAST!";
      else if (123.75 < heading && heading <= 146.25) compass_heading = "You are heading SOUTH-EAST!";
      else if (213.75 < heading && heading <= 236.25) compass_heading = "You are heading SOUTH-WEST!";
      else if (303.75 < heading && heading <= 326.25) compass_heading = "You are heading NORTH-WEST!";

      else if (11.25 < heading && heading <= 33.75) compass_heading = "You are heading NORTH-NORTH-EAST!";
      else if (56.25 < heading && heading <= 78.75) compass_heading = "You are heading EAST-SOUTH-EAST!";
      else if (101.25 < heading && heading <= 123.75) compass_heading = "You are heading EAST-SOUTH-EAST!";
      else if (146.25 < heading && heading <= 168.75) compass_heading = "You are heading SOUTH-SOUTH-EAST!";
      else if (191.25 < heading && heading <= 213.75) compass_heading = "You are heading SOUTH-SOUTH-WEST!";
      else if (236.25 < heading && heading <= 258.75) compass_heading = "You are heading WEST-SOUTH-WEST!";
      else if (281.25 < heading && heading <= 303.75) compass_heading = "You are heading WEST-NORTH-WEST!";
      else if (326.25 < heading && heading <= 348.75) compass_heading = "You are heading NORTH-NORTH-WEST!";

      Serial.println(compass_heading);

    }

  }
}

void string_separator(String str, String strs[], int result) {       //split the string into substrings, second value
  memset(strs, 0, sizeof(strs));                      //reset char array to empty values
  StringCount = 0;                                    //reset string index to 0

  while (str.length() > 6)
  {
    int index = str.indexOf(' ');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index + 1);
    }
  }

  if (result == 1)
  {
    for (int i = 0; i < StringCount; i++)
    {
      Serial.print(String("[") + String(i) + String("] = "));
      Serial.print(strs[i]);
      Serial.print("\r\n");
    }
  }
}
