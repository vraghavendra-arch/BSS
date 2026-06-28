#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include "all.h"

AsyncMqttClient mqttClient;

std::vector<mqtt_callback_f> _mqtt_callbacks;
bool done_subscription = false;

bool mqttConnected(){
    return mqttClient.connected();
}

bool mqtt_publish(const char* topic, const char* payload) {
    if (!mqttConnected()) return false;

    DEBUG_PRINT("[MQTT] PUBLISHING TOPIC: %s\nPAYLOAD: %s\n", topic, payload);

    mqttClient.publish(topic, 0, true, payload);
    return true;
}

void mqttRegisterCallback(mqtt_callback_f callback){
    _mqtt_callbacks.push_back(callback);
}

void mqtt_subscribe(const char* topic){
  mqtt_subscribeRaw(topic, 0);
}

void mqtt_subscribeRaw(const char* topic, uint8_t qos){
  DEBUG_PRINT("[MQTT] **SUBSCRIBING TOPIC -> %s\n", topic);
  mqttClient.subscribe(topic, qos);
}


void onMqttConnect(bool sessionPresent) {
  DEBUG_PRINT("Connected to MQTT.\n");
  DEBUG_PRINT("Session present: %d\n", sessionPresent);

  mqttClient.unsubscribe("#");

  for(uint8_t i=0; i<_mqtt_callbacks.size(); i++){
    (_mqtt_callbacks[i])(MQTT_EVENT_CONNECT, NULL, NULL, 0);
  }
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  DEBUG_PRINT("Disconnected from MQTT.\n");
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  DEBUG_PRINT("Subscribe acknowledged.\n");
  DEBUG_PRINT("  packetId: %d\n", packetId);
  DEBUG_PRINT("  qos: %d\n", qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  DEBUG_PRINT("Unsubscribe acknowledged.\n");
  DEBUG_PRINT("  packetId: %d\n", packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  payload[len] = '\0';
  DEBUG_PRINT("Publish received.\n");
  DEBUG_PRINT("  topic: %s\n", topic);
  DEBUG_PRINT("  Payload: %s\n", payload);
  DEBUG_PRINT("  qos: %d\n", properties.qos);
  DEBUG_PRINT("  dup: %d\n", properties.dup);

  DEBUG_PRINT("  retain: %d\n", properties.retain);
  DEBUG_PRINT("  len: %d\n", len);
  
  DEBUG_PRINT("  index: %d\n", index);

  DEBUG_PRINT("  total: %d\n", total);

  for(uint8_t i=0; i<_mqtt_callbacks.size(); i++){
    (_mqtt_callbacks[i])(MQTT_EVENT_MESSAGE, topic, payload, total);
  }

}

void onMqttPublish(uint16_t packetId) {
  DEBUG_PRINT("Publish acknowledged.\n");
  DEBUG_PRINT("  packetId: %d\n", packetId);
}

void mqttConnect(){
    static unsigned int last_tried = 0;
    static uint8_t reconnect_counter = 0;

    if(getSetting("confst", true).toInt()) return;
    
    if(mqttConnected()){
        reconnect_counter = 0;
        return;
    }

    if(millis() - last_tried <= 10000) return;
    last_tried = millis();

    DEBUG_PRINT("Connecting to MQTT...\n");
    reconnect_counter++;

    if(reconnect_counter>=MQTT_RECONNECT_MAX_ATTEMPTS){
      /*
      Special case: If the device is not able to connect to the MQTT server even after 10 attempts, it will restart.
      */
      DEBUG_PRINT("MQTT Connection failed after %d attempts. Restarting...\n", reconnect_counter);
      resetReason(CUSTOM_RESET_MQTT);
      ESP.restart();
    }

    // mqttClient.setCleanSession(true);
    // mqttClient.disconnect();
    // mqttClient.onConnect(onMqttConnect);
    // mqttClient.onDisconnect(onMqttDisconnect);
    // mqttClient.onSubscribe(onMqttSubscribe);
    // mqttClient.onUnsubscribe(onMqttUnsubscribe);
    // mqttClient.onMessage(onMqttMessage);
    // mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setKeepAlive(15);
    // mqttClient.connect();

    mqttClient.connect();

}

void mqtt2Setup() {

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  loopRegister(mqtt2Loop);
}

void mqtt2Loop() {
    if(!wifiConnected()) return;

    mqttConnect();


}
