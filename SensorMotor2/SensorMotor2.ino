/*
 * This code works with all six sensors (3 ultrasonics, 3 LIDARs) and the hall effect sensor controller
 * The pin order is Front, Right, Left
 */

int avgC = 0; // count to keep track of the running averages
const int avgCSize = 5; // size of the running averages, DO NOT CHANGE

// For LIDAR Sensors
#include <I2C.h>
#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.

int lPins[] = {4,7,8}; // Array of pins connected to the sensor Power Enable lines
const int lSize = sizeof(lPins)/sizeof(*lPins); // The length of the array
int lid[lSize]; // Array of LIDAR readings
int lArr[lSize][avgCSize]; // Array of past LIDAR readings
int lMed[lSize]; // Array of median LIDAR readings
int timeout = 1000; // time before stopping poll of values in case ack is not received 

// For Ultrasonic Sensors
int usPins[] = {3,5,6}; // pins for the ultrasonic
const int usSize = sizeof(usPins)/sizeof(*usPins);; // number of ultrasonics
long  us[usSize]; // Array of ultrasonic readings
long usArr[usSize][avgCSize]; // Array of past us readings
long usMed[usSize]; // Array of median ultrasonic readings

int tooClose = 200; // Bypass the median calculation, so no delay when displaying the too close notification
int brakeDist = 100; // Distance at which to start braking

// For the Hall Effect Sensor + Motor Control
int hsout = 11;
int hsin = 0;
float speedFactor = 1.0; // multiplied by maxSpeed later on
float maxSpeed = .2; // 1.2 mirrors original kart output
float prevSpeedFactor = 1.0;

int buttonPin = 9;

#include <Servo.h>
Servo brakeServo;
int brakePin = 12;

int sensorCount = 0;

int spd = 0;
int prevSpd = 0;
int ramp = 2;
bool stopped = false;

void setup() {
  Serial.begin(115200); //Opens serial connection at 115200bps. 
  Serial.setTimeout(5); //Sets the timeout at 10 ms so it doesn't disrupt other code
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(10); // Sets a timeout to ensure no locking up of sketch if I2C communication fails

  for (int i = 0; i < lSize; i++){
    pinMode(lPins[i], OUTPUT); // Pin to [i] LIDAR-Lite Power Enable line
    Serial.print(lPins[i]);
  }
  Serial.print(" ");
  for (int j = 0; j< usSize; j++){ // Initializes the Ultrasonic sensors
    pinMode(usPins[j], INPUT);
    Serial.print(usPins[j]);
  }
  Serial.println();
  pinMode(hsout, OUTPUT);
  if (maxSpeed > 1) {
    maxSpeed = 1;
  }

  pinMode(buttonPin, INPUT);
  pinMode(brakePin, INPUT);

  brakeServo.attach(10);
  brakeServo.write(180);
}

void loop() {
  long startTime = millis();
  //readLid(sensorCount); // Method to read LIDARs
  readUS(sensorCount); // Method to read the values of the ultrasonic sensor
  //lMedCalc(sensorCount);
  usMedCalc(sensorCount);
  motorSet();
  if (sensorCount+1 == lSize) {
    printValues(); // Method to print sensor values to serial
    avgC = (avgC+1)%avgCSize;
  }
  //delay(5);
  /*Serial.print("Loop: \t");
  Serial.println(millis()-startTime);*/
  //Serial.println(digitalRead(12));
  sensorCount = (sensorCount + 1) % lSize;
}

void readLid(int i) {
    enableDisableSensor(lPins[i]);
    lid[i] = readLidar();
}

void readUS(int i) {
    us[i] = pulseIn(usPins[i], HIGH)/10; // Result in cm
}

void enableDisableSensor(int sensorPin){
  for (int l = 0; l < lSize; l++){
      digitalWrite(lPins[l], LOW); // Turn off all sensors
  }
  digitalWrite(sensorPin, HIGH); // Turn on the selected sensor
  delay(1); // The sensor takes 1msec to wake
}

int readLidar(){
  uint8_t nackack = 198; // Setup variable to hold ACK/NACK resopnses     
  unsigned long stime = millis();
  while ((nackack != 0) && (millis()-stime < timeout)){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.write(LIDARLite_ADDRESS,RegisterMeasure, MeasureValue); // Write 0x04 to 0x00
    delay(1); // Wait 1 ms to prevent overpolling
  }

  byte distanceArray[2]; // array to store distance bytes from read function
  stime = millis();
  
  // Read 2byte distance from register 0x8f
  nackack = 199; // Setup variable to hold ACK/NACK resopnses     
  while ((nackack != 0) && (millis()-stime < timeout)){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.read(LIDARLite_ADDRESS,RegisterHighLowB, 2, distanceArray); // Read 2 Bytes from LIDAR-Lite Address and store in array
    delay(1); // Wait 1 ms to prevent overpolling
  }
  int distance = (distanceArray[0] << 8) + distanceArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int  
  return distance;   // Print Sensor Name & Distance
}

