#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>


//Display
const uint16_t DISPLAY_WIDTH = 128; 
const uint16_t DISPLAY_HEIGHT = 64;
const int8_t DISPLAY_RESET_PIN = -1; // Reset pin # (or -1 if sharing Arduino reset pin)
const String DISPLAY_DATA_TITLE = "Weight:";
const String DISPLAY_DATA_UNIT = "g";

// Load Cell
const uint8_t LOADCELL_DOUT_PIN = 16;
const uint8_t LOADCELL_SCK_PIN = 4;
const long LOADCELL_CALIBRATION = 987;
const long LOADCELL_KNOWN_WEIGHT = 100.0;
const unsigned long LOADCELL_MEASUREMENT_INTERVAL = 500; //if you get 'HX711 not ready for measuring' sampling size is too high for measurment interval
const uint8_t LOADCELL_MEASUREMENT_SAMPLING = 1; 



#endif