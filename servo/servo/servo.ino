#include <Servo.h>

int north = A0;
int east = A1;
int west = A2;
int south = A3;
int servoPin = 10;
Servo servo;

void setup() {
  Serial.begin(9600);
  servo.attach(10);
}

void loop() {
  int north_val = analogRead(north);
  int east_val = analogRead(east);
  int west_val = analogRead(west);
  int south_val = analogRead(south);
  
  int my_vals[] = {north_val, east_val, west_val, south_val};
  int max_ = north_val;
  for (int i = 0; i < 4; i++)
    if (my_vals[i] > max_)

  if (max_ == north_val) servo.write(0);
  if (max_ == east_val) servo.write(60);
  if (max_ == south_val) servo.write(120);
  if (max_ == west_val) servo.write(180);

   delay(1000);
}
