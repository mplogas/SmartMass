#ifndef RFID_H
#define RFID_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

typedef struct {
        long spoolId;
        long spoolWeight;  
} TagData;

typedef void (*rfidCallback)(TagData &data);

class RFID
{
public:
    RFID(uint8_t chipselectPin, uint8_t resetPin);
    void init(rfidCallback callback);
    void init(byte authKey[6], rfidCallback callback);
    void loop();
    bool write(TagData &tagData);
protected:    
    void dumpByteArray(byte *buffer, byte bufferSize);
private:
    rfidCallback callback;
    TagData writeData;
    byte clearBlock[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //all zeros. This can be used to delete a block.
    byte *writeBlock;
    MFRC522 *pMfrc522;
    MFRC522::MIFARE_Key key; 
    bool IsWrite = false;
    void prepareKey();
    void prepareKey(byte authKey[6]);
    bool writeTag(TagData &tagData);
    void readTag();     
    void longToByte(long &longVal, byte* byteArr);
    long byteToLong(byte *byteVal);
    bool openTag();
    void closeTag();
};

#endif