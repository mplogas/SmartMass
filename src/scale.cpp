/**
 * @file scale.cpp
 * @brief Implementation of the Scale class.
 * 
 * This file contains the implementation of the Scale class, which is used to interface with the HX711 load cell amplifier.
 * The Scale class provides methods for initializing the HX711, calibrating the scale, taring the scale, and measuring the weight.
 * 
 */
#include "scale.h"

Scale::Scale(uint8_t dOutPin, uint8_t sckPin)
{
    scale.begin(dOutPin, sckPin);
}

void Scale::init(long calibration)
{
    if (scale.is_ready())
    {
        scale.set_scale(calibration);
        scale.tare();
    }
    else
    {
        if (scale.wait_ready_timeout(2000))
            init(calibration);
        else
            Serial.println("Failed to initialize HX711.");
    }
}

void Scale::init(long calibration, unsigned long intervallMs)
{
    timeIntervallMs = intervallMs;
    init(calibration);
}

// blocking!
// TODO: make this non-blocking if possible
long Scale::calibrate(unsigned long knownWeight)
{
    long calibration = 0;
    if (scale.is_ready())
    {
        Serial.println("Tare... remove any weights from the scale.");
        delay(5000);
        calibrationStep01();
        Serial.println("Tare done...");
        delay(1000);
        Serial.print("Place a known weight on the scale...");
        delay(5000);
        calibration = calibrationStep02(knownWeight);
    }
    else
    {
        Serial.println("HX711 not ready for calibration.");
    }

    return calibration;
}
void Scale::calibrationStep01() {
        scale.set_scale();
        scale.tare(5);
}
long Scale::calibrationStep02(unsigned long knownWeight) {
        long reading = scale.get_units(10);
        Serial.print("Reading (10): ");
        Serial.println(reading);
        long calibration = reading / knownWeight;
        Serial.print("Calibration factor: ");
        Serial.println(calibration);

        return calibration;
}

bool Scale::tare()
{
    if (scale.is_ready())
    {
        scale.tare();
        return true;
    }
    else
    {
        Serial.println("HX711 not ready for taring.");
        return false;
    }
}

void Scale::measure(Scale::Measurement &measurement, uint8_t samplingSize = 5)
{
    unsigned long currentRunMs = millis();

    if (currentRunMs - measurement.ts >= timeIntervallMs)
    {
        if (scale.is_ready())
        {
            long reading = scale.get_units(samplingSize);

            measurement.ts = currentRunMs;
            measurement.result = reading;
        }
        else
        {
            Serial.println("HX711 not ready for measuring.");
        }
    }
}

bool Scale::isReady() {
    return scale.is_ready();
}