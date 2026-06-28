#include "all.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>

WiFiClient espClient;
// PubSubClient client(espClient);
ESP8266WebServer httpServer(80);

String Temp_SSID;
String Temp_Password;
String WiFi_SSID;
String WiFi_Password;
int counter = 0;
String deviceId;
bool gotWiFiCreds = false;
unsigned int gotWiFiCredsAt = 0;

// Update state management
enum UpdateState {
    IDLE,
    CHECKING,
    DOWNLOADING
} updateState = IDLE;

unsigned long lastUpdateCheck = 0;
HTTPClient *updateClient = nullptr;
WiFiClientSecure *secureClient = nullptr;
void elegantotaBegin(ESP8266WebServer &server);

uint8_t INIT_FLAG_ADDR = 0;  // Address to store initialization flag

void Set_Networks() {
  int n = WiFi.scanNetworks();
  DynamicJsonDocument doc(1024);
  JsonArray networks = doc.createNestedArray("networks");

  if (n == 0) {
    DEBUG_PRINT("No networks found\n");
    httpServer.send(200, "application/json", "{\"networks\": []}");
  } else {
    for (int i = 0; i < n; ++i) {
      JsonObject network = networks.createNestedObject();
      network["ssid"] = WiFi.SSID(i);
      network["rssi"] = WiFi.RSSI(i);
      delay(10);
    }

    String output;
    serializeJson(doc, output);
    DEBUG_PRINT("%s\n", output.c_str());

    httpServer.send(200, "application/json", output);

    String html = "<body style=\"background-color:powderblue;\">"
                  "<h1 style=\"text-align:center;\">Available Networks</h1>"
                  "<ul>";

    for (int i = 0; i < n; ++i) {
      html += "<li>" + WiFi.SSID(i) + " (RSSI: " + String(WiFi.RSSI(i)) + ")</li>";
    }

    html += "</ul>"
            "<form action=\"/getdata.cmd\" method=\"get\">"
            "SSID: <input type=\"text\" name=\"ssid\"><br>"
            "Password: <input type=\"text\" name=\"password\"><br>"
            "<input type=\"submit\" value=\"Submit\">"
            "</form>"
            "<a href=\"/\">Back</a>"
            "</body>";

    httpServer.send(200, "text/html", html);
  }
}

String extractVersionFromDownloadLink(const String& downloadLink) {
  int startIndex = downloadLink.lastIndexOf('/', downloadLink.lastIndexOf('/') - 1) + 1;  // Find the second-to-last '/'
  int endIndex = downloadLink.indexOf('/', startIndex);                                   // Find the next '/' after the version
  if (endIndex == -1) endIndex = downloadLink.indexOf('.', startIndex);                   // If no '/', find the '.'
  return downloadLink.substring(startIndex, endIndex);                                    // Extract version number
}

void handleRoot() {
  httpServer.send(200, "text/html", "<body style=\"background-color:powderblue;\">  <h1 style=\"text-align:center;\">SMART PLUG</h1> <p>Updating...</p>");
}

void Get_Wifi_Data() {
  if (httpServer.arg("ssid") == "" || httpServer.arg("password") == "") {
    httpServer.send(200, "text/html", "SSID or Password Missing");
  } else {
    Temp_SSID = httpServer.arg("ssid");
    Temp_Password = httpServer.arg("password");

    // Print the received values to Serial
    DEBUG_PRINT("SSID: %s, Password: %s\n", Temp_SSID.c_str(), Temp_Password.c_str());

    setSetting(SETTING_WIFI_SSID, Temp_SSID);
    setSetting(SETTING_WIFI_PASS, Temp_Password);
    setSetting("confst", true);
    saveSettings(true);

    DEBUG_PRINT("Restarting Controller...\n");

    httpServer.send(200, "text/html", "Data Received. ESP will restart.");
    // ESP.restart();
    gotWiFiCreds = true;
    gotWiFiCredsAt = millis();
    deferredReset(3000, CUSTOM_RESET_WIFI_CONFIG);
  }
}

void startUpdateCheck() {
    if (updateClient != nullptr) {
        delete updateClient;
    }
    if (secureClient != nullptr) {
        delete secureClient;
    }

    updateClient = new HTTPClient();
    secureClient = new WiFiClientSecure();
    secureClient->setInsecure();

    deviceId = getSetting(SETTING_DEVICE_ID);
    String versionUrl = "https://receiver.bharatsmr.com/api/bsh/update?entityId=" + 
                       String(deviceId) + "&version=" + APP_VERSION + "&entityType=" + ENTITY_TYPE;

    DEBUG_PRINT("Starting update check: %s\n", versionUrl.c_str());
    
    updateClient->begin(*secureClient, versionUrl);
    updateClient->addHeader("User-Agent", "ESP8266");
    updateClient->addHeader("Accept", "application/json");
    updateClient->setTimeout(10000);
    
    updateState = CHECKING;
}

