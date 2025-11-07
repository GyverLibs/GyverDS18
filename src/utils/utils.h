#pragma once
#include <Arduino.h>

namespace gds {

struct RAM {
    uint8_t t_lsb;
    uint8_t t_msb;
    uint8_t th;
    uint8_t tl;
    uint8_t cfg;

    int16_t getTemp() {
        return t_lsb | (t_msb << 8);
    }
    uint8_t getRes() {
        return ((cfg >> 5) & 0xff) + 9;
    }
};

class Addr {
   public:
    uint64_t addr = 0;

    Addr() {}
    Addr(const uint64_t addr) : addr(addr) {}

    operator uint64_t() {
        return addr;
    }

    operator bool() {
        return addr;
    }

    uint8_t& operator[](uint8_t i) {
        return ((uint8_t*)&addr)[i];
    }

    void copyTo(uint8_t* buf) {
        memcpy(buf, &addr, 8);
    }

    void printTo(Print& pr, bool newline = true) {
        pr.print("0x");
        uint8_t* p = (uint8_t*)&addr;
        for (int8_t i = 7; i >= 0; i--) {
            pr.print(_getChar(p[i] >> 4));
            pr.print(_getChar(p[i] & 0xF));
        }
        if (newline) pr.println();
    }

    String toString() {
        String s("0x");
        s.reserve(18);
        uint8_t* p = (uint8_t*)&addr;
        for (int8_t i = 7; i >= 0; i--) {
            s += _getChar(p[i] >> 4);
            s += _getChar(p[i] & 0xF);
        }
        return s;
    }

   private:
    char _getChar(uint8_t b) {
        return b + ((b > 9) ? 55 : '0');
    }
};

class Timer {
   public:
    Timer(uint16_t prd) : _prd(prd) {}
    void start() {
        _tmr = millis();
        if (!_tmr) --_tmr;
    }
    void stop() {
        _tmr = 0;
    }
    void setPeriod(uint16_t prd) {
        _prd = prd;
    }
    uint16_t getPeriod() {
        return _prd;
    }
    bool running() {
        return _tmr;
    }
    bool ready() {
        return _tmr && elapsed();
    }
    bool elapsed() {
        return passed() >= _prd;
    }
    uint16_t passed() {
        return uint16_t(millis()) - _tmr;
    }

   private:
    uint16_t _tmr = 0, _prd;
};

// ====================== DEPRECATED ======================

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