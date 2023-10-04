#ifndef CONSTANTS_H
#define CONSTANTS_H

// Display
static const char *DISPLAY_DATA_TITLE = "Weight:";
static const char *DISPLAY_DATA_UNIT = "g";

// Modules
static const char *MODULE_WIFI = "Wifi";
static const char *MODULE_MQTT = "MQTT";
static const char *MODULE_SCALE = "Scale";
static const char *MODULE_Display = "Display";

// Messages
static const char *MESSAGE_INITIALIZE = "initializing...";
static const char *MESSAGE_SCALE_READY = "Put a weight on your scale to get started.";
static const char *MESSAGE_TARE_START = "Remove all weights from your scale.";
static const char *MESSAGE_TARE_READY = "Done.";
static const char *MESSAGE_CALIBRATION_START = "Calibration started.";
static const char *MESSAGE_CALIBRATION_KNOWN_WEIGHT = "Done. Place a known weight on the scale.";
static const char *MESSAGE_CALIBRATION_READY = "Done. The result is: ";

// Titles
static const char *TITLE_INITIALIZE = "filamentwaage";
static const char *TITLE_ERROR = "Error";
static const char *TITLE_MODULE = "Module";
static const char *TITLE_CALIBRATION = "Calibration";

// error
static const char *ERROR_TARE_FAILED = "Taring failed. Check serial.";
static const char *ERROR_SCALE_NOT_READY = "Scale not ready yet. Check serial.";

#endif