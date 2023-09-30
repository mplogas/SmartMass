#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class Display
{
private:
    Adafruit_SSD1306 *pDevice;

public:
    struct Data
    {
        String title;
        unsigned long result;
        String unit;
    };
    Display(uint16_t displayWidth, uint16_t displayHeight, int8_t resetPin);
    void init();
    void show(Data &data);
};

#endif