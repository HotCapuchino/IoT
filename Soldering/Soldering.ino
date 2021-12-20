int north_east = A0;
int north_west = A1;
int south_west = A2;
int south_east = A3;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int north_east_val = analogRead(north_east);
  int north_west_val = analogRead(north_west);
  int south_west_val = analogRead(south_west);
  int south_east_val = analogRead(south_east);
  int my_vals[] = {north_east_val, north_west_val, south_west_val, south_east_val};
  int max_ = north_east_val;
  for (int i = 0; i < 4; i++)
    if (my_vals[i] > max_)
      max_ = my_vals[i];
  if (max_ == north_east_val) Serial.println("north-east");
  if (max_ == north_west_val) Serial.println("north-west");
  if (max_ == south_west_val) Serial.println("south-west");
  if (max_ == south_east_val) Serial.println("south-east");
  
  delay(1000);
}
