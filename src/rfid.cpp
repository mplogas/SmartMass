#include "rfid.h"

RFID::RFID(uint8_t chipselectPin, uint8_t resetPin) {
    pMfrc522 = new MFRC522(chipselectPin,resetPin);
}

void RFID::init() {
    SPI.begin();
    pMfrc522->PCD_Init();
}

void RFID::loop() {
    if(!pMfrc522->PICC_IsNewCardPresent()) return;

    Serial.println("new card found!");

    if(!pMfrc522->PICC_ReadCardSerial()) {
        Serial.println("failed to read card");
        pMfrc522->PCD_Init();
        return;
    }
    Serial.println("serial read ok");

    pMfrc522->PICC_DumpToSerial(&pMfrc522->uid);

    delay(2000);
    Serial.println("Ready to read the next card");
}


void RFID::read() {}
void RFID::write(){}