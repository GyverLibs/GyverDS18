This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverDS18.svg?color=brightgreen)](https://github.com/GyverLibs/GyverDS18/releases/latest/download/GyverDS18.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/GyverDS18.svg)](https://registry.platformio.org/libraries/gyverlibs/GyverDS18)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverDS18?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverDS18
Dallas DS18b20 Light Thermometer Library, an updated and more user-friendly version of the library[microDS18B20](https://github.com/GyverLibs/microDS18B20):
- Convenient asynchronous reading of temperature
- Reading an address like`uint64_t`Convenient work with addresses
- Processing temperature reading errors and sensor connection
- Support for parasitic nutrition
- Full access to sensor memory (read/write your data, etc.)
- Tested for AVR/ESP8266/ESP32

### Compatibility
Compatible with all Arduino platforms (Arduino features are used)

### Dependencies
- [GyverIO](https://github.com/GyverLibs/GyverIO)

## Contents
- [Connection](#wiring)
- [Use of use](#usage)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="wiring"></a>

## Connection
![scheme](/docs/scheme.png)
P.S. Instead of a 4.7k resistor, two 10k can be used in parallel.

<a id="usage"></a>

## Documentation.
### GyverDS18Single
One sensor on the pin:

```cpp
GyverDS18Single();
GyverDS18Single(uint8_t pin, bool parasite = true);

// pin
void setPin(uint8_t pin);

// discharge
bool reset();

// turn on the parasitic feeding mode (silent. on)
void setParasite(bool parasite);

// set permission (9..12 bits)
bool setResolution(uint8_t res);

// permit
uint8_t readResolution();

// Read the sensor address. 0 - error
uint64_t readAddress();

// DS18 PARASITE - parasitic, DS18 EXTERNAL - normal, 0 - error
uint8_t readPower();

// get the current temperature measurement time, ms
uint16_t getConversionTime();

// ===================== TEMP =====================

// Automatic timer survey, call in loop. Return DS18 READY (0) to Ready and Ready
uint8_t tick();

// set the ticker period. Will be reset to a minimum when calling setResolution
void setPeriod(uint16_t prd);

// temperature
bool requestTemp();

// True - temperature ready (asynchronously)
bool ready();

// The temperature is ready (waiting)
bool waitReady();

// True is waiting for conversion.
bool isWaiting();

// temperature-read
bool readTemp();

// Get the "raw" temperature (multiplied by 16)
int16_t getTempRaw();

// heat up
int16_t getTempInt();

// float
float getTemp();

// ===================== MANUAL =====================

// read the contents of RAM in buffer 5 bytes
bool readRAM(gds::RAM* ram);

// Write data into memory (th, tl)
bool writeRAM(uint8_t b0, uint8_t b1);

// Record the contents of RAM in EEPROM
bool copyRAM();

// write the contents of EEPROM into RAM
bool recallRAM();

// permit
void applyResolution(uint8_t res);
```

### GyverDS18
Direct addressing - you need to transfer the sensor address to the functions:

```cpp
GyverDS18();
GyverDS18(uint8_t pin, bool parasite = true);

// Automatic timer request for everyone to call in loop. Return DS18 READY (0) on timer readiness
uint8_t tick();

bool setResolution(uint8_t res);    // set up
bool setResolution(uint8_t res, uint64_t addr);
uint8_t readResolution(uint64_t addr);
uint8_t readPower(uint64_t addr);
bool requestTemp();     // request
bool requestTemp(uint64_t addr);
bool readTemp(uint64_t addr);
bool readRAM(gds::RAM* ram, uint64_t addr);
bool writeRAM(uint8_t b0, uint8_t b1, uint64_t addr);
bool copyRAM(uint64_t addr);
bool recallRAM(uint64_t addr);
```

### GyverDS18Array
Works with an array of addresses - you need to transmit the sensor index in the array:

```cpp
GyverDS18Array();
GyverDS18Array(uint8_t pin, uint64_t* addr = nullptr, uint8_t amount = 0, bool parasite = true);

// Automatic timer request for everyone to call in loop. Return DS18 READY (0) on timer readiness
uint8_t tick();

// read the array. NAN if reading error
void readTemps(float* arr);

// connect an array of uint64 t addresses[]
void setAddress(uint64_t* addr, uint8_t amount);

// Get the number of addresses in the array
uint8_t amount();

// read the array. NAN if reading error
void readTemps(float* arr);

bool setResolution(uint8_t res);    // set up
bool setResolution(uint8_t res, uint8_t index);
uint8_t readResolution(uint8_t index);
uint8_t readPower(uint8_t index);
bool requestTemp();     // request
bool requestTemp(uint8_t index);
bool readTemp(uint8_t index);
bool readRAM(gds::RAM* ram, uint8_t index);
bool writeRAM(uint8_t b0, uint8_t b1, uint8_t index);
bool copyRAM(uint8_t index);
bool recallRAM(uint8_t index);
```

### gds::RAM
Memory work:

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
Storage and withdrawal of address:

```cpp
Addr();
Addr(const uint64_t addr);

// address
uint64_t addr = 0;

// validity
operator bool();

// index access (up to 8)
uint8_t& operator[](uint8_t i);

// copy to an array (8 bytes)
void copyTo(uint8_t* buf);

// print
void printTo(Print& pr, bool newline = true);

// line up
String toString();
```

Example:

```cpp
uint64_t addr; // received

gds::Addr(addr).printTo(Serial);    // seal

gds::Addr a(addr);
String s = a.toString();    // linewise

uint64_t addr2 = a; // Convert to uint64 t

a[0];               // address-byte
```

### gds::Search
Search for addresses on the bus

```cpp
Search(uint8_t pin);

// Inherit gds::Addr

// Find addresses and record in the array
uint8_t scan(uint64_t* addrs, uint8_t len);

// index
gds::Addr find(uint8_t n);

// Search, call in while(), pick up from addr
bool search();

// stop
void stop();
```

## Use of use
### Parasite nutrition
The sensor can be connected via two wires (GND and DATA) - you need to close it with VCC and GND. DATA-pin should also be tightened to nutrition, and in the program you need to activate the parasitic mode.

### Address
You can connect several sensors to one pin - in this case you will need to contact them at addresses. More on that is below.

### Permission
The sensor has a temperature resolution (accuracy) setting of 9..12 bits. The higher the resolution, the smaller the temperature change step, but the longer it takes the sensor to measure:

| Resolution, bit | Step, °C | Measurement time, ms |
|-------------- |------- |------------------- |
| 9 | 0.5 | 95 |
| 10 | 0.25 | 190 |
| 11 | 0.125 | 380 |
| 12 | 0.0625 | 760 |

### One sensor
A class is used to work with one sensor on a pin.`GyverDS18Single`. The simplest version of the survey is automatically through a ticker: the library itself polls the sensor by timer and processes errors, and on successful reading it is enough to simply get the temperature:

```cpp
#include <GyverDS18.h>
GyverDS18Single ds(2);  // pin

void setup() {
    Serial.begin(115200);
    // ds.setResolution(12); // permission installation. Affects the survey period
    // ds.setPeriod(2000); // manual setting of the survey period
}

void loop() {
    if (!ds.tick()) {
        // Readiness and Successful Reading
        Serial.println(ds.getTemp());
    }
}
```

When changing the resolution, the library sets a minimum survey period for the current resolution, i.e. in auto mode, the sensor will output the result with the highest possible frequency. The survey period can be extended manually if necessary.

`ds.getTemp()`can be called anywhere in the program, but the correctness and relevance of its value is guaranteed only in the condition of the ticker.

In the same mode, you can catch errors - the library processes the status of the sensor connection and data transmission errors:

```cpp
#include <GyverDS18.h>
GyverDS18Single ds(2);  // pin

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

You can work with the sensor at a lower level. The temperature is obtained in four stages:
- Request the temperature.`requestTemp()`
- Waiting for a time equal`getConversionTime()`Depends on the approved permit
- Read the temperature`readTemp()`
- Get the temperature.`getTemp()`or`getTempInt()`

> [!NOTE]
> Immediately after turning on, the sensor has a temperature of 85 degrees in the buffer. If you do not request a temperature, the temperature will be read 85 degrees. Therefore, the library ignores the value of 85** degrees, sensors can be connected "hot" and not be afraid that somewhere in the program the number 85 will appear sharply.

> [!NOTE]
> `readTemp()`**Read the temperature from the sensor.`getTemp()`- returns the result from the buffer. Thus, it is possible to cause`getTemp()`post-call`readTemp()`several times in a row, if necessary

You can independently request a measurement and receive a response on the built-in timer - here is an example of cycling * (note: if the sensor "falls off", the cycle will be interrupted, because if the request ends in error - the timer will not be restarted) *:

```cpp
#include <GyverDS18.h>
GyverDS18Single ds(2);  // pin

void setup() {
    Serial.begin(115200);
    ds.requestTemp();  // first measurement request
}

void loop() {
    if (ds.ready()) {         // timed
        if (ds.readTemp()) {  // if the reading is successful
            Serial.print("temp: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error");
        }

        ds.requestTemp();  // query
    }
}
```

You can also use the status to start the first conversion.`isWaiting()`*(Note: if the sensor falls off, the request will occur continuously at loop speed)*

```cpp
void loop() {
    // convert
    if (!ds.isWaiting()) ds.requestTemp();

    // get out
    if (ds.ready()) {
        if (ds.readTemp()) Serial.println(ds.getTemp());
        // Waiting status will be reset here
    }
}
```

For synchronous work (with waiting), you can use this design:

```cpp
void setup() {
    Serial.begin(115200);

    // request, wait, read
    if (ds.requestTemp() && ds.waitReady() && ds.readTemp()) {
        Serial.print("temp: ");
        Serial.println(ds.getTemp());
    } else {
        Serial.println("error");
    }
}
```

### Appeal to the address
You can connect several sensors to one pin. To access sensors, you need to know their unique addresses, so first you need to get an address. To do this, you need to connect the **one** sensor to the pin and call`readAddress()`. You can also use the bus scanner below. Note: The correct address can never be equal`0`This can be used to verify correctness. *

In this library, unlike others, the address is represented by the type`uint64_t`- more convenient for recording and storage. For more convenient output, you can use a built-in tool`Addr`, an example with port output:

```cpp
#include <GyverDS18.h>
GyverDS18Single ds(2);  // pin

void setup() {
    Serial.begin(115200);

    // ReadAddress will return uint64 t
    // uint64_t addr = ds.readAddress();

    // output
    gds::Addr addr = ds.readAddress();

    // if correct
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

Class is used to address sensors at addresses`GyverDS18`:

```cpp
#include <GyverDS18.h>
GyverDS18 ds(2);  // pin

uint64_t addr = 0xCF0417505B78FF28;

void setup() {
    Serial.begin(115200);
    // First measurement request. Request all sensors on the line
    ds.requestTemp();
}

void loop() {
    if (ds.ready()) {             // timed
        // Read the specific sensor at the address
        if (ds.readTemp(addr)) {  // if the reading is successful
            Serial.print("temp: ");
            Serial.println(ds.getTemp());
        } else {
            Serial.println("error");
        }

        ds.requestTemp();  // request for the next measurement for all
    }
}
```

Notes:
- You can request and get the temperature from any particular sensor at its address, just like the other commands.
- Measurement request and permission setting can be sent to all sensors at once without specifying the address.
- bundle`readTemp()`-`getTemp()`works as follows:`readTemp()`reads data from the sensor in the library buffer, respectively`getTemp()`receives the temperature from the library buffer, so you do not need to specify the address
- `ready()`the time required by the authorization in`setResolution()`. If the sensors have different resolutions,`ready()`You will always be guided by the last one. You can use millisecond constants for independent waiting.`DS18_TCONV_9`, `DS18_TCONV_10`, `DS18_TCONV_11`, `DS18_TCONV_12`

For this class, it also works auto-reference, but in this case, it simply requests the temperature by timer without reading it:

```cpp
void loop() {
    // This is a timer with a period to setResolution, it makes a request.
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

### Address array
For more convenient interaction with multiple sensors there is a class`GyverDS18Array`. An example of reading the "bundle" of sensors in auto mode immediately in an array of temperatures:

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
    ds.setResolution(12);  // for all
}

void loop() {
    // This is a timer with a period to setResolution, it makes a request.
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

Or in manual mode:

```cpp
#include <GyverDS18Array.h>

uint64_t addr[] = {
    0xD20417515A42FF28,
    0x4D0417508099FF28,
    0xFE04175159CDFF28,
    0xCF0417505B78FF28,
};
GyverDS18Array ds(2, addr, 4);  // pin, array, length

void setup() {
    Serial.begin(115200);
    // Request all sensors on the line
    ds.requestTemp();
}

void loop() {
    if (ds.ready()) {  // timed
        // pass through the array
        for (int i = 0; i < ds.amount(); i++) {
            Serial.print("#");
            Serial.print(i);
            Serial.print(": ");
            // index
            if (ds.readTemp(i)) Serial.print(ds.getTemp());
            else Serial.print("error");
            Serial.print(", ");
        }
        Serial.println();

        ds.requestTemp();  // request for the next measurement for all
    }
}
```

### Tyre scanning
Also in the library there is a bus scanner that allows you to determine the addresses of all connected sensors. It has three operating modes:

```cpp
#define DS_PIN 2

void setup() {
    Serial.begin(115200);

    // 1. search the array
    {
        uint64_t addr[5] = {};
        uint8_t res = gds::Search(DS_PIN).scan(addr, 5);
        Serial.println(res);  // number found

        // port
        for (uint64_t& a : addr) {
            gds::Addr(a).printTo(Serial);
        }
    }

    // 2. cyclical search
    {
        gds::Search s(DS_PIN);

        while (s.search()) {
            s.printTo(Serial);
            // s.addr; // is uint64 t
        }
    }

    // 3. index search
    {
        gds::Search(DS_PIN).find(0).printTo(Serial);  // print [index 0]

        gds::Search s(DS_PIN);
        s.find(1).printTo(Serial);  // print [index 1]
        uint64_t adr = s.find(2);   // as uint64 t [index 2]
    }
}

void loop() {
}
```

Possible scenarios:

- Fill in the array for GyverDS18Array
- Refer to GyverDS18 by index (e.g.`ds.readTemp(gds::Search(DS_PIN).find(0))`)

Note: an index search will produce the same address each time among the same set of addresses, in other words – with the same connected sensors indexing will be maintained. But if you turn off one sensor, the indexation will shift, because it is essentially tied to addresses and their sorting. If you change one sensor in a bundle, all indexes can change. Therefore, it is better to use absolute addressing.

### Dealing with sensor memory
The sensor has 2 bytes of EEPROM memory (does not reset when restarting), they can be used in some of their scenarios, for example, record calibration there or something else. Important point: the recording of this data overlaps with the setting of the sensor resolution, so the function`setResolution()`FOR ALL (in)`GyverDS18Array`and`GyverDS18A`) resets these two bytes in classes, in class for one sensor - does not reset.

A function is used for recording`writeRAM()`. Scenario of recording your data and desired permission:

```cpp
// Recording data in RAM (RAM)
// Here we write our two bytes of data.
ds.writeRAM(0xAB, 0xCD);

// You can store data in the EEPROM sensor using a separate function.
ds.copyRAM();

// and read from EEPROM back to RAM
// At the start of the sensor, they are read automatically!
ds.recallRAM();
```

Reading data from memory is made into a type variable`gds:RAM`This buffer will have temperature, data and resolution. For one appeal to the sensor, you can pull out all the necessary data, especially if the calibration is stored there - you can immediately apply it:

```cpp
gds::RAM ram;
ds.readRAM(&ram);
Serial.println(ram.th, HEX);
Serial.println(ram.tl, HEX);
Serial.println(ram.getTemp() / 16.0);
Serial.println(ram.getRes());
```

<a id="versions"></a>

## Versions
- v1.0
- v1.0.3 - adjusted timings for Chinese sensors
- v1.1.0 - Optimization of work with uint64, added features
- v1.2.0

<a id="install"></a>
## Installation
- A library is required to work[GyverIO](https://github.com/GyverLibs/GyverIO)
- The library can be found under the name **GyverDS18** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/GyverDS18/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features.
- Through the library manager IDE: find the library as when installing and click "Update"
- Manually: **Delete the folder with the old version** and then put the new one in its place. “Replacement” can not be done: sometimes new versions delete files that will remain when replaced and can lead to errors!

<a id="feedback"></a>
## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.
