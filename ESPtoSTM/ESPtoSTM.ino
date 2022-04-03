#include "Config.h"
#include "WIFI.h"
#include "Server.h"
#include "MQTT.h"


void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  WIFI_init(MODE_AP);
  MQTT_init();
  if (SUB) {
    mqtt_client.subscribe((BASE_MQTT_TOPIC).c_str()); 
  }
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  mqtt_client.loop();
  if (Serial.available() > 0) {
    int val = int(Serial.read());

    if (!SUB) {
      mqtt_client.publish(String(BASE_MQTT_TOPIC).c_str(), String(val).c_str()); 
    }
  }
}
