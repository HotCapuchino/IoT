String ssidAP = "esp8266 ar";
String passwordAP = "qwerty";
String toggleLEDButton = "<form action=\"/LED\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\">Click to toggle LED</input></form>";
int led_pin = 2;
int analog_pin = A0;

String ssidCLI = "AndrewTheBest";
String passwordCLI = "qwerty";

bool MODE_AP = true;

const char* mqtt_broker = "broker.emqx.io";
int mqtt_port = 1883;
String client_id = "";
