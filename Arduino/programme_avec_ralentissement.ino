#include <I2C.h>
#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.

//Infrared
int lasersensorPins[] = {2,4,7,8}; // Array of pins connected to the sensor Power Enable lines
int lasersensorPinsArraySize = 4; // The length of the array
int laser1, laser2, laser3, laser4;
int limitedistancefront = 200, limitedistanceside = 100, limitedistanceback = 100;

//Ultrasound
int ultrasoundsensorPins[] = {3,5,6};
int ultrasoundsensorPinsArraySize = 3;
long ultrasound1, ultrasound2, ultrasound3;

//Acceleration pedal
long accelerator;
int AcceleratorPin=A0;
int count; 


void setup(){
  //Serial.begin(115200); //Opens serial connection at 115200bps. 
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
  
  
  count = 0; // incrémenter la valeur count à 0 --> elle permet de ne pas ralentir à chaque entrer dans le loop avec la fonction AcceleratorPedal
  //I2c.scan();
  
  //Serial.println("Initialization :");
  for (int i = 0; i < lasersensorPinsArraySize; i++){
    pinMode(lasersensorPins[i], OUTPUT); // Pin to first LIDAR-Lite Power Enable line
    //Serial.println(lasersensorPins[i]);
  }
  for (int j = 0; j< ultrasoundsensorPinsArraySize; j++){
    pinMode(ultrasoundsensorPins[j], INPUT);
    //Serial.println(ultrasoundsensorPins[j]);
  }
  pinMode(12,OUTPUT);
  //Serial.println("Initialization completed");
}

void loop(){ 
  //Ultrasound
  //read_sensor();
  //Infrared
  enableDisableSensor(2); // Turn on sensor attached to pin 2 and disable all others 
  laser1=readDistance();
  //enableDisableSensor(4); //Turn on sensor attached to pin 4 and disable all others  
  //laser2=readDistance();
  //enableDisableSensor(7); 
  //laser3=readDistance();
  //enableDisableSensor(8);  
  //laser4=readDistance();
  
  //Safety
  SafetyProtocol();

  //Slowing Kart 
  AcceleratorPedal();
  //Viewing
  //print_range();
  //delay(100);


}

//Accelerator function

void SafetyProtocol(){
  if ((laser1 > limitedistancefront || laser1==0) && (laser2 > limitedistanceside || laser2==0) && 
  (laser3 > limitedistanceside || laser3==0) && (laser4 > limitedistanceback || laser4==0) && 
  (ultrasound1 > (limitedistancefront*10) || ultrasound1==0) && (ultrasound2 > (limitedistanceside*10) || ultrasound2==0) && 
  (ultrasound3 > (limitedistanceside*10) || ultrasound3==0))
    {
      digitalWrite(12,LOW);  
    }
    else 
    {
      digitalWrite(12,HIGH);
      //Serial.println("Emergency STOP");
    }
}

//Ultrasound functions :

void read_sensor (){
  ultrasound1 = pulseIn(ultrasoundsensorPins[0], HIGH); //Result in mm
  //inches = ultrasound1/25.4;
  //ultrasound2 = pulseIn(ultrasoundsensorPins[1], HIGH); //Result in mm
  //inches = ultrasound2/25.4;
  //ultrasound3 = pulseIn(ultrasoundsensorPins[2], HIGH); //Result in mm
  //inches = ultrasound3/25.4;
}

//Viewing

void print_range(){
  Serial.print("S1 Ultrasound (side) = ");
  Serial.print(ultrasound1/10);
  Serial.println("cm");
  
  Serial.print("S2 Ultrasound (side) = ");
  Serial.print(ultrasound2/10);
  Serial.println("cm");
  
  Serial.print("S3 Ultrasound (side) = ");
  Serial.print(ultrasound3/10);
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
  uint8_t nackack = 100; // Setup variable to hold ACK/NACK resopnses
while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
nackack = I2c.write(LIDARLite_ADDRESS,RegisterMeasure, MeasureValue); // Write 0x04 to 0x00
delay(1); // Wait 1 ms to prevent overpolling
}
byte distanceArray[2]; // array to store distance bytes from read function
// Read 2byte distance from register 0x8f
nackack = 100; // Setup variable to hold ACK/NACK resopnses
while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
nackack = I2c.read(LIDARLite_ADDRESS,RegisterHighLowB, 2, distanceArray); // Read 2 Bytes from LIDAR-Lite Address and store in array
delay(1); // Wait 1 ms to prevent overpolling
}
int distance = (distanceArray[0] << 8) + distanceArray[1]; // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int
return distance;
   
}

long AcceleratorPedal(){
 long maxacceleration; 
 if (laser1 <= 800 && laser1 != 0 && laser2 <= 900 && laser2 != 0 && count == 0) //conditions pour abaisser la vitesse du kart --> A changer
 {
   accelerator = (accelerator * 250)/500 ; //la nouvelle valeur max (250) doit être plus petite que l'ancienne (500) --> rapport de réduction entre 0 et 1
   maxacceleration = 250; 
   count = count + 1 ;
 }
 if ((laser1 > 800 || laser1 == 0 || laser2 > 900 || laser2 == 0) && count != 0)//lorsque l'on sort de la zone de ralentissement 
 {
  accelerator = (accelerator * 500)/250 ; 
  maxacceleration = 500;
  count = 0 ;
 }
}


