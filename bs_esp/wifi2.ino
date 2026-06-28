#include "JustWifi.h"
#include <lwip/netif.h>
#include <lwip/etharp.h>
#include "all.h"

uint8_t wifiConnectCount=0;
// NOTE: Due to the wifi sleep issue - https://github.com/esp8266/Arduino/issues/2330. We are using this.
extern uint8_t ping_should_stop;
bool _wifi_connecting = false;
// extern void pingFault(void);
unsigned long last_sent_gratuitous_arp = 0;
bool _wifi_hard_reset_flag = false;

bool AP_flag = false;

void _is_wifiAPMode(bool status){
    AP_flag=status;
}

bool _is_wifiAPMode(){
    return AP_flag;
}


String getIP() {
    if (WiFi.getMode() == WIFI_AP) {
        return WiFi.softAPIP().toString();
    }
    return WiFi.localIP().toString();
}

String getNetwork() {
    if (WiFi.getMode() == WIFI_AP) {
        return jw.getAPSSID();
    }
    return WiFi.SSID();
}

double wifiDistance(int rssi) {
    double exponent = (double)(WIFI_RSSI_1M - rssi) / WIFI_PROPAGATION_CONST / 10.0;
    return round(pow(10, exponent));
}

void wifiRegister(wifi_callback_f callback) {
    jw.subscribe(callback);
}

void wifiReconnectCheck() {
    // This function is called only once in wifiConfigure
    // and wifiConfigure is called mostly once in the begining and otherwise int i = 0;
    // aftere config parse
    jw.setReconnectTimeout(WIFI_RECONNECT_INTERVAL);
}

void wifiStatus() {
    // delay(200);
    if (WiFi.getMode() == WIFI_AP_STA) {
        // Serial.println("MODE AP + STA");
        DEBUG_PRINT(PSTR("[WIFI] MODE AP + STA --------------------------------\n"));
    } else if (WiFi.getMode() == WIFI_AP) {
        //  Serial.println("MODE AP");
        DEBUG_PRINT(PSTR("[WIFI] MODE AP --------------------------------------\n"));
    } else if (WiFi.getMode() == WIFI_STA) {
        DEBUG_PRINT(PSTR("[WIFI] MODE STA -------------------------------------\n"));
    } else {
        //  Serial.println("MODE OFF");
        DEBUG_PRINT(PSTR("[WIFI] MODE OFF -------------------------------------\n"));
        DEBUG_PRINT(PSTR("[WIFI] No connection\n"));
    }

    if ((WiFi.getMode() & WIFI_AP) == WIFI_AP) {
        DEBUG_PRINT(PSTR("[WIFI] SSID %s\n"), jw.getAPSSID().c_str());
        DEBUG_PRINT(PSTR("[WIFI] PASS %s\n"), getSetting("adminPass", ADMIN_PASS).c_str());
        DEBUG_PRINT(PSTR("[WIFI] IP   %s\n"), WiFi.softAPIP().toString().c_str());
        DEBUG_PRINT(PSTR("[WIFI] MAC  %s\n"), WiFi.softAPmacAddress().c_str());
    }

    if ((WiFi.getMode() & WIFI_STA) == WIFI_STA) {
        //  Serial.println("STA SS");
        DEBUG_PRINT(PSTR("[WIFI] SSID %s\n"), WiFi.SSID().c_str());
        DEBUG_PRINT(PSTR("[WIFI] IP   %s\n"), WiFi.localIP().toString().c_str());
        DEBUG_PRINT(PSTR("[WIFI] MAC  %s\n"), WiFi.macAddress().c_str());
        DEBUG_PRINT(PSTR("[WIFI] GW   %s\n"), WiFi.gatewayIP().toString().c_str());
        DEBUG_PRINT(PSTR("[WIFI] DNS  %s\n"), WiFi.dnsIP().toString().c_str());
        DEBUG_PRINT(PSTR("[WIFI] MASK %s\n"), WiFi.subnetMask().toString().c_str());
        DEBUG_PRINT(PSTR("[WIFI] HOST %s\n"), WiFi.hostname().c_str());
        DEBUG_PRINT(PSTR("[WIFI] Channel %d \n"), WiFi.channel());
        DEBUG_PRINT(PSTR("[WIFI] Bssid %s \n"), WiFi.BSSIDstr().c_str());
        
        // jw.setWifiMode(WIFI_STA);
    }
    DEBUG_PRINT(PSTR("[WIFI] ----------------------------------------------\n"));
}

