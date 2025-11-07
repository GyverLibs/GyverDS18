#pragma once
#include <Arduino.h>

#include "./utils/utils.h"
#include "GyverOneWire.h"
#include "Search.h"

// CONST
#define DS18_READY 0
#define DS18_IDLE 1
#define DS18_ERROR 2

#define DS18_BAD_TEMP 0

#define DS18_PWR_ERROR 0
#define DS18_EXTERNAL 1
#define DS18_PARASITE 2

#define DS18_TCONV_BASE 94
#define DS18_TCONV_OFFSET 5
#define DS18_TCONV_RES(res) ((DS18_TCONV_BASE << (res - 9)) + DS18_TCONV_OFFSET)

#define DS18_TCONV_9 DS18_TCONV_RES(9)
#define DS18_TCONV_10 DS18_TCONV_RES(10)
#define DS18_TCONV_11 DS18_TCONV_RES(11)
#define DS18_TCONV_12 DS18_TCONV_RES(12)

// CMD
// #define DS18_ADDR_SEARCH 0xF0
#define DS18_ADDR_READ 0x33
#define DS18_ADDR_MATCH 0x55
#define DS18_ADDR_SKIP 0xCC
#define DS18_ALM_SEARCH 0xEC
#define DS18_CONVERT 0x44
#define DS18_READ_RAM 0xBE
#define DS18_WRITE_RAM 0x4E
#define DS18_COPY_RAM 0x48
#define DS18_RECALL_RAM 0xB8
#define DS18_READ_POWER 0xB4

class GyverDS18Single : protected GyverOneWire {
   public:
    GyverDS18Single() {}
    GyverDS18Single(uint8_t pin, bool parasite = true) {
        setPin(pin);
        _parasite = parasite;
    }

    // ===================== SYSTEM =====================

    using GyverOneWire::reset;
    using GyverOneWire::setPin;

    // включить режим паразитного питания (умолч. вкл)
    void setParasite(bool parasite) {
        _parasite = parasite;
    }

    // установить разрешение (9.. 12 бит)
    bool setResolution(uint8_t res) {
        res = constrain(res, 9, 12);
        gds::RAM ram;
        return readRAM(&ram) && writeRAM(ram.th, ram.tl, res) && applyResolution(res);
    }

    // прочитать разрешение, 0 если ошибка
    uint8_t readResolution() {
        gds::RAM ram;
        return readRAM(&ram) ? ram.getRes() : 0;
    }

    // получить текущее время измерения температуры, мс
    uint16_t getConversionTime() {
        return _tmr.getPeriod();
    }

    // прочитать адрес датчика. 0 - ошибка
    uint64_t readAddress() {
        if (!reset() && !reset()) return 0;  // 2x reset

        write(DS18_ADDR_READ);
        uint64_t addr = 0;
        uint8_t crc = 0;
        for (uint8_t i = 0; i < 8; i++) {
            uint8_t r = readByte();
            ((uint8_t*)&addr)[i] = r;
            _crc8(crc, r);
        }
        return crc ? 0 : addr;
    }

    // прочитать питание: DS18_PARASITE - паразитное, DS18_EXTERNAL - обычное, DS18_PWR_ERROR - ошибка
    uint8_t readPower() {
        return _beginSkip() ? _readPower() : DS18_PWR_ERROR;
    }

    // ===================== TEMP =====================

    // автоматический опрос по таймеру, вызывать в loop. Вернёт DS18_READY (0) по готовности и чтению
    uint8_t tick() {
        if (_tmr.elapsed()) {
            bool ok = readTemp();
            requestTemp();
            _tmr.start();
            return ok ? DS18_READY : DS18_ERROR;
        }
        return DS18_IDLE;
    }

    // установить период работы тикера. Будет сброшен на минимальный при вызове setResolution
    void setPeriod(uint16_t prd) {
        _tmr.setPeriod(prd);
    }

