#define ETALON_PIN 1
#define CALIBRATED_PIN 2

const int SIZE = 1024;
bool is_calibrated = false; 

int delta_signal[SIZE];
bool delta_signal_filled[SIZE];
int start_range;
int end_range;


void calibrate() {
  if (Serial.available() > 0) {
    int calibrated = analogRead(CALIBRATED_PIN);
    int etalon = analogRead(ETALON_PIN);
    delta_signal[calibrated] = calibrated - etalon;
    delta_signal_filled[calibrated] = true;
  }
}

bool is_calibration_passed() {
  int filled_overall = 0;
  for (int i = 0; i < SIZE; i++) {
    if (delta_signal_filled[i]) {
      filled_overall++;
    }
  } 
  if (filled_overall / float(SIZE) > 0.5) {
    is_calibrated = true;
  } 
}

int get_calibration(int sensor_value) {
  int sensor_value_copy = sensor_value;
  if (!delta_signal_filled[sensor_value]) {
    while (true) {
      sensor_value_copy++;
      if (delta_signal_filled[sensor_value_copy]) {
        return delta_signal[sensor_value_copy];
      }
      if (sensor_value_copy == SIZE - 1) {
        break;
      }
    }
    while (true) {
      sensor_value_copy--;
      if (delta_signal_filled[sensor_value_copy]) {
        return delta_signal[sensor_value_copy];
      }
      if (sensor_value_copy == 0) {
        break;
      }
    }
    return 0;    
  } else {
    return delta_signal[sensor_value];
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < SIZE; i++) {
    delta_signal_filled[i] = false;
  }
}

void loop() {
  if (!is_calibrated) {
    calibrate();
  } else {
    Serial.println(get_calibration(analogRead(CALIBRATED_PIN)));
  }
}
