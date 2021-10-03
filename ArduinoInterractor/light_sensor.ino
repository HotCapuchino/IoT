const int LED_PIN = 13;
const int LED_INTERVAL = 400;

enum class State : uint8_t {
  Off = 0, On, Blink, 
  None
};

const char TOKENS[] = {
  'd',
  'u',
  'b',
};



const uint8_t TOKEN_CNT = 3;

State get_state();
void switch_led_state(State);

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  switch_led_state(get_state());
}

State get_state() {
  if (Serial.available()) {

    char token = Serial.read();
    
    for (uint8_t i = 0; i < TOKEN_CNT; i++) {
      if (TOKENS[i] == token) return static_cast<State>(i);
    }

    if (token == '0') {
      int sensor_val = analogRead(A0);
      Serial.println(sensor_val);
    }
    else if (token == '1') {
      int sensor_val = analogRead(A1);
      Serial.println(sensor_val);
    }
    
    return State::None;
  }
}

void switch_led_state(State state) {
  switch (state) {
    case State::Off:
    case State::On:
      digitalWrite(LED_PIN, static_cast<uint8_t>(state));
      break;
    case State::Blink:
      digitalWrite(LED_PIN, (millis() / LED_INTERVAL) % 2);
      break;
    default:
      break;
  }
}
