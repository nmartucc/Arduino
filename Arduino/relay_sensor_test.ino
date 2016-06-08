int ultrasound_pin = 3;
int relay_pin = 2;

void setup() {
  Serial.begin(9600);
  delay(100);
  pinMode(ultrasound_pin, INPUT);
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  long ultrasound1 = pulseIn(ultrasound_pin, HIGH);
  Serial.print("Distance reading: ");
  Serial.println(ultrasound1);
  if (ultrasound1 < 500){
    digitalWrite(relay_pin, LOW);
    delay(3000);
    digitalWrite(relay_pin, HIGH);
  }
}
