#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class Display
{
private:
    Adafruit_SSD1306 *pDevice;
    unsigned long timeout = 30000;
    unsigned long lastUpdate = 0;
    bool displayStandby = false;;
    void clearDisplay();
    void showInitMessage();
    void showMessage(String msg);

public:
    struct Data
    {
        String title;
        unsigned long result;
        String unit;
    };
    Display(uint16_t displayWidth, uint16_t displayHeight, int8_t resetPin, unsigned long screenTimeOut);
    void init();
    void show(Data &data);
    void loop();
};

#endif