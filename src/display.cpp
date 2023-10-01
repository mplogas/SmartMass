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

void Display::initDisplay()
{
    displayStandby = false;

    pDevice->clearDisplay();
    pDevice->setTextColor(WHITE);
    pDevice->setTextSize(1);
    pDevice->setCursor(0, 20);
    pDevice->println("filamentwaage");
    pDevice->setCursor(0, 30);
    pDevice->println("initializing...");
    pDevice->display();

    lastUpdate = millis();
}

void Display::showMessage(String msg)
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

void Display::init()
{
    if (!pDevice->begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        // TODO: meaningful error handling in case the display couldn't be set up
    }
    else
    {
        initDisplay();
        delay(1500);
        showMessage("Put a weight on your scale to get started.");
        Serial.println(F("SSD1306 init succeeded"));
    }
}

void Display::show(Display::Data &data)
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