void resetWifiBeatFlags() {
    // wifi_connected_beat = false;
}

bool _wifiClean(unsigned char num) {
    bool changed = false;
    int i = 0;

    // Clean defined settings
    while (i < num) {
        // Skip on first non-defined setting
        if (!hasSetting("ssid", i)) {
            delSetting("ssid", i);
            break;
        }

        // Delete empty values
        if (!hasSetting("pass", i)) delSetting("pass", i);
        if (!hasSetting("ip", i)) delSetting("ip", i);
        if (!hasSetting("gw", i)) delSetting("gw", i);
        if (!hasSetting("mask", i)) delSetting("mask", i);
        if (!hasSetting("dns", i)) delSetting("dns", i);

        ++i;
    }

    // Delete all other settings
    while (i < WIFI_MAX_NETWORKS) {
        changed = hasSetting("ssid", i);
        delSetting("ssid", i);
        delSetting("pass", i);
        delSetting("ip", i);
        delSetting("gw", i);
        delSetting("mask", i);
        delSetting("dns", i);
        ++i;
    }

    return changed;
}

bool wifiConnected() {
    return jw.connected();
}

bool sendGratuitousARP() {
    if (!wifiConnected()) {
        return false;
    }
    bool result = false;
    for (netif* interface = netif_list; interface != nullptr; interface = interface->next) {
        if (
            (interface->flags & NETIF_FLAG_ETHARP)
            && (interface->hwaddr_len == ETHARP_HWADDR_LEN)
        #if LWIP_VERSION_MAJOR == 1
            && (!ip_addr_isany(&interface->ip_addr))
        #else
            && (!ip4_addr_isany_val(*netif_ip4_addr(interface)))
        #endif
            && (interface->flags & NETIF_FLAG_LINK_UP)
            && (interface->flags & NETIF_FLAG_UP)
        ) {
            etharp_gratuitous(interface);
            result = true;
        }
    }
    if(result){
        DEBUG_PRINT(PSTR("Sent Gratuitous ARP %d \n"), result);
    }else{
        DEBUG_PRINT(PSTR("Could not send ARP. Result is false"));
    }
    
    last_sent_gratuitous_arp = millis(); 
    return result;
     /* // TEMPORARY DISABLING HRATUITIOUS ARP
    netif* n = netif_list;
    bool res = false;
    while (n) {
        if ((n->flags & NETIF_FLAG_LINK_UP) || (n->flags & NETIF_FLAG_UP)) {
            etharp_gratuitous(n);
            res = true;
        } else {
            // DEBUG_PRINT(PSTR("netif not yet up...\n"));
        }
        n = n->next;
        delay(1);
    } */
}

void wifiDisconnect() {
    jw.disconnect();
}

void wifiDisconnectSafe() {
    _wifiConfigure();
    wifiDisconnect();
}


