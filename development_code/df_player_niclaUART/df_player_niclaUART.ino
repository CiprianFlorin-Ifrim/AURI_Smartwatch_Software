#include "DFRobotDFPlayerMini.h"
#include "Nicla_System.h"
#include "SoftwareSerial.h"

// Create the Player object
SoftwareSerial mySoftwareSerial(2, 1);
DFRobotDFPlayerMini player;

void setup() 
{
  mySoftwareSerial.begin(9600);
  nicla::begin();
  player.begin(mySoftwareSerial);
}


void loop() 
{
  player.volume(30);
  player.playMp3Folder(1);
  delay(5000);
  player.playMp3Folder(2);
  delay(2000);
}
