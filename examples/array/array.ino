#include <GyverDS18Array.h>

uint64_t addr[] = {
    0xD20417515A42FF28,
    0x4D0417508099FF28,
    0xFE04175159CDFF28,
    0xCF0417505B78FF28,
};
GyverDS18Array ds(2, addr, 4);

void setup() {
    Serial.begin(115200);
    // Запрос ВСЕМ датчикам на линии
    ds.requestTemp();
}
void loop() {
    if (ds.ready()) {  // измерения готовы по таймеру
        // проходим по массиву
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

        ds.requestTemp();  // запрос следующего измерения ДЛЯ ВСЕХ
    }
}