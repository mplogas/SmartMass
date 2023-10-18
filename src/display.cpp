
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
    pDevice->setTextSize(2);
    pDevice->setCursor(7, 10);
    pDevice->println(TITLE_INITIALIZE);
    pDevice->setTextSize(1);
    pDevice->setCursor(10, 30);
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
    pDevice->setCursor(40, 3);
    pDevice->println(TITLE_ERROR);
    pDevice->setCursor(25, 20);    
    pDevice->setTextSize(1);
    pDevice->printf(TITLE_MODULE);
    pDevice->print(error.module);    
    pDevice->setCursor(0, 35);    
    pDevice->println(error.msg);
    pDevice->display();

    lastUpdate = millis();
}

void Display::showTitle(const char *title)
{
    displayStandby = false;

    pDevice->clearDisplay();
    pDevice->setTextColor(WHITE);
    pDevice->setTextSize(2);
    pDevice->setCursor(2, 28);
    pDevice->println(title);
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
    // text
    pDevice->setTextSize(1);
    pDevice->setCursor(10, 18);
    pDevice->println(MESSAGE_CALIBRATION_READY);
    pDevice->setCursor(10, 27);
    pDevice->println(MESSAGE_CALIBRATION_RESULT);
    // result
    pDevice->setTextSize(2);
    pDevice->setCursor(15, 38);
    pDevice->print(calibration);
    pDevice->display();
    lastUpdate = millis();
}

void Display::showMeasurement(Display::Data &data)
{
    displayStandby = false;

    pDevice->clearDisplay();
    // title
    pDevice->setTextSize(1);
    pDevice->setTextColor(WHITE);
    pDevice->setCursor(10, 27);
    pDevice->println(data.title);
    // result & unit
    pDevice->setTextSize(2);
    pDevice->setCursor(10, 38);
    pDevice->print(data.result);
    pDevice->setCursor(100, 38);
    pDevice->print(data.unit);
    pDevice->display();

    lastUpdate = millis();
}

void Display::setScreenTimeOut(unsigned long screenTimeOut) {
    timeout = screenTimeOut;
}


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

void Display::loop()
{
    unsigned long current = millis();

    if ((current - lastUpdate >= timeout) && !displayStandby)
    {
        clearDisplay();
        lastUpdate = current;
        displayStandby = true;

        Serial.println();
        Serial.print(current);
        Serial.printf(" - Display blanked.");
        Serial.println();
    }
}
