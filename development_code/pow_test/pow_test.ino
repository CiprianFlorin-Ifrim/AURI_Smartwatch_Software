float i=0;
long result;

void setup(){
  Serial.begin(115200);
}

void loop(){
  Serial.println("Base 2: "); 
  for(i=0;i<50;i++){
    result=pow(2,i);
    Serial.println(result);
}
}
