#pragma once
#include <Arduino.h>

#include "GyverDS18.h"

class GyverDS18Array : protected GyverDS18 {
   public:
    GyverDS18Array() {}
    GyverDS18Array(uint8_t pin, uint64_t* addr = nullptr, uint8_t amount = 0, bool parasite = 1) : GyverDS18(pin, parasite), _addr(addr), _amount(amount) {}

    // ===================== SYSTEM =====================

    using GyverDS18::getConversionTime;
    using GyverDS18::readAddress;
    using GyverDS18::setParasite;
    using GyverDS18::setPin;

    // подключить массив адресов формата uint64_t[]
    void setAddress(uint64_t* addr, uint8_t amount) {
        _addr = addr;
        _amount = amount;
    }

    // получить количество адресов в массиве
    uint8_t amount() {
        return _amount;
    }

    // установить разрешение (9.. 12 бит) для всех
    bool setResolution(uint8_t res) {
        return GyverDS18::setResolution(res);
    }

    // установить разрешение (9.. 12 бит)
    bool setResolution(uint8_t res, uint8_t n) {
        return _valid(n) ? GyverDS18::setResolution(res, _addr[n]) : 0;
    }

    // прочитать разрешение
    uint8_t readResolution(uint8_t n) {
        return _valid(n) ? GyverDS18::readResolution(_addr[n]) : 0;
    }

    // прочитать питание: DS18_PARASITE - паразитное, DS18_EXTERNAL - обычное, 0 - ошибка
    uint8_t readPower(uint8_t n) {
        return _valid(n) ? GyverDS18::readPower(_addr[n]) : 0;
    }

    // ===================== TEMP =====================

    using GyverDS18::getTemp;
    using GyverDS18::getTempInt;
    using GyverDS18::getTempRaw;
    using GyverDS18::isWaiting;
    using GyverDS18::ready;
    using GyverDS18::waitReady;

    // запросить температуру у всех
    bool requestTemp() {
        return GyverDS18::requestTemp();
    }

    // запросить температру
    bool requestTemp(uint8_t n) {
        return _valid(n) ? GyverDS18::requestTemp(_addr[n]) : 0;
    }

    // прочитать температуру
    bool readTemp(uint8_t n) {
        return _valid(n) ? GyverDS18::readTemp(_addr[n]) : 0;
    }

    // ===================== MANUAL =====================

    using GyverDS18::applyResolution;

    // прочитать содержимое оперативной памяти в буфер 5 байт
    bool readRAM(uint8_t* buf, uint8_t n) {
        return _valid(n) ? GyverDS18::readRAM(buf, _addr[n]) : 0;
    }

    // записать данные в оперативную память (th, tl, cfg)
    bool writeRAM(uint8_t b0, uint8_t b1, uint8_t n) {
        return _valid(n) ? GyverDS18::writeRAM(b0, b1, _addr[n]) : 0;
    }

    // записать содержимое оперативной памяти в EEPROM
    bool copyRAM(uint8_t n) {
        return _valid(n) ? GyverDS18::copyRAM(_addr[n]) : 0;
    }

    // записать содержимое EEPROM в оперативную память
    bool recallRAM(uint8_t n) {
        return _valid(n) ? GyverDS18::recallRAM(_addr[n]) : 0;
    }

    // ===================== PRIVATE =====================

   private:
    uint64_t* _addr = nullptr;
    uint8_t _amount = 0;

    bool _valid(const uint8_t& n) {
        return (_addr && _amount && n < _amount);
    }
};