#include "configuration.h"
#include <ArduinoJson.h>
#include "mqttclient.h"
#include "display.h"
#include "scale.h"

StaticJsonDocument<32> doc;

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
};

MqttClient mqttClient(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD, MQTT_CLIENTID, callback);
const String fullTopic = String(MQTT_TOPIC + TOPIC_SEPARATOR + MQTT_CLIENTID); 

Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_RESET_PIN, DISPLAY_TIMEOUT);
Display::Data displayData;

Scale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
Scale::Measurement measurement;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial attach

  display.init();
  displayData.title = DISPLAY_DATA_TITLE;
  displayData.unit = DISPLAY_DATA_UNIT;

  scale.init(LOADCELL_CALIBRATION, LOADCELL_MEASUREMENT_INTERVAL);
  measurement.ts = millis();

  mqttClient.init();
  mqttClient.subscribe(fullTopic.c_str());
}

void loop()
{
  long previousRun = measurement.ts;
  unsigned long previousValue = measurement.result;
  scale.measure(measurement, LOADCELL_MEASUREMENT_SAMPLING);

  if (measurement.ts > previousRun)
  {
    displayData.result = measurement.result;
    if (measurement.result != previousValue)
    {
      display.show(displayData);

      // https://arduinojson.org/v6/how-to/use-arduinojson-with-pubsubclient/
      char buffer[32];
      doc["ts"] = measurement.ts;
      doc["value"] = measurement.result;
      serializeJson(doc, buffer);

      mqttClient.publish(fullTopic.c_str(), buffer);
    }
  }
  else
  {
    mqttClient.loop();
  }

  display.loop();
}
