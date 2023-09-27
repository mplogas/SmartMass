#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

/*
//WiFi
***REMOVED***
***REMOVED***

//MQTT
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
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