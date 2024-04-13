#pragma once
#include <Arduino.h>
#include <GyverIO.h>

#define GWIRE_RESET_WAIT 500
#define GWIRE_WAIT_FRAME 70

#define GWIRE_WRITE_WAIT 10
#define GWIRE_WRITE_SLOT 70

#define GWIRE_READ_DEL 3
#define GWIRE_READ_WAIT 10
#define GWIRE_READ_SLOT 70

#define GWIRE_DELAY(us) delayMicroseconds(us)
#define GWIRE_CLI() noInterrupts()
#define GWIRE_SEI() interrupts()

class GyverOneWire {
   public:
    // установить пин
    void setPin(uint8_t pin) {
        _pin = pin;
        gio::init(_pin, INPUT);
        gio::write(_pin, LOW);
    }

   protected:
    // сброс шины
    bool reset() {
        if (!gio::read(_pin)) return 0;

        _write(0);
        GWIRE_DELAY(GWIRE_RESET_WAIT);

        GWIRE_CLI();  // cli
        _write(1);
        GWIRE_DELAY(GWIRE_WAIT_FRAME);
        bool ok = !gio::read(_pin);
        GWIRE_SEI();  // sei

        GWIRE_DELAY(GWIRE_RESET_WAIT - GWIRE_WAIT_FRAME);
        return ok;
    }

    // записать байт
    bool write(uint8_t data) {
        for (uint8_t i = 0; i < 8; i++) {
            GWIRE_CLI();
            _write(0);
            if (data & 1) {
                GWIRE_DELAY(GWIRE_WRITE_WAIT);
                _write(1);
                GWIRE_SEI();
                GWIRE_DELAY(GWIRE_WRITE_SLOT - GWIRE_WRITE_WAIT);
            } else {
                GWIRE_DELAY(GWIRE_WRITE_SLOT - GWIRE_WRITE_WAIT);
                _write(1);
                GWIRE_SEI();
                GWIRE_DELAY(GWIRE_WRITE_WAIT);
            }
            data >>= 1;
        }
        GWIRE_DELAY(1);
        return 1;
    }

    // прочитать бит
    bool readBit() {
        GWIRE_CLI();  // cli
        _write(0);
        GWIRE_DELAY(GWIRE_READ_DEL);

        _write(1);
        GWIRE_DELAY(GWIRE_READ_WAIT);
        bool b = gio::read(_pin);
        GWIRE_SEI();  // sei

        GWIRE_DELAY(GWIRE_READ_SLOT - GWIRE_READ_DEL - GWIRE_READ_WAIT);
        return b;
    }

    // прочитать байт
    uint8_t read() {
        uint8_t data = 0;
        for (uint8_t i = 0; i < 8; i++) {
            data >>= 1;
            if (readBit()) data |= bit(7);
        }
        return data;
    }

    // переключить активный выход для паразитного питания
    void pullup(bool state) {
        if (_pulled == state) return;
        if (state) {
            gio::write(_pin, HIGH);
            gio::mode(_pin, OUTPUT);
        } else {
            gio::mode(_pin, INPUT);
            gio::write(_pin, LOW);
        }
        _pulled = state;
    }

   private:
    uint8_t _pin = 0xff;
    bool _pulled = 0;

    void _write(bool v) {
        if (v) gio::mode(_pin, INPUT);
        else gio::mode(_pin, OUTPUT);
    }
};
