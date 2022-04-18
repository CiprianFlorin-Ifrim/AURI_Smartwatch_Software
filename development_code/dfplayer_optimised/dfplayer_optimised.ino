//References:
//https://github.com/PowerBroker2/DFDFPlayerMini_Fast/blob/master/examples/example/example.ino
//https://github.com/PowerBroker2/DFPlayerMini_Fast/issues/43

#include <DFPlayerMini_Fast.h>

DFPlayerMini_Fast myMP3;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  myMP3.begin(Serial1, true, false);
  delay(1000);

  Serial.println("Setting volume to max");
  myMP3.volume(30);
}

void loop()
{
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    if (incomingCharacter == '1') {
      playVoice(1, 2);
      //1playVoice(2, 2);
    }
    if (incomingCharacter == '2') {
      playVoice(2, 3);
      //playVoice(1, 1);
    }
  }
}

void playVoice(int folder, int file) {
  myMP3.playFolder(folder, file);
  delay(200);
  while(!digitalRead(5));
}
