int avgC = 0; // count to keep track of the running averages
const int avgCSize = 5; // size of the running averages

// For Ultrasonic Sensors
int usPins[] = {6,5,3}; // pins for the ultrasonic
const int usSize = 3; // number of ultrasonics
long  us[usSize]; // Array of ultrasonic readings
long usArr[usSize][avgCSize]; // Array of past us readings
long usMed[usSize]; // Array of median ultrasonic readings

int tooClose = 500;

void setup() {
  Serial.begin(9600); //Opens serial connection at 115200bps. 
  for (int j = 0; j< usSize; j++){ // Initializes the Ultrasonic sensors
    pinMode(usPins[j], INPUT);
    Serial.println(usPins[j]);
  }
}

void loop() {
  readUS(); // Method to read the values of the ultrasonic sensor
  usMedCalc();
  printValues(); // Method to print sensor values to serial
  avgC = (avgC+1)%avgCSize;
  delay(5);
}

void readUS() {
  for (int i=0; i<usSize; i++) {
    us[i] = pulseIn(usPins[i], HIGH); // Result in cm, after y = 9.444x+85.723 relating us to cm
  }
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

void usMedCalc() {
  for (int i=0; i<usSize; i++) {
    usArr[i][avgC] = us[i];
    for (int j=0; j<avgCSize; j++) {
//      Serial.print(usArr[i][j]);
//      Serial.print(", ");
    }
//    Serial.println();
    if (us[i] > tooClose) {
      usMed[i] = med5(usArr[i][0], usArr[i][1], usArr[i][2], usArr[i][3], usArr[i][4]);
    }
    else {
      usMed[i] = us[i];
    }
  }
}

void printValues() {
  for (int i=0; i<usSize; i++) {
//    Serial.print("Ultrasonic ");
//    Serial.print(i);
//    Serial.print(": ");
//    Serial.println(us[i]);
//    Serial.print("\t");
//    Serial.println(usMed[i]);
      Serial.print(usMed[i]);
      Serial.print(", ");
  }
  Serial.println();
}

