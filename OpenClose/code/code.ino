int input_pin = A0;
int prev_val = -1;
int val = -1;

void setup() {
  Serial.begin(9600);
}

void loop() {
  val = digitalRead(input_pin);
  if (prev_val != val) {
     prev_val = val;
     if (val == 0) {
        Serial.println("Door was closed!");
     } else {
        Serial.println("Door was opened!");
     }
  }
}