long med5(long a0, long a1, long a2, long a3, long a4) { // super efficient median of 5 calculator
  long t;
  if (a0>a1) {
    t = a0;
    a0 = a1;
    a1 = t;
  }
  if (a2 > a3) {
    t = a2;
    a2 = a3;
    a3 = t;
  }
  if (a2 < a0) {
    t = a1;
    a1 = a3;
    a3 = t;
    a2 = a0;
  }
  if (a4 > a1) {
    t = a4;
    a4 = a1;
    a1 = t;
  }
  if (a4 < a2) {
    t = a1;
    a1 = a3;
    a3 = t;
    a4 = a2;
  }
  if (a3 < a4) {
    return a3;
  }
  else {
    return a4;
  }
}

int med5(int a0, int a1, int a2, int a3, int a4) { // super efficient median of 5 calculator
  long t;
  if (a0>a1) {
    t = a0;
    a0 = a1;
    a1 = t;
  }
  if (a2 > a3) {
    t = a2;
    a2 = a3;
    a3 = t;
  }
  if (a2 < a0) {
    t = a1;
    a1 = a3;
    a3 = t;
    a2 = a0;
  }
  if (a4 > a1) {
    t = a4;
    a4 = a1;
    a1 = t;
  }
  if (a4 < a2) {
    t = a1;
    a1 = a3;
    a3 = t;
    a4 = a2;
  }
  if (a3 < a4) {
    return a3;
  }
  else {
    return a4;
  }
}

void usMedCalc(int i) {
    usArr[i][avgC] = us[i];
    if (us[i] > tooClose) {
      usMed[i] = med5(usArr[i][0], usArr[i][1], usArr[i][2], usArr[i][3], usArr[i][4]);
    }
    else {
      usMed[i] = us[i];
    }
}

void lMedCalc(int i) {
    lArr[i][avgC] = lid[i];
    if (lid[i] > tooClose || lid[i] < 10) {
      lMed[i] = med5(lArr[i][0], lArr[i][1], lArr[i][2], lArr[i][3], lArr[i][4]);
    }
    else {
      lMed[i] = lid[i];
    }
}

void printValues() {
  for (int i=0; i<lSize; i++) {
    Serial.print(lMed[i]);
    Serial.print(",");
  }
  // Serial.print("\t");
  for (int i=0; i<usSize; i++) {
//    Serial.print("Ultrasonic ");
//    Serial.print(i);
//    Serial.print(": ");
    Serial.print(usMed[i]);
    Serial.print(",");
  }
  Serial.print(!digitalRead(buttonPin));
  Serial.println(',');
}

void motorSet() {
  int serialVal = Serial.parseInt();
  if (usMed[0] < tooClose || ((lMed[0] < tooClose) && (lMed[0] != 0))) {
    speedFactor = .1;
  }
  else {
    if (serialVal > 1) {
      if (serialVal > 120) {
        serialVal = 120;
      }
      speedFactor = ((float) serialVal)/100.0;
      stopped = false;
    }
    else {
      speedFactor = prevSpeedFactor;
    }
  }
  if (serialVal == 1) {
    stopped = true;
    brake();
    return;
  }
  /*Serial.print(analogRead(0));
  Serial.print("\t");
  Serial.println(analogRead(2));*/
  if ((usMed[0] > brakeDist || usMed[0] == 0) && (lMed[0] > brakeDist || lMed[0] == 0) && digitalRead(brakePin) && !stopped) { // To stop the kart with only front sensors
    spd = (int) ((analogRead(hsin)-300)*speedFactor*maxSpeed + 350)*1.4/4;
    /*if (prevSpd == 0) prevSpd = 85;
    if (spd - prevSpd > ramp) {
      spd = prevSpd + ramp;
    }*/
    analogWrite(hsout, spd);
    brakeServo.write(180);
    /*Serial.println(spd);
    Serial.println(analogRead(0));
    Serial.println("not braking");*/
  }
  else {
    brake();
  }
  prevSpd = spd;
  if (speedFactor > .101) prevSpeedFactor = speedFactor;
}

void brake() {
  //Serial.println("braking");
  spd = 0;
  prevSpd = 0;
  brakeServo.write(0);
  analogWrite(hsout, 0);
}


