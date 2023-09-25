#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>
#include "HX711.h"

// Place a known weight on the scale...Result: 98730
// Calibration factor: 987


// HX711 circuit wiring
// const int LOADCELL_DOUT_PIN = 16;
// const int LOADCELL_SCK_PIN = 4;
// const long KNOWN_WEIGHT = 100.0;

class Scale 
{
    private: 
        boolean firstRun = true;
        HX711 scale;
        unsigned long timeIntervallMs = 1000;
    //protected:
    public:
        struct Measurement {
            unsigned long ts;
            unsigned long result;
        };
    	Scale(uint8_t dOutPin, uint8_t sckPin);
        void init(long calibration);
        void init(long calibration, unsigned long measureEachMs);
        long calibrate(unsigned long knownWeight); 
        void tare();
        void measure(Measurement& measurement);
};


#endif