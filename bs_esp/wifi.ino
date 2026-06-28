#include <ESP8266WiFi.h>
#include "common.h"
#include "hardware.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

bool _apModeFlag = false;
bool _retryWifiConnect;
String DeviceId;
String ssid;
String pass;

bool isApMode(){
    return _apModeFlag;
}

void connectWifi(){
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);
        // delay(1000);
    }
}

bool isWifiConnected(){
    return (WiFi.status() == WL_CONNECTED);
}

void APMode() {
//   WiFi.softAP(DeviceId);
//   IPAddress myIP = WiFi.softAPIP();
//   DEBUG_PRINT("AP IP address: ");
//   DEBUG_PRINT(myIP);

    // Set_Networks();  // Display available networks
    // server.handleClient();


//   ESP.restart();
}

void wifiSetup() {
    _apModeFlag = getSetting(SETTING_CONFIG_MODE, 1).toInt();
    DeviceId = getSetting(SETTING_DEVICE_ID, HOSTNAME);
    ssid = getSetting(SETTING_WIFI_SSID);
    pass = getSetting(SETTING_WIFI_PASS);

    DEBUG_PRINT("AP Mode: %d \n\
                  Device ID: %s \n\
                  SSID: %s \n\
                  PASS: %s \n", _apModeFlag, DeviceId, ssid, pass);

    if(ssid==""){
        _apModeFlag = true;
    }
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.setOutputPower(getSetting("wtxp", 20).toFloat());

    if(_apModeFlag){
        WiFi.softAP(DeviceId);
        IPAddress myIP = WiFi.softAPIP();
        DEBUG_PRINT("AP IP address: %s\n", myIP.toString().c_str());

        //test
        String device = getSetting("device_id");
        DEBUG_PRINT("Device id: %s\n", device.c_str());
    }else{
        WiFi.setAutoReconnect(true);
        connectWifi();
    }
}

void wifiLoop() {
    static uint32_t lastTriedAt = 0;

    if((WiFi.status() != WL_CONNECTED) && !isApMode() && (millis() - lastTriedAt> 5000)){
        lastTriedAt = millis();
        connectWifi();
        WiFi.reconnect();
    }

    if(isApMode()){
        APMode();
    }
    
}