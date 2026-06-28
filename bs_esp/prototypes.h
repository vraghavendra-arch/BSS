
// #include <Embedis.h>
template <typename T> bool setSetting(const String& key, T value);
// template <typename T> bool setSetting(const String& key, unsigned int index, T value);
template <typename T> String getSetting(const String& key, T defaultValue);
// template <typename T> String getSetting(const String& key, unsigned int index, T defaultValue);
void saveSettings(bool force);

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------
typedef std::function<void(uint8_t, const char*, const char*, unsigned int)> mqtt_callback_f;
void mqttRegister(mqtt_callback_f callback);
String mqttSubtopic(char* topic);
bool mqttSend(const char* topic, const char* message, bool stack_msg, bool dont_enctrypt, bool useGetter, bool retain);
bool mqttSendRaw(const char* topic, const char* message, bool dont_encrypt, bool retain);

// -----------------------------------------------------------------------------
// WiFi
// -----------------------------------------------------------------------------

#include "JustWifi.h"
typedef std::function<void(justwifi_messages_t code, char* parameter)> wifi_callback_f;
void wifiRegister(wifi_callback_f callback);
// bool createLongAP(bool withDelay = true);

// -----------------------------------------------------------------------------
// RELAY
// -----------------------------------------------------------------------------
void KUSendCmd(uint8_t cmd, uint8_t payload[], uint16_t payload_len);
