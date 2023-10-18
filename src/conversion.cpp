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
    
    // Convert each pair of hexadecimal digits to a byte
    for (int i = 0; i < 16; i++) {
        if (i * 2 < input.length()) {
            String hexPair = input.substring(i * 2, (i * 2) + 2);
            block1[i] = (byte) strtol(hexPair.c_str(), NULL, 16);
        } else {
            block1[i] = 0;
        }

        if ((i + 16) * 2 < input.length()) {
            String hexPair = input.substring((i + 16) * 2, ((i + 16) * 2) + 2);
            block2[i] = (byte) strtol(hexPair.c_str(), NULL, 16);
        } else {
            block2[i] = 0;
        }

        if ((i + 32) * 2 < input.length()) {
            String hexPair = input.substring((i + 32) * 2, ((i + 32) * 2) + 2);
            block3[i] = (byte) strtol(hexPair.c_str(), NULL, 16);
        } else {
            block3[i] = 0;
        }
    }
}

void Conversion::splitToByteArrays(char *input, byte *block1, byte *block2, byte *block3) {
    // Ensure the input is not longer than 48 characters
    if (strlen(input) > 48) {
        Serial.println("Input length exceeds maximum limit");
        return;
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

    dumpByteArray(block, 16);
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

String Conversion::byteArraysToString(byte *block1, byte *block2, byte *block3) {
    String output = "";
    for (int i = 0; i < 16; i++) {
        if (block1[i] != 0) {
            char hexPair[3];
            sprintf(hexPair, "%02x", block1[i]);
            output += hexPair;
        }
        if (block2[i] != 0) {
            char hexPair[3];
            sprintf(hexPair, "%02x", block2[i]);
            output += hexPair;
        }
        if (block3[i] != 0) {
            char hexPair[3];
            sprintf(hexPair, "%02x", block3[i]);
            output += hexPair;
        }
    }
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