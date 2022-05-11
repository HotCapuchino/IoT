// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

//int sendTo;
//int recvFrom = -1;
int current_address = 17; // address to join i2c bus

static String payload = "";
//static String recvPayload = "";

void setup() {
  Serial.begin(9600);   
  Wire.begin(current_address); // join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent);
  TWAR = (current_address << 1) | 1; // enables listening broadcast channel
}

unsigned long t = millis();


void receiveEvent(int howMany) {

  String recvPayload = "";
  
  while (Wire.available()) {
    char c = Wire.read();

    if (c == '\n') {
      Serial.println("payload: " + String(recvPayload.c_str()));
      int recvFrom = atoi(recvPayload.c_str());
      int first_space_index = recvPayload.indexOf(' ');
      String message = "";   

      if (first_space_index > 0) {
        message = recvPayload.substring(recvPayload.indexOf(' ') + 1);
      }

      Serial.println("message: " + String(message.c_str()));
      Serial.println("recieved from: " + String(recvFrom));

      recvPayload = "";
      
    } else {
      recvPayload += c;
    }
  } 
}


void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      int sendTo = atoi(payload.c_str());
      int first_space_index = payload.indexOf(' ');
      String message = "";
      
      if (first_space_index > 0) {
        message = payload.substring(payload.indexOf(' ') + 1);
      }

      if (sendTo > 127 || sendTo < 0) {
        Serial.println("Wrong reciever address!");
      } else if (message.length() == 0) {
        Serial.println("Message is empty!");  
      } else {
        Serial.println("send to " + String(sendTo));
        
        Wire.beginTransmission(sendTo);

        Serial.print("message: ");
        delay(100);
        Serial.read();

        Wire.write(current_address);
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
