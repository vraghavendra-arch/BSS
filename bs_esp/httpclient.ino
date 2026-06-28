#include "all.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>
#if HTTP_CONF_SUPPORT

bool fetchDeviceIdConf = true;

bool getConfigFromServer(const char* action){
    unsigned long static last_try = 0;
    if (!wifiConnected()) {
        return false;
    }
    if (millis() - last_try < HTTP_CONF_RETRY_GAP) {
        return false;
    }
    DEBUG_PRINT(PSTR("[DEBUG] Will Fetch Conf from server"));

    if(strcmp(action, "deviceid") == 0){
        // Get the MAC address of the ESP8266
        String MacAddress = WiFi.macAddress();

        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure();  // Use setInsecure() for testing purposes only.

        HTTPClient http;

        //https://receiver.bharatsmr.com/api/pmd/get3pEMId?MacID=TEST4&DeviceID=Test2&ctType=BCT100C1
        //https://receiver.bharatsmr.com/api/bsp/getBSPId?MacID=Test3&plugId=Test1&bspType=VIRTUAL
        String url = "https://receiver.bharatsmr.com/api/"DEVICE_ID_GET_HTTP_LINK"?MacID="+ MacAddress + DEVICE_ID_GET_HTTP_DATA;
        // String url = "https://receiver.bharatsmr.com/api/pmd/get3pEMId?MacID="+ MacAddress + DEVICE_ID_GET_HTTP_DATA;
        
        // String url = "https://receiver.bharatsmr.com/api/pmd/get3pEMId?MacID=" + MacAddress + "&DeviceID=Test2&ctType=BKCT100C1";
        http.begin(*client, url);
        int httpResponseCode = http.GET();

        DEBUG_PRINT("Requesting server for device id->\n%s\n", url.c_str());

        if (httpResponseCode > 0) {


          String payload = http.getString();

          DEBUG_PRINT("HTTP Response code: %d\n",httpResponseCode);
          DEBUG_PRINT("Response: %s\n", payload.c_str());

          // Parse the JSON response
          StaticJsonDocument<200> doc;
          DeserializationError error = deserializeJson(doc, payload);

          if (!error) {

            if (doc.containsKey("DeviceID")) {

                deviceId = String(doc["DeviceID"]);
                DEBUG_PRINT("GOT THE DEVICE ID");

              setSetting(SETTING_DEVICE_ID, deviceId); //store device id
              setSetting(SETTING_CONFIG_MODE, 0); //disable config mode
              setSetting("confst", false);
              saveSettings(true);

            //   INIT_FLAG_ADDR = 1;
            }else if(doc.containsKey("plugId")){
              deviceId = String(doc["plugId"]);
              setSetting(SETTING_DEVICE_ID, deviceId); //store device id
              setSetting(SETTING_CONFIG_MODE, 0); //disable config mode
              setSetting("confst", false);
              saveSettings(true);
            }
          } else {
            DEBUG_PRINT("Failed to parse JSON\n");
            return false;
            
          }
        } else {
          DEBUG_PRINT("Error in HTTP request\n");
          return false;
          
        }
        http.end();
      }
    return true;
}

void _checkConfRequirement(){
    if (fetchDeviceIdConf) {
        if (getConfigFromServer("deviceid")) {
            fetchDeviceIdConf = false;
        }
    }
}

void httpClientSetup(){
    fetchDeviceIdConf = getSetting("confst", false).toInt();
    loopRegister(httpClientLoop);
}


void httpClientLoop(){
    _checkConfRequirement();
}
#endif