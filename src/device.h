#ifndef DEVICE_H
#define DEVICE_H

#include "configuration.h"
#include "display.h"
#include "scale.h"
#include "rfid.h"
#include <Preferences.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class Device
{
private:
    void intializeConfiguration();
    Preferences preferences;
    Display display;
    RFID rfid;
    Scale scale;
    AsyncWebServer server;
public:
    Device(Display &display, Scale &scale, RFID &rfid);
    struct Configuration
    {
        unsigned long displayTimeout;
        long loadcellCalibration;
        unsigned long loadcellKnownWeight;
        unsigned long loadcellMeasurementIntervall;
        uint8_t loadcellMeasurementSampling;
        unsigned long rfidDecay;
    };
    void init();
    void loop();
    Configuration getConfiguration();
    void setConfiguration(Configuration configuration);
    void resetConfiguration();
    void calibrateDevice();
    void tareDevice();
    void writeTag(TagData &tagData);
    void measureWeight();
};

#endif // DEVICE_H
