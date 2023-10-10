#ifndef RFID_H
#define RFID_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

class RFID
{
public:
    struct TagData {
        long spoolId;
        long spoolWeight;  
    };
    RFID(uint8_t chipselectPin, uint8_t resetPin);
    void init();
    void init(byte authKey[6]);
    void loop();
    bool write(TagData &tagData);
protected:    
    void dumpByteArray(byte *buffer, byte bufferSize);
private:
    TagData writeData;
    MFRC522 *pMfrc522;
    MFRC522::MIFARE_Key key; 
    bool IsWrite = false;
    byte clearBlock[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //all zeros. This can be used to delete a block.
    byte readbackblock[18]; // The MIFARE_Read method requires a buffer that is at least 18 bytes to hold the 16 bytes of a block.
    void prepareKey();
    void prepareKey(byte authKey[6]);
    bool writeTag(TagData &tagData);
    void readTag();     
    byte *longToByte(long &longVal);
    long byteToLong(byte *byteVal);
    bool openTag();
    void closeTag();
};

#endif