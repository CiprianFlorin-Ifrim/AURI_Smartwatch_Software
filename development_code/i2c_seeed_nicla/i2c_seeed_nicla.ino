#include <Wire.h>

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);    // start serial for output

}

void loop() {

}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
  while (Wire.available()){
   int x = Wire.read();    // receive byte as an integer
   if (x == 5){
    Serial.println("Hello World!");
   }
  }
}

// function that executes whenever data is requested by master
void requestEvent() {
  Wire.write(val); // respond with message of 1 byte
}
