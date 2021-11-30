#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", toggleLEDButton);
}

void handleLED() {
  digitalWrite(led_pin, !digitalRead(led_pin));
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleGetSensor() {
  int sensor = analogRead(analog_pin);
  server.send(200, "text/plain", String(sensor));
}

void server_init() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/LED", HTTP_POST, handleLED);
  server.on("/sensor", HTTP_GET, handleGetSensor);
}
