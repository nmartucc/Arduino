
#include <I2C.h>
#include <Servo.h>
#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.

//Infrared
int lasersensorPins[] = {2,4,7,8}; // Array of pins connected to the sensor Power Enable lines
int lasersensorPinsArraySize = 4; // The length of the array
int laser1, laser2, laser3, laser4;
int limitedistancefront = 100, limitedistanceside = 50, limitedistanceback = 75;
int angle = 90;

//Ultrasound
int ultrasoundsensorPins[] = {3,5,6};
int ultrasoundsensorPinsArraySize = 3;
long ultrasound1, ultrasound2, ultrasound3;

//Acceleration pedal
long accelerator;
int AcceleratorPin=A0;
byte etat;

Servo brakeServo;

void setup(){
  brakeServo.attach(11);//(600,2400);//Servo plug on pin 11 and the HIGH state change between 600 us (angle of 0°) to 2400 us (angle of 180°)
  Serial.begin(115200); //Opens serial connection at 115200bps.     
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
  
  I2c.scan();
  
  Serial.println("Initialization :");
  for (int i = 0; i < lasersensorPinsArraySize; i++){
    pinMode(lasersensorPins[i], OUTPUT); // Pin to first LIDAR-Lite Power Enable line
    Serial.println(lasersensorPins[i]);
  }
  for (int j = 0; j< ultrasoundsensorPinsArraySize; j++){
    pinMode(ultrasoundsensorPins[j], INPUT);
    Serial.println(ultrasoundsensorPins[j]);
  }
  pinMode(12,OUTPUT);
  Serial.println("Initialization completed");
}

void loop(){ 
  //Ultrasound
  read_sensor();
  //Infrared
  enableDisableSensor(2); // Turn on sensor attached to pin 2 and disable all others 
  laser1=readDistance();
  enableDisableSensor(4); //Turn on sensor attached to pin 4 and disable all others  
  laser2=readDistance();
  enableDisableSensor(7); 
  laser3=readDistance();
  enableDisableSensor(8);  
  laser4=readDistance();
  //Accelerator
  accelerator = analogRead(AcceleratorPin);
  //Viewing
  print_range();
  delay(2000);

if (laser1 > limitedistancefront && laser2 > limitedistanceside && laser3 > limitedistanceside && laser4 > limitedistanceback)
{
  digitalWrite(12,LOW);  
  delay(100);
}
else 
{
  digitalWrite(12,HIGH);
  Serial.println("Emergency STOP");
  delay(100);
}
if (laser1 > limitedistancefront || laser1 == 0)
{
  brakeServo.write(0);
  delay(100);
  
}
else 
{
  brakeServo.write(angle);
  Serial.println("Emergency BRAKE");
  delay(100);
}
  
  //etat=digitalRead(12);
  //Serial.println(etat);
}
//Ultrasound functions :

void read_sensor (){
  ultrasound1 = pulseIn(ultrasoundsensorPins[0], HIGH); //Result in mm
  //inches = ultrasound1/25.4;
  ultrasound2 = pulseIn(ultrasoundsensorPins[1], HIGH); //Result in mm
  //inches = ultrasound2/25.4;
  ultrasound3 = pulseIn(ultrasoundsensorPins[2], HIGH); //Result in mm
  //inches = ultrasound3/25.4;
}

//Viewing

void print_range(){
  Serial.print("S1 Ultrasound (side) = ");
  Serial.print(ultrasound1*10);
  Serial.println("cm");
  
  Serial.print("S2 Ultrasound (side) = ");
  Serial.print(ultrasound2*10);
  Serial.println("cm");
  
  Serial.print("S3 Ultrasound (side) = ");
  Serial.print(ultrasound3*10);
  Serial.println("cm");
  
  Serial.print("S1 Infrared (front) = ");
  Serial.print(laser1); 
  Serial.println("cm"); 
  
  Serial.print("S2 Infrared (side) = ");
  Serial.print(laser2);
  Serial.println("cm");
  
  Serial.print("S3 Infrared (side) = ");
  Serial.print(laser3); 
  Serial.println("cm"); 
  
  Serial.print("S4 Infrared (back) = ");
  Serial.print(laser4); 
  Serial.println("cm");

  Serial.print("A0 : ");
  Serial.println(accelerator);
}

//Infrared functions :

void enableDisableSensor(int sensorPin){
  for (int i = 0; i < lasersensorPinsArraySize; i++){
      digitalWrite(lasersensorPins[i], LOW); // Turn off all sensors
  }
  digitalWrite(sensorPin, HIGH); // Turn on the selected sensor
  delay(1); // The sensor takes 1msec to wake
}

int readDistance(){
  uint8_t nackack = 100; // Setup variable to hold ACK/NACK responses     
  while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.write(LIDARLite_ADDRESS,RegisterMeasure, MeasureValue); // Write 0x04 to 0x00
    delay(1); // Wait 1 ms to prevent overpolling
  }

  byte distanceArray[2]; // array to store distance bytes from read function
  
  // Read 2byte distance from register 0x8f
  nackack = 100; // Setup variable to hold ACK/NACK responses     
  while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.read(LIDARLite_ADDRESS,RegisterHighLowB, 2, distanceArray); // Read 2 Bytes from LIDAR-Lite Address and store in array
    delay(1); // Wait 1 ms to prevent overpolling
  }
  int distance = (distanceArray[0] << 8) + distanceArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int
  
  return distance;   // Print Sensor Name & Distance
   
}
