#ifndef RFID_H
#define RFID_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

class RFID
{
private:
    MFRC522 *pMfrc522;
    MFRC522::MIFARE_Key key; // create a MIFARE_Key struct named 'key', which will hold the card information

    int block = 2;                          // this is the block number we will write into and then read. Do not write into 'sector trailer' block, since this can make the block unusable.
    byte blockcontent[16] = {56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56};//the highlander block... there can only be one!
    //byte blockcontent[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//all zeros. This can be used to delete a block.
    byte readbackblock[18]; // This array is used for reading out a block. The MIFARE_Read method requires a buffer that is at least 18 bytes to hold the 16 bytes of a block.
    void dumpByteArray(byte *buffer, byte bufferSize);
    void prepareKey();
    int readBlock(int blockNumber, byte arrayAddress[]);
    int writeBlock(int blockNumber, byte arrayAddress[]);

public:
    RFID(uint8_t chipselectPin, uint8_t resetPin);
    void init();
    void loop();
    void write();
    void read();
};

#endif