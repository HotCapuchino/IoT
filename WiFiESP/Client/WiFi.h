#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>

String ip = "unset";
ESP8266WiFiMulti wifimulti;
WiFiClient wificlient;


String id() {
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2]) + String(mac[WL_MAC_ADDR_LENGTH - 1]);
  return macID;
}

bool startClient() {
  wifimulti.addAP(ssidCLI.c_str(), passwordCLI.c_str());
  while(wifimulti.run() != WL_CONNECTED) {

  }
  return true;
}

bool startAP() {
  Serial.print("starting");
  IPAddress appIP(192, 168, 4, 1);
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAP((ssidAP + " " + id()).c_str(), passwordAP.c_str());
  Serial.println("WiFi AP start " + ssidAP + " " + id());
  return true;
}

void WiFi_init(bool mode_ap) {
  startAP();
  if (mode_ap) {
    startAP();
    ip = WiFi.softAPIP().toString();
  } else {
    startClient();
    ip = WiFi.localIP().toString();
  }
  Serial.println("IP Address");
  Serial.println(ip);
}
