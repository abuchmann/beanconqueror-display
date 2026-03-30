#pragma once

#include <PubSubClient.h>

// Raw payload buffers filled by MQTT callback
extern char mqttPayloadBrews[4096];
extern char mqttPayloadBeans[2048];
extern bool brewsReceived;
extern bool beansReceived;

bool mqttConnect(PubSubClient &client);
void mqttSubscribe(PubSubClient &client);
bool mqttWaitForMessages(PubSubClient &client, unsigned long timeoutMs);
