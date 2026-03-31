#pragma once

#include <PubSubClient.h>

// Raw payload buffer filled by MQTT callback
extern char mqttPayloadRecipes[4096];
extern bool recipesReceived;

bool mqttConnect(PubSubClient &client);
void mqttSubscribe(PubSubClient &client);
bool mqttWaitForMessages(PubSubClient &client, unsigned long timeoutMs);
