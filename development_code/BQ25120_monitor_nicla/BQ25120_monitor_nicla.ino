#include "Nicla_System.h"
#include <bq25120a_nicla.h>                              // NICLA library for BQ25120A

byte battery_status;
byte battery_fault;
uint8_t battery_level;

void setup() {
  Serial.begin(9600);
  nicla::begin();
  //Wire1.begin();                                          // internal I2C communication to BHI260/PWM/LEDS/CHARGER IC

  pwmCDHigh();                                            // make sure we can talk to BQ25120A
  bq25120_setMaxCurrentMinVoltage(150, 3.0, true);        // set max current to 150mA, min voltage before fast recharge to 3.0V
  bq25120_setBatteryVoltage(4.2);                         // set battery voltage to 4.2V
  bq25120_setChargeCurrent(70);                           // set charge current to 110mA (lower then recomended C/2)
  bq25120_setTerminationCurrent(5);                       // set termination current to 5mA (lower then recomended C/30)
  bq25120_disableVinDPM();
  pwmCDLow();                                             // back to charge or highz mode
}

void loop() {
  //if the serial is available and a 1 is received then proceed
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      update_battery_info();

      Serial.println("\n-----------------------------------------------------");
      Serial.println("The battery level is equal to: " + String(battery_level));
      Serial.println("The battery status is equal to: " + String(battery_status));
      Serial.println("The battery fault is equal to: " + String(battery_fault));
      Serial.println("-----------------------------------------------------");
    }
  }
  delay(1000);
}

void update_battery_info(void) {
  battery_level = bq25120_getLevel();
  battery_status = bq25120_getStatus();
  battery_fault = bq25120_getFaults();
}

