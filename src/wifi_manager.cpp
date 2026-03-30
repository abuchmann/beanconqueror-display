#include "wifi_manager.h"
#include "config.h"
#include <WiFi.h>

bool wifiConnect() {
    Serial.printf("WiFi: connecting to %s...\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > WIFI_CONNECT_TIMEOUT_MS) {
            Serial.println("WiFi: connection timeout");
            return false;
        }
        delay(100);
    }

    Serial.printf("WiFi: connected, IP=%s\n", WiFi.localIP().toString().c_str());
    return true;
}

void wifiDisconnect() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}
