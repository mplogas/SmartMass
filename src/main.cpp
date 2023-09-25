#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "scale.h"

//Display
#define DISPLAY_WIDTH 128 
#define DISPLAY_HEIGHT 64
#define DISPLAY_RESET_PIN -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET_PIN);

// Load Cell
const uint8_t LOADCELL_DOUT_PIN = 16;
const uint8_t LOADCELL_SCK_PIN = 4;
const long LOADCELL_CALIBRATION = 987;
const long LOADCELL_KNOWN_WEIGHT = 100.0;
const unsigned long LOADCELL_MEASUREMENT_INTERVAL = 500; //if you get 'HX711 not ready for measuring' sampling size is too high for measurment interval
const uint8_t LOADCELL_MEASUREMENT_SAMPLING = 1; 

Scale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
Scale::Measurement measurement; 

void displayMeasurement(unsigned long weight) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    // Display static text
    display.println("Weight:");
    display.display();
    display.setCursor(0, 30);
    display.setTextSize(2);
    display.print(weight);
    display.print(" ");
    display.print("g");
    display.display();  
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial attach

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);    
    
  scale.init(LOADCELL_CALIBRATION, LOADCELL_MEASUREMENT_INTERVAL);
  measurement.ts = millis();
  delay(2000);
}

void loop() {
  long previousRun = measurement.ts;
  unsigned long previousValue = measurement.result;
  scale.measure(measurement, LOADCELL_MEASUREMENT_SAMPLING);

  if(measurement.ts > previousRun) {
    Serial.println();
    Serial.print(measurement.ts);
    Serial.printf(" - ");
    Serial.printf(" measurement: ");
    Serial.print(measurement.result);
    Serial.printf(" g");
    Serial.println();
    if(measurement.result != previousValue) displayMeasurement(measurement.result);
  }
}


