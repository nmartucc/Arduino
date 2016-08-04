void setup() {
  Serial.begin(115200);
  Serial.setTimeout(5);
}

void loop() {
  Serial.println(Serial.parseInt());
}
