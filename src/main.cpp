#include "configuration.h"
#include "constants.h"
#include "mqttclient.h"
#include "display.h"
#include "scale.h"
#include <ArduinoJson.h>

enum RunMode
{
    Initialize,
    Calibrate,
    Configure,
    Tare,
    Measure, 
    Error
};

struct Configuration {
  
  unsigned long displayTimeout; 
  long loadcellCalibration;
  unsigned long loadcellKnownWeight;
  unsigned long loadcellMeasurementIntervall;
  uint8_t loadcellMeasurementSampling;
};
Configuration config;


RunMode currentMode = RunMode::Initialize;

StaticJsonDocument<32> doc;

void callback(char *topic, byte *payload, unsigned int length)
{
  StaticJsonDocument<512> doc;
  deserializeJson(doc, payload, length);
  if(doc != NULL && doc.containsKey("action")) {
    if(strcmp(doc["action"], "tare") == 0) {
      currentMode = RunMode::Tare;
      Serial.printf(doc["action"]);
    }
    else if(strcmp(doc["action"], "calibrate") == 0) {
      currentMode = RunMode::Calibrate;
      Serial.printf(doc["action"]);
    }
    else if(strcmp(doc["action"], "configure") == 0) {
      //TODO: get config data from payload
      currentMode = RunMode::Configure;
      Serial.printf(doc["action"]);
    }
    else if(strcmp(doc["action"], "error-debug") == 0) {
      currentMode = RunMode::Error;
      Serial.printf(doc["action"]);
    }
  } 
};

MqttClient mqttClient(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD, MQTT_CLIENTID, callback);
const String fullTopic = String(MQTT_TOPIC + MQTT_TOPIC_SEPARATOR + MQTT_CLIENTID); 

Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_RESET_PIN, DISPLAY_TIMEOUT);
Display::Data displayData;
Display::Error displayError;

Scale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
Scale::Measurement measurement;

void intializeConfiguration() {
  config.displayTimeout = DISPLAY_TIMEOUT;
  config.loadcellCalibration = LOADCELL_CALIBRATION;
  config.loadcellKnownWeight = LOADCELL_KNOWN_WEIGHT;
  config.loadcellMeasurementIntervall = LOADCELL_MEASUREMENT_INTERVAL;
  config.loadcellMeasurementSampling = LOADCELL_MEASUREMENT_SAMPLING;
}

void initializeDevice(){
  display.showInitMessage();
  delay(1500);
  if(scale.isReady()) {
    display.showMessage(MESSAGE_SCALE_READY);
    currentMode = RunMode::Measure;
  } else {
    displayError.module = MODULE_SCALE;
    displayError.msg = ERROR_SCALE_NOT_READY;
    display.showErrorMessage(displayError);

    currentMode = RunMode::Error;
  }
}

void calibrateDevice(){
  display.showMessage(MESSAGE_CALIBRATION_START);
  delay(1000);
  display.showMessage(MESSAGE_TARE_START);
  delay(5000);
  scale.calibrationStep01();
  display.showMessage(MESSAGE_CALIBRATION_KNOWN_WEIGHT);
  delay(5000);
  long result = scale.calibrationStep02(config.loadcellKnownWeight);
  display.showCalibrationMessage(result);
  //TODO: mqtt response
  delay(5000);
  currentMode = RunMode::Measure;
}

void configureDevice(){
  //reload and stuff
  
  currentMode = RunMode::Measure;
}

void tare(){  
  display.showMessage(MESSAGE_TARE_START);
  delay(1500);
  if(scale.tare()) {
    display.showMessage(MESSAGE_TARE_READY);
    currentMode = RunMode::Measure;
  } else {
    displayError.module = MODULE_SCALE;
    displayError.msg = ERROR_TARE_FAILED;
    display.showErrorMessage(displayError);
    
    currentMode = RunMode::Error;
  }
}
void measure(){
  long previousRun = measurement.ts;
  unsigned long previousValue = measurement.result;
  scale.measure(measurement, config.loadcellMeasurementSampling);

  if (measurement.ts > previousRun)
  {
    displayData.result = measurement.result;
    if (measurement.result != previousValue)
    {
      display.showMeasurement(displayData);

      // https://arduinojson.org/v6/how-to/use-arduinojson-with-pubsubclient/
      char buffer[32];
      doc["ts"] = measurement.ts;
      doc["value"] = measurement.result;
      serializeJson(doc, buffer);

      mqttClient.publish(fullTopic.c_str(), buffer);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  intializeConfiguration();

  display.init();
  displayData.title = DISPLAY_DATA_TITLE;
  displayData.unit = DISPLAY_DATA_UNIT;

  scale.init(config.loadcellCalibration, config.loadcellMeasurementIntervall);
  measurement.ts = millis();

  mqttClient.init();
  mqttClient.subscribe(fullTopic.c_str());
}

void loop()
{           
  switch (currentMode)
  {  
  case RunMode::Initialize:
    initializeDevice();
    break;  
  case RunMode::Calibrate:
    calibrateDevice();
    break;  
  case RunMode::Configure:
    configureDevice();
    break;  
  case RunMode::Tare:
    tare();
    break;
  case RunMode::Error:
    //TODO: error recovery, maybe re-init? 

    // for now
    delay(5000);
    currentMode = RunMode::Measure;
    break;
  case RunMode::Measure:
  default:
    measure();
    break;
  }

  mqttClient.loop();
  display.loop();
}
