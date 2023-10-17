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
  WriteTag,
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

Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_RESET_PIN, DISPLAY_TIMEOUT);
Display::Data displayData;
Display::Error displayError;

Scale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
Scale::Measurement measurement;

RFID rfid(RFID_SS_PIN, RFID_RST_PIN);
TagData tag;

void mqttCb(char *topic, byte *payload, unsigned int length)
{
  StaticJsonDocument<512> doc;
  ArduinoJson::V6213PB2::DeserializationError serializationResult = deserializeJson(doc, payload, length);
  if (serializationResult)
  {
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
      JsonObject scaleJson = doc["scale"];
      if (scaleJson != NULL)
      {
        // instead of using containsKey() and accessing the value via key, we save ONE key lookup by using ArduinoJSONs default values.
        long loadcellCalibration = scaleJson["calibration"];
        unsigned long loadcellKnownWeight = scaleJson["known_weight"];
        unsigned long loadcellMeasurementIntervall = scaleJson["update_interval"];
        uint8_t loadcellMeasurementSampling = scaleJson["sampling_size"];

        if (loadcellCalibration != 0)
        {
          config.loadcellCalibration = loadcellCalibration;
        }
        if (loadcellKnownWeight != 0)
        {
          config.loadcellKnownWeight = loadcellKnownWeight;
        }
        if (loadcellMeasurementIntervall != 0)
        {
          config.loadcellMeasurementIntervall = loadcellMeasurementIntervall;
        }
        if (loadcellMeasurementSampling != 0)
        {
          config.loadcellMeasurementSampling = loadcellMeasurementSampling;
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
          config.displayTimeout = display["display_timeout"];
        }
      }

      currentMode = RunMode::Configure;
    }
    else if (strcmp(doc[ACTION_KEY], ACTION_WRITETAG) == 0)
    {
      Serial.println("write-tag action started");
      JsonObject tagJson = doc["tag"];
      if (tagJson != NULL)
      {
        unsigned long spoolId = tagJson["spool_id"]; // TODO: GUID/byte[16]?
        unsigned long spoolWeight = tagJson["spool_weight"];

        Serial.printf("SpoolId: ");
        Serial.print(spoolId);
        Serial.println();

        if (spoolId == 0)
          return; // early exit because we need a spool id from the backend
        else
        {
          tag.spoolId = spoolId;
        }

        if (spoolWeight != 0)
          tag.spoolWeight = spoolWeight;
      }

      currentMode = RunMode::WriteTag;
    }
    else if (strcmp(doc[ACTION_KEY], ACTION_TEST) == 0)
    {
      currentMode = RunMode::Test;
    }
  }
};

void rfidCb(TagData &data)
{
  Serial.println("CB Tagdata");
  Serial.printf("SpoolId ");
  Serial.print(data.spoolId);
  Serial.println();
  Serial.printf("Spool Weight ");
  Serial.print(data.spoolWeight);
  Serial.println();
}

MqttClient mqttClient(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD, MQTT_CLIENTID, mqttCb);
const String fullTopic = String(MQTT_TOPIC + MQTT_TOPIC_SEPARATOR + MQTT_CLIENTID);

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

void writeTag()
{
  display.showTitle(TITLE_WRITETAG);
  delay(1500);
  display.showMessage(MESSAGE_WRITETAG_START);
  delay(5000);
  if (rfid.write(tag))
  {
    display.showMessage(MESSAGE_WRITETAG_READY);
    setRunModeMeasure();
  }
  else
  {
    setRunModeError(MODULE_RFID, ERROR_TAGWRITE_FAILED);
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

  rfid.init(rfidCb);
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
  case RunMode::WriteTag:
    Serial.println("write tag loop");
    writeTag();
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

  mqttClient.loop();
  display.loop();
  rfid.loop();
}
