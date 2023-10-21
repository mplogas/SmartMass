/**
 * @file conversion.h
 * @brief Contains the declaration of the Conversion class and its member functions.
 */

#ifndef CONVERISON_H
#define CONVERISON_H

#include <Arduino.h>

/**
 * @brief A class containing static methods for converting between different data types.
 */
class Conversion {
public:
    /**
     * @brief Converts a long value to a byte array.
     * 
     * @param longVal The long value to convert.
     * @param block The byte array to store the converted value in.
     */
    static void longToByte(long &longVal, byte *block);

    /**
     * @brief Converts a long value to a byte array.
     * 
     * @param longVal The long value to convert.
     * @param block The byte array to store the converted value in.
     */
    static void ulongToByte(unsigned long &longVal, byte *block);

    /**
     * @brief Converts a UUID string to a byte array.
     * 
     * @param uuid The UUID string to convert.
     * @param block The byte array to store the converted value in.
     */
    static void uuidToByte(String &uuid, byte *block);

    /**
     * @brief Converts a UUID character array to a byte array.
     * 
     * @param uuid The UUID character array to convert.
     * @param block The byte array to store the converted value in.
     */
    static void uuidToByte(char *uuid, byte *block);

    /**
     * @brief Splits a string into three byte arrays.
     * 
     * @param input The string to split.
     * @param block1 The first byte array to store the split value in.
     * @param block2 The second byte array to store the split value in.
     * @param block3 The third byte array to store the split value in.
     */
    static void splitToByteArrays(String &input, byte *block1, byte *block2, byte *block3);

    /**
     * @brief Splits a character array into three byte arrays.
     * 
     * @param input The character array to split.
     * @param block1 The first byte array to store the split value in.
     * @param block2 The second byte array to store the split value in.
     * @param block3 The third byte array to store the split value in.
     */
    static void splitToByteArrays(char *input, byte *block1, byte *block2, byte *block3);

    /**
     * @brief Converts a byte array to a long value.
     * 
     * @param block The byte array to convert.
     * @return The converted long value.
     */
    static long byteToLong(byte *block);

    /**
     * @brief Converts a byte array to an unsigned long value.
     * 
     * @param block The byte array to convert.
     * @return The converted long value.
     */
    static unsigned long byteToULong(byte *block);

    /**
     * @brief Converts a byte array to a UUID string.
     * 
     * @param block The byte array to convert.
     * @return The converted UUID string.
     */
    static String byteToUuid(byte *block);

    /**
     * @brief Converts a byte array to a UUID character array.
     * 
     * @param block The byte array to convert.
     * @param uuid The character array to store the converted value in.
     */
    static void byteToUuid(byte *block, char *uuid);

    /**
     * @brief Converts three byte arrays to a single string.
     * 
     * @param block1 The first byte array to convert.
     * @param block2 The second byte array to convert.
     * @param block3 The third byte array to convert.
     * @return The converted string.
     */
    static String byteArraysToString(byte *block1, byte *block2, byte *block3);

    /**
     * @brief Converts three byte arrays to a single character array.
     * 
     * @param block1 The first byte array to convert.
     * @param block2 The second byte array to convert.
     * @param block3 The third byte array to convert.
     * @param charVal The character array to store the converted value in.
     */
    static void byteArraysToString(byte *block1, byte *block2, byte *block3, char *charVal);    

    /**
     * @brief Prints the contents of a byte array to the serial monitor.
     * 
     * @param buffer The byte array to print.
     * @param bufferSize The size of the byte array.
     */
    static void dumpByteArray(byte *buffer, byte bufferSize);

    /**
     * @brief Converts a string to a byte array.
     * 
     * @param input The string to convert.
     * @param block The byte array to store the converted value in.
     */
    static void stringToByteArray(String input, byte *block);

    /**
     * @brief Converts a character array to a byte array.
     * 
     * @param input The character array to convert.
     * @param block The byte array to store the converted value in.
     */
    static void charArrayToByteArray(char *input, byte *block);

    /**
     * @brief Converts a byte array to a string.
     * 
     * @param block The byte array to convert.
     * @return The converted string.
     */
    static String byteArrayToString(byte *block);

    /**
     * @brief Converts a byte array to a character array.
     * 
     * @param block The byte array to convert.
     * @param output The character array to store the converted value in.
     */
    static void byteArrayToCharArray(byte *block, char *output);
};

#endif