#include "all.h"
#include "Embedis.h"
#include "EEPROM_Rotate.h"
#include "EmbedisWrap.h"
#include "ArduinoJson.h"
#include "Ticker.h"

Ticker settingTicker;

// EEPROM_Rotate EEPROMr;
EmbedisWrap embedis(Serial);

bool _settings_save = 0;
bool _sentInfoNow = false;
bool _sentBootData = false;
bool _eepromReset = false;
bool _reportSettingsNow = false;

void settingsRegisterCommand(const String& name, void (*call)(Embedis*)) {
    Embedis::command(name, call);
};

template <typename T> String getSetting(const String& key, T defaultValue) {
    String value;
    if (!Embedis::get(key, value)) value = String(defaultValue);
    return value;
}

template <typename T> String getSetting(const String& key, unsigned int index, T defaultValue) {
    return getSetting(key + String(index), defaultValue);
}

String getSetting(const String& key) {
    return getSetting(key, "");
}

bool hasSetting(const String& key) {
    return getSetting(key).length() != 0;
}

bool hasSetting(const String& key, unsigned int index) {
    return getSetting(key, index, "").length() != 0;
}

template <typename T> bool setSetting(const String& key, T value) {
    return Embedis::set(key, String(value));
}

bool delSetting(const String& key) {
    return Embedis::del(key);
}

bool delSetting(const String& key, unsigned int index) {
    return delSetting(key + String(index));
}

unsigned long getUptime() {
    static unsigned long last_uptime = 0;
    static unsigned char uptime_overflows = 0;

    if (millis() < last_uptime) ++uptime_overflows;
    last_uptime = millis();
    unsigned long uptime_seconds = uptime_overflows * (UPTIME_OVERFLOW / 1000) + (last_uptime / 1000);

    return uptime_seconds;
}

void getInfo(JsonObject &root) {
    root["app"] = APP_NAME;
    root["version"] = APP_VERSION;
    // root["rev"] = APP_REVISION;
    // root["settings_version"] = SETTINGS_VERSION;
    // root["sensors_version"] = SENSORS_VERSION;
    root["buildDate"] = __DATE__;
    root["buildTime"] = __TIME__;
    // root["manufacturer"] = String(MANUFACTURER);
    root["mac"] = WiFi.macAddress();
    root["device"] = String(HOSTNAME);
    root["hostname"] = getSetting("hostname", HOSTNAME);
    root["network"] = getNetwork();
    root["deviceip"] = getIP();
    root["heap"] = ESP.getFreeHeap();
    root["uptime"] = getUptime();
    root["rssi"] = WiFi.RSSI();
    root["distance"] = wifiDistance(WiFi.RSSI());
    root["mqttStatus"] = mqttConnected();
    root["restartCount"] = getSetting("rc", 0).toInt();
    // root["loadaverage"] = systemLoadAverage();
}

bool sendInfo(){
    if(!mqttConnected()) return false;
    DynamicJsonDocument root(200);
    JsonObject info = root.createNestedObject("info");
    getInfo(info);
    String json;
    serializeJson(root, json);
    // mqtt_publish("info", json.c_str());
    DEBUG_PRINT("INFO: %s\n", json.c_str());

    char topic[50];
    String deviceid = getSetting(SETTING_DEVICE_ID, HOSTNAME);
    snprintf(topic, sizeof(topic), "%s/%s/config/status", MQTT_DEVICE_AS, deviceid.c_str());
    return mqtt_publish(topic, json.c_str());
}

bool sendBootData(){
    if(!mqttConnected()) return false;
    DynamicJsonDocument root(200);
    root["restartReason"] = resetReason();
    resetReason(0);
    unsigned int resetCount = getSetting("rc", 0).toInt();
    setSetting("rc", resetCount + 1);
    saveSettings(true);
    root["restartCount"] = getSetting("rc", 0).toInt();
    String json;
    serializeJson(root, json);
    // mqtt_publish("info", json.c_str());
    DEBUG_PRINT("BOOT DATA: %s\n", json.c_str());

    char topic[50];
    String deviceid = getSetting(SETTING_DEVICE_ID, "");
    snprintf(topic, sizeof(topic), "%s/%s/boot/status", MQTT_DEVICE_AS, deviceid.c_str());
    return mqtt_publish(topic, json.c_str());
}