void _wiFiStatusCallback(justwifi_messages_t code, char* parameter) {
    if (code == MESSAGE_SCANNING) {
        DEBUG_PRINT(PSTR("[WIFI] Scanning\n"));
        _is_wifiAPMode(false);
    }

    if (code == MESSAGE_SCAN_FAILED) {
        _is_wifiAPMode(false);
        DEBUG_PRINT(PSTR("[WIFI] Scan failed\n"));
    }

    if (code == MESSAGE_NO_NETWORKS) {
        DEBUG_PRINT(PSTR("[WIFI] No networks found\n"));
        // resetLeds();
        // setLedPattern(LED_PAT_CON_ERROR);
        _is_wifiAPMode(false);
    }

    if (code == MESSAGE_NO_KNOWN_NETWORKS) {
        DEBUG_PRINT(PSTR("[WIFI] No known networks found\n"));
        // resetLeds();
        // setLedPattern(LED_PAT_CON_ERROR);
        _is_wifiAPMode(false);
    }

    if (code == MESSAGE_FOUND_NETWORK) {
        DEBUG_PRINT(PSTR("[WIFI] %s\n"), parameter);
        _is_wifiAPMode(false);
    }

    if (code == MESSAGE_CONNECTING) {
        // #if DEVELOPMENT_MODE 
        // Serial.print(millis());
        // Serial.println("Connecting");
        // #endif
        DEBUG_PRINT(PSTR("[WIFI] Connecting to %s\n"), parameter);
        _wifi_connecting = true;
        // DelayCustom(20);
        // resetLeds();
        // ledPattern = 5;
        // setLedPattern(LED_PAT_CONNECTING);
        // dontCheckWiFiStatus = true;
        _is_wifiAPMode(false);
    }

    if (code == MESSAGE_CONNECT_WAITING) {
        // too much noise
        _is_wifiAPMode(false);
    }

    if (code == MESSAGE_CONNECT_FAILED) {
        DEBUG_PRINT(PSTR("[WIFI] Could not connect to %s\n"), parameter);
        // Need to set it again as
        wifiReconnectCheck();
        // resetLeds();
        // ledPattern = 1;
        // setLedPattern(LED_PAT_CON_ERROR);
        // dontCheckWiFiStatus = true;
        _wifi_connecting = false;
        _is_wifiAPMode(false);
        // #if LOW_POWER_OPTIMIZATION
        //     delateNetworkInfoForFasterConnection();
        // #endif
    }

    if (code == MESSAGE_CONNECTED) {
        // Serial.print(millis());
        // Serial.println(" Connected");
        DEBUG_PRINT(PSTR("[WIFI] Connected Wifi \n"));
        // SET WiFi Mode back to STA.
        // jw.setWifiMode(WIFI_STA);
        // Need to set it again as
        wifiReconnectCheck();
        wifiStatus();
        // connectToMqtt();
        // resetLeds();
        // dontCheckWiFiStatus = false;
        // disableWifiLedCheck(false);

        resetWifiBeatFlags();
        // ledPattern = 3;
        // setLedPattern(LED_PAT_WIFICONNECT);
        wifiConnectCount++;
        
        _wifi_connecting = false;
        
        // We are using this
        delay(100);  // FIXME TD-er: See https://github.com/letscontrolit/ESPEasy/issues/1987#issuecomment-451644424
        // startPingAlive();
        // delay(100);
        // THIS IS TRIGGERING DEVICE RESTARTS. Thats why for now not using it.
        // sendGratuitousARP();
        _is_wifiAPMode(false);
        #if LOW_POWER_OPTIMIZATION
        saveNetworkInfoForFasterConnection();
        #endif

    }

    if (code == MESSAGE_ACCESSPOINT_CREATED) {
        DEBUG_PRINT(PSTR("[WIFI] Created Access Point"));
        wifiStatus();
        _is_wifiAPMode(true);
        // setLedPattern(LED_PAT_SETUPMODE);
        
        _wifi_connecting = false;
    }

    if (code == MESSAGE_DISCONNECTED) {
        DEBUG_PRINT(PSTR("[WIFI] Disconnected\n"));
        // wifi_connected_beat = false;
        // setLedPattern(LED_PAT_CON_ERROR);
        _wifi_connecting = false;
        _is_wifiAPMode(false);
        #if LOW_POWER_OPTIMIZATION
        delateNetworkInfoForFasterConnection();
        #endif
        // _stopPingAlive();
    }

    if (code == MESSAGE_ACCESSPOINT_CREATING) {
        DEBUG_PRINT(PSTR("[WIFI] Creating access point\n"));
        // jw.setSoftAP(getSetting("hostname", HOSTNAME).c_str(), NULL, "192.168.4.1", "192.168.4.1", "255.255.0.0");
        // resetLeds();
        // dontCheckWiFiStatus = false;
        // disableWifiLedCheck(false);
        _is_wifiAPMode(false);
    }

    if (code == MESSAGE_ACCESSPOINT_FAILED) {
        DEBUG_PRINT(PSTR("[WIFI] Could not create access point\n"));
        // ledPattern=1;
        // dontCheckWiFiStatus = true;
        // resetLeds();
        // setLedPattern(LED_PAT_CON_ERROR);
        _wifi_connecting = false;
        _is_wifiAPMode(false);
        // _stopPingAlive();
    }
}

