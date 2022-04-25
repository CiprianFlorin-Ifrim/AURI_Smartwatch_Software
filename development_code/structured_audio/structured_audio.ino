//References:
//https://github.com/PowerBroker2/DFDFPlayerMini_Fast/blob/master/examples/example/example.ino
//https://github.com/PowerBroker2/DFPlayerMini_Fast/issues/43
//tts: https://www.naturalreaders.com/online/

#include "Nicla_System.h"
#include <DFPlayerMini_Fast.h>

DFPlayerMini_Fast myMP3;

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600);
  myMP3.begin(Serial1, true, false);
  delay(1000);

  Serial.println("Setting volume to max");
  myMP3.volume(30);
}

void loop()
{
  playVoice(1, 4);      //play file 4 in folder 1
  playVoice(1, 2);      //play file 4 in folder 1
  playVoice(1, 1);      //play file 4 in folder 1
  playVoice(1, 3);      //play file 4 in folder 1
  playVoice(1, 5);      //play file 4 in folder 1
  

}

void playVoice(int folder, int file) {
  myMP3.playFolder(folder, file);
  delay(150);
  while (!digitalRead(GPIO3));         //wait until the file has finished playing (MCU GPIO3 connected to Busy pin on DFPlayer Mini)
}
