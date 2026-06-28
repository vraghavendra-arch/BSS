#include "all.h"

typedef struct
{
    unsigned char num;
    unsigned char pin;
    bool inverse;
    unsigned char status;
    unsigned char lastRelayState;
    unsigned long _lastTriggred; // Used for tracking last toggle function called. Useful for delayed reporting. 
    bool _report;
    unsigned char last_reported_status;
    bool take_dimmer_action;
    bool relayPulseActive; // not defining saperately else need to change setup 
    unsigned long _last_action_taken_on; // useful for pulse type relays - Latched_relays, when we want to turn off the pin after sometime. 
} relay_t;

typedef struct
{
    unsigned char num;
    unsigned char pin;
    unsigned char value;
    bool disabled;
    int vdimval;
    unsigned char last_reported_status;
    unsigned long _lastTriggred;
    bool _report;
} dimmer_t;

std::vector<relay_t> _relays;
std::vector<dimmer_t> _dimmer;

bool _changed = false;

void setRelayStatus(uint8_t id, bool status){
    if(status == _relays[id-1].status) return;
    _relays[id-1].status = status;
    // _relays[id-1].lastRelayState = _relays[id-1].status;
    _changed = true;
    #if RELAY_PROVIDER == RELAY_PROVIDER_ESP
    setSetting("relay"+String(id), status);
    saveSettings(false);
    #endif
}

void getRelayStatus(StaticJsonDocument<200>& doc){
    JsonArray relayArray = doc.createNestedArray("switch");
    for(uint8_t i=0; i<_relays.size(); i++){
        relayArray.add(_relays[i].status);
    }

    if(dimmerCount() > 0){
        JsonArray dimmerArray = doc.createNestedArray("dimmer");
        for(uint8_t i=0; i<_dimmer.size(); i++){
            dimmerArray.add(_dimmer[i].value);
        }
    }

}

void relayToggle(uint8_t id){
    if(id==0 || id>relayCount()) return;
    relayUpdate(id, !relayStatus(id));
}

uint8_t relayCount(){
    return _relays.size();
}

unsigned char dimmerCount() {
    return _dimmer.size();
}

uint8_t relayStatus(uint8_t id){
    return _relays[id-1].status;
}

void dimmerUpdate(unsigned char id, unsigned int status){
    DEBUG_PRINT("Dimmer update : %d    %d\n", id, status);
    if(dimmerCount() == 0) return;
    int value = map(status, 20, 99, 1, 5);
    // if(id==0 || id>dimmerCount()) return;
    if(value==_dimmer[0].value) return;
    if(id != DIMMER_NUM) return;
    _dimmer[0].value = value;

    #if RELAY_PROVIDER == RELAY_PROVIDER_TOUCH_PANEL_SERIAL
    DEBUG_PRINT("Dimmer update Sending to Controller : %d, value: %d\n", _dimmer[0].num, status);
    KUsetRelayDP(_dimmer[0].num, DPID_INDEX_DIMMER_STATUS, (uint8_t*)(&status));
    #endif

    #if RELAY_PROVIDER == RELAY_PROVIDER_SERIAL
    Serial.println("");
    Serial.printf("DIM SET %d %d \n", id, status);
    #endif
    _changed = true;

}

#if DEVICE_TYPE == DEVICE_TYPE_ENERGY_METER
bool reportSwitchStatus(){
    StaticJsonDocument<200> doc;
    doc["entityId"] = getSetting(SETTING_DEVICE_ID);  //B3E00000008
    
    doc["relayState"] = relayStatus(1) ? "ON" : "OFF";


    String output;
    serializeJson(doc, output);

    char topic[50];
    String deviceid = getSetting("device_id", "");
    sprintf(topic, "energy-meter/%s/status", MQTT_DEVICE_AS);

    return mqtt_publish(topic, output.c_str());
}
#else
bool reportSwitchStatus(){
    StaticJsonDocument<200> doc;
    doc["entityId"] = getSetting(SETTING_DEVICE_ID);  //B3E00000008
    
    JsonArray switchArray = doc.createNestedArray("switch");
    for (uint8_t i = 0; i < relayCount(); i++) {
        switchArray.add(relayStatus(i + 1));
    }

    if(dimmerCount() > 0){
        JsonArray dimmerArray = doc.createNestedArray("dimmer");
        for (uint8_t i = 0; i < dimmerCount(); i++) {
            dimmerArray.add(_dimmer[i].value);
        }
    }


    String output;
    serializeJson(doc, output);

    char topic[50];
    String deviceid = getSetting("device_id", "");
    sprintf(topic, "%s/%s/%s/status", MQTT_DEVICE_AS, deviceid.c_str(), "switch");

    return mqtt_publish(topic, output.c_str());
}
#endif

