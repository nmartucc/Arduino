int out = 11;
int in = 0;
float speedFactor = .8 // THIS SHOULD NEVER BE ABOVE 1

void setup() {
  pinMode(out, OUTPUT);
  if (speedFactor > 1) {
    speedFactor = 1;
  }
}

void loop() {
  analogWrite(out, ((int) analogRead(in)*speedFactor/4));
}
