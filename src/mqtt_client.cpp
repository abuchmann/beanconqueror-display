#include "mqtt_client.h"
#include "config.h"
#include <Arduino.h>

char mqttPayloadRecipes[4096] = {0};
bool recipesReceived = false;

static const char *topicRecipes = MQTT_TOPIC_PREFIX "/beans/recipes";

static void mqttCallback(char *topic, byte *payload, unsigned int length) {
    if (strcmp(topic, topicRecipes) == 0 && length < sizeof(mqttPayloadRecipes)) {
        memcpy(mqttPayloadRecipes, payload, length);
        mqttPayloadRecipes[length] = '\0';
        recipesReceived = true;
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
    client.subscribe(topicRecipes, 1);
    Serial.println("MQTT: subscribed to topics");
}

bool mqttWaitForMessages(PubSubClient &client, unsigned long timeoutMs) {
    unsigned long start = millis();
    while (!recipesReceived && millis() - start < timeoutMs) {
        client.loop();
        delay(10);
    }
    return recipesReceived;
}
