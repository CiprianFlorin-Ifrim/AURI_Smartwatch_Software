//References:
//https://github.com/PowerBroker2/DFDFPlayerMini_Fast/blob/master/examples/example/example.ino
//https://github.com/PowerBroker2/DFPlayerMini_Fast/issues/43
//tts: https://www.naturalreaders.com/online/

#include "Nicla_System.h"
#include <DFPlayerMini_Fast.h>

DFPlayerMini_Fast myMP3;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  myMP3.begin(Serial1, true);
  delay(1000);

  Serial.println("Setting volume to max");
  myMP3.volume(30);
}

void loop()
{
  myMP3.playFolder(1, 4);
  delay(5000);
  //playVoice(1, 4);
}

void playVoice(int folder, int file) {
  myMP3.playFolder(folder, file);
  delay(150);
  while (!digitalRead(GPIO3));
}
