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
static const char *MESSAGE_TARE_START = "Remove all weights from your scale to start taring.";
static const char *MESSAGE_TARE_READY = "Done.";
static const char *MESSAGE_CALIBRATION_START = "Calibration started.";
static const char *MESSAGE_CALIBRATION_KNOWN_WEIGHT = "Done. Place a known weight on the scale.";
static const char *MESSAGE_CALIBRATION_READY = "Calibration done.";
static const char *MESSAGE_CALIBRATION_RESULT = "calibration factor:";

// Titles
static const char *TITLE_INITIALIZE = "SmartMass";
static const char *TITLE_ERROR = "Error";
static const char *TITLE_MODULE = "Module: ";
static const char *TITLE_CALIBRATION = "Calibrate";
static const char *TITLE_CONFIGURATION = "Configure";
static const char *TITLE_TARE = "Tare";

// error
static const char *ERROR_TARE_FAILED = "Taring failed. Check serial.";
static const char *ERROR_SCALE_NOT_READY = "Scale not ready yet. Check serial.";

// actions
static const char *ACTION_KEY = "action";
static const char *ACTION_TARE = "tare";
static const char *ACTION_CALIBRATE = "calibrate";
static const char *ACTION_CONFIGURE = "configure";
static const char *ACTION_TEST = "test";

#endif