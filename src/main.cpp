#include "configuration.h"
#include "constants.h"
#include "mqttclient.h"
#include "display.h"
#include "scale.h"
#include "rfid.h"
#include <ArduinoJson.h>

enum RunMode
{
  Initialize,
  Calibrate,
  Configure,
  Tare,
  Measure,
  Error,
  Test
};

struct Configuration
{

  unsigned long displayTimeout;
  long loadcellCalibration;
  unsigned long loadcellKnownWeight;
  unsigned long loadcellMeasurementIntervall;
  uint8_t loadcellMeasurementSampling;
};
Configuration config;

RunMode currentMode = RunMode::Initialize;
bool modeSwitch = true;

void callback(char *topic, byte *payload, unsigned int length)
{
  StaticJsonDocument<512> doc;
  ArduinoJson::V6213PB2::DeserializationError serializationResult = deserializeJson(doc, payload, length);
  if (serializationResult) {
    Serial.println("serialization failed.");
  }

  if (doc != NULL && doc.containsKey(ACTION_KEY))
  {

    if (strcmp(doc[ACTION_KEY], ACTION_TARE) == 0)
    {
      currentMode = RunMode::Tare;
    }
    else if (strcmp(doc[ACTION_KEY], ACTION_CALIBRATE) == 0)
    {
      if (!doc.containsKey("result"))
        currentMode = RunMode::Calibrate;
    }
    else if (strcmp(doc[ACTION_KEY], ACTION_CONFIGURE) == 0)
    {
      
      Serial.println("configure action started");
      JsonObject scale = doc["scale"];
      if (scale != NULL)
      {
        Serial.println("scale object found");
        // instead of using containsKey() and accessing the value via key, we save ONE key lookup by using ArduinoJSONs default values.
        long loadcellCalibration = scale["calibration"];
        unsigned long loadcellKnownWeight = scale["known_weight"];
        unsigned long loadcellMeasurementIntervall = scale["update_interval"];
        uint8_t loadcellMeasurementSampling = scale["sampling_size"];

        //NOTE: late night note - it seems the default configuration payload is too large, so either send scale or display. 
        // or i find a way to compress/protobuf the payload.

        if (loadcellCalibration != 0)
        {
          Serial.printf("loadcell calibration read: ");
          Serial.print(config.loadcellCalibration);
          Serial.println();
          config.loadcellCalibration = loadcellCalibration;
          Serial.printf("loadcell calibration write: ");
          Serial.print(config.loadcellCalibration);
          Serial.println();
        }
        if (loadcellKnownWeight != 0)
        {
          Serial.printf("loadcell known weight read: ");
          Serial.print(config.loadcellKnownWeight);
          Serial.println();
          config.loadcellKnownWeight = loadcellKnownWeight;
          Serial.printf("loadcell known weight write: ");
          Serial.print(config.loadcellKnownWeight);
          Serial.println();
        }
        if (loadcellMeasurementIntervall != 0)
        {
          Serial.printf("loadcell measurement intervall read: ");
          Serial.print(config.loadcellMeasurementIntervall);
          Serial.println();
          config.loadcellMeasurementIntervall = loadcellMeasurementIntervall;
          Serial.printf("loadcell measurement intervall write: ");
          Serial.print(config.loadcellMeasurementIntervall);
          Serial.println();
        }
        if (loadcellMeasurementSampling != 0)
        {          
          Serial.printf("loadcell measurement sampling read: ");
          Serial.print(config.loadcellMeasurementSampling);
          Serial.println();
          config.loadcellMeasurementSampling = loadcellMeasurementSampling;          
          Serial.printf("loadcell measurement sampling write: ");
          Serial.print(config.loadcellMeasurementSampling);
          Serial.println();
        }
      }

      JsonObject display = doc["display"];
      if (display != NULL)
      {
        // can't use the efficient way, as a display_timeout of zero and ArduinoJson type defaults are not necessarily the same thing
        // unsigned long timeout = display["display_timeout"];
        // if (timeout != 0) config.displayTimeout = timeout;
        if (display.containsKey("display_timeout"))
        {
          Serial.printf("display timeout read: ");
          Serial.print(config.displayTimeout);
          Serial.println();
          config.displayTimeout = display["display_timeout"];
          Serial.printf("display timeout read: ");
          Serial.print(config.displayTimeout);
          Serial.println();
        }
      }

      currentMode = RunMode::Configure;
    }
    else if (strcmp(doc[ACTION_KEY], ACTION_TEST) == 0)
    {
      currentMode = RunMode::Test;
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


const uint8_t RST_PIN = 15;          // Configurable, see typical pin layout above
const uint8_t SS_PIN = 5;         // Configurable, see typical pin layout above
RFID rfid(SS_PIN,RST_PIN);

void intializeConfiguration()
{
  config.displayTimeout = DISPLAY_TIMEOUT;
  config.loadcellCalibration = LOADCELL_CALIBRATION;
  config.loadcellKnownWeight = LOADCELL_KNOWN_WEIGHT;
  config.loadcellMeasurementIntervall = LOADCELL_MEASUREMENT_INTERVAL;
  config.loadcellMeasurementSampling = LOADCELL_MEASUREMENT_SAMPLING;
}

void setRunModeMeasure()
{
  modeSwitch = true;
  currentMode = RunMode::Measure;
}

void setRunModeError(const char *module, const char *msg)
{
  displayError.module = module;
  displayError.msg = msg;
  display.showErrorMessage(displayError);

  currentMode = RunMode::Error;
}
void initializeDevice()
{
  display.showInitMessage();
  delay(3000);
  if (scale.isReady())
  {
    display.showMessage(MESSAGE_SCALE_READY);
    setRunModeMeasure();
  }
  else
  {
    setRunModeError(MODULE_SCALE, ERROR_SCALE_NOT_READY);
  }
}

void calibrateDevice()
{
  display.showTitle(TITLE_CALIBRATION);
  delay(1500);
  display.showMessage(MESSAGE_CALIBRATION_START);
  delay(2500);
  display.showMessage(MESSAGE_TARE_START);
  delay(5000);
  scale.calibrationStep01();
  display.showMessage(MESSAGE_CALIBRATION_KNOWN_WEIGHT);
  delay(5000);
  long result = scale.calibrationStep02(config.loadcellKnownWeight);
  display.showCalibrationMessage(result);

  // TODO: writing calibration data immediately? yes/no?
  // config.loadcellCalibration = result;

  StaticJsonDocument<96> doc;
  char buffer[96];
  doc[ACTION_KEY] = "calibrate";
  doc["result"] = result;
  serializeJson(doc, buffer);
  mqttClient.publish(fullTopic.c_str(), buffer);

  delay(5000);
  setRunModeMeasure();
}

void configureDevice()
{
  display.showTitle(TITLE_CONFIGURATION);
  delay(1500);

  display.setScreenTimeOut(config.displayTimeout);

  
  Serial.printf("loadcell calibration: ");
  Serial.print(config.loadcellCalibration);
  Serial.println();

  Serial.printf("loadcell measurement intervall: ");
  Serial.print(config.loadcellMeasurementIntervall);
  Serial.println();
  scale.init(config.loadcellCalibration, config.loadcellMeasurementIntervall);

  delay(1500);

  setRunModeMeasure();
}

void tare()
{
  display.showTitle(TITLE_TARE);
  delay(1000);
  display.showMessage(MESSAGE_TARE_START);
  delay(1500);
  if (scale.tare())
  {
    display.showMessage(MESSAGE_TARE_READY);
    setRunModeMeasure();
  }
  else
  {
    setRunModeError(MODULE_SCALE, ERROR_TARE_FAILED);
  }
}
void measure()
{
  if (modeSwitch)
  {
    display.showMessage(MESSAGE_SCALE_READY);
    modeSwitch = false;
  }

  long previousRun = measurement.ts;
  unsigned long previousValue = measurement.result;
  scale.measure(measurement, config.loadcellMeasurementSampling);

  if (measurement.ts > previousRun)
  {
    displayData.result = measurement.result;
    if (measurement.result != previousValue)
    {
      display.showMeasurement(displayData);

      // TODO: contract JSON object
      // https://arduinojson.org/v6/how-to/use-arduinojson-with-pubsubclient/
      StaticJsonDocument<32> doc;
      char buffer[32];
      doc["ts"] = measurement.ts;
      doc["value"] = measurement.result;
      serializeJson(doc, buffer);

      mqttClient.publish(fullTopic.c_str(), buffer);
    }
  }
}

void runExperiments()
{
  // testing error display
  // displayError.module = "DEBUG";
  // displayError.msg = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
  // display.showErrorMessage(displayError);

  // testing measurement display
  // display.showMeasurement(displayData);

  // testing message display
  // display.showMessage("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent non dolor a arcu malesuada luctus et et arcu.");

  // testing init display
  // display.showInitMessage();

  // testing calibration display
  // display.showCalibrationMessage(-10456);

  // testing title display
  // display.showTitle(TITLE_CALIBRATION); // configuration should be the longest


  delay(5000);
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

  rfid.init();
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
    // TODO: error recovery, maybe re-init?

    // for now:
    delay(5000);
    setRunModeMeasure();
    break;
  case RunMode::Test:
    // This mode is used for testing and debugging various states.
    //  just drop an {"action": "test"} on the topic to trigger your experiments
    runExperiments();
    setRunModeMeasure();
    break;
  case RunMode::Measure:
  default:
    measure();
    break;
  }

  rfid.loop();
  mqttClient.loop();
  display.loop();

}
