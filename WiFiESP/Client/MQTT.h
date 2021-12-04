#include <PubSubClient.h>

PubSubClient mqtt_client(wifi_client);

void callback(char* topic, byte* payload, uint32_t len) {
  String payload_string = "";
  
  for (auto i = 0; i < len; i++) {
    payload_string += char(payload[i]);
    Serial.print(char(payload[i]));
  }

  if (payload_string == "on") {
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, HIGH);
  }

  Serial.println("-------------------------");
}

void MQTT_init() {
  mqtt_client.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt_client.setCallback(callback);
  
  String client_id = ssid_ap + String(WiFi.macAddress());

  while (!mqtt_client.connected()) {
    if (mqtt_client.connect(client_id.c_str())) {
      Serial.println("MQTT connected with id (" + client_id + ")");
    }
    else {  
      Serial.println("MQTT failed");
    }
  }
}
