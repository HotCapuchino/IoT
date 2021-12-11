int input_pin = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int val = analogRead(input_pin);
  Serial.write(val == 1024);
  delay(500);
}
