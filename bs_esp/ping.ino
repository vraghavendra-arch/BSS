#include "all.h"
#include "ArduinoJson.h"

void getRelayStatus(StaticJsonDocument<200>& doc);

bool _ping = false;
void setReportPing(bool ping){
    _ping = ping;
}

void reportPing(){
    StaticJsonDocument<200> doc;
    getRelayStatus(doc);
    String output;
    serializeJson(doc, output);

    char topic[50];
    String deviceid = getSetting("device_id", "");
    sprintf(topic, "%s/%s/ping/status", MQTT_DEVICE_AS, deviceid.c_str());
    mqtt_publish(topic, output.c_str());

}



void pingMqttCallback(uint8_t event, const char* topic, const char* payload, unsigned int length){
    if(event==MQTT_EVENT_CONNECT){
        char subscriptionTopic[50];
        String deviceid = getSetting("device_id", "");
        sprintf(subscriptionTopic, "%s/%s/ping/command", MQTT_DEVICE_AS, deviceid.c_str());
        mqtt_subscribe(subscriptionTopic);
        return;
    }


    if(event==MQTT_EVENT_MESSAGE){
        DEBUG_PRINT("Ping command received\n");
        char subscriptionTopic[50];
        String deviceid = getSetting("device_id", "");
        sprintf(subscriptionTopic, "%s/%s/ping/command", MQTT_DEVICE_AS, deviceid.c_str());
        if(strcmp(subscriptionTopic, topic)!=0) return;
    }



    setReportPing(true);
}


void pingSetup(){
    mqttRegisterCallback(pingMqttCallback);
    loopRegister(pingLoop);
}


void pingLoop(){
    static unsigned long _ping_time = 0;
    if(millis() - _ping_time > PING_INTERVAL || _ping){
        _ping_time = millis();
        reportPing();
        setReportPing(false);
    }
}