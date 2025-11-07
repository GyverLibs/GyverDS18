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
        res = constrain(res, 9, 12);
        applyResolution(res);
        return GyverDS18Single::writeRAM(0, 0, res);
    }

    // установить разрешение (9.. 12 бит)
    bool setResolution(uint8_t res, const uint64_t& addr) {
        res = constrain(res, 9, 12);
        gds::RAM ram;
        return readRAM(&ram, addr) && writeRAM(ram.th, ram.tl, res, addr);
    }

    // прочитать разрешение
    uint8_t readResolution(const uint64_t& addr) {
        gds::RAM ram;
        return readRAM(&ram, addr) ? ram.getRes() : 0;
    }

    // прочитать питание: DS18_PARASITE - паразитное, DS18_EXTERNAL - обычное, DS18_PWR_ERROR - ошибка
    uint8_t readPower(const uint64_t& addr) {
        return _beginAddr(addr) ? _readPower() : DS18_PWR_ERROR;
    }

    // ===================== TEMP =====================

    using GyverDS18Single::getTemp;
    using GyverDS18Single::getTempInt;
    using GyverDS18Single::getTempRaw;
    using GyverDS18Single::isWaiting;
    using GyverDS18Single::ready;
    using GyverDS18Single::requestTemp;
    using GyverDS18Single::setPeriod;
    using GyverDS18Single::waitReady;

    // автоматический запрос по таймеру для всех, вызывать в loop. Вернёт DS18_READY (0) по готовности
    uint8_t tick() {
        if (_tmr.elapsed()) {
            requestTemp();
            _tmr.start();
            return DS18_READY;
        }
        return DS18_IDLE;
    }

    // запросить температру
    bool requestTemp(const uint64_t& addr) {
        return _beginAddr(addr) ? _requestTemp() : false;
    }

    // прочитать температуру
    bool readTemp(const uint64_t& addr) {
        gds::RAM ram;
        return readRAM(&ram, addr) ? _saveTemp(ram.getTemp()) : false;
    }

    // ===================== MANUAL =====================

    // прочитать содержимое оперативной памяти в буфер 5 байт
    bool readRAM(uint8_t* buf, const uint64_t& addr) {
        return _beginAddr(addr) ? _readRAM(buf) : false;
    }
    bool readRAM(gds::RAM* ram, const uint64_t& addr) {
        return readRAM((uint8_t*)ram, addr);
    }

    // записать данные в оперативную память (th, tl, res)
    bool writeRAM(uint8_t th, uint8_t tl, uint8_t res, const uint64_t& addr) {
        return _beginAddr(addr) ? _writeRAM(th, tl, res) : false;
    }

    // записать данные в оперативную память (th, tl)
    bool writeRAM(uint8_t th, uint8_t tl, const uint64_t& addr) {
        return writeRAM(th, tl, readResolution(addr), addr);
    }

    // записать содержимое оперативной памяти в EEPROM
    bool copyRAM(const uint64_t& addr) {
        return _beginAddr(addr) ? _copyRAM() : false;
    }

    // записать содержимое EEPROM в оперативную память
    bool recallRAM(const uint64_t& addr) {
        return _beginAddr(addr) ? write(DS18_RECALL_RAM) : false;
    }

    // ===================== PRIVATE =====================
   private:
    bool _beginAddr(const uint64_t& addr) {
        if (_parasite) pullup(false);
        if (!addr || !reset()) return false;

        write(DS18_ADDR_MATCH);
        for (uint8_t i = 0; i < 8; i++) {
            write(((uint8_t*)&addr)[i]);
        }
        return true;
    }
};
