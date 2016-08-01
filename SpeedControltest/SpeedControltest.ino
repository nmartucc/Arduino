int out = 11;
int in = 0;
float speedFactor = 1; // 1.2 mostly mirrors the input
int speedLimit = 1.2; // Limit for the speed

void setup() {
  pinMode(out, OUTPUT);
  Serial.begin(9600);
  if (speedFactor > speedLimit) {
    speedFactor = speedLimit;
  }
}

void loop() {
  analogWrite(out, (int) analogRead(in)*speedFactor/4);
  Serial.println("HallEffect: " + String(analogRead(in), DEC) + "\t = " + String((int) ((float)analogRead(in)/1024*5000), DEC) + "mV\t Output: " + String(analogRead(2), DEC) + + " =\t" + String((int) ((float)analogRead(2)/1024*5000), DEC) + "mV");
}