void reportAllSettings(){
    if(!mqttConnected()) return;
    DynamicJsonDocument root(200);
    for (unsigned int i = 0; i < _settingsKeyCount(); i++) {
        String key = settingsKeyName(i);
        String value = getSetting(key);
        root[key] = value;
    }
    String json;
    serializeJson(root, json);
    // mqtt_publish("settings", json.c_str());
    DEBUG_PRINT("SETTINGS: %s\n", json.c_str());

    char topic[50];
    String deviceid = getSetting(SETTING_DEVICE_ID, HOSTNAME);
    snprintf(topic, sizeof(topic), "%s/%s/config/status", MQTT_DEVICE_AS, deviceid.c_str());
    mqtt_publish(topic, json.c_str());
}

void processConfig(JsonObject config){
    for (JsonPair kv : config) {
        DEBUG_PRINT("Key: %s, Value: %s\n", kv.key().c_str(), kv.value().as<String>().c_str());
        setSetting(kv.key().c_str(), kv.value().as<String>());
    }
    saveSettings(true);
}

void processAction(const String& action){
    if(action=="restart"){
        settingTicker.once_ms(1000, [](){
            resetReason(CUSTOM_RESET_WEB);
            ESP.restart();
        });
        
    }
    if(action=="factoryset"){
        settingTicker.once_ms(1000, [](){settingFactoryReset();});
    }
    if(action=="reconnect"){
        settingTicker.once_ms(1000, wifiDisconnect);
    }
    if(action=="disconnect"){
        wifiDisconnectSafe();
    }
    if(action=="checkforupdate"){
        startUpdateCheck();
    }
    if(action=="reportsettings"){
        DEBUG_PRINT("REPORTING CONFIG SETTINGS...");
        _reportSettingsNow = true;
    }
    if(action=="reportinfo"){
        _sentInfoNow = false;
    }

    #if POWER_PROVIDER != POWER_PROVIDER_NONE
    if(action == "resetenergy"){
        settingTicker.once_ms(1000, resetEnergyValue);
        // resetEnergyValue();
    }
    #endif

    if(action == "enable_eota"){
        settingTicker.once_ms(1000, [](){
            setSetting(SETTINGS_ENABLE_ELEGANT_OTA, true);
            saveSettings(true);
            resetReason(CUSTOM_RESET_WEB);
            ESP.restart();
        });
    }

    if(action == "disable_eota"){
        settingTicker.once_ms(1000, [](){
            setSetting(SETTINGS_ENABLE_ELEGANT_OTA, false);
            saveSettings(true);
            resetReason(CUSTOM_RESET_WEB);
            ESP.restart();
        });
    }
}

void mqttCallbackConfig(uint8_t event, const char* topic, const char* payload, unsigned int length){
    DEBUG_PRINT("Mqtt callback config\n");
    if(event==MQTT_EVENT_CONNECT){
        char subscriptionTopic[50];
        DEBUG_PRINT("Connect event, config\n");
        String deviceid = getSetting(SETTING_DEVICE_ID, HOSTNAME);
        snprintf(subscriptionTopic, sizeof(subscriptionTopic), "%s/%s/config/command", MQTT_DEVICE_AS, deviceid.c_str());
        mqtt_subscribe(subscriptionTopic); 
        return;
    }

    if(event==MQTT_EVENT_MESSAGE){
        char subscriptionTopic[50];
        String deviceid = getSetting("device_id", "");
        snprintf(subscriptionTopic, sizeof(subscriptionTopic), "%s/%s/config/command", MQTT_DEVICE_AS, deviceid.c_str());
        if(strcmp(subscriptionTopic, topic)!=0) return;
    }

    StaticJsonDocument<200> root;
    DeserializationError error = deserializeJson(root, payload);

    if(error){
        DEBUG_PRINT("deserializeJson() failed: %s\n", error.f_str());
        return;
    }

    if(root.containsKey("action")){
        processAction(root["action"]);
        
    }
    if(root.containsKey("config") && root["config"].is<JsonObject>()){
        processConfig(root["config"]);

    }
}

void sendSwitchMode(){
    String mode = getSetting(SWITCH_MODE, SWITCH_MANUAL_OVERRIDE_DISABLED);
    Serial.printf(PSTR("%s %s %s\n"),"CONFIG", SWITCH_MODE, mode.c_str());
}

