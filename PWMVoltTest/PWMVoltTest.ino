int out = 10;
int in = 0;
int pwm = 0;

void setup() {
  pinMode(out, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  analogWrite(out, pwm);
  Serial.println(analogRead(in));
}
