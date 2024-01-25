
/**
 * @file main.cpp
 * @brief Main file for the filament scale project.
 *
 * This file contains the main function and the implementation of the RunMode enum and Configuration struct.
 * It also includes the implementation of the MqttClient, Display, Scale, and RFID classes.
 *
 * The main function initializes the device, subscribes to the MQTT broker, and enters a loop where it measures the weight of the filament spool and publishes it to the broker.
 * The device can be configured, calibrated, tared, and write a tag to the RFID reader.
 *
 * @author Marc Plogas
 * @date 2023
 */

#include "configuration.h"
#include "constants.h"
#include "mqttclient.h"
#include "display.h"
#include "scale.h"
#include "rfid.h"
#include <ArduinoJson.h>
#include <Preferences.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


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

Preferences preferences;

/**
 * @brief Struct for storing configuration data in memory.
 *
 */
struct Configuration
{

  unsigned long displayTimeout;
  long loadcellCalibration;
  unsigned long loadcellKnownWeight;
  unsigned long loadcellMeasurementIntervall;
  uint8_t loadcellMeasurementSampling;
  unsigned long rfidDecay;
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
TagData wTag; // tag data to be written to the RFID reader
TagData rTag; // tag data read from the RFID reader
unsigned long lastTagRead = 0;

/**
 * Callback function for MQTT messages. Parses the message payload as a JSON object and performs actions based on the "action" key.
 * @param topic The MQTT topic the message was received on.
 * @param payload The message payload.
 * @param length The length of the message payload.
 */
void mqttCb(char *topic, byte *payload, unsigned int length)
{
  StaticJsonDocument<512> doc;
  DeserializationError serializationResult = deserializeJson(doc, payload, length);
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

      JsonObject rfidJson = doc["rfid"];
      if (rfidJson != NULL)
      {
        unsigned long rfidDecay = rfidJson["decay"];
        if (rfidDecay != 0)
        {
          config.rfidDecay = rfidDecay;
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
        // support for char and char* is not implemented in ArduinoJson 6.18+, so we have to use String
        String spoolId = tagJson["spool_id"];
        unsigned long spoolWeight = tagJson["spool_weight"];
        String material = tagJson["material"];
        String color = tagJson["color"];
        String manufacturer = tagJson["manufacturer"];
        String spoolName = tagJson["spool_name"];
        unsigned long timestamp = tagJson["timestamp"];

        Serial.printf("SpoolId: ");
        Serial.print(spoolId);
        Serial.println();

        if (spoolId.isEmpty())
        {
          Serial.println("spool id is empty");
          return; // early exit because we need a spool id from the backend
        }
        else
        {
          wTag.spoolId = spoolId;
        }

        if(spoolWeight != 0)
          wTag.spoolWeight = spoolWeight;
        if (!material.isEmpty())
          wTag.material = material;
        if (!color.isEmpty())
          wTag.color = color;
        if (!manufacturer.isEmpty())
          wTag.manufacturer = manufacturer;
        if (!spoolName.isEmpty())
          wTag.spoolName = spoolName;
        if (timestamp != 0)
          wTag.timestamp = timestamp;
      }

      currentMode = RunMode::WriteTag;
    }
    else if (strcmp(doc[ACTION_KEY], ACTION_TEST) == 0)
    {
      currentMode = RunMode::Test;
    }
  }
};

/**
 * Callback function for RFID tag data. Prints the tag data to the serial console for debugging purposes.
 */
void rfidCb(TagData &data)
{
    rTag = data;
    lastTagRead = millis();

    Serial.println("CB Tagdata");
    Serial.printf("SpoolId ");
    Serial.print(rTag.spoolId);
    Serial.println();
    Serial.printf("Spool Weight ");
    Serial.print(rTag.spoolWeight);
    Serial.println();
    Serial.printf("Material ");
    Serial.print(rTag.material);
    Serial.println();
    Serial.printf("Color ");
    Serial.print(rTag.color);
    Serial.println();
    Serial.printf("Manufacturer ");
    Serial.print(rTag.manufacturer);
    Serial.println();
    Serial.printf("Spool Name ");
    Serial.print(rTag.spoolName);
    Serial.println();
    Serial.printf("Timestamp ");
    Serial.print(rTag.timestamp);
    Serial.println();
}

MqttClient mqttClient(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD, MQTT_CLIENTID, MQTT_TOPIC, mqttCb);

char statusTopic[128], heartbeatTopic[128], commandTopic[128], responseTopic[128];

/**
 * @brief Initializes the configuration struct with default values.
 */
