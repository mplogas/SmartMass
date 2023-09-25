#include "scale.h"

/*
    	Scale(uint8_t dOutPin, uint8_t sckPin);
        void init(long calibration);
        void init(long calibration, unsigned long measureEachMs);
        long calibrate(unsigned long knownWeight); 
*/

Scale::Scale(uint8_t dOutPin, uint8_t sckPin) {
    scale.begin(dOutPin, sckPin);
}

void Scale::init(long calibration) {
    if (scale.is_ready()) { 
        scale.set_scale(calibration);
        scale.tare();
    } else {
        if (scale.wait_ready_timeout(2000)) init(calibration);
        else Serial.println("Failed to initialize HX711.");
    }
}

void Scale::init(long calibration, unsigned long intervallMs) { 
    timeIntervallMs = intervallMs;
    init(calibration);
}

// blocking!
// TODO: make this non-blocking if possible
long Scale::calibrate(unsigned long knownWeight) {
    long calibration = 0;
    if (scale.is_ready()) { 
        Serial.println("Tare... remove any weights from the scale.");
        delay(5000);      
        scale.set_scale();    
        scale.tare();
        Serial.println("Tare done...");
        delay(1000);
        Serial.print("Place a known weight on the scale...");
        delay(5000);
        long reading = scale.get_units(10);
        Serial.print("Result: ");
        Serial.println(reading);
        calibration = reading/knownWeight;
        Serial.print("Calibration factor: ");
        Serial.println(calibration);        
    } else {
        Serial.println("HX711 not ready for calibration.");
    }

    return calibration;
}

void Scale::tare() {
     if (scale.is_ready()) { 
        scale.tare();
     } else {
        Serial.println("HX711 not ready for taring.");
     }
}


void Scale::measure(Scale::Measurement& measurement) {
    unsigned long currentRunMs = millis();

    if (currentRunMs - measurement.ts >= timeIntervallMs) {
        if (scale.is_ready()) { 
            long reading = scale.get_units(10);
            
            measurement.ts = currentRunMs;    
            measurement.result = reading;

            // Serial.print("HX711 reading -avg(10): ");
            // Serial.println(reading);
        } else {
            Serial.println("HX711 not ready for measuring.");
        }
    }
}