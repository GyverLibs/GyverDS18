#include <Arduino.h>
#include <GyverDS18.h>

GyverDS18Single ds(D3, false);

void setup() {
    Serial.begin(9600);
    gds::buf64 addr{0};

    while(ds.search(&addr)) {
      for(byte i = 0; i < 8; i++) Serial.printf("%02X", addr.u8[i]);
      Serial.println();
  }
}

void loop() {
}
