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
  static auto lastCheck = millis();
  BHY2.update();

  if (millis() - lastCheck >= 1000) {
    lastCheck = millis();
    
    Serial.println(orientation.toString());
    
    string_separator(orientation.toString(), compass_array, 0);
    int heading = round((compass_array[4]).toFloat());
    int roll = round((compass_array[12]).toFloat());

    Serial.println(heading);
    Serial.println(roll);
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
