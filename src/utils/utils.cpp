#include "utils.h"

namespace gds {

uint8_t calcResolution(uint8_t* buf) {
    return ((buf[4] >> 5) & 0xff) + 9;
}

int16_t calcTemp(uint8_t* buf) {
    return buf[0] | (buf[1] << 8);
}

void copyAddress(uint64_t address, uint8_t* buf) {
    for (uint8_t i = 0; i < 8; i++) {
        buf[i] = (uint8_t)address;
        address >>= 8;
    }
}

void printAddress(uint64_t address, Print& p, bool newline) {
    p.print("0x");
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t b = (uint8_t)address;
        if (b < 0xf) p.print(0);
        p.print(b, HEX);
        address >>= 8;
    }
    if (newline) p.println();
}

}  // namespace gds