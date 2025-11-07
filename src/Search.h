#pragma once
#include <Arduino.h>

#include "./utils/utils.h"
#include "GyverOneWire.h"

namespace gds {

// https://github.com/GyverLibs/GyverDS18/pull/6
// Выполняем поиск нескольких устройств на одной шине, для этого мастер отправляет команду F0
// Процедура проходит в 3 этапа для раскрытия каждого бита адреса:
//
// ВСЕ ПОДЧИНЕННЫЕ устройства ОДНОВРЕМЕННО отправляют.
// 1) бит адреса - так как тайминг при отправке нуля составляет 60-120µs,
// а единицы 1-15µs, то при одновременной отправке 1 и 0 с разных устройств, придет только 0.
// 2) инвертированный бит адреса - если 1, то первый бит был 0
// у всех устройств на шине, если 0, то были устройства с 1 в адресе.
//
// мастер отправляет
// 3)  бит 1 или 0 для выбора устройств с кем продолжить общение

class Search : private GyverOneWire, public gds::Addr {
   public:
    using GyverOneWire::GyverOneWire;
    using GyverOneWire::setPin;

    // найти адреса и записать в массив
    uint8_t scan(uint64_t* addrs, uint8_t len) {
        uint8_t i = 0;
        while (i < len && search()) {
            addrs[i++] = addr;
        }
        stop();
        return i;
    }

    // найти по индексу
    gds::Addr find(uint8_t n) {
        ++n;
        while (n-- && search());
        stop();
        return *this;
    }

    // поиск, вызывать в while(), забирать из addr
    bool search() {
        uint8_t last_zero = 0, id_bit_num = 1, rom_byte_num = 0, rom_byte_mask = 1, dir = 0;
        bool res = false;
        uint8_t* p = (uint8_t*)&addr;

        if (!_last_flag) {
            // сбрасываем шину, если сброс с ошибкой, то обнуляем флаги поиска
            if (!reset()) {
                stop();
                return false;
            }

            // поиск всех адресов
            write(0xF0);

            do {
                // читаем первый и второй биты
                uint8_t id_bit = readBit();
                uint8_t cmp_id_bit = readBit();

                // 11 - нет устройств на шине
                if (id_bit == 1 && cmp_id_bit == 1) {
                    break;
                } else {
                    // 01 - у всех устройств бит в адресе rom 0
                    // 10 - у всех устройств бит в адресе rom 1
                    // 00 - есть устройства с 0 и 1
                    if (id_bit != cmp_id_bit) {
                        dir = id_bit;
                    } else {
                        // если мы еще не дошли до развилки с прошлого поиска,
                        // то берем прошлое значение бита из ранее сохраненного адреса
                        if (id_bit_num < _last_discr) {
                            dir = (p[rom_byte_num] & rom_byte_mask) > 0;
                        } else {
                            // если дошли до развилки, то в этот раз берем 1
                            dir = (id_bit_num == _last_discr);
                        }

                        // если взяли 0, то запишем номер бита как "последний ноль"
                        // позже мы сохраним номер бита как развилку _last_discr
                        if (dir == 0) last_zero = id_bit_num;
                    }

                    // выставляем бит в переменной адреса с помощью маски размером в байт
                    if (dir == 1) {
                        p[rom_byte_num] |= rom_byte_mask;
                    } else {
                        p[rom_byte_num] &= ~rom_byte_mask;
                    }

                    // увеличиваем переменную кол-ва битов и пушим бит в маске влево
                    ++id_bit_num;
                    rom_byte_mask <<= 1;
                }

                // отправляем выбранный бит подчиненным устройствам на шине
                writeBit(dir);

                // мы полностью пропушили байт нашей маски влево 00000000
                // выставляем его опять в значение 00000001 и начинаем работу
                // со следующим байтом адреса
                if (!rom_byte_mask) {
                    ++rom_byte_num;
                    rom_byte_mask = 1;
                }
            } while (rom_byte_num < 8);

            // собрали адрес, помечаем развилку
            if (id_bit_num >= 65) {
                _last_discr = last_zero;

                // если развилок не было, значит это последнее устройство на шине
                if (_last_discr == 0) _last_flag = true;

                res = true;
            }
        }

        // адрес не найден, сбрасываем счетчики
        if (!res || !p[0]) {
            stop();
            return false;
        }

        return res;
    }

    // остановить поиск
    void stop() {
        _last_discr = 0;
        _last_flag = false;
    }

   private:
    bool _last_flag = false;
    uint8_t _last_discr = 0;
};

}  // namespace gds