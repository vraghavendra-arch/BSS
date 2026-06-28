# Firmware Architecture

This document explains how the code in `bs_esp/` fits together and where to make common changes.

## Core Pattern

The firmware is built around a small cooperative module system.

```cpp
std::vector<void (*)()> _loops;

void loopRegister(void (*callback)()) {
  _loops.push_back(callback);
}

void loop() {
  for (unsigned char i = 0; i < _loops.size(); i++) {
      (_loops[i])();
  }
}
```

Each module has a `...Setup()` function that initializes state and then calls `loopRegister(...)`. The main loop stays generic.

Keep registered loop functions short. Network calls, serial protocols, EEPROM commits, and animation updates should be split into small state-based steps where possible.

## Include and Configuration Chain

Most source files include `all.h`, which pulls in:

- `general.h` - shared constants, settings keys, MQTT topics, provider IDs
- `hardware.h` - selected hardware profile from build flags
- `common.h` - currently empty placeholder
- `prototypes.h` - cross-module declarations
- `default.h` - fallback macro values
- MQTT/JSON/version headers

Device selection starts in `platformio.ini`:

```ini
[env:rgbw_light]
build_flags = -DRGBW_LIGHT=1
```

That macro selects this kind of section in `hardware.h`:

```cpp
#elif defined(RGBW_LIGHT)
    #define HOSTNAME "RGBW-LIGHT"
    #define MQTT_DEVICE_AS "rgbw-light"
    #define LIGHT_PROVIDER LIGHT_PROVIDER_PWM
    #define RED_PIN 4
    #define GREEN_PIN 5
    #define BLUE_PIN 12
    #define WHITE_PIN 13
#endif
```

If a macro is not defined by the selected profile, `default.h` supplies the default.

## Startup Sequence

`bs_esp.ino` calls modules in this order:

1. `eepromSetup()`
2. `settingsSetup()`
3. `wifi2Setup()`
4. `httpSetup()`
5. `httpClientSetup()`
6. `mqtt2Setup()`
7. `relaySetup()`
8. `buttonSetup()`
9. `pingSetup()` if enabled
10. `powerSetup()` if a power provider is enabled
11. `lightSetup()` if a light provider is enabled
12. `elegantotaSetup()` if enabled

This order matters. Settings must be ready before WiFi, WiFi before remote HTTP/MQTT work, and relays/buttons before device interaction.

## Module Map

### Settings: `settings.ino`

Responsibilities:

- Starts EEPROM/Embedis storage
- Provides `getSetting(...)`, `setSetting(...)`, `delSetting(...)`, and `saveSettings(...)`
- Registers serial commands such as `BSGET`, `BSSET`, `BSDEL`, `BSGETALL`, `FACTORYSET`, and `RESTART`
- Handles MQTT config commands
- Reports boot info and settings over MQTT
- Performs factory reset while preserving `device_id`

Config MQTT command topic:

```text
<MQTT_DEVICE_AS>/<device_id>/config/command
```

Recognized config actions include:

- `restart`
- `factoryset`
- `reconnect`
- `disconnect`
- `checkforupdate`
- `reportsettings`
- `reportinfo`
- `resetenergy`
- `enable_eota`
- `disable_eota`

### WiFi: `wifi2.ino`

Responsibilities:

- Configures `JustWifi`
- Loads stored networks from `ssid0`, `pass0`, and optional static IP settings
- Creates AP mode when needed
- Publishes WiFi status through debug logs
- Sends periodic gratuitous ARP while connected

Useful helpers:

- `wifiConnected()`
- `wifiDisconnect()`
- `wifiDisconnectSafe()`
- `getIP()`
- `getNetwork()`
- `_is_wifiAPMode()`

### HTTP Server: `http.ino`

Responsibilities:

- Starts `ESP8266WebServer` on port 80
- Provides local WiFi provisioning endpoints
- Starts ElegantOTA on the same server when enabled
- Contains the update check state machine

Endpoints:

```text
/                         status page
/getnetwork.cmd           scan WiFi networks
/setdata.cmd              store ssid/password query params
```

OTA/update checks call:

```text
https://receiver.bharatsmr.com/api/bsh/update
```

The code intentionally keeps the app version in `version.h`, not EEPROM.

### HTTP Client: `httpclient.ino`

Responsibilities:

- Fetches a server-issued device ID after WiFi credentials are configured
- Uses the profile-specific `DEVICE_ID_GET_HTTP_LINK` and `DEVICE_ID_GET_HTTP_DATA`
- Stores returned `DeviceID` or `plugId` into `device_id`
- Clears config state by setting `confst` to false

### MQTT: `mqtt2.ino`

Responsibilities:

- Owns the global `AsyncMqttClient`
- Connects when WiFi is available
- Reconnects every 10 seconds while disconnected
- Restarts the ESP after `MQTT_RECONNECT_MAX_ATTEMPTS`
- Dispatches connect/message events to registered callbacks

Modules subscribe in their MQTT callback when they receive `MQTT_EVENT_CONNECT`.

### Relay and Dimmer: `relay.ino`

Responsibilities:

