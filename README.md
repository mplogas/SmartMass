# Filamentwaage
## A scale built to weigh you 3D printing / FDM filament spool.

#### Motivation

Recently, I saw on YouTube a video of someone using a filament scale connected to a filament management system and got excited. I do print a lot, but I don't have the need for a proper filament management system yet. Still I wondered more than once if there's enough filament on a left-over spool for a print and I often just use a new spool.

So I went to this creators Discord, hoping to get a BOM, some code and STLs to build my own scale. Unfortunately, he wasn't sharing STLs or code (and later even turning me down when I offered to add a connector to his management system). Hence I decided to build my own filament scale based on an ESP32, with OLED display, WiFi, MQTT and whatnot - how hard could it be for an IoT SME right?

#### BOM (bill of materials)

- a 2kg / 3kg load cell
- an HX711 signal ampifier
- an 0.96" 128x64 OLED display
- an ESP32-WROOM / ESP8266
- PLA / PETG / ... to print the models

#### Getting started

**Hardware**
1. Assemble the model
2. solder 

**Code:**
1. Open the project with platformio, I'm using the platformio plugin in VSCode.
2. copy ```configuration.h.template``` to ```configuration.h``` and fill in WiFi, MQTT and PIN information 
    1. *TODO: add detailed guide for options*
3. build and upload the code