void relayUpdate(unsigned char id, unsigned char status){
    DEBUG_PRINT("Relay update : %d    %d\n", id, status);
    if(id==0 || id>relayCount()) return;
    if(status==relayStatus(id)) return;

    // sendUartCommandSetRelayStatus(id, status);
    // setRelayStatus(id, status);
    #if RELAY_PROVIDER == RELAY_PROVIDER_SERIAL
    Serial.printf("REL SET %d %d \n", id, status);
    #endif
    #if RELAY_PROVIDER == RELAY_PROVIDER_SOFTWARE_SERIAL
    sendUartCommandSetRelayStatus(id, status);
    #endif
    #if RELAY_PROVIDER == RELAY_PROVIDER_TOUCH_PANEL_SERIAL
    uint32_t value = status;
    KUsetRelayDP(_relays[id - 1].num, 0x02, (uint8_t*)(&value));
    #endif
    setRelayStatus(id, status);

    #if LED_PROVIDER == LED_PROVIDER_NEOPIXEL
    if(status){
        setOnOffValue(id-1, true);
    }else{
        setOnOffValue(id-1, false);
    }
    #endif

    // StaticJsonDocument<200> doc;

    // doc["entityId"] = getSetting(SETTING_DEVICE_ID);  //B3E00000008
    // doc["relayState"] = status ? "ON" : "OFF";
    // // JsonArray switchArray = doc.createNestedArray("switch");
    // // for (uint8_t i = 0; i < relayCount(); i++) {
    // //     switchArray.add(relayStatus(i + 1));
    // // }

    // String output;
    // serializeJson(doc, output);

    // #if defined(ENERGY_METER_1)
    // if(mqttConnected()) mqtt_publish("smart-switch/status", output.c_str());
    // #elif POWER_PROVIDER == POWER_PROVIDER_BL6552

    // if(mqttConnected()) mqtt_publish(MQTT_TOPIC_STATUS, output.c_str());
    // #else
    // if(mqttConnected()) mqtt_publish(MQTT_TOPIC_STATUS, output.c_str());
    // #endif

}

void relayAllUpdate(uint8_t status){
    for(uint8_t i=1; i<=relayCount(); i++){
        relayUpdate(i, status);
    }
}

void dimmerUpdate(uint8_t value){
    // if(dimmerCount() == 0) return;
    // for(uint8_t i=1; i<=dimmerCount(); i++){
        dimmerUpdate(_dimmer[0].num, value);
    // }
}

