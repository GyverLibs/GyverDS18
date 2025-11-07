// автоматический опрос по таймеру

#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);
    ds.setResolution(12);
}
void loop() {
    // по готовности и успешному чтению
    if (!ds.tick()) {
        Serial.println(ds.getTemp());
    }

    // с обработкой ошибок
    // switch (ds.tick()) {
    //     case DS18_READY:
    //         Serial.println(ds.getTemp());
    //         break;

    //     case DS18_ERROR:
    //         Serial.println("Error");
    //         break;
    // }
}