void _wifiConfigure() {
    DEBUG_PRINT(PSTR("[DEBUG] Debugging Wifi Configure \n"));
    jw.setHostname(getSetting("hostname", IDENTIFIER()).c_str());
    // jw.setSoftAP(getSetting("hostname", HOSTNAME).c_str(), NULL, "192.168.4.1", "192.168.4.1", "255.255.0.0");
    jw.setSoftAP(getSetting("hostname", IDENTIFIER()).c_str());
    jw.setConnectTimeout(WIFI_CONNECT_TIMEOUT);
    wifiReconnectCheck();
    jw.setAPMode(AP_MODE_ALONE);
    jw.cleanNetworks();


    // TODO: Following has to be tested, i think it can be enabled even for low power optimization enabled devices. 
    // But for now, i dont have time tim verify it. Later verify and if it has no impact, then remove the elif condition and keep it.
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.setOutputPower(getSetting("wtxp", WIFI_OUTPUT_POWER).toFloat());

    // Clean settings
    _wifiClean(WIFI_MAX_NETWORKS);

    int i;
    for (i = 0; i < WIFI_MAX_NETWORKS; i++) {
        if (getSetting("ssid" + String(i)).length() == 0) break;
        DEBUG_PRINT(PSTR("[DEBUG] %s\n"), getSetting("ssid" + String(i)).c_str());
        // uint8_t bssid[] = {0xC0,0x9F,0xE1,0xEF,0xBE,0x14};
        // uint8_t bssid[] = {0x68,0xFF,0x7B,0x4D,0xCA,0x79};
        // uint8_t channel = 8;
        uint8_t bssid[6] = {0,0,0,0,0,0};
        uint8_t channel = 0;
        
        if (getSetting("ip" + String(i)).length() == 0) {
            jw.addNetwork(
                getSetting("ssid" + String(i)).c_str(),
                getSetting("pass" + String(i)).c_str());

        } else {
            jw.addNetwork(
                getSetting("ssid" + String(i)).c_str(),
                getSetting("pass" + String(i)).c_str(),
                getSetting("ip" + String(i)).c_str(),
                getSetting("gw" + String(i)).c_str(),
                getSetting("mask" + String(i)).c_str(),
                getSetting("dns" + String(i)).c_str());
        }
    }

    // Scan for best network only if we have more than 1 defined and wifiScan is set to true
    jw.scanNetworks(getSetting("wifiScan", WIFI_SCAN_NETWORKS).toInt() == 1 && i > 1);
    // jw.scanNetworks(getSetting("wifiScan", WIFI_SCAN_NETWORKS).toInt() == 1);
}

void _wifiScanFix() {
    int result = WiFi.scanComplete();
    if (result == -2 || result == 0) {
        DEBUG_PRINT(PSTR("[WIFI] AP Scan failed, scan result : %d \n"), result);
        // WiFi.scanDelete();
        WiFi.scanNetworks(true);
    }

    // else{

    //     WiFi.scanDelete();
    //     if(WiFi.scanComplete() == -2){
    //         WiFi.scanNetworks();
    //     }
    // }
}

void wifiScanSetup() {
    _wifiScanFix();
}

void wifi2Setup(){
    wifiScanSetup();
    WiFi.setSleepMode(WIFI_NONE_SLEEP);

    if(WiFi.getMode() == WIFI_OFF) {
        WiFi.enableSTA(true);
    }

    _wifiConfigure();

    // setConfigKeys(_wifi_config_keys);

    // afterConfigParseRegister(_wifiConfigure);
    wifiRegister(_wiFiStatusCallback);
    loopRegister(wifi2Loop);
}

void wifi2Loop() {
    jw.loop();
    // Gratuitous arp
    // https://github.com/esp8266/Arduino/issues/5998
    if (millis() - last_sent_gratuitous_arp > GRATUITIOUS_ARP_DELAY) {
        sendGratuitousARP();
    }
    if(_wifi_hard_reset_flag){
        DEBUG_PRINT("Hard RESETTING THE WIFI \n");
        _wifi_hard_reset_flag = false;
        wifiDisconnectSafe();
        jw.turnOff();
        jw.turnOn();
    }
}