#if RELAY_PROVIDER == RELAY_PROVIDER_TOUCH_PANEL_SERIAL
void backLightMqttCallback(uint8_t event, const char* topic, const char* payload, unsigned int length){
    DEBUG_PRINT("Backlight mqtt callback\n");
    if(event==MQTT_EVENT_CONNECT){
        String deviceid = getSetting("device_id", "");
        char topic[50];
        sprintf(topic, "%s/%s/%s/command", MQTT_DEVICE_AS, deviceid.c_str(), "backlight");
        mqtt_subscribe(topic);
        return;
    }

    if(event==MQTT_EVENT_MESSAGE){
        DEBUG_PRINT("Relay mqtt callback message\n");
        char subscribeTopic[50];
        String deviceid = DEVICE_ID();
        snprintf(subscribeTopic, sizeof(subscribeTopic), "%s/%s/backlight/command", MQTT_DEVICE_AS, deviceid.c_str());
        if(strcmp(topic, subscribeTopic)!=0) return;
    }

    StaticJsonDocument<200> root;
    DeserializationError error = deserializeJson(root, payload);
    if(error){
        DEBUG_PRINT("deserializeJson() failed: %s\n", error.f_str());
        return;
    }
    DEBUG_PRINT("Backlight mqtt callback 1\n");

    if(root.containsKey("entityId")){
        String deviceid = root["entityId"];
        if(strcmp(deviceid.c_str(), DEVICE_ID().c_str())!=0) return;
    }

    DEBUG_PRINT("Backlight mqtt callback 2\n");

    if(root.containsKey("bklt")){
        JsonArray backlight = root["bklt"];

        String output;
        serializeJson(backlight, output);
        DEBUG_PRINT("Backlight: %s\n", output.c_str());

        setSetting(SETTING_TOUCH_PANEL_BKLT, output);
        saveSettings(false);

        uint8_t backlightSettings[9] = {0};
        backlightSettings[0] = backlight[0];
        backlightSettings[1] = backlight[1];
        backlightSettings[2] = backlight[2];
        backlightSettings[3] = backlight[3];
        backlightSettings[4] = backlight[4];
        backlightSettings[5] = backlight[5];
        backlightSettings[6] = backlight[6];
        backlightSettings[7] = backlight[7];
        backlightSettings[8] = backlight[8];

        for(uint8_t i=0; i<relayCount(); i++){
            DEBUG_PRINT("%d, backlight: %d\n", _relays[i].num, backlightSettings[i]);
        }

        KUsetRelayRGBbackLgtDP(1, DPID_RELAY_RGB_BACKLGT, backlightSettings);

    }

}
#endif


void relayMqttCallback(uint8_t event, const char* topic, const char* payload, unsigned int length){
    // if(strcmp(topic, MQTT_TOPIC_RELAY)!=0) return;
    if(event==MQTT_EVENT_CONNECT){
        #if POWER_PROVIDER == POWER_PROVIDER_BL0940
        mqtt_subscribe(MQTT_TOPIC_RELAY_PLUG);

        #elif POWER_PROVIDER == POWER_PROVIDER_BL6552
        
        char subscriptionTopic[50];
        String deviceid = getSetting("device_id", "");
        sprintf(subscriptionTopic, "%s/%s", MQTT_TOPIC_RELAY, deviceid.c_str());
        
        mqtt_subscribe(subscriptionTopic);

        #else
        char subscribeTopic[50];
        String deviceid = DEVICE_ID();
        snprintf(subscribeTopic, sizeof(subscribeTopic), "%s/%s/switch/command", MQTT_DEVICE_AS, deviceid.c_str());
        mqtt_subscribe(subscribeTopic);
        #endif

        return;
    }

    if(event == MQTT_EVENT_MESSAGE){
        char subscriptionTopic[50];
        String deviceid = getSetting("device_id", "");

        #if DEVICE_TYPE == DEVICE_TYPE_ENERGY_METER

        #if POWER_PROVIDER == POWER_PROVIDER_BL0940
        if(strcmp(topic, MQTT_TOPIC_RELAY_PLUG)!=0) return;
        #elif POWER_PROVIDER == POWER_PROVIDER_BL6552
        sprintf(subscriptionTopic, "%s/%s", MQTT_TOPIC_RELAY, deviceid.c_str());
        #endif

        #else
        snprintf(subscriptionTopic, sizeof(subscriptionTopic), "%s/%s/switch/command", MQTT_DEVICE_AS, deviceid.c_str());
        #endif

        if(strcmp(topic, subscriptionTopic)!=0) return;
    }

    StaticJsonDocument<200> root;
    DeserializationError error = deserializeJson(root, payload);
    if (error){
        DEBUG_PRINT("deserializeJson() failed: %s\n", error.f_str());
        return;
    }
    #if DEVICE_TYPE == DEVICE_TYPE_ENERGY_METER
    
    if(!root.containsKey("entityId") || !root.containsKey("relayStatus")) return;
    // Extract relayStatus
    const char* deviceID = root["entityId"];
    const char* relayStatus = root["relayStatus"];

    DEBUG_PRINT("Device id: %s, status: %s\n", deviceID, relayStatus);

    // Check if the entityId matches this device's ID
    if (strcmp(DEVICE_ID().c_str(), deviceID)!=0){
        DEBUG_PRINT("Not equal\n");
        return;
    } 

    DEBUG_PRINT("Taking Action-> id: %d, status: %s\n", 1, relayStatus);
    if(strcmp(relayStatus, "ON")==0){
        relayUpdate(1, true);
    }else if(strcmp(relayStatus, "OFF")==0){
        relayUpdate(1, false);
    }

    #else
    if(root.containsKey("entityId")){
        const char* deviceID = root["entityId"];
        if(strcmp(deviceID, DEVICE_ID().c_str())!=0) return;
    }

    const char* type = root["type"];
    const char* value = root["value"];

    if(type==NULL || value==NULL) return;

    if(strcmp(type, "switch")==0){
        uint8_t id = atoi(value)/10;
        uint8_t status = atoi(value)%10;
        relayUpdate(id, status==1);
    }else if(strcmp(type, "dimmer")==0){
        uint8_t dimmerid = root["dimmer"];
        uint8_t dimVal = atoi(value);
        DEBUG_PRINT("Dimmer id: %d, value: %d\n", dimmerid, dimVal);
        dimVal = map(dimVal, 1, 5, 20, 99);
        dimmerUpdate(dimmerid, dimVal);
    }
    #endif
}

