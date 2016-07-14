/* 
http://pulsedlight3d.com
This sketch demonstrates getting distance from multiple LIDAR-Lite sensors by enabling the Power Enable line of the sensor 
to read from, and disabling the others.
It utilizes the 'Arduino I2C Master Library' from DSS Circuits:
http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library 
You can find more information about installing libraries here:
http://arduino.cc/en/Guide/Libraries
*/

/*
 * To test the LIDAR sensors, put the digital pins they are connected to in the sPins Array
 * Everything else is taken care of
 */

#include <I2C.h>
#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.

int sPins[] = {4,7,8}; // Array of pins connected to the sensor Power Enable lines
int sPinsSize = sizeof(sPins)/sizeof(*sPins); // The length of the array
int timeout = 1000; // time before stopping poll of values in case ack is not received

void setup(){
  Serial.begin(115200); //Opens serial connection at 115200bps.     
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
  
  for (int i = 0; i < sPinsSize; i++){
    pinMode(sPins[i], OUTPUT); // Pin to [i] LIDAR-Lite Power Enable line
    Serial.print(sPins[i]);
  }
  Serial.println();
}

void loop(){
  for (int i=0; i<sPinsSize; i++) {
    enableDisableSensor(sPins[i]); // Turn on sensor attached to pin [i] and disable all others
    Serial.print(readDistance()); // Read Distance from Sensor
    Serial.print("."); // Print "." to separate readings
  }
  Serial.println();
}


void enableDisableSensor(int sensorPin){
  for (int l = 0; l < sPinsSize; l++){
      digitalWrite(sPins[l], LOW); // Turn off all sensors
  }
  digitalWrite(sensorPin, HIGH); // Turn on the selected sensor
  delay(1); // The sensor takes 1msec to wake
}

int readDistance(){
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
    delay(10); // Wait 1 ms to prevent overpolling
  }
  int distance = (distanceArray[0] << 8) + distanceArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int
  
  return distance;   // Print Sensor Name & Distance
   
}
