#include "all.h"
#if defined(TUNABLE_LIGHT)
uint8_t brightness = 100;
uint32_t colour = 0x000000;
uint16_t kelvin = 2700;

uint8_t writeDimmValue = 0;
uint8_t dimmValue = 0;
bool disableTransition = false;
volatile int tickk = 100;
void ICACHE_RAM_ATTR timerISR(){
    ((writeDimmValue - tickk) > 0) ? digitalWrite(LIGHT_PWM_PIN1, LOW) : digitalWrite(LIGHT_PWM_PIN1, HIGH);
    ((writeDimmValue - tickk) > 0) ? digitalWrite(LIGHT_PWM_PIN2, LOW) : digitalWrite(LIGHT_PWM_PIN2, HIGH);
    ((writeDimmValue - tickk) > 0) ? digitalWrite(LIGHT_PWM_PIN3, LOW) : digitalWrite(LIGHT_PWM_PIN3, HIGH);
    ((writeDimmValue - tickk) > 0) ? digitalWrite(LIGHT_PWM_PIN4, LOW) : digitalWrite(LIGHT_PWM_PIN4, HIGH);
    tickk--;
    tickk = tickk==0?100:tickk;
}


uint8_t getBrightness(){
    return brightness;
}


uint32_t getColour(){
    return colour;
}

uint16_t getKelvin(){
    return kelvin;
}

bool getLightState(){
    return brightness > 0;
}

void setLightState(bool state){
    if(state){
        dimmValue = brightness;
    }else{
        dimmValue = 0;
    }
    disableTransition = true;
}
void checkWiFiConnection() {
    if(WiFi.status() != WL_CONNECTED) {
        static unsigned long lastChangeTime = 0;
        static bool increasing = true;
        unsigned long currentMillis = millis();
       
        if (currentMillis - lastChangeTime >= 1000) {
            lastChangeTime = currentMillis;
            if (increasing) {
                DEBUG_PRINT("WiFi not connected in wifi loop\n");
                // setBrightness(100);
                brightness = 100;
                increasing = false;
            } else {
                // setBrightness(0);
                brightness = 0;
                increasing = true;
            } 
        }
    }
    else {
        getSetting(SETTING_LIGHT_BRIGHTNESS, "100").toInt();
    }
    // If not connected to WiFi, enter AP mode
    // WiFi.mode(WIFI_AP);
}

void setBrightness(uint8_t val){
    brightness = val;
    dimmValue = brightness;

    DEBUG_PRINT("Setting brightness to: %d\n", brightness);
    setSetting(SETTING_LIGHT_BRIGHTNESS, String(brightness));
    saveSettings(false);
}



void getLightString(JsonDocument& doc){
    // doc["entityId"] = DEVICE_ID();
    doc["brightness"] = String(getBrightness());

    #if LIGHT_HAS_COLOUR
    doc["colour"] = String(getColour());
    #endif


    #if LIGHT_HAS_KELVIN
    doc["kelvin"] = String(getKelvin());
    #endif
}



void lightMqttCallback(uint8_t event, const char* topic, const char* payload, unsigned int length){
    // if(strcmp(topic, MQTT_TOPIC_RELAY)!=0) return;
    DEBUG_PRINT("Light mqtt callback & event: %d\n", event);
    if(event==MQTT_EVENT_CONNECT){
        DEBUG_PRINT("Relay mqtt callback connect\n");
        #if defined(ENERGY_METER_1)
        mqtt_subscribe(MQTT_TOPIC_RELAY_PLUG);

        #elif POWER_PROVIDER == POWER_PROVIDER_BL6552
        
        char topic[50];
        String deviceid = getSetting("device_id", "");
        sprintf(topic, "%s/%s", MQTT_TOPIC_RELAY, deviceid.c_str());
        
        mqtt_subscribe(MQTT_TOPIC_RELAY);
        #else
        char topic[50];
        String deviceid = getSetting("device_id", "");
        snprintf(topic, sizeof(topic), "%s/%s/light/command", MQTT_DEVICE_AS, deviceid.c_str());
        mqtt_subscribe(topic);
        #endif

        return;
    }

    if(event==MQTT_EVENT_MESSAGE){
        DEBUG_PRINT("Relay mqtt callback message\n");
        char topic[50];
        String deviceid = getSetting("device_id", "");
        snprintf(topic, sizeof(topic), "%s/%s/light/command", MQTT_DEVICE_AS, deviceid.c_str());
        if(strcmp(topic, topic)!=0) return;
    }


    StaticJsonDocument<200> root;
    DeserializationError error = deserializeJson(root, payload);
    if (error){
        DEBUG_PRINT("deserializeJson() failed: %s\n", error.f_str());
        return;
    }

    // Extract relayStatus
    const char* deviceID = root["entityId"];
    const char* type = root["type"];
    
    if(strcmp(type, "brightness")==0){
        setBrightness(root["value"].as<uint8_t>());
    }
    // const char* relayStatus = root["relayStatus"];


    // DEBUG_PRINT("Device id: %s, status: %s\n", deviceID, relayStatus);

    // // Check if the entityId matches this device's ID
    // if (strcmp(DEVICE_ID().c_str(), deviceID)!=0){
    //     DEBUG_PRINT("Not equal\n");
    //     return;
    // } 

    // DEBUG_PRINT("Taking Action-> id: %d, status: %s\n", 1, relayStatus);
    // relayUpdate(1, (strcmp(relayStatus, "ON")==0));
    
}

void lightSetup() {
    brightness = getSetting(SETTING_LIGHT_BRIGHTNESS, "100").toInt();
    dimmValue = brightness;

    timer1_isr_init();
    timer1_attachInterrupt(timerISR);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
    int Loop_time=50000/500;
    timer1_write(250);

    #ifdef LIGHT_PWM_PIN1
    pinMode(LIGHT_PWM_PIN1, OUTPUT);
    #endif

    #ifdef LIGHT_PWM_PIN2
    pinMode(LIGHT_PWM_PIN2, OUTPUT);
    #endif

    #ifdef LIGHT_PWM_PIN3
    pinMode(LIGHT_PWM_PIN3, OUTPUT);
    #endif

    #ifdef LIGHT_PWM_PIN4
    pinMode(LIGHT_PWM_PIN4, OUTPUT);
    #endif

    mqttRegisterCallback(lightMqttCallback);
    loopRegister(lightLoop);

}

void lightLoop() {
    lightTransitionLoop();
    checkWiFiConnection();

    // static int test =0;

    // if(millis() - test > 5000){
    //     test = millis();
    //     DEBUG_PRINT("Light loop: %d\n", writeDimmValue);
    //     dimmValue = dimmValue==0?100:0;
    // }
}



void lightTransitionLoop(){
    static int transitionLast = 0;

    if(disableTransition){
        disableTransition = false;
        writeDimmValue = dimmValue;
        return;
    }

    if(millis() - transitionLast < 25) return;
    transitionLast = millis();

    if(writeDimmValue == dimmValue) return;

    if        (writeDimmValue < dimmValue)        writeDimmValue++; 
    else if   (writeDimmValue > dimmValue)        writeDimmValue--;
    

}
#endif