void dimmerEnable() {
    if(dimmerCount() == 0) return;

    int use_dimmer = getSetting("use_dimmer_1", "1").toInt();
    if (use_dimmer == 0) {
        _dimmer[0].disabled = true;
        // setDimmerToFull();
    }
}

void retrieveRelayLastState(){
    #if RELAY_PROVIDER == RELAY_PROVIDER_ESP
    for(uint8_t i=0; i<_relays.size(); i++){
        _relays[i].status = getSetting("relay"+String(i+1), 0).toInt();
        DEBUG_PRINT("[RETRIEVING] Relay %d, status: %d\n", i+1, _relays[i].status);
        digitalWrite(_relays[i].pin, _relays[i].status);
        #if LED_PROVIDER == LED_PROVIDER_NEOPIXEL
        setOnOffValue(i, _relays[i].status);
        #endif
        // relayUpdate(i+1, _relays[i].status);
    }
    #elif RELAY_PROVIDER == RELAY_PROVIDER_SERIAL
    Serial.println("");
    Serial.printf(PSTR("REL GET \n"));
    #elif RELAY_PROVIDER == RELAY_PROVIDER_SOFTWARE_SERIAL
    sendUartCommandGetRelayStatus();
    #endif

    #if LIGHT_PROVIDER == LIGHT_PROVIDER_PWM
    setLightState(_relays[0].status);
    #endif

    #if RELAY_PROVIDER == RELAY_PROVIDER_TOUCH_PANEL_SERIAL
    String backlight = getSetting(SETTING_TOUCH_PANEL_BKLT, SETTING_TOUCH_PANEL_BACKLIGHT);
    JsonDocument doc;
    deserializeJson(doc, backlight);
    JsonArray backlightArray = doc.as<JsonArray>();

    uint8_t backlightSettings[9] = {0};
    backlightSettings[0] = backlightArray[0];
    backlightSettings[1] = backlightArray[1];
    backlightSettings[2] = backlightArray[2];
    backlightSettings[3] = backlightArray[3];
    backlightSettings[4] = backlightArray[4];
    backlightSettings[5] = backlightArray[5];
    backlightSettings[6] = backlightArray[6];
    backlightSettings[7] = backlightArray[7];
    backlightSettings[8] = backlightArray[8];

    for(uint8_t i=0; i<relayCount(); i++){
        DEBUG_PRINT("%d, backlight: %d\n", _relays[i].num, backlightSettings[i]);
    }

    KUsetRelayRGBbackLgtDP(1, DPID_RELAY_RGB_BACKLGT, backlightSettings);
    #endif
}

void askHeartBeat() {
#if RELAY_PROVIDER == RELAY_PROVIDER_TOUCH_PANEL_SERIAL
    uint16_t payload_len = 1;
    uint8_t payload_buffer[1];
    payload_buffer[0] = 0;
    DEBUG_PRINT(PSTR("[SERIAL] HeartBeat to Controller \n"));
    KUSendCmd(CMD_Heartbeat, payload_buffer, payload_len);
#endif
}


