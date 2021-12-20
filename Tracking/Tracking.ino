int trig_pin = 11;
int echo_pin = 10;
int pir_pin = 9;
long duration;
int infra_pin = A0;
int open_close_pin = 4;
int prev_val = -1;
int val = -1;
bool opened = false;


void setup() {
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  Serial.begin(9600);
}

// 1st task
void getSoundDistance() {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  duration = pulseIn(echo_pin, HIGH);
  float distance = duration / 2. * 0.0343; // in cm
//  Serial.print("Sound distance in cm: ");
  Serial.println(distance);
}

// 2nd task (failed) 
void getPIRPresense() {
  Serial.print("PIR value: ");
   if (digitalRead(pir_pin) == 1) {
      Serial.println("someone's here");
   } else {
      Serial.println("nobody's here");
   }
}

void getInfraValue() {
  // формулу брал здесь - https://3d-diy.ru/wiki/arduino-datchiki/infrakrasnyj-datchik-rasstojanija/
  float infra_value = analogRead(infra_pin) * 0.0048828125; 
  infra_value = 32 * pow(infra_value, -1.1);
  Serial.print("Infra value in cm: ");
  Serial.println(infra_value);
}

void getMaxOpenCloseDist() {
  val = digitalRead(open_close_pin);
  if (prev_val != val) {
    prev_val = val;
    if (val == 0) {
      if (!opened) {
         opened = true;
         Serial.print("Max dist: ");  
        getSoundDistance();
      }
    } else {
      opened = false;
    }
  }
}

void loop() {
//  1st
//  getSoundDistance();

//  2nd failed
//  getPIRPresense();

//  3rd
//  getInfraValue();

  getMaxOpenCloseDist();
  delay(500);
}
