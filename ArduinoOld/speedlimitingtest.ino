double vmin = 0.923/5*255;
double vcurrent = vmin;
void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  Serial.begin(9600);
  analogWrite(3, vmin);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  char input = Serial.read();
  if (input == 'd' && input < 250){
    vcurrent += 5.0;
  }
  Serial.println(vcurrent);
  analogWrite(3, vcurrent);
}
