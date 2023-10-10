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
    //dumpByteArray(key.keyByte, MFRC522::MF_KEY_SIZE);
}
void RFID::prepareKey(byte authKey[6])
{
    for (uint8_t i = 0; i < 6; i++)
    {
        key.keyByte[i] = authKey[i];
    }
    //dumpByteArray(key.keyByte, MFRC522::MF_KEY_SIZE);
}

byte *RFID::longToByte(long &longVal)
{
    return (byte *)&longVal;
}

long RFID::byteToLong(byte *byteVal)
{
    return *((long *)byteVal);
}

void RFID::init()
{
    prepareKey();
    SPI.begin();
    pMfrc522->PCD_Init();
}

void RFID::init(byte authKey[6])
{
    prepareKey(authKey);
    SPI.begin();
    pMfrc522->PCD_Init();
}

bool RFID::write(RFID::TagData &tagData)
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

    // looking for spoolid and weight first
    byte sector = 0;
    byte spoolIdBlock = 1;
    byte spoolWeightBlock = 2;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    byte trailerBlock = 3;

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
    }
    Serial.println(F("SpoolId:"));
    dumpByteArray(buffer, 16);
    Serial.println();

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolWeightBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spool weight failed."));
    }
    Serial.println(F("spool weight:"));
    dumpByteArray(buffer, 16);
    Serial.println();

    // Dump the sector data
    Serial.println(F("Current data in sector:"));
    pMfrc522->PICC_DumpMifareClassicSectorToSerial(&(pMfrc522->uid), &key, sector);
    Serial.println();

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

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolIdBlock, longToByte(writeData.spoolId), 16);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Writing SpoolId failed"));
        return false;
    }
    Serial.println(F("Writing SpoolId success"));

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolWeightBlock, longToByte(writeData.spoolWeight), 16);
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
