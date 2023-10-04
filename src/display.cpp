#include "display.h"

Display::Display(uint16_t displayWidth, uint16_t displayHeight, int8_t resetPin, unsigned long screenTimeOut)
{
    timeout = screenTimeOut;
    pDevice = new Adafruit_SSD1306(displayWidth, displayHeight, &Wire, resetPin);
}

void Display::clearDisplay()
{
    pDevice->clearDisplay();
    pDevice->display();
}

void Display::showInitMessage()
{
    displayStandby = false;

    pDevice->clearDisplay();
    pDevice->setTextColor(WHITE);
    pDevice->setTextSize(1);
    pDevice->setCursor(0, 20);
    pDevice->println(TITLE_INITIALIZE);
    pDevice->setCursor(0, 30);
    pDevice->println(MESSAGE_INITIALIZE);
    pDevice->display();

    lastUpdate = millis();
}

void Display::showErrorMessage(Display::Error &error)
{
    displayStandby = false;

    pDevice->clearDisplay();
    pDevice->setTextColor(WHITE);
    pDevice->setTextSize(2);
    pDevice->setCursor(45, 3);
    pDevice->println(TITLE_ERROR);
    pDevice->setCursor(0, 20);    
    pDevice->setTextSize(1);
    pDevice->printf(TITLE_MODULE);
    pDevice->print(error.module);    
    pDevice->setCursor(0, 35);    
    pDevice->println(error.msg);
    pDevice->display();

    lastUpdate = millis();
}

void Display::showMessage(const char *msg)
{
    displayStandby = false;
    pDevice->clearDisplay();
    pDevice->setTextSize(1);
    pDevice->setTextColor(WHITE);
    pDevice->setCursor(0, 15);
    pDevice->println(msg);
    pDevice->display();
    lastUpdate = millis();
}

void Display::showCalibrationMessage(long calibration)
{
    displayStandby = false;

    pDevice->clearDisplay();
    pDevice->setTextColor(WHITE);
    pDevice->setTextSize(1);
    pDevice->setCursor(0, 5);
    pDevice->println(TITLE_CALIBRATION);
    pDevice->setCursor(0, 15);
    pDevice->println(MESSAGE_CALIBRATION_READY);
    pDevice->setTextSize(2);
    pDevice->setCursor(10, 22);
    pDevice->println(calibration);
    pDevice->display();

    lastUpdate = millis();
}

//initializes the display initializes and upon success shows the init display for 1.5s and then shows the ready screen
bool Display::init()
{
    bool result = pDevice->begin(SSD1306_SWITCHCAPVCC, 0x3C);
    if(result){
        Serial.println(F("SSD1306 init succeeded"));
    } else {
        Serial.println(F("SSD1306 allocation failed"));
    }

    return result;
}

void Display::showMeasurement(Display::Data &data)
{
    displayStandby = false;

    pDevice->clearDisplay();
    // title
    pDevice->setTextSize(1);
    pDevice->setTextColor(WHITE);
    pDevice->setCursor(0, 10);
    pDevice->println(data.title);
    // result & unit
    pDevice->setCursor(0, 30);
    pDevice->setTextSize(2);
    pDevice->print(data.result);
    pDevice->print(" ");
    pDevice->print(data.unit);
    pDevice->display();

    lastUpdate = millis();
}

void Display::loop()
{
    unsigned long current = millis();

    if ((current - lastUpdate >= timeout) && !displayStandby)
    {
        // Serial.print(current);
        // Serial.printf(" - ");
        // Serial.print(lastUpdate);
        // Serial.printf(" = ");
        // Serial.print(current - lastUpdate);
        // Serial.printf(" (");
        // Serial.print(timeout);
        // Serial.printf(")");
        // Serial.println();

        clearDisplay();
        lastUpdate = current;
        displayStandby = true;

        Serial.println();
        Serial.print(current);
        Serial.printf(" - Display blanked.");
        Serial.println();
    }
}
