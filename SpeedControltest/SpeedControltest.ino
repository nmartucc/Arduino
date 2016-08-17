int hsout = 11;
int hsin = 0;
float speedFactor = 1; // 1.2 mostly mirrors the input
int limit = 1.2; // Limit for the speed

void setup() {
  pinMode(hsout, OUTPUT);
  Serial.begin(9600);
  if (speedFactor > 1) {
    speedFactor = 1;
  }
}

void loop() {
  analogWrite(hsout, (int) (analogRead(hsin)*speedFactor+350-300*speedFactor)*limit/4);
  Serial.println("HallEffect: " + String(analogRead(hsin), DEC) + "\t = " + String((int) ((float)analogRead(hsin)/1024*5000), DEC) + "mV\t Output: " + String(analogRead(2), DEC) + + " =\t" + String((int) ((float)analogRead(2)/1024*5000), DEC) + "mV");
}
