// автоматический опрос по таймеру

#include <GyverDS18.h>
GyverDS18 ds(2);  // пин

void setup() {
    Serial.begin(115200);
    ds.setResolution(12);   // для всех
}

void loop() {
    // это таймер с периодом к setResolution, он сам делает request
    if (!ds.tick()) {

        if (ds.readTemp(0x4D0417508099FF28)) {
            Serial.print("temp 1: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error 1");
        }

        if (ds.readTemp(0xC3041750E553FF28)) {
            Serial.print("temp 2: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error 2");
        }
    }
}