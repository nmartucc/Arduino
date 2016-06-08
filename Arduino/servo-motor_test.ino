#include <Servo.h>

//this program do a rotation of 180° with 45° steps. And when the angle is 180°, the servo-motor come back to 0° with -45° steps.  

int angle = 45;
int i = 1;
Servo brakeServo;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  brakeServo.attach(2);//,600,2400);
}

void loop() {
  // put your main code here, to run repeatedly:
     
    brakeServo.write(angle);
    delay(1000);
    if((angle<=135 && i== 1) || (angle==0 && i==2))
    {
      angle = angle + 45;
      i = 1;     
    }
    else if ((angle==180 && i==1) || (angle>=45 && i==2))
    {
      angle = angle - 45;
      i = 2;
    }  
    //Serial.println(angle,DEC);
}
