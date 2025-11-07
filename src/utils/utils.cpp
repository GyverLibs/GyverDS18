#include "utils.h"

namespace gds {

uint8_t calcResolution(uint8_t* buf) {
    return ((buf[4] >> 5) & 0xff) + 9;
}

int16_t calcTemp(uint8_t* buf) {
    return buf[0] | (buf[1] << 8);
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