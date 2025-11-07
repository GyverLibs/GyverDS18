#include <GyverDS18.h>

#define DS_PIN 2

void setup() {
    Serial.begin(115200);

    // поиск в массив
    {
        uint64_t addr[5] = {};
        uint8_t res = gds::Search(DS_PIN).scan(addr, 5);
        Serial.println(res);  // кол-во найденных

        for (uint64_t& a : addr) {
            // вывод в порт
            gds::Addr(a).printTo(Serial);
        }
    }

    // цикличный поиск
    {
        gds::Search s(DS_PIN);
        while (s.search()) {
            s.printTo(Serial);
        }
    }

    // поиск по индексу
    {
        gds::Search(DS_PIN).find(0).printTo(Serial);  // печать [1]

        gds::Search s(DS_PIN);
        s.find(1).printTo(Serial);  // печать [2]
        uint64_t adr = s.find(2);   // как uint64_t [3]
    }
}

void loop() {
}