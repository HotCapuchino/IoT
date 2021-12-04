#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

void handle_root();
void handle_led();
void handle_sensor();

void server_init() {
  server.on("/", HTTP_GET, handle_root);
  server.on("/LED", HTTP_POST, handle_led);
  server.on("/sensor", HTTP_GET, handle_sensor);
  server.begin();

  Serial.println("HTTP server started");
}

void handle_root() {
  server.send(
    200,
    "text/html",
    "<form action='/LED' method='POST'><button>Switch</button></form><a href='/sensor'>GET SENSOR VALUE</a>"
  );
}

void handle_led() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  server.sendHeader("Location", "/");
  server.send(303);
}

void handle_sensor() {
  auto sensor_value = analogRead(A0);
  server.send(
    200,
    "text/html",
    "Sensor value: " + String(sensor_value)
  );
}
