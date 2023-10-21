#ifndef RFID_H
#define RFID_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#include "conversion.h"

/**
 * @brief Struct to hold RFID tag data.
 *
 */
typedef struct
{
    // String instead of char array because ArduinoJson doesn't support char arrays
    String spoolId;      // uuid
    unsigned long spoolWeight;    // grams
    String material;     // PLA, ABS, PETG, etc.
    String color;        // hex
    String manufacturer; // Prusa, Hatchbox, etc.
    String spoolName;    // name of the spool
    unsigned long timestamp;      // timestamp of spool creation
} TagData;

/**
 * @brief Callback function type for RFID events.
 *
 * @param data TagData struct containing the RFID tag data.
 */
typedef void (*rfidCallback)(TagData &data);

/**
 * @brief Class for interfacing with RFID reader.
 *
 */
class RFID
{
public:
    /**
     * @brief Constructor for RFID class.
     *
     * @param chipselectPin The pin number for the chip select (CS) pin of the RFID reader.
     * @param resetPin The pin number for the reset (RST) pin of the RFID reader.
     */
    RFID(uint8_t chipselectPin, uint8_t resetPin);

    /**
     * @brief Initializes the RFID reader with default authentication key.
     *
     * @param callback Callback function to be called when RFID tag is read.
     */
    void init(rfidCallback callback);

    /**
     * @brief Initializes the RFID reader with custom authentication key.
     *
     * @param authKey Array of 6 bytes representing the authentication key.
     * @param callback Callback function to be called when RFID tag is read.
     */
    void init(byte authKey[6], rfidCallback callback);

    /**
     * @brief Main loop function for the RFID reader.
     *
     */
    void loop();

    /**
     * @brief Writes data to the RFID tag.
     *
     * @param tagData TagData struct containing the data to be written to the RFID tag.
     * @return true if write was successful, false otherwise.
     */
    bool write(TagData &tagData);

    // /**
    //  * @brief Clears the RFID tag. This is done by writing all zeros to the tag and applying a new authentication key.
    //  *
    //  * @return true if clear was successful, false otherwise.
    //  */
    // bool clearTag(byte authKey[6]);

private:
    rfidCallback callback;                                                  // Callback function to be called when RFID tag is read.
    TagData writeData;                                                      // TagData struct containing the data to be written to the RFID tag.
    byte clearBlock[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // all zeros. This can be used to delete a block.
    MFRC522 *pMfrc522;                                                      // Pointer to the MFRC522 object.
    MFRC522::MIFARE_Key key;                                                // MIFARE key object.
    MFRC522::PICC_Command authKey = MFRC522::PICC_CMD_MF_AUTH_KEY_A;        // default key is A.
    const byte spoolIdBlock = 1;
    const byte spoolWeightBlock = 2;
    const byte spoolManufacturerBlock = 4;
    const byte spoolMaterialBlock = 5;
    const byte spoolColorBlock = 6;
    const byte spoolNameBlock1 = 8;
    const byte spoolNameBlock2 = 9;
    const byte spoolNameBlock3 = 10;
    const byte spoolTimestampBlock = 12;
    bool IsWrite = false;             // Flag to indicate if the RFID tag is being written to.
    void prepareKey();                // Helper function to prepare the default authentication key.
    void prepareKey(byte authKey[6]); // Helper function to prepare a custom authentication key.
    bool writeTag(TagData &tagData);  // Helper function to write data to the RFID tag.
    void readTag();                   // Helper function to read data from the RFID tag.
    bool readBlock(byte blockId, byte buffer[18]); // Helper function to read a certain block from the RFID tag, includes call to authenticate().
    bool writeBlock(byte blockId, byte block[16], byte size); // Helper function to write a certain block to the RFID tag, includes call to authenticate().
    bool openTag();  // Helper function to open the RFID tag for writing.
    void closeTag(); // Helper function to close the RFID tag after writing.
    bool authenticate(MFRC522::PICC_Command key, byte blockId); // Helper function to authenticate the RFID tag.
};

#endif