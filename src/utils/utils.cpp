#include "utils.h"

namespace gds {

void crc8(uint8_t& crc, uint8_t data) {
#ifdef __AVR__
    uint8_t counter = 0;
    uint8_t buffer = 0;
    asm volatile(
        "EOR %[crc_out], %[data_in] \n\t"
        "LDI %[counter], 8          \n\t"
        "LDI %[buffer], 0x8C        \n\t"
        "_loop_start_%=:            \n\t"
        "LSR %[crc_out]             \n\t"
        "BRCC _loop_end_%=          \n\t"
        "EOR %[crc_out], %[buffer]  \n\t"
        "_loop_end_%=:              \n\t"
        "DEC %[counter]             \n\t"
        "BRNE _loop_start_%="
        : [crc_out] "=r"(crc), [counter] "=d"(counter), [buffer] "=d"(buffer)
        : [crc_in] "0"(crc), [data_in] "r"(data));
#else
    for (uint8_t i = 0; i < 8; i++) {
        crc = ((crc ^ data) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
        data >>= 1;
    }
#endif
}

uint8_t calcResolution(uint8_t* buf) {
    return ((buf[4] >> 5) & 0x03) + 9;
}

int16_t calcTemp(uint8_t* buf) {
    return int16_t(uint16_t(buf[0]) | (uint16_t(buf[1]) << 8));
}

void copyAddress(const uint64_t& addr, uint8_t* buf) {
    memcpy(buf, &addr, 8);
}

static char _getChar(uint8_t b) {
    return (char)(b + ((b > 9) ? 55 : '0'));
}

void printAddress(const uint64_t& addr, Print& pr, bool newline) {
    pr.print("0x");
    uint8_t* p = (uint8_t*)&addr;
    for (int8_t i = 7; i >= 0; i--) {
        pr.print(_getChar(p[i] >> 4));
        pr.print(_getChar(p[i] & 0xF));
    }
    if (newline) pr.println();
}

String addressToString(const uint64_t& addr) {
    String s("0x");
    s.reserve(18);
    uint8_t* p = (uint8_t*)&addr;
    for (int8_t i = 7; i >= 0; i--) {
        s += _getChar(p[i] >> 4);
        s += _getChar(p[i] & 0xF);
    }
    return s;
}

}  // namespace gds
