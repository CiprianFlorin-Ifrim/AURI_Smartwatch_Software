//References:
//https://github.com/PowerBroker2/DFDFPlayerMini_Fast/blob/master/examples/example/example.ino
//https://github.com/PowerBroker2/DFPlayerMini_Fast/issues/43

#include <DFPlayerMini.h>

DFPlayerMini player;


void setup()
{
  Serial1.begin(9600);
  player.init(GPIO3, P0_09, P0_20);
}

void loop()
{
  player.playFileAndWait(3);


}
