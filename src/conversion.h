
#ifndef CONVERISON_H
#define CONVERISON_H

#include <Arduino.h>

class Conversion {
public:
    static void longToByte(long &longVal, byte *block);
    static void uuidToByte(String &uuid, byte *block);
    static void uuidToByte(char *uuid, byte *block);
    static void splitToByteArrays(String &input, byte *block1, byte *block2, byte *block3);
    static void splitToByteArrays(char *input, byte *block1, byte *block2, byte *block3);
    static long byteToLong(byte *block);
    static String byteToUuid(byte *block);
    static void byteToUuid(byte *block, char *uuid);
    static String byteArraysToString(byte *block1, byte *block2, byte *block3);
    static void byteArraysToString(byte *block1, byte *block2, byte *block3, char *charVal);    
    static void dumpByteArray(byte *buffer, byte bufferSize);
};

#endif