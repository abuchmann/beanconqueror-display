# BeanConqueror E-Paper Display

An ESP32-based e-paper display that shows your latest coffee recipes from [BeanConqueror](https://beanconqueror.com/) via MQTT.

## Hardware

- **Display**: Good Display GDP073EW1 (GDEP073E01 panel) — 7.3", 800x480, 6-color E Ink Spectra 6
- **MCU**: ESP32 (standard, not S3)
- **MQTT Broker**: Mosquitto (or any broker with standard TCP listener on port 1883)

### Wiring

| E-Paper Pin | ESP32 GPIO |
|-------------|-----------|
| BUSY        | 4         |
| RST         | 16        |
| DC          | 17        |
| CS          | 5         |
| CLK         | 18 (SCK)  |
| DIN         | 23 (MOSI) |

## What it shows

One row per active bean showing the most recent brew recipe:

```
+-----------------------------------------------------------------------+
| BeanConqueror                                                         |
+-----------------------------------------------------------------------+
| ROASTER / BEAN                GRIND       TEMP    RECIPE              |
| ──────────────────────────────────────────────────────────────────    |
| Local Roasters                18g @ 15    93°C    36g in 0:30        |
|   Ethiopia Yirgacheffe                                                |
| ──────────────────────────────────────────────────────────────────    |
| Sweet Bloom                   15g @ 12    85°C    220g in 1:30       |
|   Colombia Huila                                                      |
+-----------------------------------------------------------------------+
```

## Prerequisites

- Python 3.12+ (for PlatformIO)
- BeanConqueror app with MQTT export enabled (publishes to `beanconqueror/brews/recent` and `beanconqueror/beans/active`)
- MQTT broker accessible via TCP on port 1883 (BeanConqueror connects via WebSocket, the ESP32 via standard TCP — both work on the same Mosquitto broker)

## Setup

1. **Clone and create a virtual environment:**

   ```bash
   git clone https://github.com/abuchmann/beanconqueror-display.git
   cd beanconqueror-display
   python3 -m venv .venv
   source .venv/bin/activate
   pip install platformio
   ```

2. **Configure credentials** — copy the template and edit:

   ```bash
   cp src/config.h.example src/config.h
   ```

   Then edit `src/config.h`:

   ```c
   #define WIFI_SSID "your_wifi"
   #define WIFI_PASSWORD "your_password"
   #define MQTT_BROKER "192.168.1.100"
   #define MQTT_PORT 1883
   #define MQTT_USER "your_mqtt_user"
   #define MQTT_PASSWORD "your_mqtt_password"
   ```

3. **Adjust pin wiring** if needed in `src/config.h`.

4. **Build:**

   ```bash
   pio run
   ```

5. **Flash** (connect ESP32 via USB):

   ```bash
   pio run -t upload
   ```

6. **Monitor serial output** (optional, for debugging):

   ```bash
   pio device monitor
   ```

## How it works

1. ESP32 wakes from deep sleep
2. Connects to WiFi, then MQTT broker
3. Subscribes to `beanconqueror/brews/recent` and `beanconqueror/beans/active` (retained messages arrive immediately)
4. Parses JSON, groups brews by bean, picks the most recent brew per bean
5. Computes a CRC32 hash — only refreshes the display if data changed
6. Goes back to deep sleep for 5 minutes (configurable via `SLEEP_INTERVAL_US` in `config.h`)

The e-paper retains its image with zero power during sleep.

## Mosquitto broker setup

Your Mosquitto needs both a TCP listener (for the ESP32) and a WebSocket listener (for BeanConqueror app):

```
# mosquitto.conf
listener 1883
listener 1884
protocol websockets
```

## Project structure

```
src/
├── main.cpp              # Entry point: WiFi → MQTT → parse → display → sleep
├── config.h              # WiFi, MQTT, pin configuration
├── data_model.h          # Data structs
├── wifi_manager.h/.cpp   # WiFi connect/disconnect
├── mqtt_client.h/.cpp    # MQTT subscribe and message buffering
├── json_parser.h/.cpp    # ArduinoJson parsing, brew-per-bean grouping
└── display_layout.h/.cpp # GxEPD2 drawing: header, recipe table
```

## License

MIT
