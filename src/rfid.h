

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
    char *spoolId;      // uuid
    long spoolWeight;   // grams
    char *material;     // PLA, ABS, PETG, etc.
    char *color;        // hex
    char *manufacturer; // Prusa, Hatchbox, etc.
    char *spoolName;    // name of the spool

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

private:
    rfidCallback callback;                                                  // Callback function to be called when RFID tag is read.
    TagData writeData;                                                      // TagData struct containing the data to be written to the RFID tag.
    byte clearBlock[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // all zeros. This can be used to delete a block.
    byte *writeBlock;                                                       // Pointer to the block to be written to the RFID tag.
    MFRC522 *pMfrc522;                                                      // Pointer to the MFRC522 object.
    MFRC522::MIFARE_Key key;                                                // MIFARE key object.
    bool IsWrite = false;                                                   // Flag to indicate if the RFID tag is being written to.
    void prepareKey();                                                      // Helper function to prepare the default authentication key.
    void prepareKey(byte authKey[6]);                                       // Helper function to prepare a custom authentication key.
    bool writeTag(TagData &tagData);                                        // Helper function to write data to the RFID tag.
    void readTag();                                                         // Helper function to read data from the RFID tag.
    bool openTag();                                                         // Helper function to open the RFID tag for writing.
    void closeTag();                                                        // Helper function to close the RFID tag after writing.
};

#endif