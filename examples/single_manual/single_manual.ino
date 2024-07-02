// пример с запросом и чтением по своему таймеру

#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);
}

void loop() {
    static uint32_t tmr;
    // время таймера НЕ МЕНЬШЕ чем текущее getConversionTime()
    if (millis() - tmr >= ds.getConversionTime()) {
        tmr = millis();

        // чтение
        if (ds.readTemp()) {
            Serial.print("temp: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("read error");
        }

        // запрос
        if (!ds.requestTemp()) {
            Serial.println("request error");
        }
    }
}

// второй вариант
/*
void loop() {
    // запрос
    static uint32_t tmr;
    // период таймера не меньше, чем getConversionTime()
    if (millis() - tmr >= 1000) {
        tmr = millis();
        if (!ds.requestTemp()) Serial.println("request error");
    }


    // чтение
    if (ds.ready()) {
        if (ds.readTemp()) {
            Serial.print("temp: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("read error");
        }
    }
}
*/