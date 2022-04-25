#include "Nicla_System.h"
#include "Arduino_BHY2.h"

SensorBSEC        bsec(SENSOR_ID_BSEC);
SensorOrientation orientation(SENSOR_ID_ORI);

void setup() {
  nicla::begin();
  BHY2.begin();
  bsec.begin();
  orientation.begin();

  Serial.begin(9600);
  
  BHY2.update();
  delay(5000);
}

void loop() {
  UpdateNiclaSensors();
  float heading = orientation.heading();
  float pitch    = orientation.pitch();
  float roll   = orientation.roll();

  uint8_t  sensor_accuracy = bsec.accuracy();
  uint16_t air_quality     = bsec.iaq();
  uint32_t carbon_dioxide  = bsec.co2_eq();
  float    volatile_mix    = bsec.b_voc_eq();

  Serial.println("\n-----------------------------------------------------");
  Serial.println("Orientation is: " + String(heading));
  Serial.println("Roll is: " + String(pitch));
  Serial.println("Pitch is: " + String(roll));
  Serial.println();
  Serial.println("Sensor Accuracy is: " + String(sensor_accuracy));
  Serial.println("Air Quality is: " + String(air_quality));
  Serial.println("Carbon Dioxide is: " + String(carbon_dioxide));
  Serial.println("Pitch is: " + String(volatile_mix));
  Serial.println("-----------------------------------------------------");
  
  delay(4000);
}

void UpdateNiclaSensors() {
  for (uint8_t i = 0; i < 5; i++) {
    BHY2.update();                                                                //necessary to update sensor
    delay(10);
  }
}
