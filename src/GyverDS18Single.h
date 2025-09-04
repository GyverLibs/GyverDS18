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

    using GyverOneWire::reset;
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
        if (!reset() && !reset()) return 0;  // 2x reset
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

    // Выполняем поиск нескольких устройств на одной шине.
    // для этого мастер отправляет команду F0.
    // процедура проходит в 3 этапа для раскрытия каждого бита адреса:
    //
    // ВСЕ ПОДЧИНЕННЫЕ устройства ОДНОВРЕМЕННО отправляют.
    // 1) бит адреса - так как тайминг при отправке нуля составляет 60-120µs,
    // а единицы 1-15µs, то при одновременной отправке 1 и 0 с разных устройств, придет только 0.
    // 2) инвертированный бит адреса - если 1, то первый бит был 0
    // у всех устройств на шине, если 0, то были устройства с 1 в адресе.
    //
    // мастер отправляет
    // 3)  бит 1 или 0 для выбора устройств с кем продолжить общение
    bool search(gds::buf64 *newAddr) {
        uint8_t id_bit_number;
        uint8_t last_zero, rom_byte_number;
        bool search_result;
        uint8_t id_bit, cmp_id_bit;

        unsigned char rom_byte_mask, search_direction;

        id_bit_number = 1;
        last_zero = 0;
        rom_byte_number = 0;
        rom_byte_mask = 1;
        search_result = false;

        if (!_last_device_flag) { // !! длинный if !!

            // сбрасываем шину, если сброс с ошибкой, то обнуляем флаги поиска
            // и выходим
            if (!reset()) {
                _last_discrepancy = 0;
                _last_device_flag = false;
                return false;
            }

            // команда поиска всех адресов
            write(DS18_ADDR_SEARCH);

            do {

                // читаем первый и второй биты
                id_bit = readBit();
                cmp_id_bit = readBit();

                // 11 - нет устройств на шине
                if ((id_bit) == 1 && (cmp_id_bit == 1)) {
                    break;
                } else {
                    // 01 - у всех устройств бит в адресе rom 0
                    // 10 - у всех устройств бит в адресе rom 1
                    // 00 - есть устройства с 0 и 1
                    if (id_bit != cmp_id_bit) {
                        search_direction = id_bit;
                    } else {
                        // если мы еще не дошли до развилки с прошлого поиска,
                        // то берем прошлое значение бита из ранее сохраненного адреса
                        if (id_bit_number < _last_discrepancy) {
                            search_direction = ((addr.u8[rom_byte_number] & rom_byte_mask) > 0);
                        } else {
                            // если дошли до развилки, то в этот раз берем 1
                            search_direction = (id_bit_number == _last_discrepancy);
                        }
                        // если взяли 0, то запишем номер бита как "последний ноль"
                        // позже мы сохраним номер бита как развилку _last_discrepancy
                        if (search_direction == 0) {
                            last_zero = id_bit_number;
                        }
                    }

                    // выставляем бит в переменной адреса с помощью маски размером в байт
                    // != для 1 и &= ~ для 0
                    if (search_direction == 1) {
                        addr.u8[rom_byte_number] |= rom_byte_mask;
                    } else {
                        addr.u8[rom_byte_number] &= ~rom_byte_mask;
                    }
                    // увеличиваем переменную кол-ва битов
                    // и пушим бит в маске влево
                    id_bit_number++;
                    rom_byte_mask <<= 1;

                }

                // отправляем выбранный бит подчиненным устройствам на шине.
                writeBit(search_direction);

                // мы полностью пропушили байт нашей маски влево 00000000
                // выставляем его опять в значение 00000001 и начинаем работу
                // со следующим байтом адреса
                if (rom_byte_mask == 0) {
                    rom_byte_number++;
                    rom_byte_mask = 1;
                }
            } while (rom_byte_number < 8);

            // мы собрали адрес, помечаем развилку
            if (!(id_bit_number < 65)) {
                _last_discrepancy = last_zero;

                // если развилок не было,
                // значит это последнее устройство на шине
                if (_last_discrepancy == 0) {
                    _last_device_flag = true;
                }
                search_result = true;
            }
        } // конец длинного if

        // адрес не найден, сбрасываем счетчики
        // следующий вызов функции будет будто
        // мы вызываем ее в первый раз
        if (!search_result || !addr.u8[0]) {
            _last_discrepancy = 0;
            _last_device_flag = false;
            search_result = false;
        } else {
        // копируем адрес во внешнюю переменную
        for (byte i = 0; i < 8; i++)
            newAddr->u8[i] = addr.u8[i];
        }
        return search_result;
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
    // search
    gds::buf64 addr{0};
    uint8_t _last_discrepancy;
    bool _last_device_flag;

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