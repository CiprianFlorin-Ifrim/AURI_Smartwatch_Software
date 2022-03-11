#include "DFRobotDFPlayerMini.h"
#include "Nicla_System.h"
#include "SoftwareSerial.h"

// Create the Player object
DFRobotDFPlayerMini player;

void setup() 
{
  Serial1.begin(9600);
  nicla::begin();
  player.begin(Serial1);
}


void loop() 
{
  player.volume(30);
  player.playMp3Folder(1);
  delay(5000);
  player.playMp3Folder(2);
  delay(2000);
}
