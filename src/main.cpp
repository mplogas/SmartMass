#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "configuration.h"
#include "display.h"
#include "scale.h"

Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_RESET_PIN);
Display::Data displayData;

Scale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
Scale::Measurement measurement; 

WiFiClient wifi;
PubSubClient mqtt(wifi);
StaticJsonDocument<32> doc;

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void setupWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("WiFi client IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}

void setupMqtt() {
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback); //maybe we won't need it vOv
  //TODO: increase timeout or built reconnect!

  while (!mqtt.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the MQTT broker\n", client_id.c_str());
    if (mqtt.connect(client_id.c_str(), MQTT_USER, MQTT_PASSWORD)) {
        Serial.println("MQTT broker connected");
    } else {
        Serial.print("failed with state ");
        Serial.print(mqtt.state());
        delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial attach

  setupWifi();
  setupMqtt();

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
    displayData.result = measurement.result;
    if(measurement.result != previousValue) {
      display.show(displayData);
      
      //https://arduinojson.org/v6/how-to/use-arduinojson-with-pubsubclient/
      char buffer[32];
      doc["ts"] = measurement.ts;
      doc["value"] = measurement.result;
      serializeJson(doc, buffer);

      if(mqtt.connected()) mqtt.publish(MQTT_TOPIC, buffer);
      else Serial.println("mqtt not connected");
    }
  }
}


