
/**
 * @file scale.h
 * @brief Header file for Scale class that interfaces with HX711 load cell amplifier.
 * 
 * This file contains the declaration of Scale class that provides methods to initialize, calibrate, tare and measure weight using HX711 load cell amplifier.
 * The class also contains a struct Measurement that holds the timestamp and weight measurement.
 * 
 */
#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>
#include "HX711.h"


/**
 * @brief Class for interfacing with an HX711 load cell amplifier and measuring weight.
 */
class Scale
{
private:
    boolean firstRun = true; // Flag to indicate if the scale has been initialized.
    HX711 scale; // Instance of the HX711 library for communicating with the load cell amplifier.
    unsigned long timeIntervallMs = 1000; // Time interval in milliseconds between measurements.
public:
    /**
     * @brief Struct for storing a measurement result.
     */
    struct Measurement
    {
        unsigned long ts; // Timestamp of the measurement.
        long result; // Measured weight in units of the calibration factor.
    };

    /**
     * @brief Constructor for the Scale class.
     * @param dOutPin The data output pin of the HX711 amplifier.
     * @param sckPin The clock input pin of the HX711 amplifier.
     */
    Scale(uint8_t dOutPin, uint8_t sckPin);

    /**
     * @brief Initializes the scale with a calibration factor.
     * @param calibration The calibration factor to use for measuring weight.
     */
    void init(long calibration);

    /**
     * @brief Initializes the scale with a calibration factor and measurement interval.
     * @param calibration The calibration factor to use for measuring weight.
     * @param measureEachMs The time interval in milliseconds between measurements.
     */
    void init(long calibration, unsigned long measureEachMs);

    /**
     * @brief Performs a calibration procedure using a known weight.
     * @param knownWeight The weight in units of the calibration factor of the known weight used for calibration.
     * @return The calibration factor calculated from the known weight.
     */
    long calibrate(unsigned long knownWeight);

    /**
     * @brief Performs the first step of the calibration procedure.
     */
    void calibrationStep01();

    /**
     * @brief Performs the second step of the calibration procedure using a known weight.
     * @param knownWeight The weight in units of the calibration factor of the known weight used for calibration.
     * @return The calibration factor calculated from the known weight.
     */
    long calibrationStep02(unsigned long knownWeight);

    /**
     * @brief Sets the current load cell reading as the tare weight.
     * @return True if the tare weight was successfully set, false otherwise.
     */
    bool tare();

    /**
     * @brief Measures the weight using the current calibration factor and stores the result in the provided Measurement struct.
     * @param measurement The Measurement struct to store the measurement result in.
     * @param samplingSize The number of samples to take and average for the measurement.
     */
    void measure(Measurement &measurement, uint8_t samplingSize);

    /**
     * @brief Checks if the scale is ready to take a measurement.
     * @return True if the scale is ready, false otherwise.
     */
    bool isReady();
};

#endif