#include "Arduino_BHY2.h"
#include "Nicla_System.h"

SensorBSEC bsec(SENSOR_ID_BSEC);
int StringCount;
String bsec_array[20];

void setup() {
  nicla::begin();
  Serial.begin(115200);
  BHY2.begin();
  bsec.begin();

}

void loop() {
  static auto lastCheck = millis();
  BHY2.update();

  // Check sensor values every second
  if (millis() - lastCheck >= 2000) {
    lastCheck = millis();
    Serial.println(bsec.toString());
    Serial.print("\r\n");
    string_separator(bsec.toString(), bsec_array, true);
    
  }
}



void string_separator(String str, String strs[], bool result) {                                       //split the string into substrings, second value
  //memset(strs, 0, sizeof(strs));                                                                                  //reset char array to empty values
  StringCount = 0;                                                                                                         //reset string index to 0

  while (str.length() > 0)
  {
    int index = str.indexOf(' ');
    if (index == -1) // No space found
    {
      Serial.println("If Statement " + String(index) + "  " + String(StringCount));
      strs[StringCount++] = str;
      break;
    }
    else
    {
      Serial.println("Else Statement " + String(index) + "  " + String(StringCount));
      if (isDigit(str.charAt(StringCount+0))) {strs[StringCount++] = str.substring(0, index);}
      str = str.substring(index + 1);
    }
  }

  if (result == true)
  {
    for (int i = 0; i < StringCount; i++)
    {
      Serial.print(String("[") + String(i) + String("] = "));
      Serial.print(strs[i]);
      Serial.print("\r\n");
    }
  }
}
