#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <GxEPD2_7C.h>
#include <epd7c/GxEPD2_730c_GDEP073E01.h>
#include <rom/crc.h>

#include "config.h"
#include "data_model.h"
#include "wifi_manager.h"
#include "mqtt_client.h"
#include "json_parser.h"
#include "display_layout.h"

// Display instance (paged: HEIGHT/4 to fit in RAM)
GxEPD2_7C<GxEPD2_730c_GDEP073E01, GxEPD2_730c_GDEP073E01::HEIGHT / 4> display(
    GxEPD2_730c_GDEP073E01(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

// Change detection — survives deep sleep
RTC_DATA_ATTR uint32_t lastDataHash = 0;
RTC_DATA_ATTR uint32_t bootCount = 0;

static uint32_t computeHash(const DisplayData &data) {
    return crc32_le(0, (const uint8_t *)&data, sizeof(data));
}

void setup() {
    Serial.begin(115200);
    bootCount++;
    Serial.printf("\n=== BeanConqueror Display, boot #%u ===\n", bootCount);

    display.init(115200);

    // Connect WiFi
    if (!wifiConnect()) {
        drawError(display, "WiFi Error");
        esp_sleep_enable_timer_wakeup(30 * 1000000ULL);  // retry in 30s
        esp_deep_sleep_start();
    }

    // Connect MQTT
    WiFiClient wifiClient;
    PubSubClient mqtt(MQTT_BROKER, MQTT_PORT, wifiClient);
    mqtt.setBufferSize(4096);

    if (!mqttConnect(mqtt)) {
        drawError(display, "MQTT Error");
        wifiDisconnect();
        esp_sleep_enable_timer_wakeup(30 * 1000000ULL);
        esp_deep_sleep_start();
    }

    // Subscribe and wait for retained messages
    mqttSubscribe(mqtt);
    bool received = mqttWaitForMessages(mqtt, MQTT_CONNECT_TIMEOUT_MS);

    mqtt.disconnect();
    wifiDisconnect();

    if (!received) {
        Serial.println("Timeout waiting for MQTT messages");
        if (bootCount == 1) {
            drawError(display, "Waiting for data...");
        }
        esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL_US);
        esp_deep_sleep_start();
    }

    // Parse JSON into display data
    DisplayData data;
    if (!parseDisplayData(mqttPayloadRecipes, data)) {
        Serial.println("Failed to parse MQTT data");
        if (bootCount == 1) {
            drawError(display, "Data Error");
        }
        esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL_US);
        esp_deep_sleep_start();
    }

    // Only refresh display if data changed
    uint32_t hash = computeHash(data);
    if (hash != lastDataHash) {
        Serial.printf("Data changed (hash %08x -> %08x), refreshing display\n", lastDataHash, hash);
        drawDisplay(display, data);
        lastDataHash = hash;
    } else {
        Serial.println("Data unchanged, skipping display refresh");
    }

    display.hibernate();

    Serial.printf("Sleeping for %llu seconds\n", SLEEP_INTERVAL_US / 1000000ULL);
    esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL_US);
    esp_deep_sleep_start();
}

void loop() {
    // Never reached — deep sleep restarts from setup()
}
