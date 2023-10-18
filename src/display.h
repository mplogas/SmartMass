#ifndef DISPLAY_H
#define DISPLAY_H

#include "constants.h"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

/**
 * @brief The Display class provides an interface for controlling an SSD1306 OLED display.
 * 
 */
class Display
{
private:
    Adafruit_SSD1306 *pDevice; /**< Pointer to the Adafruit_SSD1306 object used to control the display. */
    unsigned long timeout = 30000; /**< The time in milliseconds before the display goes into standby mode. */
    unsigned long lastUpdate = 0; /**< The time in milliseconds of the last display update. */
    bool displayStandby = false; /**< Flag indicating whether the display is in standby mode. */
    /**
     * @brief Clears the display.
     * 
     */
    void clearDisplay();
public:
    /**
     * @brief The Data struct contains the data to be displayed on the screen.
     * 
     */
    struct Data
    {
        const char *title; /**< The title of the measurement. */
        long result; /**< The measurement result. */
        const char *unit; /**< The unit of the measurement. */
    };
    /**
     * @brief The Error struct contains information about an error that occurred.
     * 
     */
    struct Error {
        const char *module; /**< The module where the error occurred. */
        const char *msg; /**< The error message. */
    };
    /**
     * @brief Constructs a new Display object.
     * 
     * @param displayWidth The width of the display in pixels.
     * @param displayHeight The height of the display in pixels.
     * @param resetPin The pin used to reset the display.
     * @param screenTimeOut The time in milliseconds before the display goes into standby mode.
     */
    Display(uint16_t displayWidth, uint16_t displayHeight, int8_t resetPin, unsigned long screenTimeOut);
    /**
     * @brief Initializes the display.
     * 
     * @return true if initialization was successful, false otherwise.
     */
    bool init();
    /**
     * @brief Updates the display.
     * 
     */
    void loop();
    /**
     * @brief Sets the time in milliseconds before the display goes into standby mode.
     * 
     * @param screenTimeOut The time in milliseconds before the display goes into standby mode.
     */
    void setScreenTimeOut(unsigned long screenTimeOut); 
    /**
     * @brief Displays the measurement data.
     * 
     * @param data The measurement data to be displayed.
     */
    void showMeasurement(Data &data);
    /**
     * @brief Displays an initialization message.
     * 
     */
    void showInitMessage();
    /**
     * @brief Displays an error message.
     * 
     * @param error The error information to be displayed.
     */
    void showErrorMessage(Error &error);
    /**
     * @brief Displays a message.
     * 
     * @param msg The message to be displayed.
     */
    void showMessage(const char *msg);
    /**
     * @brief Displays a title.
     * 
     * @param title The title to be displayed.
     */
    void showTitle(const char *title);
    /**
     * @brief Displays a calibration message.
     * 
     * @param calibration The calibration value to be displayed.
     */
    void showCalibrationMessage(long calibration);
};

#endif