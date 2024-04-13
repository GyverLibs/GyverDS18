#pragma once
#include <Arduino.h>

namespace gds {

// прочитать разрешение из внешнего буфера (5 байт)
uint8_t calcResolution(uint8_t* buf);

// прочитать температуру из внешнего буфера (5 байт)
int16_t calcTemp(uint8_t* buf);

// копировать адрес в буфер размером 8
void copyAddress(uint64_t address, uint8_t* buf);

// вывести адрес в Print
void printAddress(uint64_t address, Print& p, bool newline = true);

}  // namespace gds