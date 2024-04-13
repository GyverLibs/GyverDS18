#pragma once
#include <Arduino.h>

namespace gds {

// прочитать разрешение из внешнего буфера (5 байт)
uint8_t calcResolution(uint8_t* buf);

// прочитать температуру из внешнего буфера (5 байт)
int16_t calcTemp(uint8_t* buf);

// копировать адрес в буфер размером 8
void copyAddress(const uint64_t& address, uint8_t* buf);

// вывести адрес в Print
void printAddress(const uint64_t& address, Print& p, bool newline = true);

// вывести адрес в String
String addressToString(const uint64_t& address);

union buf64 {
    uint64_t u64;
    uint8_t u8[8];
};

}  // namespace gds