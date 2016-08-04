#include <Servo.h>

Servo serv;

void setup() {
  // put your setup code here, to run once:
  serv.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  serv.write(0);
  delay(1000);
  serv.write(180);
  delay(1000);
}
