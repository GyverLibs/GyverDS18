// полный тест датчика

#include <GyverDS18.h>
GyverDS18Single ds(2);

void setup() {
    Serial.begin(115200);

    if (ds.setResolution(12)) Serial.println("setResolution: ok");
    else Serial.println("setResolution: error");

    uint8_t res = ds.readResolution();
    if (res) {
        Serial.print("readResolution: ");
        Serial.println(res);
    } else Serial.println("readResolution: error");

    uint8_t power = ds.readPower();
    if (power == 0) Serial.println("readPower: error");
    else if (power == DS18_EXTERNAL) Serial.println("readPower: external");
    else if (power == DS18_PARASITE) Serial.println("readPower: parasite");

    if (ds.requestTemp()) Serial.println("requestTemp: ok");
    else Serial.println("requestTemp: error");

    if (ds.waitReady()) {
        if (ds.readTemp()) {
            Serial.print("readTemp: ");
            Serial.println(ds.getTemp());
        } else Serial.println("readTemp: error");
    }

    uint64_t addr = ds.readAddress();
    if (addr) {
        Serial.print("readAddress: ");
        gds::printAddress(addr, Serial);
    } else Serial.println("readAddress: error");

    // ram/eeprom manual
    ds.writeRAM(0xAB, 0xCD);
    ds.copyRAM();
    ds.recallRAM();
    uint8_t buf[5];
    ds.readRAM(buf);
    Serial.println(buf[2], HEX);
    Serial.println(buf[3], HEX);
    Serial.println(gds::calcTemp(buf) / 16.0);
    Serial.println(gds::calcResolution(buf));
}

void loop() {
}