void handleUpdateState() {
    if (!updateClient) return;

    DEBUG_PRINT("=======Update state: %d\n", updateState);

    switch (updateState) {
        case CHECKING: {
            int httpCode = updateClient->GET();  // This sends the request and gets response
            DEBUG_PRINT("======HTTP code: %d\n", httpCode);
            if (httpCode > 0) {
                if (httpCode == HTTP_CODE_OK) {
                    String payload = updateClient->getString();
                    DEBUG_PRINT("Update response: %s\n", payload.c_str());

                    DynamicJsonDocument doc(1024);
                    DeserializationError error = deserializeJson(doc, payload);
                    
                    if (!error) {
                        String message = doc["message"];
                        if (message.indexOf("No update found") == -1) {
                            bool success = doc["success"];
                            String downloadLink = doc["downloadLink"];
                            
                            if (success && downloadLink != "") {
                                String newVersion = extractVersionFromDownloadLink(downloadLink);
                                /*
                                We will not be using EEPROM to store the verson. Instead we will be using the version.h file and APP_VERSION macro. [do not as WHY!] [Refer NOTE.txt]
                                */
                                // setSetting(SETTING_OTA_APP_VERSION, newVersion);
                                // saveSettings(true);
                                
                                DEBUG_PRINT("Starting download from: %s\n", downloadLink.c_str());
                                ESPhttpUpdate.update(*secureClient, downloadLink);
                            }
                        }
                    }
                }
                updateState = IDLE;
                delete updateClient;
                delete secureClient;
                updateClient = nullptr;
                secureClient = nullptr;
            }
            break;
        }

        default:
            break;
    }
}

void otaCheckCallback(uint8_t event, const char* topic, const char* payload, unsigned int length){
  DEBUG_PRINT("OTA mqtt callback\n");
    if(event==MQTT_EVENT_CONNECT){
        String deviceid = getSetting("device_id", "");
        char subscribeTopic[50];
        sprintf(subscribeTopic, "%s/%s/%s/command", MQTT_DEVICE_AS, deviceid.c_str(), "ota");
        mqtt_subscribe(subscribeTopic);
        return;
    }

    if(event==MQTT_EVENT_MESSAGE){
        DEBUG_PRINT("OTA mqtt callback message\n");
        char subscribeTopic[50];
        String deviceid = DEVICE_ID();
        snprintf(subscribeTopic, sizeof(subscribeTopic), "%s/%s/%s/command", MQTT_DEVICE_AS, deviceid.c_str(), "ota");
        if(strcmp(topic, subscribeTopic)!=0) return;
    }

    StaticJsonDocument<200> root;
    DeserializationError error = deserializeJson(root, payload);
    if(error){
        DEBUG_PRINT("deserializeJson() failed: %s\n", error.f_str());
        return;
    }
    DEBUG_PRINT("OTA mqtt callback 1\n");

    if(root.containsKey("entityId")){
        String deviceid = root["entityId"];
        if(strcmp(deviceid.c_str(), DEVICE_ID().c_str())!=0) return;
    }

    if(root.containsKey("ota")){
        String ota = root["ota"];
        if(1 == atoi(ota.c_str())){
            startUpdateCheck();
        }
    }
}

void httpSetup(){
    // if(!_is_wifiAPMode()) return;

    DEBUG_PRINT("UPDATE VERSION: %s\n", APP_VERSION);
    
    httpServer.on("/", handleRoot);
   httpServer.on("/setdata.cmd", Get_Wifi_Data);
    httpServer.on("/getnetwork.cmd", Set_Networks);
    httpServer.begin();

    #if ELEGANT_OTA_SUPPORT
    if(getSetting(SETTINGS_ENABLE_ELEGANT_OTA, false).toInt()) elegantotaBegin(httpServer);
    #endif

    // mqttRegisterCallback(otaCheckCallback);

    loopRegister(httpLoop);
}

void httpLoop() {
  static int lastChecked = 0;
  httpServer.handleClient();

  // if(millis() - gotWiFiCredsAt > 2000 && gotWiFiCreds){
  //   /*
  //   After getting the Wifi credentials, need to restart and request for the device id separately. [Do not ask WHY!] [Refer NOTE.txt]
  //   */
  //   resetReason(CUSTOM_RESET_WIFI_CONFIG);
  //   ESP.restart();
  // }


  if(_is_wifiAPMode() || !wifiConnected()) return;

  /*
  Using a state machine to check for updates. This is to avoid blocking the code.
  */


  // unsigned long currentMillis = millis();
  // if (currentMillis - lastUpdateCheck >= UPDATE_CHECK_INTERVAL) {
  //   lastUpdateCheck = currentMillis;
  //   if (updateState == IDLE) {
  //     startUpdateCheck();
  //   }
  // }

  if (updateState != IDLE) {
    handleUpdateState();
  }
}