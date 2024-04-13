#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);

    uint64_t addr = ds.readAddress();
    if (addr) {
        Serial.print("address: ");
        gds::printAddress(addr, Serial);
    } else {
        Serial.println("error");
    }
}

void loop() {
}