void sendControllerSettings() {
    // Send Setting for Sensors etc...
    // Serial.flush();
    // sendSensorConfigViaMiddleware();
    // #if SENSOR_SUPPORT
    //     sendSensorConfig(false);
    // #endif
        // Serial.flush();
        delay(30);
        // Get Current Relay Statuses
        retrieveRelaysFromChildAndSendSettings();
        delay(30);
        // dimmerRetrieve();
        delay(15);
        sendSwitchMode();
    // #if defined(CURTAIN_DRIVER)
    //     askCurtainPosition();
    // #endif
    }

void saveSettings(bool force){
    _settings_save = true;
    if(force) EEPROMr.commit();
}

void settingFactoryReset(){
    //retrieve anything before erasing and keep it secure.
    String deviceid = getSetting(SETTING_DEVICE_ID, HOSTNAME);
    
    for (unsigned int i = 0; i < SPI_FLASH_SEC_SIZE; i++) {
        EEPROMr.write(i, 0xFF);
    }

    setSetting(SETTING_DEVICE_ID, deviceid);
    resetReason(CUSTOM_RESET_FACTORY);
    saveSettings(true);

    ESP.restart();
}

unsigned int _settingsKeyCount() {
    unsigned count = 0;
    unsigned pos = SPI_FLASH_SEC_SIZE - 1;
    while (size_t len = EEPROMr.read(pos)) {
        if (0xFF == len) break;
        pos = pos - len - 2;
        len = EEPROMr.read(pos);
        pos = pos - len - 2;
        count++;
    }
    return count;
}

String settingsKeyName(unsigned int index) {
    String s;

    unsigned count = 0;
    unsigned pos = SPI_FLASH_SEC_SIZE - 1;
    while (size_t len = EEPROMr.read(pos)) {
        if (0xFF == len) break;
        pos = pos - len - 2;
        if (count == index) {
            s.reserve(len);
            for (unsigned char i = 0; i < len; i++) {
                s += (char)EEPROMr.read(pos + i + 1);
            }
            break;
        }
        count++;
        len = EEPROMr.read(pos);
        pos = pos - len - 2;
    }

    return s;
}

void eepromResetSetup(){
    int count = getSetting("cnt", "0").toInt();
    DEBUG_PRINT("\n\nEEPROM RESET COUNT: %d\n\n", count);
    setSetting("cnt", String(count+1));
    saveSettings(false);

    if(count>=5){
        settingFactoryReset();
    }
}

void eepromesetCounter(){
    DEBUG_PRINT("EEPROM RESET COUNTER\n");
    setSetting("cnt", "0");
    saveSettings(false);
}


