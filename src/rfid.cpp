/**
 * @file rfid.cpp
 * @brief Implementation of the RFID class for reading and writing data to RFID tags.
 *
 * This file contains the implementation of the RFID class, which provides methods for reading and writing data to RFID tags.
 * The class uses the MFRC522 library for communication with the RFID reader.
 *
 */
#include "rfid.h"

RFID::RFID(uint8_t chipselectPin, uint8_t resetPin)
{
    pMfrc522 = new MFRC522(chipselectPin, resetPin);
}

void RFID::prepareKey()
{
    for (uint8_t i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }
    // Conversion::dumpByteArray(key.keyByte, MFRC522::MF_KEY_SIZE);
}
void RFID::prepareKey(byte authKey[6])
{
    for (uint8_t i = 0; i < 6; i++)
    {
        key.keyByte[i] = authKey[i];
    }
    // Conversion::dumpByteArray(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void RFID::init(rfidCallback rfidCb)
{
    prepareKey();
    SPI.begin();
    pMfrc522->PCD_Init();
    callback = rfidCb;
}

void RFID::init(byte authKey[6], rfidCallback rfidCb)
{
    prepareKey(authKey);
    SPI.begin();
    pMfrc522->PCD_Init();
    callback = rfidCb;
}

bool RFID::write(TagData &tagData)
{
    bool result = writeTag(tagData);
    if (result)
        Serial.println(F("Tag written."));

    return result;
}

void RFID::loop()
{
    readTag();
}

void RFID::readTag()
{
    if (!openTag())
        return;

    byte size = 18; // The MIFARE_Read method requires a buffer that is at least 18 bytes to hold the 16 bytes of a block.
    byte buffer[size];
    TagData td;

    if (readBlock(spoolIdBlock, buffer))
    {
        td.spoolId = Conversion::byteToUuid(buffer);
    }
    else
    {
        Serial.println(F("Reading spoolid failed."));
        return; // early exit w/o spoolid
    }

    if (readBlock(spoolWeightBlock, buffer))
    {
        td.spoolWeight = Conversion::byteToLong(buffer);
    }
    else
    {
        Serial.println(F("Reading spool weight failed."));
    }

    if (readBlock(spoolManufacturerBlock, buffer))
    {
        td.manufacturer = Conversion::byteArrayToString(buffer);
    }
    else
    {
        Serial.println(F("Reading spool manufacturer failed."));
    }

    if (readBlock(spoolMaterialBlock, buffer))
    {
        td.material = Conversion::byteArrayToString(buffer);
    }
    else
    {
        Serial.println(F("Reading spool material failed."));
    }

    if (readBlock(spoolColorBlock, buffer))
    {
        td.color = Conversion::byteArrayToString(buffer);
    }
    else
    {
        Serial.println(F("Reading spool color failed."));
    }

    if (readBlock(spoolNameBlock1, buffer))
    {
        byte buffer2[size];
        byte buffer3[size];
        if (readBlock(spoolNameBlock2, buffer2))
        {
            if (readBlock(spoolNameBlock3, buffer3))
            {
                td.spoolName = Conversion::byteArraysToString(buffer, buffer2, buffer3);
            }
            else
            {
                Serial.println(F("Reading spool name block3 failed."));
            }
        }
        else
        {
            Serial.println(F("Reading spool name block2 failed."));
        }
    }
    else
    {
        Serial.println(F("Reading spool name block1 failed."));
    }

    if (readBlock(spoolTimestampBlock, buffer))
    {
        td.timestamp = Conversion::byteToLong(buffer);
    }
    else
    {
        Serial.println(F("Reading spool timestamp failed."));
    }

    // Dump the sector data
    // Serial.println(F("Current data in sector:"));
    // pMfrc522->PICC_DumpMifareClassicSectorToSerial(&(pMfrc522->uid), &key, sector);
    // Serial.println();

    callback(td);
    closeTag();
}

bool RFID::readBlock(byte blockId, byte buffer[18])
{
    if (authenticate(authKey, blockId))
    {
        byte size = 18;
        MFRC522::StatusCode status;
        status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(blockId, buffer, &size);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.printf("Reading block ");
            Serial.print(blockId);
            Serial.printf(" failed");
            Serial.println();
            return false;
        }
        else
        {
            Serial.printf("Reading block ");
            Serial.print(blockId);
            Serial.printf(" succeded");
            Serial.println();
            return true;
        }
    }
    else
    {
        Serial.printf("authenticating block ");
        Serial.print(blockId);
        Serial.printf(" failed");
        Serial.println();
        return false;
    }
}

bool RFID::writeBlock(byte blockId, byte block[16], byte size)
{
    if (authenticate(authKey, blockId))
    {
        if ((MFRC522::StatusCode)pMfrc522->MIFARE_Write(blockId, block, size) != MFRC522::STATUS_OK)
        {
            Serial.printf("Writing block ");
            Serial.print(blockId);
            Serial.printf(" failed");
            Serial.println();
            return false;
        }
        else
        {
            Serial.printf("Writing block ");
            Serial.print(blockId);
            Serial.printf(" succeded");
            Serial.println();
            return true;
        }
    }
    else
    {
        Serial.printf("authenticating block ");
        Serial.print(blockId);
        Serial.printf(" failed");
        Serial.println();
        return false;
    }
}

