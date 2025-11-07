[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverDS18.svg?color=brightgreen)](https://github.com/GyverLibs/GyverDS18/releases/latest/download/GyverDS18.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/GyverDS18.svg)](https://registry.platformio.org/libraries/gyverlibs/GyverDS18)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverDS18?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverDS18
Лёгкая библиотека для термометров Dallas DS18b20, обновлённая и более удобная версия библиотеки [microDS18B20](https://github.com/GyverLibs/microDS18B20):
- Удобное асинхронное чтение температуры
- Чтение адреса как `uint64_t`, удобная работа с адресами
- Обработка ошибок чтения температуры и подключения датчика
- Поддержка паразитного питания
- Полный доступ к памяти датчика (чтение/запись своих данных итд)
- Протестировано на AVR/ESP8266/ESP32

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

### Зависимости
- [GyverIO](https://github.com/GyverLibs/GyverIO)

## Содержание
- [Подключение](#wiring)
- [Использование](#usage)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="wiring"></a>

## Подключение
![scheme](/docs/scheme.png)
P.S. Вместо резистора на 4.7к можно использовать параллельно два по 10к

<a id="usage"></a>

## Документация
### GyverDS18Single
Один датчик на пине:

```cpp
GyverDS18Single();
GyverDS18Single(uint8_t pin, bool parasite = true);

// установить пин
void setPin(uint8_t pin);

// сброс шины
bool reset();

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

// автоматический опрос по таймеру, вызывать в loop. Вернёт DS18_READY (0) по готовности и чтению
uint8_t tick();

// установить период работы тикера. Будет сброшен на минимальный при вызове setResolution
void setPeriod(uint16_t prd);

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
bool readRAM(gds::RAM* ram);

// записать данные в оперативную память (th, tl)
bool writeRAM(uint8_t b0, uint8_t b1);

// записать содержимое оперативной памяти в EEPROM
bool copyRAM();

// записать содержимое EEPROM в оперативную память
bool recallRAM();

// применить разрешение
void applyResolution(uint8_t res);
```

### GyverDS18
Прямая адресация - нужно передавать адрес датчика в функции:

```cpp
GyverDS18();
GyverDS18(uint8_t pin, bool parasite = true);

// автоматический запрос по таймеру для всех, вызывать в loop. Вернёт DS18_READY (0) по готовности таймера
uint8_t tick();

bool setResolution(uint8_t res);    // установить у всех
bool setResolution(uint8_t res, uint64_t addr);
uint8_t readResolution(uint64_t addr);
uint8_t readPower(uint64_t addr);
bool requestTemp();     // запросить у всех
bool requestTemp(uint64_t addr);
bool readTemp(uint64_t addr);
bool readRAM(gds::RAM* ram, uint64_t addr);
bool writeRAM(uint8_t b0, uint8_t b1, uint64_t addr);
bool copyRAM(uint64_t addr);
bool recallRAM(uint64_t addr);
```

### GyverDS18Array
Работает с массивом адресов - нужно передавать индекс датчика в массиве:

```cpp
GyverDS18Array();
GyverDS18Array(uint8_t pin, uint64_t* addr = nullptr, uint8_t amount = 0, bool parasite = true);

// автоматический запрос по таймеру для всех, вызывать в loop. Вернёт DS18_READY (0) по готовности таймера
uint8_t tick();

// прочитать в массив. NAN если ошибка чтения
void readTemps(float* arr);

// подключить массив адресов формата uint64_t[]
void setAddress(uint64_t* addr, uint8_t amount);

// получить количество адресов в массиве
uint8_t amount();

// прочитать в массив. NAN если ошибка чтения
void readTemps(float* arr);

bool setResolution(uint8_t res);    // установить у всех
bool setResolution(uint8_t res, uint8_t index);
uint8_t readResolution(uint8_t index);
uint8_t readPower(uint8_t index);
bool requestTemp();     // запросить у всех
bool requestTemp(uint8_t index);
bool readTemp(uint8_t index);
bool readRAM(gds::RAM* ram, uint8_t index);
bool writeRAM(uint8_t b0, uint8_t b1, uint8_t index);
bool copyRAM(uint8_t index);
bool recallRAM(uint8_t index);
```

### gds::RAM
Работа с памятью:

```cpp
uint8_t t_lsb;
uint8_t t_msb;
uint8_t th;
uint8_t tl;
uint8_t cfg;

int16_t getTemp();
uint8_t getRes();
```

### gds::Addr
Хранение и вывод адреса:

```cpp
Addr();
Addr(const uint64_t addr);

// адрес
uint64_t addr = 0;

// валидность адреса
operator bool();

// доступ по индексу (до 8)
uint8_t& operator[](uint8_t i);

// скопировать в массив (8 байт)
void copyTo(uint8_t* buf);

// напечатать в Print
void printTo(Print& pr, bool newline = true);

// вывести в строку
String toString();
```

Пример:

```cpp
uint64_t addr; // получен как-либо

gds::Addr(addr).printTo(Serial);    // печать

gds::Addr a(addr);
String s = a.toString();    // в строку

uint64_t addr2 = a; // конвертируется в uint64_t

a[0];               // доступ к байтам адреса
```

### gds::Search
Поиск адресов на шине

```cpp
Search(uint8_t pin);

// наследует gds::Addr

// найти адреса и записать в массив
uint8_t scan(uint64_t* addrs, uint8_t len);

// найти по индексу
gds::Addr find(uint8_t n);

// поиск, вызывать в while(), забирать из addr
bool search();

// остановить поиск
void stop();
```

## Использование
### Паразитное питание
Датчик может подключаться по двум проводам (GND и DATA) - нужно замкнуть у него VCC и GND. DATA-пин так же должен быть подтянут к питанию, а в программе нужно активировать паразитный режим.

### Адресация
Можно подключить несколько датчиков на один пин - в этом случае нужно будет обращаться к ним по адресам. Подробнее об этом - ниже.

### Разрешение
Датчик имеет настройку разрешения (точности) измерения температуры 9.. 12 бит. Чем выше разрешение - тем меньше шаг изменения температуры, но тем больше времени требуется датчику для измерения:

| Разрешение, бит | Шаг, °C | Время измерения, мс |
|-----------------|---------|---------------------|
| 9               | 0.5     | 95                  |
| 10              | 0.25    | 190                 |
| 11              | 0.125   | 380                 |
| 12              | 0.0625  | 760                 |

### Один датчик
Для работы с одним датчиком на пине используется класс `GyverDS18Single`. Самый простой вариант опроса - автоматически через тикер: библиотека сама опрашивает датчик по таймеру и обрабатывает ошибки, а по успешному чтению достаточно просто получить температуру:

```cpp
#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);
    // ds.setResolution(12);    // установка разрешения. Влияет на период опроса
    // ds.setPeriod(2000);      // ручная установка периода опроса
}

void loop() {
    if (!ds.tick()) {
        // по готовности и успешному чтению
        Serial.println(ds.getTemp());
    }
}
```

При изменении разрешения библиотека устанавливает минимальный период опроса для текущего разрешения, т.е. в авто режиме датчик будет выдавать результат с максимально возможной частотой. Период опроса можно увеличить вручную, если это нужно.

`ds.getTemp()` можно вызывать в любом месте программы, но корректность и актуальность его значения гарантируется только в условии тикера.

В этом же режиме можно отловить ошибки - библиотека обрабатывает статус подключения датчика и ошибки передачи данных:

```cpp
#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);
}

void loop() {
    switch (ds.tick()) {
        case DS18_READY:
            Serial.println(ds.getTemp());
            break;

        case DS18_ERROR:
            Serial.println("Error");
            break;
    }
}
```

Можно работать с датчиком на более низком уровне. Температура получается в четыре этапа:
- Запросить температуру `requestTemp()`
- Подождать время, равное `getConversionTime()` - зависит от установленного разрешения
- Прочитать температуру `readTemp()`
- Получить температуру `getTemp()` или `getTempInt()`

> [!NOTE]
> Сразу после включения датчик имеет в буфере температуру 85 градусов. Если не запросить температуру - будет прочитана температура 85 градусов. Поэтому в библиотеке **игнорируется значение 85** градусов, датчики можно подключать "на горячую" и не бояться, что где то в программе резко появится цифра 85

> [!NOTE]
> `readTemp()` именно **читает** температуру с датчика, а `getTemp()` - возвращает результат из буфера. Таким образом можно вызывать `getTemp()` после вызова `readTemp()` несколько раз подряд, если это нужно

Можно самостоятельно запрашивать измерение и получать ответ по встроенному таймеру - вот пример циклической работы *(примечание: если датчик "отвалится", то цикл прервётся, т.к. если запрос завершится ошибкой - таймер не будет перезапущен)*:

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

Для запуска первой конвертации также можно использовать статус `isWaiting()` *(примечание: если датчик "отвалится", то запрос будет происходить постоянно со скоростью loop)*:

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
void setup() {
    Serial.begin(115200);

    // запросить, подождать, прочитать
    if (ds.requestTemp() && ds.waitReady() && ds.readTemp()) {
        Serial.print("temp: ");
        Serial.println(ds.getTemp());
    } else {
        Serial.println("error");
    }
}
```

### Обращение по адресу
Можно подключить несколько датчиков на один пин. Для обращения к датчикам нужно знать их уникальные адреса, поэтому сначала нужно получить адрес. Для этого нужно подключить **один** датчик к пину и вызвать `readAddress()`. Также можно воспользоваться сканером шины, о нём ниже. *Примечание: корректный адрес никогда не может быть равен `0`, это можно использовать для проверки корректности.*

В данной библиотеке, в отличие от других, адрес представлен типом `uint64_t` - более удобном для записи и хранения. Для более удобного вывода можно использовать встроенный инструмент `Addr`, пример с выводом в порт:

```cpp
#include <GyverDS18.h>
GyverDS18Single ds(2);  // пин

void setup() {
    Serial.begin(115200);

    // readAddress вернёт uint64_t
    // uint64_t addr = ds.readAddress();

    // для вывода
    gds::Addr addr = ds.readAddress();

    // если корректный
    if (addr) {
        Serial.print("address: ");
        addr.printTo(Serial);
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

Для этого класса тоже работает авторежим, но в этом случае он просто запрашивает температуру по таймеру, не читая её:

```cpp
void loop() {
    // это таймер с периодом к setResolution, он сам делает request
    if (!ds.tick()) {

        if (ds.readTemp(0x4D0417508099FF28)) {
            Serial.print("temp 1: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error 1");
        }

        if (ds.readTemp(0xC3041750E553FF28)) {
            Serial.print("temp 2: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error 2");
        }
    }
}
```

### Массив адресов
Для более удобного взаимодействия с несколькими датчиками есть класс `GyverDS18Array`. Пример чтения "связки" датчиков в авто режиме сразу в массив температур:

```cpp
#include <GyverDS18Array.h>

uint64_t addr[] = {
    0x4D0417508099FF28,
    0xFE04175159CDFF28,
    0xC3041750E553FF28,
    0xCF0417505B78FF28,
};
GyverDS18Array ds(2, addr, 4);

void setup() {
    Serial.begin(115200);
    ds.setResolution(12);  // для всех
}

void loop() {
    // это таймер с периодом к setResolution, он сам делает request
    if (!ds.tick()) {
        
        float temps[4];
        ds.readTemps(temps);
        for (float t : temps) {
            Serial.println(t);
        }
        Serial.println();
    }
}
```

Или в "ручном" режиме:

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

### Сканирование шины
Также в библиотеке есть сканер шины, который позволяет определить адреса всех подключенных датчиков. Он имеет три режима работы:

```cpp
#define DS_PIN 2

void setup() {
    Serial.begin(115200);

    // 1. поиск в массив
    {
        uint64_t addr[5] = {};
        uint8_t res = gds::Search(DS_PIN).scan(addr, 5);
        Serial.println(res);  // кол-во найденных

        // выведем в порт
        for (uint64_t& a : addr) {
            gds::Addr(a).printTo(Serial);
        }
    }

    // 2. цикличный поиск
    {
        gds::Search s(DS_PIN);

        while (s.search()) {
            s.printTo(Serial);
            // s.addr;  // это uint64_t
        }
    }

    // 3. поиск по индексу
    {
        gds::Search(DS_PIN).find(0).printTo(Serial);  // печать [индекс 0]

        gds::Search s(DS_PIN);
        s.find(1).printTo(Serial);  // печать [индекс 1]
        uint64_t adr = s.find(2);   // как uint64_t [индекс 2]
    }
}

void loop() {
}
```

Возможные сценарии:

- Заполнить массив для GyverDS18Array
- Обращаться через GyverDS18 по индексу (напр. `ds.readTemp(gds::Search(DS_PIN).find(0))`)

Примечание: поиск по индексу будет выдавать каждый раз тот же адрес среди одного набора адресов, другими словами - при одних и тех же подключенных датчиках индексация будет сохраняться. Но если отключить один датчик - индексация сместится, т.к. по сути привязана к адресам и их сортировке. Если поменять один датчик в связке - могут измениться все индексы. Поэтому лучше пользоваться абсолютной адресацией.

### Работа с памятью датчика
Датчик имеет 2 байта EEPROM памяти (не сбрасывается при перезагрузке), их можно использовать в каких-то своих сценариях, например записывать туда калибровку или что то ещё. Важный момент: запись этих данных пересекается с установкой разрешения датчика, поэтому функция `setResolution()` ДЛЯ ВСЕХ (в `GyverDS18Array` и `GyverDS18A`) сбрасывает эти два байта в классах, в классе для одного датчика - не сбрасывает.

Для записи используется функция `writeRAM()`. Сценарий записи своих данных и желаемого разрешения:

```cpp
// запись данных в RAM (оперативную память)
// здесь мы пишем два своих байта данных
ds.writeRAM(0xAB, 0xCD);

// можно сохранить данные в EEPROM датчика при помощи отдельной функции
ds.copyRAM();

// и прочитать из EEPROM обратно в RAM
// при запуске датчика они читаются автоматически!
ds.recallRAM();
```

Чтение данных из памяти производится в переменную типа `gds:RAM`, в этом буфере будет температура, данные и разрешение. За одно обращение к датчику можно вытащить все нужные данные, особенно полезно если там хранится калибровка - можно сразу её применить:

```cpp
gds::RAM ram;
ds.readRAM(&ram);
Serial.println(ram.th, HEX);
Serial.println(ram.tl, HEX);
Serial.println(ram.getTemp() / 16.0);
Serial.println(ram.getRes());
```

<a id="versions"></a>

## Версии
- v1.0
- v1.0.3 - поправлены тайминги для китайских датчиков
- v1.1.0 - оптимизация работы с uint64, добавлены фичи
- v1.2.0

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