void settingsSetup(){
    EEPROMr.begin(SPI_FLASH_SEC_SIZE);

        Embedis::dictionary(F("EEPROM"),
                        SPI_FLASH_SEC_SIZE,
                        [](size_t pos) -> char { return EEPROMr.read(pos); },
                        [](size_t pos, char value) { EEPROMr.write(pos, value); },
#if AUTO_SAVE
                        // []() { EEPROM.commit(); }
                        []() { _settings_save = true; }
#else
                        []() {}
#endif
    );

    settingsRegisterCommand(F("BSGETALL"), [](Embedis* e) {
        unsigned int size = _settingsKeyCount();
        for (unsigned int i = 0; i < size; i++) {
            String key = settingsKeyName(i);
            String value = getSetting(key);
            Serial.printf("+%s => %s\n", key.c_str(), value.c_str());
        }
        Serial.println("Done\n");
    });

    settingsRegisterCommand(F("FACTORYSET"), [](Embedis* e) {
        settingFactoryReset();
    });

    settingsRegisterCommand(F("BSGET"), [](Embedis* e) {
        if (e->argc != 2) {
            Serial.println("Invalid Argumants");
            return;
        }
        Serial.println(getSetting(String(e->argv[1])));
    });
    settingsRegisterCommand(F("BSSET"), [](Embedis* e) {
        if (e->argc != 3) {
            Serial.println("Invalid Argumants");
            return;
        }

        int key_len = e->argv[2] - e->argv[1] - 1;
        int value_len = e->argv[3] - e->argv[2] - 1;
        setSetting(rightString(e->argv[1], key_len), rightString(e->argv[2], value_len));
        saveSettings(false);
    });

    settingsRegisterCommand(F("RESTART"), [](Embedis* e) {
        resetReason(CUSTOM_RESET_TERMINAL);
        saveSettings(true);
        ESP.restart();
    });

    settingsRegisterCommand(F("GETFREEHEAP"), [](Embedis* e) {
        Serial.print("HEAP: ");
        Serial.println(ESP.getFreeHeap());
    });

    settingsRegisterCommand(F("BSDEL"), [](Embedis* e) {
        if (e->argc != 2) {
            Serial.println("Invalid Argumants");
            return;
        }
        delSetting(String(e->argv[1]));
        saveSettings(false);
        Serial.println("Done");
    });

    settingsRegisterCommand(F("RELAY"), [](Embedis* e) {
        if (e->argc < 2) {
            return e->response(Embedis::ARGS_ERROR);
        }
        int id = String(e->argv[1]).toInt();
        if (e->argc > 2) {
            int value = String(e->argv[2]).toInt();
            if (value == 2) {
                // relayToggle(id);
            } else {
                relayUpdate(id, value == 1);
            }
        }
        DEBUG_PRINT(PSTR("Status: %s\n"), relayStatus(id) ? "true" : "false");
        DEBUG_PRINT(PSTR("Done\n"));
    });

    settingsRegisterCommand(F("SEND_CONTROLLER_SETTINGS"), [](Embedis* e) {
        // #if ENABLE_CONTROLLER_SETTINGS
                sendControllerSettings();
        // #endif
        
            delay(50);
        
            DEBUG_PRINT(PSTR("Done\n"));
    });

    settingsRegisterCommand(F("DIM_UPDATE"), [](Embedis* e) {
        #if SEND_RELAY_UPDATE_ON_BOOT
            if(millis() < (RELAY_UPDATE_ON_BOOT_DELAY + 1000)) return;
        #endif        
        if (e->argc < 2) {
            return e->response(Embedis::ARGS_ERROR);
        }
        int dim_val = String(e->argv[1]).toInt();
        DEBUG_PRINT(PSTR("Dimmer Value: %d\n"), dim_val);
        dimmerUpdate(dim_val);
    });

    settingsRegisterCommand(F("RELAY_STATUS"), [](Embedis* e) {
        #if SEND_RELAY_UPDATE_ON_BOOT
            if(millis() < (RELAY_UPDATE_ON_BOOT_DELAY + 1000)) return;
        #endif
        if (e->argc < 2) {
            // DO nothing.
            return;
        }
        int status = String(e->argv[1]).toInt();
        // relayAllUpdate((uint8_t)status);
        if (e->argc > 2) {
            dimmerUpdate(String(e->argv[2]).toInt());
        }
    });

    settingsRegisterCommand(F("RELAY_UPDATE"), [](Embedis* e) {   
        if (e->argc < 3) {
            return e->response(Embedis::ARGS_ERROR);
        }
        int id = String(e->argv[1]).toInt();
        if (e->argc > 2) {
            int value = String(e->argv[2]).toInt();
            relayUpdate((unsigned char)id, (unsigned char)value);
        }
        DEBUG_PRINT(PSTR("Status: %s\n"), relayStatus(id) ? "true" : "false");
        DEBUG_PRINT(PSTR("Done\n"));
    });

    #if ENABLE_EEPROM_RESET
    eepromRe
    setSetup();
    #endif
    
    mqttRegisterCallback(mqttCallbackConfig);

    loopRegister(settingsLoop);

}

void settingsLoop(){
    if (_settings_save) {
        bool commited = EEPROMr.commit();
        // Serial.println(commited ? "Commited": "Error");
        if (commited) {
            _settings_save = false;
        }
    }

    if(!_sentInfoNow){
        if(sendInfo()){
            _sentInfoNow = true;
        }
    }

    if(!_sentBootData){
        if(sendBootData()){
            _sentBootData = true;
        }
    }


    #if TOUCH_PANEL_SERIAL_COMMANDS
    KUSerialprocess();
    #endif

    #if !TOUCH_PANEL_SERIAL_COMMANDS
    embedis.process();
    #endif


    if(millis() > 10000 && !_eepromReset){
        _eepromReset = true;
        eepromesetCounter();
    }

    if(_reportSettingsNow){
        _reportSettingsNow = false;
        reportAllSettings();
    }

}