void increaseDimmingValue(){
    if(dimmerCount() == 0) return;
    if(_dimmer[0].value == 5) return;
    int value = map((_dimmer[0].value>4?5:_dimmer[0].value+1), 1, 5, 20, 99);
    dimmerUpdate(DIMMER_NUM, value);
}

void decreaseDimmingValue(){
    if(dimmerCount() == 0) return;
    if(_dimmer[0].value == 1) return;
    int value = map((_dimmer[0].value<2?1:_dimmer[0].value-1), 1, 5, 20, 99);
    dimmerUpdate(DIMMER_NUM, value);
}

void takeDimmerAction(){
    #if DIMMER_PROVIDER == DIMMER_PROVIDER_ESP
    static uint8_t last_dimmer_value = 0;

    if(last_dimmer_value == _dimmer[0].value) return;
    last_dimmer_value = _dimmer[0].value;

    switch(_dimmer[0].value){
        case 0:
            digitalWrite(DIMMER_PIN1, !DIMPOS1_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN2, !DIMPOS2_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN3, !DIMPOS3_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN4, !DIMPOS4_ON_STATE);  //LOW
            break;
        case 1:
            digitalWrite(DIMMER_PIN2, DIMPOS2_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN1, !DIMPOS1_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN3, !DIMPOS3_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN4, !DIMPOS4_ON_STATE);  //LOW
            break;
        case 2:
            digitalWrite(DIMMER_PIN3, DIMPOS3_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN1, !DIMPOS1_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN2, !DIMPOS2_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN4, !DIMPOS4_ON_STATE);  //LOW
            break;
        case 3:
        
            digitalWrite(DIMMER_PIN3, DIMPOS3_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN1, DIMPOS1_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN2, !DIMPOS2_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN4, !DIMPOS4_ON_STATE);  //LOW
            break;
        case 4:
        
            digitalWrite(DIMMER_PIN3, DIMPOS3_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN2, DIMPOS2_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN1, !DIMPOS1_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN4, !DIMPOS4_ON_STATE);  //LOW
            break;
        case 5:
                
            digitalWrite(DIMMER_PIN4, DIMPOS4_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN3, DIMPOS3_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN1, !DIMPOS1_ON_STATE);  //LOW
            delay(30);
            digitalWrite(DIMMER_PIN2, !DIMPOS2_ON_STATE);  //LOW
            break;
        default:
            break;
    }
    #endif
}

void setDimmerToFull() {
    #ifndef DIMMER_NUM
        return;
    #endif
    Serial.println("");
    delay(10);
    Serial.printf(PSTR("simmer set full"));
    Serial.printf(PSTR("DIM SET %d %d \n"), _dimmer[0].num, 99);
}

void retrieveRelaysFromChildAndSendSettings(){
    #if RELAY_PROVIDER == RELAY_PROVIDER_SERIAL
        // SERIAL_SEND("RELAY_STATUS\n");
        Serial.println("");
        Serial.printf(PSTR("REL GET \n"));
        if (_dimmer[0].disabled == true) {
            Serial.println("");
            Serial.printf(PSTR("CONFIG DIM DIS %d 1 \n"), _dimmer[0].num);
            setDimmerToFull();
        } else {
            Serial.println("");
            Serial.printf(PSTR("CONFIG DIM DIS %d 0 \n"), _dimmer[0].num);
        }
    #endif
    
}

void sheduledCheck(){
    #if RELAY_PROVIDER != RELAY_PROVIDER_ESP
    static uint32_t lastChecked = 0;
    if(millis() - lastChecked > 12000){
        lastChecked = millis();
        #if RELAY_PROVIDER == RELAY_PROVIDER_SERIAL
        Serial.println("");
        Serial.printf(PSTR("REL GET \n"));
        #endif
        #if RELAY_PROVIDER == RELAY_PROVIDER_SOFTWARE_SERIAL
        sendUartCommandGetRelayStatus();
        #endif
        #if RELAY_PROVIDER == RELAY_PROVIDER_TOUCH_PANEL_SERIAL
        askHeartBeat();
        #endif
    }
    #endif
}


