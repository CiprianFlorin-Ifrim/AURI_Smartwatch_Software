#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial softwareSerial(10, 11);

// Create the Player object
DFRobotDFPlayerMini player;

void setup() 
{
  softwareSerial.begin(9600);
  player.begin(softwareSerial);
}


void loop() 
{
  player.volume(30);
  player.playMp3Folder(1);
  delay(5000);
  player.playMp3Folder(2);
  delay(2000);
}
