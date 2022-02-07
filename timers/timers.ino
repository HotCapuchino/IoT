void setup() {
   Serial.begin(9600);
}

void calculatePi(int mode) {
  delay(1000);
  unsigned long iters = 1000000;
  float pi = 0;
  unsigned long start = 0;
  
  if (mode == 1) {
    // 1 case - 49154 millis - THE FASTEST 
    float d = 1.;
    float a = 4.;
    start = millis();
    for(unsigned long i=1; i < iters; i++) {
      pi += a / d;
      a = -a;
      d += 2.;
    }
  } else if (mode == 2) {
    // 2 case - 49154 millis, not precise, pi ~ 0.79
    float d = 1.;
    float a = 1.;
    start = millis();
    for(unsigned long i=1; i < iters; i++) {
      pi += a / d;
      a = -a;
      d += 2.;
    }
  } else {
    // 3 case - 57956 millis
    float d = 1.;
    float a = 1.;
    start = millis();
    for(unsigned long i=1; i < iters; i++) {
      pi += a * 4. / d;
      a = -a;
      d += 2.;
    }
  }

  Serial.print(pi);
  Serial.print("calculation of ");
  Serial.print(iters);
  Serial.print(" itterations took ");
  Serial.println(millis() - start);
}

void loop() {
  calculatePi(1);  
}
