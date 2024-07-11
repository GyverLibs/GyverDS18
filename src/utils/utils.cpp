#include "utils.h"

namespace gds {

uint8_t calcResolution(uint8_t* buf) {
    return ((buf[4] >> 5) & 0xff) + 9;
}

int16_t calcTemp(uint8_t* buf) {
    return buf[0] | (buf[1] << 8);
}

void copyAddress(const uint64_t& address, uint8_t* buf) {
    buf64 addr{address};
    for (uint8_t i = 0; i < 8; i++) {
        buf[i] = addr.u8[i];
    }
}

static char _getChar(uint8_t b) {
    return (char)(b + ((b > 9) ? 55 : '0'));
}

void printAddress(const uint64_t& address, Print& p, bool newline) {
    buf64 addr{address};
    p.print("0x");
    for (uint8_t i = 0; i < 8; i++) {
        p.print(_getChar(addr.u8[7 - i] >> 4));
        p.print(_getChar(addr.u8[7 - i] & 0xF));
    }
    if (newline) p.println();
}

String addressToString(const uint64_t& address) {
    String s("0x");
    s.reserve(18);
    buf64 addr{address};
    for (uint8_t i = 0; i < 8; i++) {
        s += _getChar(addr.u8[7 - i] >> 4);
        s += _getChar(addr.u8[7 - i] & 0xF);
    }
    return s;
}

}  // namespace gds