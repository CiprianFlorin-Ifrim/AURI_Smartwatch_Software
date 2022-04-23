#include "Arduino_BHY2.h"
#include "Nicla_System.h"

SensorBSEC bsec(SENSOR_ID_BSEC);
String bsec_array[4];

void setup() {
  nicla::begin();
  Serial.begin(9600);
  BHY2.begin();
  bsec.begin();

}

void loop() {
  static auto lastCheck = millis();
  BHY2.update();

  // Check sensor values every second
  if (millis() - lastCheck >= 2000) {
    lastCheck = millis();
    

    StringSeparator(bsec.toString(), bsec_array);
    unsigned short iaq = (bsec_array[0]).toInt();
    float voc_eq = ((bsec_array[1]).toFloat())/100;
    unsigned short co2_eq = (bsec_array[2]).toInt();
    uint8_t accuracy = (bsec_array[3]).toInt();

    Serial.println(bsec.toString());
    Serial.println(String("Volatile Organic Gasses: ") + String(voc_eq, 2) + String(" ppm"));
    Serial.println(String("Estimation of CO2 levels: ") + String(co2_eq) + String(" ppm"));
    Serial.println(String("Air Quality Index is: ") + String(iaq));
    Serial.println(String("IAQ Sensor Accuracy Level: ") + String(accuracy));
    Serial.print("\r\n");
    

  }
}


void StringSeparator(String str, String strs[]) {                                       //split the string into substrings, second value
  memset(strs, 0, sizeof(strs));                                                                                  //reset char array to empty values
  uint8_t StringCount = 0;                                                                                                         //reset string index to 0

  while (str.length() > 0) {
    int8_t index = str.indexOf(' ');
    if (index == -1) {                                                                              // No space found
        strs[StringCount++] = str;
        break;
    } else {
        if (isDigit(str.charAt(StringCount+0))) {strs[StringCount++] = str.substring(0, index);}
        str = str.substring(index + 1);
    }
  }
}
