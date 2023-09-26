#include "display.h"

Display::Display(uint16_t displayWidth, uint16_t displayHeight, int8_t resetPin = -1) {
    pDevice = new Adafruit_SSD1306(displayWidth, displayHeight, &Wire, resetPin);    
}

void Display::init() {
    if(!pDevice->begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
        //TODO: meaningful error handling in case the display couldn't be set up
    } else {
        delay(1500);
        pDevice->clearDisplay();
        pDevice->setTextColor(WHITE);      
        Serial.println(F("SSD1306 init succeeded"));   
    }
}

void Display::show(Display::Data& data) {
    pDevice->clearDisplay();
    //title
    pDevice->setTextSize(1);
    pDevice->setTextColor(WHITE);
    pDevice->setCursor(0, 10);
    pDevice->println(data.title);
    pDevice->display();
    //result & unit
    pDevice->setCursor(0, 30);
    pDevice->setTextSize(2);
    pDevice->print(data.result);
    pDevice->print(" ");
    pDevice->print(data.unit);
    pDevice->display();  
}

