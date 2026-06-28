//Not using this anymore

// #include <PubSubClient.h>
// #include "all.h"
// #include "ESP8266WiFi.h"

// const char* mqtt_server = MQTT_SERVER;
// bool _mqtt_connected = 0;

// WiFiClient mqttEspClient;

// PubSubClient client(mqttEspClient);

// bool mqtt_isConnected(void){
//     return _mqtt_connected;
// }

// bool mqttCanSend(){
//     return mqtt_isConnected();
// }

// // void mqttRegisterCallback(mqtt_callback_f callback){
// //     client.setCallback(callback);
// // }

// void reconnect() {
//     if (!client.connected()) {
//         if (client.connect("ESP32Client")) {
//             _mqtt_connected = true;
//         } 
//     }
// }

// void mqttSetup() {
//     if(isApMode()) return;
//     client.setServer(mqtt_server, 1883);
//     reconnect();
//     // mqtt_subscribeTopic(MQTT_TOPIC_TEST);
//     loopRegister(mqttLoop);
// }

// void mqttLoop() {
//     static int last = 0;
//     if(isApMode()) return;
//     if (!client.connected()) {
//         _mqtt_connected = false;
//         reconnect();
//     }
//     client.loop();
// }
// // bool mqtt_publish(const char* topic, const char* payload) {
// //     if (!client.connected()) return false;
// //     client.setBufferSize(2048);
        

// //     client.publish(topic, payload);
// //     return true;
// // }

// // bool mqtt_subscribeTopic(const char* topic) {
// //     if (client.connected()) return false;
    
// //     client.subscribe(topic);
// //     return true;
// // }