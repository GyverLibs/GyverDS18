[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverDS18.svg?color=brightgreen)](https://github.com/GyverLibs/GyverDS18/releases/latest/download/GyverDS18.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/GyverDS18.svg)](https://registry.platformio.org/libraries/gyverlibs/GyverDS18)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverDS18?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverDS18
Лёгкая библиотека для термометров Dallas DS18b20, обновлённая и более удобная версия библиотеки [microDS18B20](https://github.com/GyverLibs/microDS18B20).
- Удобное асинхронное чтение температуры
- Чтение адреса как uint64_t, удобная работа с адресами
- Обработка ошибок чтения температуры
- Поддержка паразитного питания
- Полный доступ к памяти датчика (чтение/запись своих данных итд)
- Протестировано на AVR/ESP8266/ESP32

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

### Зависимости
[GyverIO](https://github.com/GyverLibs/GyverIO)

## Содержание
- [Подключение](#wiring)
- [Использование](#usage)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="wiring"></a>

## Подключение
![scheme](/docs/scheme.png)
P.S. Вместо резистора на 4.7к можно использовать параллельно два по 10к =)

> Для режима "паразитного питания" нужно замкнуть VCC и GND датчика. Пин МК так же должен быть подтянут к питанию!

<a id="usage"></a>

## Использование
### Инициализация
```cpp
// один датчик на пине
GyverDS18Single();
GyverDS18Single(uint8_t pin, bool parasite = true);

// прямая адресация
GyverDS18();
GyverDS18(uint8_t pin, bool parasite = true);

// массив адресов
GyverDS18Array();
GyverDS18Array(uint8_t pin, uint64_t* addr = nullptr, uint8_t amount = 0, bool parasite = true);
```

### Описание классов
```cpp
// установить пин
void setPin(uint8_t pin);

// включить режим паразитного питания (умолч. вкл)
void setParasite(bool parasite);

// установить разрешение (9.. 12 бит)
bool setResolution(uint8_t res);

// прочитать разрешение
uint8_t readResolution();

// прочитать адрес датчика. 0 - ошибка
uint64_t readAddress();

// прочитать питание: DS18_PARASITE - паразитное, DS18_EXTERNAL - обычное, 0 - ошибка
uint8_t readPower();

// получить текущее время измерения температуры, мс
uint16_t getConversionTime();

// ===================== TEMP =====================

// запросить температуру
bool requestTemp();

// true - температура готова (асинхронно)
bool ready();

// true - температура готова (ждать)
bool waitReady();

// true - идёт ожидание конвертации
bool isWaiting();

// прочитать температуру
bool readTemp();

// получить "сырую" температуру (умножена на 16)
int16_t getTempRaw();

// получить int температуру
int16_t getTempInt();

// получить float температуру
float getTemp();

// ===================== MANUAL =====================

// прочитать содержимое оперативной памяти в буфер 5 байт
bool readRAM(uint8_t* buf);

// записать данные в оперативную память (th, tl)
bool writeRAM(uint8_t b0, uint8_t b1);

// записать содержимое оперативной памяти в EEPROM
bool copyRAM();

// записать содержимое EEPROM в оперативную память
bool recallRAM();

// применить разрешение
void applyResolution(uint8_t res);
```

Отличия в классе `GyverDS18` - нужно передавать адрес датчика:
```cpp
bool setResolution(uint8_t res);    // установить у всех
bool setResolution(uint8_t res, uint64_t addr);
uint8_t readResolution(uint64_t addr);
uint8_t readPower(uint64_t addr);
bool requestTemp();     // запросить у всех
bool requestTemp(uint64_t addr);
bool readTemp(uint64_t addr);
bool readRAM(uint8_t* buf, uint64_t addr);
bool writeRAM(uint8_t b0, uint8_t b1, uint64_t addr);
bool copyRAM(uint64_t addr);
bool recallRAM(uint64_t addr);
```

Отличия в классе `GyverDS18Array` - нужно передавать индекс датчика в массиве:
```cpp
// подключить массив адресов формата uint64_t[]
void setAddress(uint64_t* addr, uint8_t amount);

// получить количество адресов в массиве
uint8_t amount();

bool setResolution(uint8_t res);    // установить у всех
bool setResolution(uint8_t res, uint8_t index);
uint8_t readResolution(uint8_t index);
uint8_t readPower(uint8_t index);
bool requestTemp();     // запросить у всех
bool requestTemp(uint8_t index);
bool readTemp(uint8_t index);
bool readRAM(uint8_t* buf, uint8_t index);
bool writeRAM(uint8_t b0, uint8_t b1, uint8_t index);
bool copyRAM(uint8_t index);
bool recallRAM(uint8_t index);
```

### Дополнительные функции
```cpp
// прочитать разрешение из внешнего буфера (5 байт)
uint8_t gds::calcResolution(uint8_t* buf);

// прочитать температуру из внешнего буфера (5 байт)
int16_t gds::calcTemp(uint8_t* buf);

// копировать адрес в буфер размером 8
void gds::copyAddress(uint64_t address, uint8_t* buf);

// вывести адрес в Print
void gds::printAddress(uint64_t address, Print& p, bool newline = true);

// вывести адрес в String
String gds::addressToString(uint64_t address);
```

### Один датчик
Для работы с одним датчиком на пине используется класс `GyverDS18Single`. Температура получается в четыре этапа:
- Запросить температуру `requestTemp()`
- Подождать время, равное `getConversionTime()` (зависит от установленного разрешения)
- Прочитать температуру `readTemp()`
- Получить температуру `getTemp()` или `getTempInt()`

> Примечание: сразу после включения датчик имеет в буфере температуру 85 градусов. Если не запросить температуру - будет прочитана температура 85 градусов. Поэтому в библиотеке **игнорируется значение 85** градусов, датчики можно подключать "на горячую" и не бояться, что где то в программе резко появится цифра 85.

В библиотеке есть встроенный асинхронный таймер на время конвертации, базовый пример циклической работы с датчиком выглядит так:
```cpp
#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);
    ds.requestTemp();  // первый запрос на измерение
}
void loop() {
    if (ds.ready()) {         // измерения готовы по таймеру
        if (ds.readTemp()) {  // если чтение успешно
            Serial.print("temp: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error");
        }

        ds.requestTemp();  // запрос следующего измерения
    }
}
```

> Примечание: `readTemp()` именно **читает** температуру с датчика, а `getTemp()` - возвращает результат из буфера библиотеки. Таким образом можно вызывать `getTemp()` несколько раз подряд, если это нужно (после вызова `readTemp()`)

Для запуска первой конвертации также можно использовать статус `isWaiting()`:
```cpp
void loop() {
    // запросить конвертацию
    if (!ds.isWaiting()) ds.requestTemp();

    // получить и вывести
    if (ds.ready()) {
        if (ds.readTemp()) Serial.println(ds.getTemp());
        // здесь статус waiting будет сброшен
    }
}
```

Для синхронной работы (с ожиданием) можно использовать такую конструкцию:
```cpp
ds.requestTemp();   // запросить

if (ds.waitReady() && ds.readTemp()) {  // подождать, затем прочитать
    Serial.print("temp: ");
    Serial.println(ds.getTemp());
} else {
    Serial.println("error");
}
```

### Обращение по адресу
Можно подключить несколько датчиков на один пин. Для обращения к датчикам нужно знать их уникальные адреса, поэтому сначала нужно получить адрес. Для этого нужно подключить **один** датчик к пину и вызвать `readAddress()`. Примечание: корректный адрес никогда не может быть равен `0`, это можно использовать для проверки корректности.

В данной библиотеке, в отличие от многих других, адрес представлен типом `uint64_t` - более удобном для записи и хранения. На AVR Arduino такой тип не выводится в `Serial`. Для вывода можно использовать встроенную функцию `gds::printAddress`

```cpp
#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);

    uint64_t addr = ds.readAddress();
    if (addr) {
        Serial.print("address: ");
        gds::printAddress(addr, Serial);
        // пример вывода: 0xCF0417505B78FF28
    } else {
        Serial.println("error");
    }
}

void loop() {
}
```

Для обращения к датчикам по адресам используется класс `GyverDS18`:
```cpp
#include <GyverDS18.h>
GyverDS18 ds(2);  // пин

uint64_t addr = 0xCF0417505B78FF28;

void setup() {
    Serial.begin(115200);
    // первый запрос на измерение. Запрос ВСЕМ датчикам на линии
    ds.requestTemp();
}
void loop() {
    if (ds.ready()) {             // измерения готовы по таймеру
        // читаем КОНКРЕТНЫЙ датчик по адресу
        if (ds.readTemp(addr)) {  // если чтение успешно
            Serial.print("temp: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error");
        }

        ds.requestTemp();  // запрос следующего измерения ДЛЯ ВСЕХ
    }
}
```

Примечания:
- Можно запросить и получить температуру с любого конкретного датчика по его адресу, ровно как и остальные команды
- Запрос измерения и установку разрешения можно отправлять всем датчикам сразу, не указывая адрес
- Связка `readTemp()`-`getTemp()` работает следующим образом: `readTemp()` читает данные с указанного датчика в буфер библиотеки, соответственно `getTemp()` получает температуру из буфера библиотеки, поэтому для вызова не нужно указание адреса
- `ready()` ожидает время, установленное соответственно разрешению в `setResolution()`. Если датчикам установить разное разрешение - `ready()` всегда будет ориентироваться на последнее установленное! Для самостоятельного ожидания можно использовать константы миллисекунд `DS18_TCONV_9`, `DS18_TCONV_10`, `DS18_TCONV_11`, `DS18_TCONV_12`

### Несколько датчиков
Для удобного взаимодействия с несколькими датчиками есть класс `GyverDS18Array`. Пример чтения "связки" датчиков:
```cpp
#include <GyverDS18Array.h>

uint64_t addr[] = {
    0xD20417515A42FF28,
    0x4D0417508099FF28,
    0xFE04175159CDFF28,
    0xCF0417505B78FF28,
};
GyverDS18Array ds(2, addr, 4);  // пин, массив, длина

void setup() {
    Serial.begin(115200);
    // Запрос ВСЕМ датчикам на линии
    ds.requestTemp();
}
void loop() {
    if (ds.ready()) {  // измерения готовы по таймеру
        // проходим по массиву
        for (int i = 0; i < ds.amount(); i++) {
            Serial.print("#");
            Serial.print(i);
            Serial.print(": ");
            // читаем по индексу
            if (ds.readTemp(i)) Serial.print(ds.getTemp());
            else Serial.print("error");
            Serial.print(", ");
        }
        Serial.println();

        ds.requestTemp();  // запрос следующего измерения ДЛЯ ВСЕХ
    }
}
```

### Работа с памятью датчика
Датчик имеет 2 байта EEPROM памяти (не сбрасывается при перезагрузке), их можно использовать в каких-то своих сценариях. Например записывать туда калибровку или что то ещё. Важный момент: запись этих данных пересекается с установкой разрешения датчика, поэтому установленное разрешение хранится в библиотеке и само записывается при отправке, причём функция `setResolution()` сбрасывает эти два байта. Для записи используется функция `writeRAM()`, которая также перезаписывает разрешение. Сценарий записи своих данных и желаемого разрешения:

```cpp
// данная функция не устанавливает разрешение, 
// а просто запоминает его в библиотеке
ds.applyResolution(12);

// запись данных в RAM (оперативную память)
// здесь мы пишем два своих байта данных, и здесь же устанавливается разрешение,
// заданное в applyResolution() или setResolution()
ds.writeRAM(0xAB, 0xCD);

// можно сохранить данные в EEPROM датчика при помощи отдельной функции
ds.copyRAM();

// и прочитать из EEPROM обратно в RAM
// при запуске датчика они читаются автоматически!
ds.recallRAM();
```

Чтение данных из памяти производится в буфер размером `5`, в этом буфере будет температура (байты 0 и 1), данные (байты 2 и 3) и разрешение (байт 4). Таким образом за одно обращение к датчику можно вытащить все нужные данные, особенно полезно если там хранится калибровка - можно сразу её применить:
```cpp
uint8_t buf[5];
ds.readRAM(buf);
Serial.println(buf[2], HEX);    // первый байт
Serial.println(buf[3], HEX);    // второй байт

Serial.println(gds::calcTemp(buf) / 16.0);  // вывести температуру
Serial.println(gds::calcResolution(buf));   // вывести разрешение
```

<a id="versions"></a>

## Версии
- v1.0
- v1.0.3 - поправлены тайминги для китайских датчиков
- v1.1.0 - оптимизация работы с uint64, добавлены фичи

<a id="install"></a>
## Установка
- Для работы требуется библиотека [GyverIO](https://github.com/GyverLibs/GyverIO)
- Библиотеку можно найти по названию **GyverDS18** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverDS18/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код
