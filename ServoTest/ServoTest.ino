#include <Servo.h>

Servo serv;

void setup() {
  // put your setup code here, to run once:
  serv.attach(10);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  serv.write(0);
  Serial.println("at 0");
  delay(1000);
  serv.write(180);
  delay(2000);
}
