#include "rfid.h"

RFID::RFID(uint8_t chipselectPin, uint8_t resetPin)
{
    pMfrc522 = new MFRC522(chipselectPin, resetPin);
}

void RFID::dumpByteArray(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void RFID::prepareKey()
{
    for (uint8_t i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }
    // dumpByteArray(key.keyByte, MFRC522::MF_KEY_SIZE);
}
void RFID::prepareKey(byte authKey[6])
{
    for (uint8_t i = 0; i < 6; i++)
    {
        key.keyByte[i] = authKey[i];
    }
    // dumpByteArray(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void RFID::longToByte(long &longVal, byte *byteArr)
{
    byteArr[0] = (longVal >> 24) & 0xFF;
    byteArr[1] = (longVal >> 16) & 0xFF;
    byteArr[2] = (longVal >> 8) & 0xFF;
    byteArr[3] = longVal & 0xFF;
    // clearing the remaining blocks
    for (int i = sizeof(byteArr); i < 16; i++)
    {
        byteArr[i] = 0xFF;
    }

    dumpByteArray(byteArr, 16);
    Serial.println();
}

long RFID::byteToLong(byte *byteVal)
{
    long result = 0;
    for (int i = 0; i < 4; i++)
    {
        result = (result << 8) | byteVal[i];
    }
    return result;
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
    byte trailerBlock = 3;
    byte sector = 0;
    byte spoolIdBlock = 1;
    byte spoolWeightBlock = 2;
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
    td.spoolId = byteToLong(buffer);

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolWeightBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spool weight failed."));
    }
    td.spoolWeight = byteToLong(buffer);

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

    // looking for spoolid and weight first
    byte sector = 0;
    byte size = 16;
    byte spoolIdBlock = 1;
    byte spoolWeightBlock = 2;
    MFRC522::StatusCode status;
    byte trailerBlock = 3;

    status = (MFRC522::StatusCode)pMfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(pMfrc522->uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("PCD_Authenticate() failed with Key B"));
        return false;
    }
    byte byteArr[size];
    longToByte(tagData.spoolId, byteArr);
    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolIdBlock, byteArr, size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Writing SpoolId failed"));
        return false;
    }
    Serial.println(F("Writing SpoolId success"));

    longToByte(tagData.spoolWeight, byteArr);
    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolWeightBlock, byteArr, size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Writing spool weight failed"));
        return false;
    }
    Serial.println(F("Writing spool weight success"));

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
