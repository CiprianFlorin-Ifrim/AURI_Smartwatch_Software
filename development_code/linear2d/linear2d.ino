#include <LinearRegression2d.h>

LinearRegression2d lr = LinearRegression2d();
double values[2];

void setup() {
    Serial.begin(9600);     
}

void loop() {
    Serial.println("Start learn");
    //y = 0.5*x1 + 3*x2 + 0.75
    lr.learn(0.0, 93.0, 0.0);
    lr.learn(1.11, 93.0, 0.0);
    lr.learn(2.78, 93.0, 1.0);
    lr.learn(0.0, 93.0, 0.0);
    lr.learn(1.11, 93.0, 0.0);
    lr.learn(2.78, 93.0, 1.0);

    Serial.println("End learn");

    float fake_temp = 2.78;
    float fake_hum = 93.0;
    Serial.println(int(lr.calculate(fake_temp,fake_hum)));




    //Serial.println("Reset");
    //lr.reset();
    
    delay(300000);
}
