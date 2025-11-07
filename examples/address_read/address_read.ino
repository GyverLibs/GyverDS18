#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);

    gds::Addr addr = ds.readAddress();
    if (addr) {
        Serial.print("address: ");
        addr.printTo(Serial);
    } else {
        Serial.println("error");
    }
}

void loop() {
}