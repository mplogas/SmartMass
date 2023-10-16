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
    Serial.printf("long to byte - long: ");
    Serial.print(longVal);
    Serial.println();    
    byte *result = (byte *)&longVal;
    dumpByteArray(result, sizeof(result));  
    Serial.println();        
    Serial.printf("buffer size: ");
    Serial.println(sizeof(result));
    Serial.println();      

    return result;
}

long RFID::byteToLong(byte *byteVal)
{
    int size = 18;
    Serial.printf("long to byte - byte: ");
    dumpByteArray(byteVal, size);
    Serial.println();       
    Serial.printf("buffer size: ");
    Serial.println(sizeof(byteVal));
    Serial.println();    
    long result = *((long *)byteVal);
    Serial.printf("byte to long - long: ");
    Serial.print(result);
    Serial.println();

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

    // looking for spoolid and weight first
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
    Serial.println(F("SpoolId:"));
    Serial.print(td.spoolId);
    Serial.println();

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Read(spoolWeightBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Reading spool weight failed."));
    }
    td.spoolWeight = byteToLong(buffer);
    Serial.println(F("spool weight:"));
    Serial.print(td.spoolWeight);
    Serial.println();

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

    longToByte(tagData.spoolId);

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolIdBlock, clearBlock, 16);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.println(F("Writing SpoolId failed"));
        return false;
    }
    Serial.println(F("Writing SpoolId success"));

    status = (MFRC522::StatusCode)pMfrc522->MIFARE_Write(spoolWeightBlock, clearBlock, 16);
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
