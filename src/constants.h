#ifndef CONSTANTS_H
#define CONSTANTS_H

// Display
const char *DISPLAY_DATA_TITLE = "Weight:";
const char *DISPLAY_DATA_UNIT = "g";

// Modules
const char *MODULE_WIFI = "Wifi";
const char *MODULE_MQTT = "MQTT";
const char *MODULE_SCALE = "Scale";
const char *MODULE_Display = "Display";

// Messages
const char *MESSAGE_INITIALIZE = "initializing...";
const char *MESSAGE_SCALE_READY = "Put a weight on your scale to get started.";
const char *MESSAGE_TARE_START = "Remove all weights from your scale.";
const char *MESSAGE_TARE_READY = "Done.";
const char *MESSAGE_CALIBRATION_START = "Calibration started.";
const char *MESSAGE_CALIBRATION_KNOWN_WEIGHT = "Done. Place a known weight on the scale.";
const char *MESSAGE_CALIBRATION_READY = "Done. The result is: ";

// Titles
const char *TITLE_INITIALIZE = "filamentwaage";
const char *TITLE_ERROR = "Error";
const char *TITLE_MODULE = "Module";
const char *TITLE_CALIBRATION = "Calibration";

// error
const char *ERROR_TARE_FAILED = "Taring failed. Check serial.";
const char *ERROR_SCALE_NOT_READY = "Scale not ready yet. Check serial.";

#endif