    // запросить температуру
    bool requestTemp() {
        return _beginSkip() ? _requestTemp() : false;
    }

    // true - температура готова (асинхронно)
    bool ready() {
        if (_tmr.ready()) {
            _tmr.stop();
            return true;
        }
        return false;
    }

    // true - температура готова (ждать)
    bool waitReady() {
        if (!_tmr.running()) return false;
        while (!ready()) yield();
        return true;
    }

    // true - идёт ожидание конвертации
    bool isWaiting() {
        return _tmr.running();
    }

    // прочитать температуру
    bool readTemp() {
        gds::RAM ram;
        return readRAM(&ram) ? _saveTemp(ram.getTemp()) : false;
    }

    // получить "сырую" температуру (умножена на 16)
    int16_t getTempRaw() {
        return _tbuf;
    }

    // получить int температуру
    int16_t getTempInt() {
        return _tbuf >> 4;
    }

    // получить float температуру
    float getTemp() {
        return _tbuf / 16.0;
    }

    // ===================== MANUAL =====================

    // прочитать содержимое оперативной памяти в буфер 5 байт
    bool readRAM(uint8_t* buf) {
        return _beginSkip() ? _readRAM(buf) : false;
    }
    bool readRAM(gds::RAM* ram) {
        return readRAM((uint8_t*)ram);
    }

    // записать данные в оперативную память (th, tl, res)
    bool writeRAM(uint8_t th, uint8_t tl, uint8_t res) {
        return _beginSkip() ? _writeRAM(th, tl, res) : false;
    }

    // записать данные в оперативную память (th, tl)
    bool writeRAM(uint8_t th, uint8_t tl) {
        return writeRAM(th, tl, readResolution());
    }

    // записать содержимое оперативной памяти в EEPROM
    bool copyRAM() {
        return _beginSkip() ? _copyRAM() : false;
    }

    // записать содержимое EEPROM в оперативную память
    bool recallRAM() {
        return _beginSkip() ? write(DS18_RECALL_RAM) : false;
    }

    // применить разрешение
    uint8_t applyResolution(uint8_t res) {
        _tmr.setPeriod(DS18_TCONV_RES(res));
        return res;
    }

    // ======================== PRIVATE ========================
   protected:
    bool _parasite = true;
    gds::Timer _tmr{DS18_TCONV_12};

    bool _beginSkip() {
        if (_parasite) pullup(false);
        return reset() ? write(DS18_ADDR_SKIP) : false;
    }
    uint8_t _readPower() {
        write(DS18_READ_POWER);
        return readBit() ? DS18_EXTERNAL : DS18_PARASITE;
    }
    bool _saveTemp(int16_t temp) {
        if (temp == 0x0550) return false;  // пропустить 85 градусов
        _tbuf = temp;
        return true;
    }
    bool _requestTemp() {
        write(DS18_CONVERT);
        if (_parasite) pullup(true);
        _tmr.start();
        return true;
    }
    bool _readRAM(uint8_t* buf) {
        write(DS18_READ_RAM);
        uint8_t crc = 0;
        bool notEmpty = false;
        for (uint8_t i = 0; i < 9; i++) {
            uint8_t r = readByte();
            if (i < 5) buf[i] = r;
            if (r) notEmpty = true;
            _crc8(crc, r);
        }
        return notEmpty && !crc;
    }
    bool _writeRAM(uint8_t b0, uint8_t b1, uint8_t res) {
        write(DS18_WRITE_RAM);
        write(b0);
        write(b1);
        write(((res - 9) << 5) | 0b00011111);
        return true;
    }
    bool _copyRAM() {
        write(DS18_COPY_RAM);
        if (_parasite) {
            pullup(true);
            delay(10);
            pullup(false);
        }
        return true;
    }

   private:
    int16_t _tbuf = DS18_BAD_TEMP;

    void _crc8(uint8_t& crc, uint8_t data) {
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
};