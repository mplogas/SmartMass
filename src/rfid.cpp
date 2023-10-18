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
    MFRC522::StatusCode status;
    byte buffer[size];

    status = (MFRC522::StatusCode)pMfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(pMfrc522->uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("PCD_Authenticate() failed with Key A"));
        return;
    }
    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolIdBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spoolid failed."));
        return; // early exit w/o spoolid
    }
    TagData td;
    td.spoolId = Conversion::byteToUuid(buffer);

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolWeightBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spool weight failed."));
    } else {
        td.spoolWeight = Conversion::byteToLong(buffer);        
    }

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolManufacturerBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spool manufacturer failed."));
    } else {
        td.manufacturer = Conversion::byteArrayToString(buffer);
    } 

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolMaterialBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spool material failed."));
    } else {
        td.material = Conversion::byteArrayToString(buffer);
    }

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolColorBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spool color failed."));
    } else {
        td.color = Conversion::byteArrayToString(buffer);
    }

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolNameBlock1, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spool name failed."));
    } else {
        byte buffer2[size];
        byte buffer3[size];
        status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolNameBlock2, buffer2, &size);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.println(F("Reading spool name block2 failed."));
        } else {
            status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolNameBlock3, buffer3, &size);
            if (status != MFRC522::STATUS_OK)
            {
                Serial.println(F("Reading spool name block3 failed."));
            } else {
                td.spoolName = Conversion::byteArraysToString(buffer, buffer2, buffer3);
            }
        }
    }

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolTimestampBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spool timestamp failed."));
    } else {
        td.timestamp = Conversion::byteToLong(buffer);
    }

    // Dump the sector data
    // Serial.println(F("Current data in sector:"));
    // pMfrc522->PICC_DumpMifareClassicSectorToSerial(&(pMfrc522->uid), &key, sector);
    // Serial.println();

    callback(td);
    closeTag();
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

    status = (MFRC522::StatusCode)pMfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(pMfrc522->uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("PCD_Authenticate() failed with Key B"));
        return false;
    }
    byte block[size];
    Conversion::uuidToByte(tagData.spoolId, block);
    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolIdBlock, block, size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Writing SpoolId failed"));
        return false;
    }
    Serial.println(F("Writing SpoolId success"));

    if (tagData.spoolWeight != 0)
    {
        Conversion::longToByte(tagData.spoolWeight, block);
        status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolWeightBlock, block, size);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.println(F("Writing spool weight failed"));
            return false;
        }
        Serial.println(F("Writing spool weight success"));
    }
    else
    {
        Serial.println(F("Skipping spool weight"));
    }

    if (!tagData.manufacturer.isEmpty())
    {
        Conversion::stringToByteArray(tagData.manufacturer, block);
        status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolManufacturerBlock, block, size);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.println(F("Writing spool manufacturer failed"));
            return false;
        }
        Serial.println(F("Writing spool manufacturer success"));
    }
    else
    {
        Serial.println(F("Skipping spool manufacturer"));
    }

    if (!tagData.material.isEmpty())
    {
        Conversion::stringToByteArray(tagData.material, block);
        status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolMaterialBlock, block, size);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.println(F("Writing spool material failed"));
            return false;
        }
        Serial.println(F("Writing spool material success"));
    }
    else
    {
        Serial.println(F("Skipping spool material"));
    }

    if (!tagData.color.isEmpty())
    {
        Conversion::stringToByteArray(tagData.color, block);
        status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolColorBlock, block, size);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.println(F("Writing spool color failed"));
            return false;
        }
        else
        {
            Serial.println(F("Writing spool color success"));
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
        status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolNameBlock1, block, size);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.println(F("Writing spool name block failed"));
            return false;
        }
        else
        {
            status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolNameBlock2, block2, size);
            if (status != MFRC522::STATUS_OK)
            {
                Serial.println(F("Writing spool name block2 failed"));
                return false;
            }
            else
            {
                status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolNameBlock3, block3, size);
                if (status != MFRC522::STATUS_OK)
                {
                    Serial.println(F("Writing spool name block3 failed"));
                    return false;
                }
                else
                {
                    Serial.println(F("Writing spool name success"));
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
        Conversion::longToByte(tagData.timestamp, block);
        status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolTimestampBlock, block, size);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.println(F("Writing spool timestamp failed"));
            return false;
        }
        else
        {
            Serial.println(F("Writing spool timestamp success"));
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
