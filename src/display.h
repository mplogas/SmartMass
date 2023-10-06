#ifndef DISPLAY_H
#define DISPLAY_H

#include "constants.h"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class Display
{
private:
    Adafruit_SSD1306 *pDevice;
    unsigned long timeout = 30000;
    unsigned long lastUpdate = 0;
    bool displayStandby = false;
    void clearDisplay();
public:
    struct Data
    {
        const char *title;
        long result;
        const char *unit;
    };
    struct Error {
        const char *module;
        const char *msg;
    };
    Display(uint16_t displayWidth, uint16_t displayHeight, int8_t resetPin, unsigned long screenTimeOut);
    bool init();
    void showMeasurement(Data &data);
    void loop();
    void showInitMessage();
    void showErrorMessage(Error &error);
    void showMessage(const char *msg);
    void showTitle(const char *title);
    void showCalibrationMessage(long calibration);
};

#endif