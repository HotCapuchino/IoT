#include <TroykaIMU.h>
#include <SoftwareSerial.h>

// Slave

Accelerometer accelerometer;
SoftwareSerial linkSerial(10, 11);

void setup() {
    Serial.begin(9600);
    accelerometer.begin();

    while (!Serial) continue;
    linkSerial.begin(4800);
    while (!linkSerial) continue;
}

void loop() {
    float ax = accelerometer.readAccelerationAX();
    float ay = accelerometer.readAccelerationAY();
    float az = accelerometer.readAccelerationAZ();

    Serial.println(accelerometer.readAccelerationAX());
    Serial.println(accelerometer.readAccelerationAY());
    Serial.println(accelerometer.readAccelerationAZ());

    linkSerial.write(String(ax, 3).c_str());
    linkSerial.write('#');
    linkSerial.write(String(ay, 3).c_str());
    linkSerial.write('#');
    linkSerial.write(String(az, 3).c_str());
    linkSerial.write('#');
    delay(1000);
}
