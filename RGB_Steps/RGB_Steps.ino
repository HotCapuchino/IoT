#include "Config.h"
#include "WIFI.h"
#include "leds.h"
#include "MQTT.h"


void setup(void){
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  for(int i=0; i< 3; i++) {
    digitalWrite(led, !digitalRead(led));
    delay(500);
  }
  leds_init();
  WIFI_init(false);
  MQTT_init();
  mqtt_cli.publish("lab/leds/strip/state", "hello emqx");
//  mqtt_cli.subscribe("lab/leds/strip/set_leds");
  mqtt_cli.subscribe(String("lab/leds/strip/set_leds_bytes/" + String(ledID)).c_str());
  mqtt_cli.subscribe(String("lab/leds/strip/rotate_leds/" + String(ledID)).c_str());
  
}

void loop(void){                
  mqtt_cli.loop();
}
