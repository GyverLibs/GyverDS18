// автоматический опрос по таймеру

#include <GyverDS18Array.h>

uint64_t addr[] = {
    0x4D0417508099FF28,
    0xFE04175159CDFF28,
    0xC3041750E553FF28,
    0xCF0417505B78FF28,
};
GyverDS18Array ds(2, addr, 4);

void setup() {
    Serial.begin(115200);
    ds.setResolution(12);  // для всех
}

void loop() {
    // это таймер с периодом к setResolution, он сам делает request
    if (!ds.tick()) {
        
        float temps[4];
        ds.readTemps(temps);
        for (float t : temps) {
            Serial.println(t);
        }
        Serial.println();

        /*
        // вручную
        for (int i = 0; i < ds.amount(); i++) {
            Serial.print("#");
            Serial.print(i);
            Serial.print(": ");
            // читаем по индексу
            if (ds.readTemp(i)) Serial.print(ds.getTemp());
            else Serial.print("error");
            Serial.print(", ");
        }
        Serial.println();
        */
    }
}