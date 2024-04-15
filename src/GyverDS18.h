#pragma once
#include <Arduino.h>

#include "GyverDS18Single.h"

class GyverDS18 : protected GyverDS18Single {
   public:
    using GyverDS18Single::GyverDS18Single;

    // ===================== SYSTEM =====================

    using GyverDS18Single::getConversionTime;
    using GyverDS18Single::readAddress;
    using GyverDS18Single::setParasite;
    using GyverDS18Single::setPin;

    // установить разрешение (9.. 12 бит) для всех
    bool setResolution(uint8_t res) {
        return GyverDS18Single::setResolution(res);
    }

    // установить разрешение (9.. 12 бит)
    bool setResolution(uint8_t res, const uint64_t& addr) {
        applyResolution(res);
        return writeRAM(0x00, 0xff, addr);
    }

    // прочитать разрешение
    uint8_t readResolution(const uint64_t& addr) {
        uint8_t buf[5];
        return readRAM(buf, addr) ? gds::calcResolution(buf) : 0;
    }

    // прочитать питание: DS18_PARASITE - паразитное, DS18_EXTERNAL - обычное, 0 - ошибка
    uint8_t readPower(const uint64_t& addr) {
        return _beginAddr(addr) ? _readPower() : 0;
    }

    // ===================== TEMP =====================

    using GyverDS18Single::getTemp;
    using GyverDS18Single::getTempInt;
    using GyverDS18Single::getTempRaw;
    using GyverDS18Single::isWaiting;
    using GyverDS18Single::ready;
    using GyverDS18Single::waitReady;

    // запросить температуру у всех
    bool requestTemp() {
        return GyverDS18Single::requestTemp();
    }

    // запросить температру
    bool requestTemp(const uint64_t& addr) {
        return _beginAddr(addr) ? _requestTemp() : 0;
    }

    // прочитать температуру
    bool readTemp(const uint64_t& addr) {
        uint8_t buf[5];
        return readRAM(buf, addr) ? _readTemp(gds::calcTemp(buf)) : 0;
    }

    // ===================== MANUAL =====================

    using GyverDS18Single::applyResolution;

    // прочитать содержимое оперативной памяти в буфер 5 байт
    bool readRAM(uint8_t* buf, const uint64_t& addr) {
        return _beginAddr(addr) ? _readRAM(buf) : 0;
    }

    // записать данные в оперативную память (th, tl)
    bool writeRAM(uint8_t b0, uint8_t b1, const uint64_t& addr) {
        return _beginAddr(addr) ? _writeRAM(b0, b1, _getConfig()) : 0;
    }

    // записать содержимое оперативной памяти в EEPROM
    bool copyRAM(const uint64_t& addr) {
        return _beginAddr(addr) ? _copyRAM() : 0;
    }

    // записать содержимое EEPROM в оперативную память
    bool recallRAM(const uint64_t& addr) {
        return _beginAddr(addr) ? write(DS18_RECALL_RAM) : 0;
    }

    // ===================== PRIVATE =====================
   private:
    bool _beginAddr(const uint64_t& address) {
        if (_parasite) pullup(false);
        if (!reset()) return 0;

        write(DS18_ADDR_MATCH);
        gds::buf64 addr{address};
        for (uint8_t i = 0; i < 8; i++) {
            write(addr.u8[i]);
        }
        return 1;
    }
};
