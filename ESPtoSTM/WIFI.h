#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifimulti;
WiFiClient wifi_client;

String ip = "IP UNSET";

String id() {
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);

  String mac_id = String(mac[WL_MAC_ADDR_LENGTH - 2]) + String(mac[WL_MAC_ADDR_LENGTH - 1]);
  return mac_id;
}

bool start_AP() {
  IPAddress ap_ip(192, 168, 4, 1);
  WiFi.disconnect();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ap_ip, ap_ip, IPAddress(255, 255, 255, 0));
  WiFi.softAP((ssid_ap + " " + id()).c_str(), password_ap.c_str());

  Serial.println("WIFI AP start " + ssid_ap + " " + id());

  return true;
}

bool start_client() {
  wifimulti.addAP(ssid_client.c_str(), password_client.c_str());
  while (wifimulti.run() != WL_CONNECTED);
  return true;
}

void WIFI_init(bool mode_ap) {
  if (mode_ap) {
    start_AP();
    ip = WiFi.softAPIP().toString();
  }
  else {
    start_client();
    ip = WiFi.localIP().toString();
  }

  Serial.print("IP Address: ");
  Serial.println(ip);
}
