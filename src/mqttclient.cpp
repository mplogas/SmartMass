#include "mqttclient.h"

MqttClient::MqttClient(const char *wifi_ssid, const char *wifi_password, const char *mqtt_broker, int mqtt_port, const char *mqtt_user, const char *mqtt_password, const char *mqtt_clientid, const char *mqtt_basetopic, mqttCallback mqtt_callback)
{
    wifiSsid = wifi_ssid;
    wifiPassword = wifi_password;
    mqttBroker = mqtt_broker;
    mqttPort = mqtt_port;
    mqttUser = mqtt_user;
    mqttPassword = mqtt_password;
    mqttClientId = mqtt_clientid;
    callback = mqtt_callback;   
    buildTopic(mqtt_basetopic, "heartbeat", mqtt_clientid, mqttHeartbeatTopic);
    sprintf(heartbeatPayload, "{\"device_id\":\"%s\",\"action\":\"heartbeat\",\"status\":\"ok\"}", mqtt_clientid);

    mqtt = new PubSubClient(wifi);
}

boolean MqttClient::wifiConnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        uint8_t attempt = RETRY_ATTEMPTS;
        while (WiFi.status() != WL_CONNECTED && attempt)
        {
            WiFi.begin(wifiSsid, wifiPassword);
            uint8_t timeout = RETRY_TIMEOUT;
            while (timeout && (WiFi.status() != WL_CONNECTED))
            {
                timeout--;
                delay(1000);
            }
            attempt--;
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi connection attempts exhausted. Giving up.");
            Serial.println();
            return false;
        } else {
            Serial.printf("WiFi client IP: ");
            Serial.print(WiFi.localIP());
            Serial.println();
            Serial.printf("RRSI: ");
            Serial.print(WiFi.RSSI());
            Serial.println();
        }
    }
    return true;
}

// TODO: non-blocking all the way! //https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_reconnect_nonblocking/mqtt_reconnect_nonblocking.ino
boolean MqttClient::mqttConnect()
{
    if (!mqtt->connected())
    {
        mqtt->setCallback(callback);
        mqtt->setSocketTimeout(MQTT_TIMEOUT);
        mqtt->setBufferSize(512);
        mqtt->setKeepAlive(MQTT_TIMEOUT);
        mqtt->setServer(mqttBroker, mqttPort);

        uint8_t attempt = RETRY_ATTEMPTS;
        while (!mqtt->connected() && attempt)
        {
            mqtt->connect(mqttClientId, mqttUser, mqttPassword);
            uint8_t timeout = RETRY_TIMEOUT;
            while (!mqtt->connected() && timeout)
            {
                delay(1000);
                timeout--;
            }
            attempt--;
        }

        if (!mqtt->connected())
        {
            Serial.println("MQTT connection attempts exhausted. Giving up.");
            Serial.println();
            return false;
        }
    }

    return true;
}

void MqttClient::emitHeartbeat()
{
    unsigned long now = millis();
    if (now - lastHeartbeat >= heartbeatInterval)
    {
        lastHeartbeat = now;       
        
        publish(mqttHeartbeatTopic, heartbeatPayload);
        // Serial.print(now);
        // Serial.printf(" - emitted hearbeat");
        // Serial.println();
    }
}

void MqttClient::loop()
{
    if (wifiConnect() && mqttConnect()) {
        emitHeartbeat();
        mqtt->loop();
    }
}

void MqttClient::disconnect()
{
    if (mqtt->connected())
    {
        mqtt->disconnect();
        Serial.println("Disconnected from MQTT broker.");
        Serial.println();
    }
}
void MqttClient::init()
{
    if (wifiConnect() && mqttConnect())
        Serial.println("Connected to MQTT broker.");
    else
        Serial.println("Failed to connect to MQTT broker.");

    Serial.println();
}

void MqttClient::publish(const char *topic, const char *payload)
{
    if (wifiConnect() && mqttConnect())
    {
        mqtt->publish(topic, payload);
        Serial.print(millis());
        Serial.printf(" - published payload to topic ");
        Serial.print(topic);
        Serial.println();
    }
}

void MqttClient::subscribe(const char *topic)
{
    if (wifiConnect() && mqttConnect())
    {
        mqtt->subscribe(topic, 0);
        Serial.print(millis());
        Serial.printf(" - subscribed to topic ");
        Serial.print(topic);
        Serial.println();
    }
}

void MqttClient::buildTopic(const char *base, const char *action, const char *clientid, char *buffer)
{
    int length = 0;
    const char *mqttTopicSeparator = "/";
    if (strlen(base) > 0)
    {
        length += strlen(base);
        length += strlen(mqttTopicSeparator);
    }
    length += strlen(action);
    length += strlen(mqttTopicSeparator);
    length += strlen(clientid);
    length += 1;

    try
    {
        if (strlen(base) > 0)
        {
            strcpy(buffer, base);
            strcat(buffer, mqttTopicSeparator);
        }
        strcat(buffer, action);
        strcat(buffer, mqttTopicSeparator);
        strcat(buffer, clientid);

        Serial.printf("Built topic: %s\n", buffer);
    }
    catch(const std::exception& e)
    {
        Serial.printf("Failed to build topic: %s\n", e.what());
    }
}
