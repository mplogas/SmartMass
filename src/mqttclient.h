#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

typedef void (*func_t)(char *topic, byte *payload, unsigned int length);
#define MQTT_TIMEOUT 60
#define RETRY_ATTEMPTS 5

class MqttClient
{
private:
    WiFiClient wifi;
    PubSubClient* mqtt;
    const char* wifiSsid;
    const char* wifiPassword;
    const char* mqttBroker;
    int mqttPort;
    const char* mqttUser;
    const char* mqttPassword; 
    const char *mqttClientId;
    func_t callback;
    bool wifiConnect();
    bool mqttConnect();
    void mqttDisconnect();    
protected:
    void connect();
    void disconnect();
public:
    enum MessageType { Telemetry, Payload};
    MqttClient(const char *wifiSsid, const char *wifiPassword, const char *mqttBroker, int mqttPort, const char *mqttUser, const char *mqttPassword, const char *mqttClientId, func_t callback);
    ~MqttClient();
    void init();
    void loop();
    void publish(const char *topic, char[] payload);
    void subscribe(const char *topic);
};

#endif