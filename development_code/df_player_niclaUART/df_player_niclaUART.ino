#include "Nicla_System.h"
#include <DFPlayerMini_Fast.h>

DFPlayerMini_Fast myMP3;

void setup()
{
  nicla::begin();
  Serial.begin(9600);
  Serial1.begin(9600);
  myMP3.begin(Serial1, true);
  delay(1000);

  myMP3.volume(30);
}

void loop() {
  while (Serial.available()) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      Serial.println("Command activated");
      playVoice(1, 10);
    }
  }
}

void playVoice(int folder, int file) {
  myMP3.playFolder(folder, file);
  delay(200);
  while (!digitalRead(GPIO3));
}
