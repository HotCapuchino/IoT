#include <PubSubClient.h>

PubSubClient mqtt_client(wifi_client);

void callback(char* topic, byte* payload, uint32_t len) {
  String payload_string = "";

  char data = 0;
  char order = 1;
  for (int i = len - 1 ; i >= 0; i--) {
    data += char((char) payload[i] - '0') * order;
    order *= 10;
  }
  Serial.write(data);
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
