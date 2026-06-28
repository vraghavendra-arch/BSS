#include "all.h"
#if ELEGANT_OTA_SUPPORT
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <ElegantOTA.h>

ESP8266WebServer otaServer(80);

unsigned long ota_progress_millis = 0;

void onOTAStart() {
  // Log when OTA has started
  DEBUG_PRINT("OTA update started!\n");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
// Log every 1 second
    if (millis() - ota_progress_millis > 1000) {
        ota_progress_millis = millis();
        DEBUG_PRINT("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
    }
}

void onOTAEnd(bool success) {
    // Log when OTA has finished
    if (success) {
        DEBUG_PRINT("OTA update finished successfully!\n");
    } else {
        DEBUG_PRINT("There was an error during OTA update!\n");
    }
    // <Add your own code here>
}

void elegantotaBegin(ESP8266WebServer &server){
    ElegantOTA.begin(&server);    // Start ElegantOTA
}

void elegantotaLoop(void) {
    // otaServer.handleClient();
    ElegantOTA.loop();
}

void elegantotaSetup(void) {

    bool enableElegantOta = getSetting(SETTINGS_ENABLE_ELEGANT_OTA, false).toInt();
    if(!enableElegantOta) return;

    DEBUG_PRINT("");
    DEBUG_PRINT("IP address: %s\n", WiFi.localIP().toString().c_str());
    DEBUG_PRINT("OTA setup started\n");

    otaServer.on("/", []() {
        DEBUG_PRINT("Request came\n");
        otaServer.send(200, "text/plain", "Hi! This is ElegantOTA Demo.");
    });

    // ElegantOTA callbacks
    ElegantOTA.onStart(onOTAStart);
    ElegantOTA.onProgress(onOTAProgress);
    ElegantOTA.onEnd(onOTAEnd);

    // otaServer.begin();
    DEBUG_PRINT("HTTP otaServer started\n");
    loopRegister(elegantotaLoop);
}
#endif

