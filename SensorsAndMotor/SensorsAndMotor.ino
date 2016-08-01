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

int tooClose = 100; // Bypass the median calculation, so no delay when displaying the too close notification
int brake = 50; // Distance at which to start braking

// For the Hall Effect Sensor + Motor Control
int hsout = 11;
int hsin = 0;
float speedFactor = .75; // 1 mostly mirrors the input, multiplied by 1.2 later on

void setup() {
  Serial.begin(115200); //Opens serial connection at 115200bps. 
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails

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
  if (speedFactor > 1) {
    speedFactor = 1;
  }
}

void loop() {
  readLid(); // Method to read LIDARs
  readUS(); // Method to read the values of the ultrasonic sensor
  lMedCalc();
  usMedCalc();
  motorSet();
  printValues(); // Method to print sensor values to serial
  avgC = (avgC+1)%avgCSize;
  delay(5);
}

void readLid() {
  for (int i=0; i<lSize; i++) {
    enableDisableSensor(lPins[i]);
    lid[i] = readLidar();
  }
}

void readUS() {
  for (int i=0; i<usSize; i++) {
    us[i] = pulseIn(usPins[i], HIGH)/10; // Result in cm
  }
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

void usMedCalc() {
  for (int i=0; i<usSize; i++) {
    usArr[i][avgC] = us[i];
    if (us[i] > tooClose) {
      usMed[i] = med5(usArr[i][0], usArr[i][1], usArr[i][2], usArr[i][3], usArr[i][4]);
    }
    else {
      usMed[i] = us[i];
    }
  }
}

void lMedCalc() {
  for (int i=0; i<lSize; i++) {
    lArr[i][avgC] = lid[i];
    if (lid[i] > tooClose) {
      lMed[i] = med5(lArr[i][0], lArr[i][1], lArr[i][2], lArr[i][3], lArr[i][4]);
    }
    else {
      lMed[i] = lid[i];
    }
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
  Serial.println();
}

void motorSet() {
  /* bool braking = false;
  for (int i=0; i<lSize; i++) {
    if (lMed[i] < brake) {
      braking = true;
    }
  }
  for (int i=0; i<usSize; i++) {
    if (usMed[i] < brake) {
      braking = true;
    }
  }
  if (!braking) { */ // To stop the kart with all ultrasonics
  if (usMed[0] > brake && lMed[0] > brake) {
    analogWrite(hsout, (int) (analogRead(hsin)*speedFactor + 320 - 300*speedFactor)*1.2/4);
    // Serial.println(analogRead(hsin));
  }
}


