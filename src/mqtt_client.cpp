#include "mqtt_client.h"
#include "config.h"
#include <Arduino.h>

char mqttPayloadBrews[4096] = {0};
char mqttPayloadBeans[2048] = {0};
bool brewsReceived = false;
bool beansReceived = false;

static const char *topicBrews = MQTT_TOPIC_PREFIX "/brews/recent";
static const char *topicBeans = MQTT_TOPIC_PREFIX "/beans/active";

static void mqttCallback(char *topic, byte *payload, unsigned int length) {
    if (strcmp(topic, topicBrews) == 0 && length < sizeof(mqttPayloadBrews)) {
        memcpy(mqttPayloadBrews, payload, length);
        mqttPayloadBrews[length] = '\0';
        brewsReceived = true;
        Serial.printf("MQTT: received %s (%u bytes)\n", topic, length);
    } else if (strcmp(topic, topicBeans) == 0 && length < sizeof(mqttPayloadBeans)) {
        memcpy(mqttPayloadBeans, payload, length);
        mqttPayloadBeans[length] = '\0';
        beansReceived = true;
        Serial.printf("MQTT: received %s (%u bytes)\n", topic, length);
    }
}

bool mqttConnect(PubSubClient &client) {
    Serial.printf("MQTT: connecting to %s:%d...\n", MQTT_BROKER, MQTT_PORT);
    client.setCallback(mqttCallback);

    const char *clientId = "beanconqueror-display";
    bool connected;
    if (strlen(MQTT_USER) > 0) {
        connected = client.connect(clientId, MQTT_USER, MQTT_PASSWORD);
    } else {
        connected = client.connect(clientId);
    }

    if (!connected) {
        Serial.printf("MQTT: connection failed, state=%d\n", client.state());
        return false;
    }

    Serial.println("MQTT: connected");
    return true;
}

void mqttSubscribe(PubSubClient &client) {
    client.subscribe(topicBrews, 1);
    client.subscribe(topicBeans, 1);
    Serial.println("MQTT: subscribed to topics");
}

bool mqttWaitForMessages(PubSubClient &client, unsigned long timeoutMs) {
    unsigned long start = millis();
    while ((!brewsReceived || !beansReceived) && millis() - start < timeoutMs) {
        client.loop();
        delay(10);
    }
    return brewsReceived && beansReceived;
}