bool RFID::writeTag(TagData &tagData)
{
    if (!openTag())
    {
        Serial.println("no tag found");
        return false;
    }

    byte size = 16;
    MFRC522::StatusCode status;
    byte block[size];

    if (!tagData.spoolId.isEmpty())
    {
        Conversion::uuidToByte(tagData.spoolId, block);
        if (!writeBlock(spoolIdBlock, block, size))
        {
            Serial.println(F("Writing spool id failed"));
            return false; // early exit
        }
        Serial.println(F("Writing spool id succeeded"));
    }
    else
    {
        Serial.println(F("Empty spool id"));
        return false; // early exit
    }

    if (tagData.spoolWeight != 0)
    {
        Conversion::ulongToByte(tagData.spoolWeight, block);
        if (!writeBlock(spoolWeightBlock, block, size))
        {
            Serial.println(F("Writing spool weight failed"));
        }
        else
        {
            Serial.println(F("Writing spool weight succeeded"));
        }
    }
    else
    {
        Serial.println(F("Skipping spool weight"));
    }

    if (!tagData.manufacturer.isEmpty())
    {
        Conversion::stringToByteArray(tagData.manufacturer, block);
        if (!writeBlock(spoolManufacturerBlock, block, size))
        {
            Serial.println(F("Writing spool manufacturer failed"));
        }
        else
        {
            Serial.println(F("Writing spool manufacturer succeeded"));
        }
    }
    else
    {
        Serial.println(F("Skipping spool manufacturer"));
    }

    if (!tagData.material.isEmpty())
    {
        Conversion::stringToByteArray(tagData.material, block);
        if (!writeBlock(spoolMaterialBlock, block, size))
        {
            Serial.println(F("Writing spool material failed"));
        }
        else
        {
            Serial.println(F("Writing spool material succeeded"));
        }
    }
    else
    {
        Serial.println(F("Skipping spool material"));
    }

    if (!tagData.color.isEmpty())
    {
        Conversion::stringToByteArray(tagData.color, block);
        if (!writeBlock(spoolColorBlock, block, size))
        {
            Serial.println(F("Writing spool color failed"));
        }
        else
        {
            Serial.println(F("Writing spool color succeeded"));
        }
    }
    else
    {
        Serial.println(F("Skipping spool color"));
    }

    if (!tagData.spoolName.isEmpty())
    {
        byte block2[size];
        byte block3[size];
        Conversion::splitToByteArrays(tagData.spoolName, block, block2, block3);
        if (!writeBlock(spoolNameBlock1, block, size))
        {
            Serial.println(F("Writing spool name block1 failed"));
        }
        else
        {
            if (!writeBlock(spoolNameBlock2, block2, size))
            {
                Serial.println(F("Writing spool name block2 failed"));
            }
            else
            {
                if (!writeBlock(spoolNameBlock3, block3, size))
                {
                    Serial.println(F("Writing spool name block3 failed"));
                }
                else
                {
                    Serial.println(F("Writing spool name succeeded"));
                }
            }
        }
    }
    else
    {
        Serial.println(F("Skipping spool name"));
    }

    if (tagData.timestamp != 0)
    {
        Conversion::ulongToByte(tagData.timestamp, block);
        if (!writeBlock(spoolTimestampBlock, block, size))
        {
            Serial.println(F("Writing spool timestamp failed"));
        }
        else
        {
            Serial.println(F("Writing spool timestamp succeeded"));
        }
    }
    else
    {
        Serial.println(F("Skipping spool timestamp"));
    }

    // Dump the sector data
    // Serial.println(F("Current data in sector:"));
    // pMfrc522->PICC_DumpMifareClassicSectorToSerial(&(pMfrc522->uid), &key, sector);
    // Serial.println();

    closeTag();
    return true;
}

// bool RFID::clearTag(byte authKey[6])
// {
//     if (!openTag())
//         return false;

//     byte size = 16;
//     MFRC522::StatusCode status;

//     status = (MFRC522::StatusCode)pMfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(pMfrc522->uid));
//     if (status != MFRC522::STATUS_OK)
//     {
//         Serial.println(F("PCD_Authenticate() failed with Key B"));
//         return false;
//     }

//     for (byte i = 1; i < 16; i++)
//     {
//         status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(i, clearBlock, size);
//         if (status != MFRC522::STATUS_OK)
//         {
//             Serial.print(F("Writing block "));
//             Serial.print(i);
//             Serial.println(F(" failed"));
//             return false;
//         }
//         else
//         {
//             Serial.print(F("Writing block "));
//             Serial.print(i);
//             Serial.println(F(" success"));
//         }
//     }
// }

bool RFID::openTag()
{
    if (!pMfrc522->PICC_IsNewCardPresent())
        return false;
    if (!pMfrc522->PICC_ReadCardSerial())
        return false;
    MFRC522::PICC_Type piccType = pMfrc522->PICC_GetType(pMfrc522->uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
        return false;

    Serial.println(F("tag found."));
    return true;
}

void RFID::closeTag()
{
    // Halt PICC & top encryption on PCD
    pMfrc522->PICC_HaltA();
    pMfrc522->PCD_StopCrypto1();
}

bool RFID::authenticate(MFRC522::PICC_Command keySlot, byte blockId)
{
    MFRC522::StatusCode status;
    status = (MFRC522::StatusCode)pMfrc522->PCD_Authenticate(keySlot, blockId, &this->key, &(pMfrc522->uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("PCD_Authenticate() failed with Key "));
        Serial.println(keySlot);
        return false;
    }
    return true;
}
