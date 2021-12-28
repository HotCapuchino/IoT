#include <TroykaIMU.h>
#include <SoftwareSerial.h>

// Master

Accelerometer accelerometer;
SoftwareSerial linkSerial(10, 11);
float wrist[3];

void setup() {
    Serial.begin(9600);
    accelerometer.begin();

    while (!Serial) continue;
    linkSerial.begin(4800);
    while (!linkSerial) continue;
}

String define_gesture(float arm_az) {
    if (arm_az >= 1.5 && arm_az <= 5.5) {
      return "Thumb Up!";
    } else {
      if (wrist[0] >= -9 && wrist[0] <= -5) {
        return "Wrist In!";
      } else if (wrist[0] <= 8 && wrist[0] >= 4) {
        return "Wrist Out!";
      } else {
        return "Unrecognizable gesture!";
      }
    }
    return "Unrecognizable gesture!";
}

void loop() {
  
    float arm_ax = accelerometer.readAccelerationAX();
    float arm_ay = accelerometer.readAccelerationAY();
    float arm_az = accelerometer.readAccelerationAZ();

    if (linkSerial.available()) {
      String result = "";
      while (linkSerial.available()) {
        result += (char)linkSerial.read();
      }

      int str_index = 0;
      for (int i = 0; i < 3; i++) {
        String current_str = "";
        
        while (result[str_index] != '#') {
          current_str += result[str_index];
          str_index++;
        }
        wrist[i] = current_str.toFloat();
        str_index += 1;
      }
    }
    
    Serial.println(define_gesture(arm_az));
    
    delay(1000);
}
