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
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    dumpByteArray(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void RFID::init() {
    prepareKey();
    SPI.begin();
    pMfrc522->PCD_Init();
}

void RFID::loop() {
    if(!pMfrc522->PICC_IsNewCardPresent()) return;
    Serial.println("new card found!");

    if(!pMfrc522->PICC_ReadCardSerial()) {
        Serial.println("failed to read card");
        return;
    }
    Serial.println("serial read ok");

// Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dumpByteArray(pMfrc522->uid.uidByte, pMfrc522->uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = pMfrc522->PICC_GetType(pMfrc522->uid.sak);
    Serial.println(pMfrc522->PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 1;
    byte blockAddr      = 4;
    byte dataBlock[]    = {
        0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
        0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
        0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
        0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
    };    
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);    

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) pMfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(pMfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(pMfrc522->GetStatusCodeName(status));
        return;
    }

     // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    pMfrc522->PICC_DumpMifareClassicSectorToSerial(&(pMfrc522->uid), &key, sector);
    Serial.println();

    // Read data from the block
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) pMfrc522->MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(pMfrc522->GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dumpByteArray(buffer, 16); Serial.println();
    Serial.println();

    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) pMfrc522->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(pMfrc522->uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(pMfrc522->GetStatusCodeName(status));
        return;
    }

    // Write data to the block
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dumpByteArray(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) pMfrc522->MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(pMfrc522->GetStatusCodeName(status));
    }
    Serial.println();   

// Read data from the block (again, should now be what we have written)
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) pMfrc522->MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(pMfrc522->GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dumpByteArray(buffer, 16); Serial.println();

    // Check that data in block is what we have written
    // by counting the number of bytes that are equal
    Serial.println(F("Checking result..."));
    byte count = 0;
    for (byte i = 0; i < 16; i++) {
        // Compare buffer (= what we've read) with dataBlock (= what we've written)
        if (buffer[i] == dataBlock[i])
            count++;
    }
    Serial.print(F("Number of bytes that match = ")); Serial.println(count);
    if (count == 16) {
        Serial.println(F("Success :-)"));
    } else {
        Serial.println(F("Failure, no match :-("));
        Serial.println(F("  perhaps the write didn't work properly..."));
    }
    Serial.println();

    // Dump the sector data
    Serial.println(F("Current data in sector:"));
    pMfrc522->PICC_DumpMifareClassicSectorToSerial(&(pMfrc522->uid), &key, sector);
    Serial.println();

    // Halt PICC
    pMfrc522->PICC_HaltA();
    // Stop encryption on PCD
    pMfrc522->PCD_StopCrypto1();

    delay(2000);
    Serial.println("Ready to read the next card");
}


void RFID::read() {
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
void RFID::write(TagData &data) {
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
}