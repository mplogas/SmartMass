#include "mqttclient.h"



MqttClient::MqttClient(const char *wifi_ssid, const char *wifi_password, const char *mqtt_broker, int mqtt_port=1833, const char *mqtt_user, const char *mqtt_password, const char *mqtt_clientid, func_t mqtt_callback)
{
    wifiSsid = wifi_ssid;
    wifiPassword = wifi_password;
    mqttBroker = mqtt_broker;
    mqttPort = mqtt_port;
    mqttUser = mqtt_user;
    mqttPassword = mqtt_password;
    mqttClientId = mqtt_clientid;
    callback = mqtt_callback;

    mqtt = new PubSubClient(wifi);
}

MqttClient::~MqttClient(){}

//TODO: non-blocking all the way! //https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_reconnect_nonblocking/mqtt_reconnect_nonblocking.ino
bool MqttClient::wifiConnect() {
    if(WiFi.status() != WL_CONNECTED){
        while (WiFi.status() != WL_CONNECTED){
            WiFi.begin(wifiSsid, wifiPassword);
            uint8_t attempt = RETRY_ATTEMPTS;
            while (attempt && (WiFi.status() != WL_CONNECTED)) {
                attempt--;
                delay(1000);
            }
        }
    }
}

bool MqttClient::mqttConnect() {
    bool result = true;
    if(!mqtt->connected()) {
        result = false;
        mqtt->setCallback(callback);
        mqtt->setSocketTimeout(MQTT_TIMEOUT);
        mqtt->setKeepAlive(MQTT_TIMEOUT);
        mqtt->setServer(mqttBroker, mqttPort);
        
        uint8_t attempt = RETRY_ATTEMPTS;
        while((!mqtt->connected()) && attempt) {
            result = mqtt->connect(mqttClientId, mqttUser, mqttPassword);
            attempt--;
            delay(1000);
        }
    }

    return result;
}


void MqttClient::disconnect() {}
void MqttClient::init() {}
//void MqttClient::publish(const char *topic, char[] payload){}
void MqttClient::subscribe(const char *topic){}