void relaySetup(){
    #ifdef RELAY1_NUM
    _relays.push_back((relay_t){RELAY1_NUM, RELAY1_PIN,  RELAY1_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif
    #ifdef RELAY2_NUM
    _relays.push_back((relay_t){RELAY2_NUM, RELAY2_PIN,  RELAY2_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif
    #ifdef RELAY3_NUM
    _relays.push_back((relay_t){RELAY3_NUM, RELAY3_PIN,  RELAY3_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif
    #ifdef RELAY4_NUM
    _relays.push_back((relay_t){RELAY4_NUM, RELAY4_PIN,  RELAY4_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif
    #ifdef RELAY5_NUM
    _relays.push_back((relay_t){RELAY5_NUM, RELAY5_PIN,  RELAY5_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif
    #ifdef RELAY6_NUM
    _relays.push_back((relay_t){RELAY6_NUM, RELAY6_PIN,  RELAY6_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif
    #ifdef RELAY7_NUM
    _relays.push_back((relay_t){RELAY7_NUM, RELAY7_PIN,  RELAY7_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif
    #ifdef RELAY8_NUM
    _relays.push_back((relay_t){RELAY8_NUM, RELAY8_PIN,  RELAY8_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif
        #ifdef RELAY9_NUM
    _relays.push_back((relay_t){RELAY9_NUM, RELAY9_PIN,  RELAY9_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif
    #ifdef RELAY10_NUM
    _relays.push_back((relay_t){RELAY10_NUM, RELAY10_PIN,  RELAY10_INVERSE, 0, 0, 0, 0, false, false,0});
    #endif

    #if LED_PROVIDER != LED_PROVIDER_NONE
    Led_setup();
    #endif


    #if DIMMER_NUM > 0
    _dimmer.push_back((dimmer_t){DIMMER_NUM, 0, 0, false, 0,0,0,0});
    dimmerEnable();

    #ifdef DIMMER_PIN1
    pinMode(DIMMER_PIN1, OUTPUT);
    digitalWrite(DIMMER_PIN1, !DIMPOS1_ON_STATE);  //LOW
    #endif

    #ifdef DIMMER_PIN2
    pinMode(DIMMER_PIN2, OUTPUT);
    digitalWrite(DIMMER_PIN2, !DIMPOS2_ON_STATE);  //LOW
    #endif

    #ifdef DIMMER_PIN3
    pinMode(DIMMER_PIN3, OUTPUT);
    digitalWrite(DIMMER_PIN3, !DIMPOS3_ON_STATE);  //LOW
    #endif

    #ifdef DIMMER_PIN4
    pinMode(DIMMER_PIN4, OUTPUT);
    digitalWrite(DIMMER_PIN4, !DIMPOS4_ON_STATE);  //LOW
    #endif
    #endif

    for(uint8_t i=0; i<_relays.size(); i++){
        pinMode(_relays[i].pin, OUTPUT);
        digitalWrite(_relays[i].pin, LOW);
    }

    #if RELAY_PROVIDER == RELAY_PROVIDER_SOFTWARE_SERIAL
    softwareSerialProviderSetup();
    #endif

     #if RELAY_PROVIDER == RELAY_PROVIDER_TOUCH_PANEL_SERIAL
    KUSerialSetup();
    mqttRegisterCallback(backLightMqttCallback);
    #endif

    retrieveRelayLastState();

    mqttRegisterCallback(relayMqttCallback);

    loopRegister(relayLoop);
    
}

void relayLoop(){
    if(_changed){
        for(uint8_t i=0; i<_relays.size(); i++){
            digitalWrite(_relays[i].pin, _relays[i].status);
        }
        takeDimmerAction();
        if(reportSwitchStatus()){
            _changed = false;
        }
    }


    #if RELAY_PROVIDER != RELAY_PROVIDER_ESP
    sheduledCheck();
    #if RELAY_PROVIDER == RELAY_PROVIDER_SOFTWARE_SERIAL
    softwareSerialProviderLoop();
    #endif
    #endif
}