- Builds relay and dimmer lists from `RELAYx_NUM`, `RELAYx_PIN`, and `DIMMER_NUM`
- Supports ESP GPIO, serial, software serial, and touch-panel serial providers
- Restores relay state for ESP-driven relays
- Handles MQTT switch/dimmer commands
- Reports switch status after state changes
- Updates NeoPixel relay indicators when enabled

Command payloads for switch devices generally use:

```json
{
  "entityId": "DEVICE_ID",
  "type": "switch",
  "value": "11"
}
```

For dimmers:

```json
{
  "entityId": "DEVICE_ID",
  "type": "dimmer",
  "dimmer": 5,
  "value": "3"
}
```

Energy meter variants use `relayStatus` with `ON`/`OFF`.

### Buttons: `button.ino`

Responsibilities:

- Creates `DebounceEvent` objects for each configured button
- Maps physical button events into firmware events
- Executes configured actions for press, click, double click, long click, and very long click

Common actions:

- `BUTTON_MODE_TOGGLE`
- `BUTTON_MODE_RESET`
- `BUTTON_MODE_FACTORY`
- `BUTTON_MODE_INCREASE_SPEED`
- `BUTTON_MODE_DECREASE_SPEED`
- `BUTTON_MODE_CUSTOMACT`

Per-button behavior is configured in `hardware.h` with macros such as `BUTTON1_CLICK`, `BUTTON1_LNGCLICK`, and `BUTTON1_RELAY`.

### Tunable Light: `light.ino`

Compiled for `TUNABLE_LIGHT`.

Responsibilities:

- Drives PWM pins using timer1
- Stores brightness in `lght`
- Accepts MQTT brightness commands
- Performs smooth transitions between current and target brightness

### RGBW Light: `rgbw.ino` and `Animations.*`

Compiled for `RGBW_LIGHT`.

Responsibilities:

- Drives four PWM outputs: red, green, blue, white
- Stores brightness in `lght`
- Stores manual color in `colour`
- Stores last mode in `last_mode`
- Starts/stops animation mode through MQTT
- Smoothly transitions brightness and RGBW channel values

Light command topic:

```text
<MQTT_DEVICE_AS>/<device_id>/light/command
```

Brightness command:

```json
{
  "entityId": "DEVICE_ID",
  "type": "brightness",
  "value": 180
}
```

Manual RGBW command:

```json
{
  "entityId": "DEVICE_ID",
  "type": "colour",
  "colour": [153, 0, 150, 0]
}
```

Animation command:

```json
{
  "entityId": "DEVICE_ID",
  "type": "animations",
  "model": "northernlights"
}
```

Known animation models:

- `northernlights`
- `rainbow`
- `love`
- `candle`
- `sunset`
- `ocean`
- `forest`
- `ice`
- `lava`
- `galaxy`
- `fireplace`
- `cyberpunk`
- `cinematic`
- `zen`
- `party`

### Power Metering: `power.ino`, `power_bl6552.ino`, `power_bl0940.ino`

Responsibilities:

- Chooses the chip provider from `POWER_PROVIDER`
- Reads voltage/current/power values
- Filters readings with `MeanPowerFilter`
- Accumulates active, apparent, and reactive energy
- Stores energy totals in EEPROM
- Publishes periodic power reports over MQTT

Provider values:

- `POWER_PROVIDER_NONE`
- `POWER_PROVIDER_BL6552` for three-phase metering
- `POWER_PROVIDER_BL0940` for single-phase metering

### OTA: `ota.ino`

Responsibilities:

- Enables ElegantOTA when `ELEGANT_OTA_SUPPORT` is true and `eota` setting is enabled
- Registers OTA progress callbacks
- Uses the HTTP server started by `http.ino`

## Adding a New Device Variant

1. Add a new `[env:...]` section to `platformio.ini`.
2. Set a unique `build_flags` macro.
3. Add a matching `#elif defined(...)` block in `hardware.h`.
4. Define at least `HOSTNAME`, `MQTT_DEVICE_AS`, `DEVICE_ID_GET_HTTP_LINK`, `DEVICE_ID_GET_HTTP_DATA`, `ENTITY_TYPE`, and any providers/pins needed.
5. Build the new environment.
6. Verify boot logs, WiFi provisioning, MQTT connection, and command/status topics.

## Adding a New Module

Use the existing pattern:

```cpp
void featureSetup() {
    // initialize pins, settings, callbacks
    loopRegister(featureLoop);
}

void featureLoop() {
    // short non-blocking work
}
```

Then call `featureSetup()` from `setup()` behind a provider macro or feature flag.

## Common Maintenance Rules

- Keep hardware differences in `hardware.h`.
- Keep default values in `default.h`.
- Keep shared setting keys and topic constants in `general.h`.
- Do not add heavy logic directly in `bs_esp.ino`.
- Use `mqttRegisterCallback(...)` for MQTT subscribers.
- Use `loopRegister(...)` for repeated module work.
- Use `saveSettings(false)` for deferred EEPROM commits and `saveSettings(true)` when the value must survive an immediate restart.
- Keep `APP_VERSION` in `version.h`.
