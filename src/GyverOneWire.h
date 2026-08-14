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

#ifdef __AVR__
#define GWIRE_CLI()             \
    uint8_t _gwire_sreg = SREG; \
    noInterrupts()
#define GWIRE_SEI() SREG = _gwire_sreg
#else
#define GWIRE_CLI() noInterrupts()
#define GWIRE_SEI() interrupts()
#endif

class GyverOneWire {
   public:
    GyverOneWire() {}
    GyverOneWire(uint8_t pin) {
        setPin(pin);
    }

    // установить пин
    void setPin(uint8_t pin) {
        if (_pulled) pullup(false);
        _pin = pin;
        gio::init(_pin, INPUT);
    }

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
        return _writeByte(data, false);
    }

   protected:
    // записать байт и включить активную подтяжку сразу после последнего бита
    bool writePower(uint8_t data) {
        return _writeByte(data, true);
    }

   public:
    // записать бит
    bool writeBit(bool data) {
        _writeBit(data, false);
        return true;
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
    uint8_t readByte() {
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
        gio::mode(_pin, v ? INPUT : OUTPUT);
    }

    void _writeBit(bool data, bool power) {
        GWIRE_CLI();
        _write(0);
        if (data) {
            GWIRE_DELAY(GWIRE_WRITE_WAIT);
            _write(1);
            if (power) pullup(true);
            GWIRE_SEI();
            GWIRE_DELAY(GWIRE_WRITE_SLOT - GWIRE_WRITE_WAIT);
        } else {
            GWIRE_DELAY(GWIRE_WRITE_SLOT - GWIRE_WRITE_WAIT);
            _write(1);
            if (power) pullup(true);
            GWIRE_SEI();
            GWIRE_DELAY(GWIRE_WRITE_WAIT);
        }
    }

    bool _writeByte(uint8_t data, bool power) {
        for (uint8_t i = 0; i < 8; i++) {
            _writeBit(data & 1, power && i == 7);
            data >>= 1;
        }
        if (!power) GWIRE_DELAY(1);
        return true;
    }
};
