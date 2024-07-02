// простой пример с цикличным чтением
// примечание: запрос делается только после удачного чтения
// Если датчик отключился - цикл прервётся! Смотри пример single_manual

#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);
    ds.requestTemp();  // первый запрос на измерение
}
void loop() {
    if (ds.ready()) {         // измерения готовы по таймеру
        if (ds.readTemp()) {  // если чтение успешно
            Serial.print("temp: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error");
        }

        ds.requestTemp();  // запрос следующего измерения
    }
}