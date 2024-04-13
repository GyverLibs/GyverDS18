#include <GyverDS18.h>
GyverDS18 ds(2);  // пин

uint64_t addr = 0xCF0417505B78FF28;

void setup() {
    Serial.begin(115200);
    // первый запрос на измерение. Запрос ВСЕМ датчикам на линии
    ds.requestTemp();
}
void loop() {
    if (ds.ready()) {             // измерения готовы по таймеру
        // читаем КОНКРЕТНЫЙ датчик по адресу
        if (ds.readTemp(addr)) {  // если чтение успешно
            Serial.print("temp: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error");
        }

        ds.requestTemp();  // запрос следующего измерения ДЛЯ ВСЕХ
    }
}