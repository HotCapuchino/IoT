#include <Wire.h>

int sendTo;
int current_address = 2; // address to join i2c bus

static String payload = "";
static String recvPayload = "";

void setup() {
  Serial.begin(9600);   
  Wire.begin(current_address); // join i2c bus (address optional for master)
  Wire.onReceive(recieveEvent);
  TWAR = (current_address << 1) | 1; // enables listening broadcast channel
}

byte receive_device_id() {
  byte device_id = -1;
  byte order = 1;
  
  while (Wire.available()) {
    byte digit = Wire.read();

    if (digit == ' ' || digit == '\0') break;

    digit -= '0';

    if (device_id == byte(-1)) device_id = 0;

    device_id = device_id * order + digit;
    order *= 10;
  }

  return device_id;
}

String receive_message() {
  String message = "";

  while (Wire.available()) {
    message += char(Wire.read());
  }

  return message;
}


void recieveEvent(int howMany) {
  byte recvFrom = receive_device_id();
  String message = receive_message();

  if (recvFrom == byte(-1)) {
    return;
  }
  
  Serial.println("message: " + message);
  Serial.println("recieved from " + String(recvFrom));
}


void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      int first_space_index = payload.indexOf(' ');
      String message = "";
      
      if (first_space_index > 0) {
        message = payload.substring(payload.indexOf(' ') + 1);
        String number_chars = payload.substring(0, payload.indexOf(' '));
        
        bool is_number = true;
        for (int i = 0; i < number_chars.length(); i++) {
          if (!isDigit(number_chars[i])) {
            is_number = false;
            break;
          }
        }
        sendTo = is_number ? atoi(number_chars.c_str()) : -1;
      }

      if (sendTo > 127 || sendTo < 0) {
        Serial.println("Wrong reciever address!");
      } else if (message.length() == 0) {
        Serial.println("Message is empty!");  
      } else {
        Serial.println("send to " + String(sendTo));
        
        Wire.beginTransmission(sendTo);

        Serial.print("message: ");
        Serial.read();

        Wire.write(String(current_address).c_str());
        Wire.write(' ');
        for (int i = 0; i < message.length(); i++) {
          Serial.print(message[i]);
          Wire.write(message[i]);
        }
        Serial.print("\n");
  
        Wire.endTransmission();
      }
      
      payload = ""; 
    }
    else {
      payload += c;
    }
  }
}
