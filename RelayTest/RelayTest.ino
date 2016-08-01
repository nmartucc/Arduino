int out = 11;
int in = 12;
bool on = true;

void setup() {
  pinMode(out, OUTPUT);
  pinMode(in, INPUT);
}

void loop() {
  digitalWrite(out, digitalRead(in) == on);
}
