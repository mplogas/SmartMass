#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

typedef void (*mqttCallback)(char *topic, byte *payload, unsigned int length);

/**
 * @brief Timeout for MQTT connection in seconds.
 */
#define MQTT_TIMEOUT 60

/**
 * @brief Number of retry attempts for MQTT connection.
 */
#define RETRY_ATTEMPTS 5

/**
 * @brief Timeout for retry attempts in seconds.
 */
#define RETRY_TIMEOUT 3 // seconds

/**
 * @brief Separator for MQTT topics.
 */
static const String MQTT_TOPIC_SEPARATOR = "/";

/**
 * @brief Class for handling MQTT client connections.
 */
class MqttClient
{
private:
    WiFiClient wifi;
    PubSubClient *mqtt;
    const char *wifiSsid;
    const char *wifiPassword;
    const char *mqttBroker;
    int mqttPort;
    const char *mqttUser;
    const char *mqttPassword;
    const char *mqttClientId;
    const char *mqttHeartbeatTopic;
    const char *heartbeatPayload = "{ \"action\": \"heartbeat\", \"status\": \"ok\" }";
    unsigned long lastHeartbeat = 0;    
    const unsigned long heartbeatInterval = 60000; //milliseconds
    mqttCallback callback;

    /**
     * @brief Connects to WiFi network.
     * @return True if successful, false otherwise.
     */
    bool wifiConnect();

    /**
     * @brief Connects to MQTT broker.
     * @return True if successful, false otherwise.
     */
    bool mqttConnect();

    /**
     * @brief Disconnects from MQTT broker.
     */
    void mqttDisconnect();

    /**
     * @brief emits hearbeat message to MQTT broker.
     */
    void emitHeartbeat();

protected:
    /**
     * @brief Connects to WiFi and MQTT broker.
     */
    void connect();

    /**
     * @brief Disconnects from MQTT broker and WiFi network.
     */
    void disconnect();

public:
    /**
     * @brief Enum for MQTT message types.
     */
    enum MessageType
    {
        Telemetry,
        Payload
    };

    /**
     * @brief Constructor for MqttClient class.
     * @param wifiSsid SSID of the WiFi network.
     * @param wifiPassword Password of the WiFi network.
     * @param mqttBroker IP address or hostname of the MQTT broker.
     * @param mqttPort Port number of the MQTT broker.
     * @param mqttUser Username for MQTT broker authentication.
     * @param mqttPassword Password for MQTT broker authentication.
     * @param mqttClientId Client ID for MQTT connection.
     * @param mqttHeartbeatTopic Base topic for MQTT heartbeat messages.
     * @param callback Callback function for MQTT messages.
     */
    MqttClient(const char *wifiSsid, const char *wifiPassword, const char *mqttBroker, int mqttPort, const char *mqttUser, const char *mqttPassword, const char *mqttClientId, const char *mqttHeartbeatTopic, mqttCallback callback);

    /**
     * @brief Destructor for MqttClient class.
     */
    ~MqttClient();

    /**
     * @brief Initializes WiFi and MQTT connections.
     */
    void init();

    /**
     * @brief Loops through MQTT messages.
     */
    void loop();

    /**
     * @brief Publishes a message to an MQTT topic.
     * @param topic Topic to publish to.
     * @param payload Message payload.
     */
    void publish(const char *topic, const char *payload);

    /**
     * @brief Subscribes to an MQTT topic.
     * @param topic Topic to subscribe to.
     */
    void subscribe(const char *topic);
};

#endif