void intializeConfiguration()
{
  preferences.begin("smartmass", true);
  config.displayTimeout = preferences.getULong("d_timeout", DISPLAY_TIMEOUT);
  config.loadcellCalibration = preferences.getLong("lc_calibr", LOADCELL_CALIBRATION);
  config.loadcellKnownWeight = preferences.getULong("lc_weight", LOADCELL_KNOWN_WEIGHT);
  config.loadcellMeasurementIntervall =  preferences.getULong("lc_interval", LOADCELL_MEASUREMENT_INTERVAL);
  config.loadcellMeasurementSampling = preferences.getInt("lc_sampling", LOADCELL_MEASUREMENT_SAMPLING);
  config.rfidDecay = preferences.getULong("rfid_decay", RFID_DECAY);

  preferences.end();
}

/**
 * @brief Sets the current run mode to Measure and displays the ready message.
 */
void setRunModeMeasure()
{
  modeSwitch = true;
  currentMode = RunMode::Measure;
}

/**
 * @brief Sets the current run mode to Error and displays the error message.
 */
void setRunModeError(const char *module, const char *msg)
{
  displayError.module = module;
  displayError.msg = msg;
  display.showErrorMessage(displayError);

  currentMode = RunMode::Error;
}

/**
 * @brief Initializes the scale.
 */
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

/**
 * @brief Calibrates the scale.
 */
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

  StaticJsonDocument<256> doc;
  char buffer[256];
  doc["device_id"] = MQTT_CLIENTID;
  doc[ACTION_KEY] = "calibrate";
  doc["result"] = result;
  serializeJson(doc, buffer);
  mqttClient.publish(responseTopic, buffer);

  delay(5000);
  setRunModeMeasure();
}

/**
 * @brief Configures the scale.
 */
void configureDevice()
{
  display.showTitle(TITLE_CONFIGURATION);
  delay(1500);

  preferences.begin("smartmass", false);
  preferences.putULong("d_timeout", config.displayTimeout);
  preferences.putLong("lc_calibr", config.loadcellCalibration);
  preferences.putULong("lc_weight", config.loadcellKnownWeight);
  preferences.putULong("lc_interval", config.loadcellMeasurementIntervall);
  preferences.putInt("lc_sampling", config.loadcellMeasurementSampling);
  preferences.end();

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

/**
 * @brief Tares the scale.
 */
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

/**
 * @brief Writes a tag to the RFID reader.
 */
void writeTag()
{
  display.showTitle(TITLE_WRITETAG);
  delay(1500);
  display.showMessage(MESSAGE_WRITETAG_START);
  delay(5000);
  if (rfid.write(wTag))
  {
    display.showMessage(MESSAGE_WRITETAG_READY);
    setRunModeMeasure();
  }
  else
  {
    setRunModeError(MODULE_RFID, ERROR_TAGWRITE_FAILED);
  }
}

/**
 * @brief Measures the weight of the filament spool and publishes it to the MQTT broker.
 */
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
      StaticJsonDocument<256> doc;
      char buffer[256];
      doc["device_id"] = MQTT_CLIENTID;
      if(rTag.spoolId != NULL && millis() - lastTagRead < config.rfidDecay) {        
        doc["spool_id"] = rTag.spoolId;
      } 
      doc["value"] = measurement.result;
      serializeJson(doc, buffer);

      mqttClient.publish(statusTopic, buffer);
    }
  }
}

/**
 * @brief Runs experiments for testing and debugging.
 */
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


AsyncWebServer server(80);
const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


/**
 * @brief The setup function. Initializes the serial console, the display, the scale, and the MQTT client.
 */
void setup()
{
  Serial.begin(115200);

  intializeConfiguration();

  MqttClient::buildTopic(MQTT_TOPIC, "status", MQTT_CLIENTID, statusTopic);
  MqttClient::buildTopic(MQTT_TOPIC, "heartbeat", MQTT_CLIENTID, heartbeatTopic);
  MqttClient::buildTopic(MQTT_TOPIC, "command", MQTT_CLIENTID, commandTopic);
  MqttClient::buildTopic(MQTT_TOPIC, "response", MQTT_CLIENTID, responseTopic);

  display.init();
  display.setScreenTimeOut(config.displayTimeout);
  displayData.title = DISPLAY_DATA_TITLE;
  displayData.unit = DISPLAY_DATA_UNIT;

  scale.init(config.loadcellCalibration, config.loadcellMeasurementIntervall);
  measurement.ts = millis();

  mqttClient.init();
  mqttClient.subscribe(commandTopic);

  rfid.init(rfidCb);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "Hello, world");
  });

      // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String message;
      if (request->hasParam(PARAM_MESSAGE)) {
          message = request->getParam(PARAM_MESSAGE)->value();
      } else {
          message = "No message sent";
      }
      request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
      String message;
      if (request->hasParam(PARAM_MESSAGE, true)) {
          message = request->getParam(PARAM_MESSAGE, true)->value();
      } else {
          message = "No message sent";
      }
      request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.onNotFound(notFound);

  server.begin();
}


/**
 * @brief The loop function. Enters a loop where it measures the weight of the filament spool and publishes it to the broker.
 */
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
