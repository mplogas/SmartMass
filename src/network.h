#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

/*
//WiFi
const char *WIFI_SSID = "waldlink-iot";
const char *WIFI_PASSWORD = "iot-only.2021";

//MQTT
const char *MQTT_BROKER = "192.168.103.3";
const char *MQTT_TOPIC = "filamentwaage/";
const char *MQTT_USER = "filamentwaage";
const char *MQTT_PASSWORD = "filamentwaage.2023";
const int MQTT_PORT = 1883;
*/


class network
{
private:
    WiFiClient wifi;
    PubSubClient* mqtt;
public:
    enum MessageType { Telemetry, Payload};
    network();
    ~network();
    void init(const char *wifiSsid, const char *wifiPassword, const char *mqttBroker, const char *mqttUser, const char *mqttPassword, int mqttPort=1833);
    void publish(const char *topic, MessageType type, char payload);
    void subscribe();
};

network::network()
{
    mqtt = new PubSubClient(wifi);
}

network::~network()
{
}


#endif