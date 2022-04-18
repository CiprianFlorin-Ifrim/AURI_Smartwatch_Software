//reference: https://wiki.dfrobot.com/DFDFPlayer_Mini_SKU_DFR0299
//https://github.com/PowerBroker2/DFDFPlayerMini_Fast/blob/master/examples/example/example.ino
//https://forum.arduino.cc/t/dfDFPlayer-based-mp3-DFPlayer-struggle-playing-next-tracks-based-play-finished/631515/3

//very nice: https://forum.arduino.cc/t/dfDFPlayer-detect-when-song-is-done-playing/502188/34

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

// Create the Player object
DFRobotDFPlayerMini player;

void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);
  player.begin(Serial1, true, false);
}


void loop() 
{
  playVoice(2, 1, 30);
  playVoice(15, 2, 30); 
}


int playVoice(int folder, int file, int volume) {
  Serial.println(String(folder) + "," +  String(file) + "," +  String(volume));
  player.volume(volume);
  player.playFolder(folder, file);
  delay(100);
  int playerState = 0;
  while (playerState != 512) {
    delay(500);          // prevents clicks and static during playback - increase if necessary.
    playerState = player.readState();
  }
  //player.volume(0);
  return playerState;
}
