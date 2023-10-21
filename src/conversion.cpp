#include "conversion.h"

void Conversion::dumpByteArray(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void Conversion::uuidToByte(String &uuid, byte *block) {
    // Remove the hyphens
    uuid.replace("-", "");
    
    // Ensure the UUID is the correct length
    if (uuid.length() != 32) {
        Serial.println("Invalid UUID length");
        return;
    }
    
    // Convert each pair of hexadecimal digits to a byte
    for (int i = 0; i < 16; i++) {
        String hexPair = uuid.substring(i * 2, (i * 2) + 2);
        block[i] = (byte) strtol(hexPair.c_str(), NULL, 16);
    }
}



void Conversion::uuidToByte(char *uuid, byte *block) {
    // Remove the hyphens
    for (int i = 0, j = 0; i < 36; ++i) {
        if (uuid[i] != '-') {
            uuid[j++] = uuid[i];
        }
    }
    uuid[32] = '\0'; // Null-terminate the string after removing hyphens
    
    // Convert each pair of hexadecimal digits to a byte
    for (int i = 0; i < 16; i++) {
        char hexPair[3] = {uuid[i * 2], uuid[(i * 2) + 1], '\0'};
        block[i] = (byte) strtol(hexPair, NULL, 16);
    }
}

void Conversion::splitToByteArrays(String &input, byte *block1, byte *block2, byte *block3) {
    // Ensure the input is not longer than 48 characters
    if (input.length() > 48) {
        Serial.println("Input length exceeds maximum limit");
        return;
    }
    
    // slice the string into 3 parts, each 16 characters long and fill up the blocks
    for (int i = 0; i < 16; i++) {
        if (i < input.length()) {
            block1[i] = (byte)input[i];
        } else {
            block1[i] = 0;
        }
        
        if ((i + 16) < input.length()) {
            block2[i] = (byte)input[i + 16];
        } else {
            block2[i] = 0;
        }
        
        if ((i + 32) < input.length()) {
            block3[i] = (byte)input[i + 32];
        } else {
            block3[i] = 0;
        }
    }
}

void Conversion::splitToByteArrays(char *input, byte *block1, byte *block2, byte *block3) {
    // Ensure the input is not longer than 48 characters
    if (strlen(input) > 48) {
        Serial.println("Input length exceeds maximum limit and will be cut off at 48 characters");
    }

    // Convert each pair of hexadecimal digits to a byte
    for (int i = 0; i < 16; i++) {
        if (i * 2 < strlen(input)) {
            char hexPair[3] = {input[i * 2], input[(i * 2) + 1], '\0'};
            block1[i] = (byte) strtol(hexPair, NULL, 16);
        } else {
            block1[i] = 0;
        }

        if ((i + 16) * 2 < strlen(input)) {
            char hexPair[3] = {input[(i + 16) * 2], input[((i + 16) * 2) + 1], '\0'};
            block2[i] = (byte) strtol(hexPair, NULL, 16);
        } else {
            block2[i] = 0;
        }

        if ((i + 32) * 2 < strlen(input)) {
            char hexPair[3] = {input[(i + 32) * 2], input[((i + 32) * 2) + 1], '\0'};
            block3[i] = (byte) strtol(hexPair, NULL, 16);
        } else {
            block3[i] = 0;
        }
    }
}

void Conversion::longToByte(long &longVal, byte *block)
{
    block[0] = (longVal >> 24) & 0xFF;
    block[1] = (longVal >> 16) & 0xFF;
    block[2] = (longVal >> 8) & 0xFF;
    block[3] = longVal & 0xFF;
    // clearing the remaining blocks
    for (int i = sizeof(block); i < 16; i++)
    {
        block[i] = 0xFF;
    }

    Serial.println();
}

void Conversion::ulongToByte(unsigned long &longVal, byte *block)
{
    block[0] = (longVal >> 24) & 0xFF;
    block[1] = (longVal >> 16) & 0xFF;
    block[2] = (longVal >> 8) & 0xFF;
    block[3] = longVal & 0xFF;
    // clearing the remaining blocks
    for (int i = sizeof(block); i < 16; i++)
    {
        block[i] = 0xFF;
    }

    Serial.println();
}

String Conversion::byteToUuid(byte *block) {
    String uuid = "";
    for (int i = 0; i < 16; i++) {
        if (i == 4 || i == 6 || i == 8 || i == 10) {
            uuid += "-";
        }
        char hexPair[3];
        sprintf(hexPair, "%02x", block[i]);
        uuid += hexPair;
    }
    return uuid;
}

void Conversion::byteToUuid(byte *block, char *uuid) {
    for (int i = 0; i < 16; i++) {
        if (i == 4 || i == 6 || i == 8 || i == 10) {
            strcat(uuid, "-");
        }
        char hexPair[3];
        sprintf(hexPair, "%02x", block[i]);
        strcat(uuid, hexPair);
    }
}

long Conversion::byteToLong(byte *byteVal)
{
    long result = 0;
    for (int i = 0; i < 4; i++)
    {
        result = (result << 8) | byteVal[i];
    }
    return result;
}

unsigned long Conversion::byteToULong(byte *byteVal)
{
    unsigned long result = 0;
    for (int i = 0; i < 4; i++)
    {
        result = (result << 8) | byteVal[i];
    }
    return result;
}

String Conversion::byteArraysToString(byte *block1, byte *block2, byte *block3) {
    char output[48] = "";
    for (int i = 0; i < 16; i++) {
        if (block1[i] != 0) output[i] = (char)block1[i];
        if (block2[i] != 0) output[i + 16] = (char)block2[i];
        if (block3[i] != 0) output[i + 32] = (char)block3[i];
    }

    Serial.println(output);
    return output;
}

void Conversion::byteArraysToString(byte *block1, byte *block2, byte *block3, char *output) {
    for (int i = 0; i < 16; i++) {
        if (block1[i] != 0) {
            char hexPair[3];
            sprintf(hexPair, "%02x", block1[i]);
            strcat(output, hexPair);
        }
        if (block2[i] != 0) {
            char hexPair[3];
            sprintf(hexPair, "%02x", block2[i]);
            strcat(output, hexPair);
        }
        if (block3[i] != 0) {
            char hexPair[3];
            sprintf(hexPair, "%02x", block3[i]);
            strcat(output, hexPair);
        }
    }
}

void Conversion::stringToByteArray(String input, byte *block) {
    for (int i = 0; i < 16; i++) {
        if (i < input.length()) {
            block[i] = (byte)input[i];
        } else {
            block[i] = 0;
        }
    }
}

void Conversion::charArrayToByteArray(char *input, byte *block) {
    for (int i = 0; i < 16; i++) {
        if (i < strlen(input)) {
            block[i] = (byte)input[i];
        } else {
            block[i] = 0;
        }
    }
}

String Conversion::byteArrayToString(byte *block) {
    String output = "";
    for (int i = 0; i < 16; i++) {
        if (block[i] != 0) {
            output += (char)block[i];
        } else {
            break;
        }
    }
    return output;
}

void Conversion::byteArrayToCharArray(byte *block, char *output) {
    for (int i = 0; i < 16; i++) {
        if (block[i] != 0) {
            output[i] = (char)block[i];
        } else {
            output[i] = '\0';
            break;
        }
    }
}