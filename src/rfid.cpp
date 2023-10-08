#include "rfid.h"

RFID::RFID(uint8_t chipselectPin, uint8_t resetPin) {
    pMfrc522 = new MFRC522(chipselectPin,resetPin);
}

void RFID::dumpByteArray(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void RFID::prepareKey() {
    for (uint8_t i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    dumpByteArray(key.keyByte, MFRC522::MF_KEY_SIZE);
}
void RFID::prepareKey(byte authKey[6]) {
    for (uint8_t i = 0; i < 6; i++) {
        key.keyByte[i] = authKey[i];
    }
    dumpByteArray(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void RFID::init() {
    prepareKey();
    SPI.begin();
    pMfrc522->PCD_Init();
}

void RFID::init(byte authKey[6]) {
    prepareKey(authKey);
    SPI.begin();
    pMfrc522->PCD_Init();
}

void RFID::write(RFID::TagData &data) {
    IsWrite = true;
}
void RFID::read() {
    // TODO: callback instead of read(), see mqtt for reference
}

void RFID::loop() {
    //some initial checks
    if(!pMfrc522->PICC_IsNewCardPresent()) return;
    if(!pMfrc522->PICC_ReadCardSerial()) return;
    MFRC522::PICC_Type piccType = pMfrc522->PICC_GetType(pMfrc522->uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) return;
    Serial.println(F("tag found."));

    // if(IsWrite) writeTag();
    // else readTag();


    // Halt PICC & top encryption on PCD
    pMfrc522->PICC_HaltA(); 
    pMfrc522->PCD_StopCrypto1();
}


void RFID::readTag() {
    if(!pMfrc522->PICC_IsNewCardPresent()) return;
    if(!pMfrc522->PICC_ReadCardSerial()) return;
    MFRC522::PICC_Type piccType = pMfrc522->PICC_GetType(pMfrc522->uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) return;

    Serial.println(F("tag for reading found."));
    //looking for spoolid and weight first
    byte sector = 0;
    byte spoolIdBlock = 1;
    byte spoolWeightBlock = 2;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer); 
    byte trailerBlock   = 3; 

    status = (MFRC522::StatusCode) pMfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(pMfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.println(F("PCD_Authenticate() failed with Key A"));
        return;
    }    
    status = (MFRC522::StatusCode) pMfrc522->MIFARE_Read(spoolIdBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.println(F("Reading spoolid failed."));
    }
    Serial.println(F("SpoolId:"));
    dumpByteArray(buffer, 16);
    Serial.println();

    status = (MFRC522::StatusCode) pMfrc522->MIFARE_Read(spoolWeightBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.println(F("Reading spool weight failed."));
    }
    Serial.println(F("spool weight:"));
    dumpByteArray(buffer, 16);
    Serial.println();

        // Dump the sector data
    Serial.println(F("Current data in sector:"));
    pMfrc522->PICC_DumpMifareClassicSectorToSerial(&(pMfrc522->uid), &key, sector);
    Serial.println();

    // Halt PICC
    pMfrc522->PICC_HaltA();
    // Stop encryption on PCD
    pMfrc522->PCD_StopCrypto1();
}
void RFID::writeTag(TagData &data) {
    if(!pMfrc522->PICC_IsNewCardPresent()) return;
    if(!pMfrc522->PICC_ReadCardSerial()) return;
    MFRC522::PICC_Type piccType = pMfrc522->PICC_GetType(pMfrc522->uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) return;

    Serial.println(F("tag for writing found."));
    //looking for spoolid and weight first
    byte sector = 0;
    byte spoolIdBlock = 1;
    byte spoolWeightBlock = 2;
    MFRC522::StatusCode status;
    byte trailerBlock   = 3; 

    status = (MFRC522::StatusCode) pMfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(pMfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.println(F("PCD_Authenticate() failed with Key B"));
        return;
    }  

    status = (MFRC522::StatusCode) pMfrc522->MIFARE_Write(spoolIdBlock, data.spoolId, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.println(F("Writing SpoolId failed"));
    } 
    Serial.println(F("Writing SpoolId success"));

    status = (MFRC522::StatusCode) pMfrc522->MIFARE_Write(spoolWeightBlock, data.spoolWeight, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.println(F("Writing spool weight failed"));
    } 
    Serial.println(F("Writing spool weight success"));

        // Dump the sector data
    Serial.println(F("Current data in sector:"));
    pMfrc522->PICC_DumpMifareClassicSectorToSerial(&(pMfrc522->uid), &key, sector);
    Serial.println();

    // Halt PICC
    pMfrc522->PICC_HaltA();
    pMfrc522->PCD_StopCrypto1();
    IsWrite = false;
}