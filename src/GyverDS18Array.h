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
        return GyverDS18::setResolution(res, _get(n));
    }

    // прочитать разрешение
    uint8_t readResolution(uint8_t n) {
        return GyverDS18::readResolution(_get(n));
    }

    // прочитать питание: DS18_PARASITE - паразитное, DS18_EXTERNAL - обычное, 0 - ошибка
    uint8_t readPower(uint8_t n) {
        return GyverDS18::readPower(_get(n));
    }

    // ===================== TEMP =====================

    using GyverDS18::getTemp;
    using GyverDS18::getTempInt;
    using GyverDS18::getTempRaw;
    using GyverDS18::isWaiting;
    using GyverDS18::ready;
    using GyverDS18::setPeriod;
    using GyverDS18::tick;
    using GyverDS18::waitReady;

    // прочитать в массив. NAN если ошибка чтения
    void readTemps(float* arr) {
        for (uint8_t i = 0; i < _amount; i++) {
            arr[i] = readTemp(i) ? getTemp() : NAN;
        }
    }

    // запросить температуру у всех
    bool requestTemp() {
        return GyverDS18::requestTemp();
    }

    // запросить температру
    bool requestTemp(uint8_t n) {
        return GyverDS18::requestTemp(_get(n));
    }

    // прочитать температуру
    bool readTemp(uint8_t n) {
        return GyverDS18::readTemp(_get(n));
    }

    // ===================== MANUAL =====================

    // прочитать содержимое оперативной памяти в буфер 5 байт
    bool readRAM(uint8_t* buf, uint8_t n) {
        return GyverDS18::readRAM(buf, _get(n));
    }

    // записать данные в оперативную память (th, tl, cfg)
    bool writeRAM(uint8_t b0, uint8_t b1, uint8_t n) {
        return GyverDS18::writeRAM(b0, b1, _get(n));
    }

    // записать содержимое оперативной памяти в EEPROM
    bool copyRAM(uint8_t n) {
        return GyverDS18::copyRAM(_get(n));
    }

    // записать содержимое EEPROM в оперативную память
    bool recallRAM(uint8_t n) {
        return GyverDS18::recallRAM(_get(n));
    }

    // ===================== PRIVATE =====================

   private:
    uint64_t* _addr = nullptr;
    uint8_t _amount = 0;

    uint64_t _get(uint8_t n) {
        return (_addr && n < _amount) ? _addr[n] : 0;
    }
};