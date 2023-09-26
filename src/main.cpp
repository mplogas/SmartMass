#include "configuration.h"
#include "display.h"
#include "scale.h"

Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_RESET_PIN);
Display::Data displayData;

Scale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
Scale::Measurement measurement; 


void setup() {
  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial attach

  display.init();
  displayData.title = DISPLAY_DATA_TITLE;
  displayData.unit = DISPLAY_DATA_UNIT;
    
  scale.init(LOADCELL_CALIBRATION, LOADCELL_MEASUREMENT_INTERVAL);
  measurement.ts = millis();
  delay(1000);
}

void loop() {
  long previousRun = measurement.ts;
  unsigned long previousValue = measurement.result;
  scale.measure(measurement, LOADCELL_MEASUREMENT_SAMPLING);

  if(measurement.ts > previousRun) {
    // Serial.println();
    // Serial.print(measurement.ts);
    // Serial.printf(" - ");
    // Serial.printf(" measurement: ");
    // Serial.print(measurement.result);
    // Serial.printf(" g");
    // Serial.println();
    displayData.result = measurement.result;
    if(measurement.result != previousValue) display.show(displayData);
  }
}


