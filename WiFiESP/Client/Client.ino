#include "Config.h"
#include "WIFI.h"
#include "Server.h"
#include "MQTT.h"

void setup() {
  Serial.begin(115200); 
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  WIFI_init(MODE_AP);

  server_init();

  MQTT_init();  
  mqtt_client.publish((BASE_MQTT_TOPIC + String("/state")).c_str(), "Hello");
  mqtt_client.subscribe((BASE_MQTT_TOPIC + String("/command")).c_str());
}

void loop() {
  server.handleClient();
  int analogValue = analogRead(A0);
  mqtt_client.loop();
  if (analogValue <  40) {
    mqtt_client.publish((BASE_MQTT_TOPIC + String("/state")).c_str(), "on");
  } else {
    mqtt_client.publish((BASE_MQTT_TOPIC + String("/state")).c_str(), "off");
  }
//  mqtt_client.subscribe((BASE_MQTT_TOPIC + String("/command")).c_str());
}
