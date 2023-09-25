#include <Arduino.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;
const long KNOWN_WEIGHT = 100.0;
boolean FIRST_RUN = true;

HX711 scale;

void setup() {
  Serial.begin(115200);  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop() {
  if(FIRST_RUN)  {
    if (scale.is_ready()) {
      // taring
      Serial.println("Tare... remove any weights from the scale.");
      delay(5000);      
      scale.set_scale();    
      scale.tare();
      Serial.println("Tare done...");
      delay(1000);
      Serial.print("Place a known weight on the scale...");
      delay(5000);
      long reading = scale.get_units(10);
      Serial.print("Result: ");
      Serial.println(reading);
      long calibration = reading/KNOWN_WEIGHT;
      Serial.print("Calibration factor: ");
      Serial.println(calibration);
      FIRST_RUN = false;
      Serial.println("Tare again... remove any weights from the scale.");
      delay(5000);
      scale.set_scale(calibration);
      scale.tare();
    } 
    else {
      Serial.println("HX711 not found.");
    }
  } else {
      if (scale.is_ready()) {
        // continuous
        long reading = scale.get_units(10);
        Serial.print("HX711 reading -avg(10): ");
        Serial.println(reading);
      }  else {
        Serial.println("HX711 not found.");
      }
    }

  delay(1000);
  
}



// display demo stuff
// #include <SSD1306Wire.h>
// #include "images.h"

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels

// SSD1306Wire display(0x3c, D2, D15, GEOMETRY_128_64);

// #define DEMO_DURATION 3000
// typedef void (*Demo)(void);

// int demoMode = 0;
// int counter = 1;

// void setup() {
//   Serial.begin(115200);
//   Serial.println();
//   Serial.println();


//   // Initialising the UI will init the display too.
//   display.init();

//   display.flipScreenVertically();
//   display.setFont(ArialMT_Plain_10);

// }

// void drawFontFaceDemo() {
//   // Font Demo1
//   // create more fonts at http://oleddisplay.squix.ch/
//   display.setTextAlignment(TEXT_ALIGN_LEFT);
//   display.setFont(ArialMT_Plain_10);
//   display.drawString(0, 0, "Hello world");
//   display.setFont(ArialMT_Plain_16);
//   display.drawString(0, 10, "Hello world");
//   display.setFont(ArialMT_Plain_24);
//   display.drawString(0, 26, "Hello world");
// }

// void drawTextFlowDemo() {
//   display.setFont(ArialMT_Plain_10);
//   display.setTextAlignment(TEXT_ALIGN_LEFT);
//   display.drawStringMaxWidth(0, 0, 128,
//                              "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore." );
// }

// void drawTextAlignmentDemo() {
//   // Text alignment demo
//   display.setFont(ArialMT_Plain_10);

//   // The coordinates define the left starting point of the text
//   display.setTextAlignment(TEXT_ALIGN_LEFT);
//   display.drawString(0, 10, "Left aligned (0,10)");

//   // The coordinates define the center of the text
//   display.setTextAlignment(TEXT_ALIGN_CENTER);
//   display.drawString(64, 22, "Center aligned (64,22)");

//   // The coordinates define the right end of the text
//   display.setTextAlignment(TEXT_ALIGN_RIGHT);
//   display.drawString(128, 33, "Right aligned (128,33)");
// }

// void drawRectDemo() {
//   // Draw a pixel at given position
//   for (int i = 0; i < 10; i++) {
//     display.setPixel(i, i);
//     display.setPixel(10 - i, i);
//   }
//   display.drawRect(12, 12, 20, 20);

//   // Fill the rectangle
//   display.fillRect(14, 14, 17, 17);

//   // Draw a line horizontally
//   display.drawHorizontalLine(0, 40, 20);

//   // Draw a line horizontally
//   display.drawVerticalLine(40, 0, 20);
// }

// void drawCircleDemo() {
//   for (int i = 1; i < 8; i++) {
//     display.setColor(WHITE);
//     display.drawCircle(32, 32, i * 3);
//     if (i % 2 == 0) {
//       display.setColor(BLACK);
//     }
//     display.fillCircle(96, 32, 32 - i * 3);
//   }
// }

// void drawProgressBarDemo() {
//   int progress = (counter / 5) % 100;
//   // draw the progress bar
//   display.drawProgressBar(0, 32, 120, 10, progress);

//   // draw the percentage as String
//   display.setTextAlignment(TEXT_ALIGN_CENTER);
//   display.drawString(64, 15, String(progress) + "%");
// }

// void drawImageDemo() {
//   // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
//   // on how to create xbm files
//   display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
// }

// Demo demos[] = {drawFontFaceDemo, drawTextFlowDemo, drawTextAlignmentDemo, drawRectDemo, drawCircleDemo, drawProgressBarDemo, drawImageDemo};
// int demoLength = (sizeof(demos) / sizeof(Demo));
// long timeSinceLastModeSwitch = 0;

// void loop() {
//   // clear the display
//   display.clear();
//   // draw the current demo method
//   demos[demoMode]();

//   display.setFont(ArialMT_Plain_10);
//   display.setTextAlignment(TEXT_ALIGN_RIGHT);
//   display.drawString(128, 54, String(millis()));
//   // write the buffer to the display
//   display.display();

//   if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
//     demoMode = (demoMode + 1)  % demoLength;
//     timeSinceLastModeSwitch = millis();
//   }
//   counter++;
//   delay(10);
// }