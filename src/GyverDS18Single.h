#pragma once
#include <Arduino.h>

#include "utils/GyverOneWire.h"
#include "utils/utils.h"

// CONST
#define DS18_BAD_TEMP 0

#define DS18_EXTERNAL 1
#define DS18_PARASITE 2

#define DS18_TCONV_OFFSET 5
#define DS18_TCONV_9 (94 + DS18_TCONV_OFFSET)
#define DS18_TCONV_10 (188 + DS18_TCONV_OFFSET)
#define DS18_TCONV_11 (375 + DS18_TCONV_OFFSET)
#define DS18_TCONV_12 (750 + DS18_TCONV_OFFSET)

// CMD
#define DS18_ADDR_SEARCH 0xF0
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

class GyverDS18Single : public GyverOneWire {
   public:
    GyverDS18Single() {}
    GyverDS18Single(uint8_t pin, bool parasite = true) {
        setPin(pin);
        _parasite = parasite;
    }

    // ===================== SYSTEM =====================

    using GyverOneWire::setPin;

    // включить режим паразитного питания (умолч. вкл)
    void setParasite(bool parasite) {
        _parasite = parasite;
    }

    // установить разрешение (9.. 12 бит)
    bool setResolution(uint8_t res) {
        applyResolution(res);
        return writeRAM(0x00, 0xff);
    }

    // прочитать разрешение
    uint8_t readResolution() {
        uint8_t buf[5];
        return readRAM(buf) ? gds::calcResolution(buf) : 0;
    }

    // получить текущее время измерения температуры, мс
    uint16_t getConversionTime() {
        return _tconv;
    }

    // прочитать адрес датчика. 0 - ошибка
    uint64_t readAddress() {
        if (!reset()) return 0;
        write(DS18_ADDR_READ);
        uint8_t crc = 0;
        gds::buf64 addr{0};
        for (uint8_t i = 0; i < 8; i++) {
            uint8_t r = read();
            addr.u8[i] = r;
            _crc8(crc, r);
        }
        return crc ? 0 : addr.u64;
    }

    // прочитать питание: DS18_PARASITE - паразитное, DS18_EXTERNAL - обычное, 0 - ошибка
    uint8_t readPower() {
        return _beginSkip() ? _readPower() : 0;
    }

    // ===================== TEMP =====================

    // запросить температуру
    bool requestTemp() {
        return _beginSkip() ? _requestTemp() : 0;
    }

    // true - температура готова (асинхронно)
    bool ready() {
        if (_wait && (uint16_t)((uint16_t)millis() - _tmr) >= _tconv) {
            _wait = 0;
            return 1;
        }
        return 0;
    }

    // true - температура готова (ждать)
    bool waitReady() {
        if (!_wait) return 0;
        while (!ready()) yield();
        return 1;
    }

    // true - идёт ожидание конвертации
    bool isWaiting() {
        return _wait;
    }

    // прочитать температуру
    bool readTemp() {
        uint8_t buf[5];
        return readRAM(buf) ? _readTemp(gds::calcTemp(buf)) : 0;
    }

    // получить "сырую" температуру (умножена на 16)
    int16_t getTempRaw() {
        return _tbuf;
    }

    // получить int температуру
    int16_t getTempInt() {
        return _tbuf / 16;
    }

    // получить float температуру
    float getTemp() {
        return _tbuf / 16.0;
    }

    // ===================== MANUAL =====================

    // прочитать содержимое оперативной памяти в буфер 5 байт
    bool readRAM(uint8_t* buf) {
        return _beginSkip() ? _readRAM(buf) : 0;
    }

    // записать данные в оперативную память (th, tl)
    bool writeRAM(uint8_t b0, uint8_t b1) {
        return _beginSkip() ? _writeRAM(b0, b1, _getConfig()) : 0;
    }

    // записать содержимое оперативной памяти в EEPROM
    bool copyRAM() {
        return _beginSkip() ? _copyRAM() : 0;
    }

    // записать содержимое EEPROM в оперативную память
    bool recallRAM() {
        return _beginSkip() ? write(DS18_RECALL_RAM) : 0;
    }

    // применить разрешение
    void applyResolution(uint8_t res) {
        _res = constrain(res, 9, 12);
        switch (_res) {
            case 9:
                _tconv = DS18_TCONV_9;
                break;
            case 10:
                _tconv = DS18_TCONV_10;
                break;
            case 11:
                _tconv = DS18_TCONV_11;
                break;
            case 12:
                _tconv = DS18_TCONV_12;
                break;
        }
    }

    // ======================== PRIVATE ========================
   protected:
    bool _parasite = true;

    uint8_t _getConfig() {
        return ((_res - 9) << 5) | 0b00011111;
    }
    uint8_t _readPower() {
        write(DS18_READ_POWER);
        return readBit() ? DS18_EXTERNAL : DS18_PARASITE;
    }
    bool _readTemp(const int16_t& temp) {
        if (temp == 0x0550) return 0;  // пропустить 85 градусов
        _tbuf = temp;
        return 1;
    }
    bool _requestTemp() {
        write(DS18_CONVERT);
        if (_parasite) pullup(true);
        _wait = 1;
        _tmr = millis();
        return 1;
    }
    bool _readRAM(uint8_t* buf) {
        write(DS18_READ_RAM);
        uint8_t crc = 0;
        bool notEmpty = false;
        for (uint8_t i = 0; i < 9; i++) {
            uint8_t r = read();
            if (i < 5) buf[i] = r;
            if (r) notEmpty = true;
            _crc8(crc, r);
        }
        return notEmpty && !crc;
    }
    bool _writeRAM(const uint8_t& b0, const uint8_t& b1, const uint8_t& b2) {
        write(DS18_WRITE_RAM);
        write(b0);
        write(b1);
        write(b2);
        return 1;
    }
    bool _beginSkip() {
        if (_parasite) pullup(false);
        return reset() ? write(DS18_ADDR_SKIP) : 0;
    }
    bool _copyRAM() {
        write(DS18_COPY_RAM);
        if (_parasite) {
            pullup(true);
            delay(10);
            pullup(false);
        }
        return 1;
    }

   private:
    uint16_t _tconv = DS18_TCONV_12;
    int16_t _tbuf = DS18_BAD_TEMP;
    uint16_t _tmr = 0;
    bool _wait = 0;
    uint8_t _res = 12;

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