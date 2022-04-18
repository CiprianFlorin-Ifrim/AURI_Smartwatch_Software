#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SI1145.h"
#include "Nicla_System.h"

Adafruit_SI1145 uv = Adafruit_SI1145();

void setup() {
  nicla::begin();
  Serial.begin(9600);
  Serial.println("Adafruit SI1145 test");
  if (! uv.begin()) {
    Serial.println("Didn't find Si1145");
    while (1);
  }
}

void loop() {
  float sensorValue = analogRead(A0);
  float sensorVoltage = sensorValue / 1024 * 3.3;
  float UV_index_float = sensorVoltage / 0.1;
  int UV_index_rounded = round(UV_index_float);
  float UVindex = uv.readUV();
  UVindex /= 100.0;

  Serial.println("===================");
  Serial.print("Vis: "); Serial.println(uv.readVisible());
  Serial.print("IR: "); Serial.println(uv.readIR());
  Serial.print("UV: ");  Serial.println(UVindex);
  Serial.println("===================");
  Serial.print("sensor reading = ");
  Serial.print(sensorValue);
  Serial.print("        sensor voltage = ");
  Serial.print(sensorVoltage);
  Serial.println(" V");
  Serial.println("The UV Index float is: " + String(UV_index_float, 2));
  Serial.println("The UV Index rounded is: " + String(UV_index_rounded));
  Serial.println("===================");
  Serial.println("\n");
  delay(3000);

}
