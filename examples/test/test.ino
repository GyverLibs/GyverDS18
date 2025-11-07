// полный тест датчика

#include <GyverDS18.h>
GyverDS18Single ds(2);

void setup() {
    Serial.begin(115200);

    // resolution
    if (ds.setResolution(12)) Serial.println("setResolution: ok");
    else Serial.println("setResolution: error");

    uint8_t res = ds.readResolution();
    if (res) {
        Serial.print("readResolution: ");
        Serial.println(res);
    } else Serial.println("readResolution: error");

    // power
    uint8_t power = ds.readPower();
    if (power == DS18_PWR_ERROR) Serial.println("readPower: error");
    else if (power == DS18_EXTERNAL) Serial.println("readPower: external");
    else if (power == DS18_PARASITE) Serial.println("readPower: parasite");

    // temp
    if (ds.requestTemp()) Serial.println("requestTemp: ok");
    else Serial.println("requestTemp: error");

    if (ds.waitReady()) {
        if (ds.readTemp()) {
            Serial.print("readTemp: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("readTemp: error");
        }
    }

    // address
    gds::Addr addr = ds.readAddress();
    if (addr) {
        Serial.print("readAddress: ");
        addr.printTo(Serial);
    } else Serial.println("readAddress: error");

    // ram/eeprom manual
    ds.writeRAM(0xAB, 0xCD);
    ds.copyRAM();
    ds.recallRAM();

    gds::RAM ram;
    ds.readRAM(&ram);
    Serial.println(ram.th, HEX);
    Serial.println(ram.tl, HEX);
    Serial.println(ram.getTemp() / 16.0);
    Serial.println(ram.getRes());
}

void loop() {
}