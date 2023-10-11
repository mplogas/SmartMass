# SmartMass
## Make every gram count!

### A smart weighing solution for almost anything, but FDM filament spools in particular. 

*pre-release quick project overview*

#### Motivation

The almighty YT algorithm suggested a 3D filament weighing solution to me recently, and as I have quite a few "almost empty" filament spools laying around I reached out to the maker. Unfortunately, no part of the suggested solution was open or free so I decided to build my own solution. From a simple local solution with a tiny OLED display, the scope quickly escalated with useful features such as MQTT read/write or RFID read/write.

As of now, the SmartMass device connects to an MQTT broker via WiFi and posts events to the ```smartmass/<deviceid>``` topic. The scale can be configured, calibrated and tared via MQTT using the documented payload in `./payload`. It aslso supports reading (and writing) filament spool metadata on Mifare 1K S50 tags (they're available as stickers!) with an RFID-RC522 module.

#### BOM (bill of materials)

- a 2kg / 3kg load cell
- an HX711 signal ampifier
- an 0.96" 128x64 OLED display
- an RFID-RC522 module
- an ESP32-WROOM / ESP8266
- PLA / PETG / ... to print the models

#### Getting started

**Broker**
1. Set up an MQTT broker (mosquitto, emqx, hive, ...) and create a user for the device

**Hardware**

*TODO* 

**Code:**
1. Open the project with platformio, I'm using the platformio plugin in VSCode.
2. copy ```configuration.h.template``` to ```configuration.h``` and fill in WiFi, MQTT and PIN information 
    1. *TODO: add detailed guide for options*
3. build and upload the code

