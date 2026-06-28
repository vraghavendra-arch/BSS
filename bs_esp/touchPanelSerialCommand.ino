#if TOUCH_PANEL_SERIAL_COMMANDS
#include <LittleFS.h>
#include "all.h"
struct KUCTRL {
    uint16_t new_dim = 0;                   // Tuya dimmer value temp
    bool ignore_dim = false;                // Flag to skip serial send to prevent looping when processing inbound states from the faceplate interaction
    uint8_t cmd_status = 0;                 // Current status of serial-read
    uint8_t cmd_checksum = 0;               // Checksum of tuya command
    uint8_t data_len = 0;                   // Data lenght of command
    int wifi_state = -2;                // Keep MCU wifi-status in sync with WifiState()
    uint8_t heartbeat_timer = 0;            // 10 second heartbeat timer for tuya module
                                            // #ifdef USE_ENERGY_SENSOR
                                            //     uint32_t lastPowerCheckTime = 0;        // Time when last power was checked
                                            // #endif                                      // USE_ENERGY_SENSOR
    char *buffer = nullptr;                 // Serial receive buffer
    int byte_counter = 0;                   // Index in serial receive buffer
    bool low_power_mode = false;            // Normal or Low power mode protocol
    bool send_success_next_second = false;  // Second command success in low power mode
};

// typedef struct {
//     uint16_t dpId;
//     char dpValue[20];
//     uint8_t dpType;
//     bool reportToServer;
//     bool dontReportToHomePong;
// } custom_dp;

// std::vector<custom_dp> customDps;

struct KUCTRL kuCtrl; 
bool are_relays_configured = false;

void KUSendCmd(uint8_t cmd, uint8_t payload[] = nullptr, uint16_t payload_len = 0) {
    uint8_t checksum = (0xFF + cmd + (payload_len >> 8) + (payload_len & 0xFF));
    uint8_t _sendBuffer[6 + payload_len + 1];
    int i = 0;
    _sendBuffer[i++] = BYTE_1;
    _sendBuffer[i++] = BYTE_2;
    _sendBuffer[i++] = (uint8_t)BYTE_3;
    // _sendBuffer[i++] = CMD_SET;
    _sendBuffer[i++] = cmd;
    _sendBuffer[i++] = payload_len >> 8;
    _sendBuffer[i++] = payload_len & 0xFF;
    

    // Serial.write(BYTE_1);
    // Serial.write(BYTE_2);
    // Serial.write((uint8_t)BYTE_3);  // version 00
    // Serial.write(CMD_SET);
    // Serial.write(payload_len >> 8);    // following data length (Hi)
    // Serial.write(payload_len & 0xFF);  // following data length (Lo)
                                       //   snprintf_P(log_data, sizeof(log_data), PSTR("TYA: Send \"55aa00%02x%02x%02x"), cmd, payload_len >> 8, payload_len & 0xFF);
    for (uint32_t j = 0; j < payload_len; ++j) {
        // Serial.write(payload[i]);
        _sendBuffer[i++] = payload[j];
        checksum += payload[j];
    }
    _sendBuffer[i] = checksum;
    #if DEBUG_KU_CONTROLLER
        // DEBUG_PRINT(PSTR("[DEBUG] Sending data to the KU Controller \n"));
        for(int k = 0; k < sizeof(_sendBuffer); k++){
            DEBUG_PRINT(PSTR(" %x "), _sendBuffer[k]);
        }
        DEBUG_PRINT(PSTR("\n"));
    #endif

    Serial.flush();
    for(int k = 0; k < sizeof(_sendBuffer); k++){
        Serial.write(_sendBuffer[k]);
        // Serial.print(_sendBuffer[k],HEX);
    }
    // Serial.write(checksum);
    Serial.flush();
}

void KUsetRelayDP(uint8_t id, uint8_t type, uint8_t *value) {
    uint16_t payload_len = 4;
    uint8_t payload_buffer[8];
    // Serial.println();
    switch (type) {
        case DPID_INDEX_RELAY_STATUS:
        {
            payload_len += 2;
            payload_buffer[0] = DPID_INDEX_RELAY_STATUS;
            payload_buffer[1] = KU_TYPE_BOOL;
            payload_buffer[2] = 0x00;
            payload_buffer[3] = 0x02;
            payload_buffer[4] = id;
            payload_buffer[5] = value[0];
            break;
        }
        case DPID_INDEX_DIMMER_STATUS:
        {
            payload_len += 2;
            payload_buffer[0] = DPID_INDEX_DIMMER_STATUS;
            payload_buffer[1] = KU_TYPE_VALUE;
            payload_buffer[2] = 0x00;
            payload_buffer[3] = 0x02;
            payload_buffer[4] = id;
            payload_buffer[5] = value[0];
            break;
        }
        case DPID_EEPROM_ADDRESS_VALUE:
        {
            payload_len += 2;
            payload_buffer[0] = DPID_EEPROM_ADDRESS_VALUE;
            payload_buffer[1] = KU_TYPE_VALUE;
            payload_buffer[2] = 0x00;
            payload_buffer[3] = 0x02;
            payload_buffer[4] = id; //EEPROM Address on attiny side
            payload_buffer[5] = value[0]; //Value to be set on EEPROM on attiny side
            break;
        }
        case DPID_RELAY_INDX_FOR_VS:
        {
            payload_len += 2;
            payload_buffer[0] = DPID_RELAY_INDX_FOR_VS;
            payload_buffer[1] = KU_TYPE_VALUE;
            payload_buffer[2] = 0x00;
            payload_buffer[3] = 0x02;
            payload_buffer[4] = id;
            payload_buffer[5] = value[0];
            break;
        }
        case DPID_INDEX_CURTAIN_STATUS:
        {
            payload_len += 2;
            payload_buffer[0] = DPID_INDEX_CURTAIN_STATUS;
            payload_buffer[1] = KU_TYPE_VALUE;
            payload_buffer[2] = 0x00;
            payload_buffer[3] = 0x02;
            payload_buffer[4] = id;
            payload_buffer[5] = value[0];
            break;
        }        
        case DPID_LIGHT_DIMMER_FLICK_CONFIG:
        {
            payload_len += 2;
            payload_buffer[0] = DPID_LIGHT_DIMMER_FLICK_CONFIG;
            payload_buffer[1] = KU_TYPE_VALUE;
            payload_buffer[2] = 0x00;
            payload_buffer[3] = 0x02;
            payload_buffer[4] = id;
            payload_buffer[5] = value[0];
            break;
        }

        case DPID_INDEX_CHILD_LOCK:
        {
            payload_len += 2;
            payload_buffer[0] = DPID_INDEX_CHILD_LOCK;
            payload_buffer[1] = KU_TYPE_BOOL;
            payload_buffer[2] = 0x00;
            payload_buffer[3] = 0x02;
            payload_buffer[4] = id;
            payload_buffer[5] = value[0];
            break;
        }
        default:
        {
            DEBUG_PRINT(PSTR("------------Invalid set DPID--------------"));
            break;
        }
    }
    KUSendCmd(CMD_SetDP, payload_buffer, payload_len);
}

void KUsetRelayRGBbackLgtDP(uint8_t id, uint8_t type, uint8_t *value) {
    uint16_t payload_len = 14;
    uint8_t payload_buffer[16];

	payload_buffer[0] = DPID_RELAY_RGB_BACKLGT;
	payload_buffer[1] = KU_TYPE_VALUE;
	payload_buffer[2] = 0x00;
	payload_buffer[3] = 0x0A;
	payload_buffer[4] = id;
	payload_buffer[5] = value[0];
	payload_buffer[6] = value[1];
	payload_buffer[7] = value[2];
	payload_buffer[8] = value[3];
	payload_buffer[9] = value[4];
	payload_buffer[10] = value[5];
	payload_buffer[11] = value[6];
	payload_buffer[12] = value[7];
	payload_buffer[13] = value[8];
    KUSendCmd(CMD_SetDP, payload_buffer, payload_len);
}

// void customDpString(JsonObject &root) {
//     for (unsigned char i = 0; i < customDps.size(); i++) {
//         int dontReportToHomePong = customDps[i].dontReportToHomePong;
//         if (!dontReportToHomePong) {
//             String DpId = String(customDps[i].dpId);
//             String DpVal = String(customDps[i].dpValue).c_str();
//             root[DpId] = DpVal;
//         }
//     }
//     return;
// }

const size_t BYTES_PER_INT = sizeof(int); 

uint8_t intToCharArray(byte buffer[], int in)
{
    int c = 0;
    bool firstByte = false;

    for (size_t i = 0; i < BYTES_PER_INT; i++) {
        size_t shift = 8 * (BYTES_PER_INT - 1 - i);
        buffer[i] = (in >> shift) & 0xff;
        if (!firstByte && buffer[i] != 0) {
            firstByte = true;
        }
        if (firstByte) {
            c++;
        }
    }

    // #if DEBUG_KU_CONTROLLER
    //     DEBUG_PRINT(PSTR("[DEBUG] printing buffer values \n"));
    //     for(int k = 0; k < sizeof(buffer); k++){
    //         DEBUG_PRINT(PSTR(" %x "), buffer[k]);
    //     }
    //     DEBUG_PRINT(PSTR("\n"));
    // #endif

    return c;
}

void KUSetCustomSettingDP(int DPid, uint8_t type, int value ) {
    uint8_t payload_buffer[16];
    payload_buffer[0] = DPid;
    payload_buffer[1] = type;
    byte buffer[BYTES_PER_INT];
    uint8_t numberOfBytes = intToCharArray(buffer,value);
    payload_buffer[2] = numberOfBytes >> 8;
    payload_buffer[3] = numberOfBytes & 0xFF;
    uint16_t payload_len = 4;
    payload_len += numberOfBytes;

    for( uint8_t i=0; i<numberOfBytes; i++ ) {
        payload_buffer[4+i] = buffer[sizeof(buffer)-numberOfBytes+i];

        // #if DEBUG_KU_CONTROLLER
        //     DEBUG_PRINT(PSTR("[DEBUG]  i : "));
        //     DEBUG_PRINT(PSTR(" %x \n"), i);
        //     DEBUG_PRINT(PSTR(" %x \n"), buffer[sizeof(buffer)-numberOfBytes+i]);
        // #endif
    }

    KUSendCmd(CMD_SetDP, payload_buffer, payload_len);
    int32_t intValueFromBytes = byteArrayToInt(buffer, sizeof(buffer)-numberOfBytes, sizeof(buffer));
    // storeCustomDPValue(DPid, intValueFromBytes);
    // activeHomePong(true);
}

// void storeCustomDPValue(int DPid, int32_t value) {
//     bool foundCustomDp = false;
//     char temp[20];
//     sprintf(temp, "%d", value);

//     for(int i=0; i < customDps.size(); i++) {
//         if(customDps[i].dpId == DPid) {
//             foundCustomDp = true;
//             // customDps[i].dpValue = strdup(String(value).c_str());
//             DEBUG_PRINT(PSTR("[KU_CONTROLLER] esp free heap ........... : %d \n"), ESP.getFreeHeap());
//             strcpy(customDps[i].dpValue, temp);

//             #if DEBUG_KU_CONTROLLER
//                 DEBUG_PRINT(PSTR("[KU_CONTROLLER] DPValue set from char* : %s \n"), String(customDps[i].dpValue).c_str());
//             #endif
//             // if(customDps[i].reportToServer) {
//             //     reportCustomDpToServer(i);
//             // }
//             break;
//         }
//     }
    
//     if(!foundCustomDp) {
//         custom_dp newCustomDp;
//         newCustomDp.dpId = DPid;
//         strcpy(newCustomDp.dpValue, temp);
//         newCustomDp.reportToServer = false;
//         newCustomDp.dontReportToHomePong = false;
//         customDps.push_back(newCustomDp);
//     }
// }

// void reportCustomDpToServer(uint8_t index) {
//     if (!canSendMqttMessage()) {
//         return;
//     }
//     DynamicJsonBuffer jsonBuffer(100);
//     bool sent = true;

//     String dpId = String(customDps[index].dpId);
//     String dpValue = String(customDps[index].dpValue).c_str();

//     #if DEBUG_KU_CONTROLLER
//         DEBUG_PRINT(PSTR("[KU_CONTROLLER] Reporting DP Value to Server : %s \n"), dpValue.c_str());
//     #endif

//     JsonObject& root = jsonBuffer.createObject();

//     char topic[MAX_CONFIG_TOPIC_SIZE] = "";
//     strncpy_P(topic, MQTT_TOPIC_CUSTOM_COMMANDS, MAX_CONFIG_TOPIC_SIZE);

//     root[dpId] = dpValue;
//     String output;
//     root.printTo(output);
//     String mqttTopic = String(MQTT_TOPIC_CONFIG) + "/" + String(topic);
//     sent = sent && mqttSend(mqttTopic.c_str(), output.c_str());
    
//     // if (!sent) {
//     //     return false;
//     // }
//     return;
// }

void KUQueryProuctInfoChild() {
    // return;
    if(areRealysConfigured()) {
        return;
    }
    uint16_t payload_len = 0;
    uint8_t payload_buffer[1];
    payload_buffer[0] = 0x00;
    DEBUG_PRINT(PSTR("[SERIAL] Asking Child for product info \n"));
    KUSendCmd(CMD_QueryProduct, payload_buffer, payload_len);
}

bool initKuBuffer(){
    kuCtrl.buffer = new char[KU_BUFFER_SIZE];
    clearKUBuffer();
    return kuCtrl.buffer != NULL;
}

void clearKUBuffer(){
    if(kuCtrl.buffer != NULL){
        memset(kuCtrl.buffer, 0, KU_BUFFER_SIZE);
    }
}

// void printCustomDPValues() {
//     #if DEBUG_KU_CONTROLLER
//         DEBUG_PRINT(PSTR("Printing custom DP values \n"));
//         for(int i=0; i < customDps.size(); i++){
//             DEBUG_PRINT(PSTR("DPID : %d \n"), customDps[i].dpId);
//             char* cStringValue = customDps[i].dpValue;
//             DEBUG_PRINT(PSTR("DPValue : %s \n"), String(cStringValue).c_str());
//             DEBUG_PRINT(PSTR("dpType : %d \n"), customDps[i].dpType);
//             DEBUG_PRINT(PSTR("reportToServer : %d \n"), customDps[i].reportToServer);
//             DEBUG_PRINT(PSTR("dontReportToHomePong : %d \n"), customDps[i].dontReportToHomePong);
//         }
//     #endif
// }

bool isCustomConfigFileExists() {
    File file = LittleFS.open("/customConfig.json", "r");
    if(!file) {
        DEBUG_PRINT(PSTR("[CUSTOM_CONFIG] File does not exist or Failed to open\n"));
        return false;
    }

    // DynamicJsonBuffer jsonBuffer(400);
    // String line = file.readString();
    // JsonObject& customConfig = jsonBuffer.parseObject(line);
    // DEBUG_PRINT(PSTR("[CUSTOM_CONFIG] File Content:%s \n" ),line.c_str());

    file.close();
    return true;
}

// void initialiseCustomDPValues() {
//     File file = LittleFS.open("/customConfig.json", "r");
//     if(!file) {
//         DEBUG_PRINT(PSTR("[CUSTOM_CONFIG] File does not exist or Failed to open\n"));
//         return;
//     }

//     DynamicJsonBuffer jsonBuffer(400);
//     DynamicJsonBuffer jsonCmdBuffer(200);
//     String line = file.readString();
//     JsonObject& customConfig = jsonBuffer.parseObject(line);
//     DEBUG_PRINT(PSTR("[CUSTOM_CONFIG] File Content:%s \n" ),line.c_str());

//     JsonArray &configArray = customConfig["cmds"].as<JsonArray>();
//     for (const auto &config : configArray)
//     {
//         String configData = config.as<String>();
//         JsonVariant cmdConfig = jsonCmdBuffer.parseObject(configData);
        
//         int cid = cmdConfig["cid"].as<int>();
//         int cty = cmdConfig["cty"].as<int>();
//         int rTS = cmdConfig["rTS"].as<int>();
//         int dRHP = cmdConfig["dRHP"].as<int>();

//         #if DEBUG_KU_CONTROLLER 
//             DEBUG_PRINT(PSTR("[KU_CONTROLLER] config: %s\n"), configData.c_str());
//             DEBUG_PRINT(PSTR("[KU_CONTROLLER] cid: %d\n"), cid);
//             DEBUG_PRINT(PSTR("[KU_CONTROLLER] cty: %d\n"), cty);
//             DEBUG_PRINT(PSTR("[KU_CONTROLLER] rTS: %d\n"), rTS);
//             DEBUG_PRINT(PSTR("[KU_CONTROLLER] dRHP: %d\n"), dRHP);
//         #endif

//         custom_dp newCustomDp;
//         newCustomDp.dpId = cid;
//         // newCustomDp.dpValue = {0};
//         newCustomDp.dpType = cty;
//         newCustomDp.reportToServer = rTS;
//         newCustomDp.dontReportToHomePong = dRHP;
//         customDps.push_back(newCustomDp);
//     }

//     file.close();
// }

void KUSerialSetup() {
    initKuBuffer();
    // if(!isCustomConfigFileExists()) {
    //     DEBUG_PRINT(PSTR("[CUSTOM_CONFIG] Going to fetch Custom Config \n"));
    //     // setFetchCustomConfig(true);
    // }
    // initialiseCustomDPValues();
    // #if MQTT_REPORT_CUSTOM_COMMANDS
    //     setConfigKeys(custom_command_config_keys);
    // #endif
    // #if DEVELOPEMEMT_MODE || DEBUG_KU_CONTROLLER
    //     settingsRegisterCommand(F("testSetDP"), [](Embedis *e){
    //         KUSetCustomSettingDP(String(e->argv[1]).toInt(), String(e->argv[2]).toInt(), String(e->argv[3]).toInt());           
    //     });
    //     settingsRegisterCommand(F("printDP"), [](Embedis *e){
    //         printCustomDPValues();
    //     });
    // #endif
    // TODO: Loop for updating wifi status etc to child MCU.. 
}

// This thins is not right. Very error prone and could lead to exceptions. 
// TODO: Fix it. Or better use tuya protocol from espurna. 

void KUSerialprocess(void) {
    char serial_in_byte;
    while (Serial.available()) {
        serial_in_byte = Serial.read();
        DEBUG_PRINT(PSTR("[PROCESS LOOP] %x "), serial_in_byte);
        Serial.println(serial_in_byte, HEX);
        if(kuCtrl.buffer == NULL ||  kuCtrl.byte_counter>= KU_BUFFER_SIZE){
            // Clear Buffer 
            // And return
            clearKUBuffer();
            kuCtrl.byte_counter = 0;
            return;
        }
        // THIS IS WRONG - WHAT IF one of the byte in the payload is 0x55. 
        if (serial_in_byte == 0x55) {  // Start KU Packet
            kuCtrl.cmd_status = 1;
            kuCtrl.buffer[kuCtrl.byte_counter++] = serial_in_byte;
            kuCtrl.cmd_checksum += serial_in_byte;
        } else if (kuCtrl.cmd_status == 1 && serial_in_byte == 0xAA) {  // Only packtes with header 0x55AA are valid
            kuCtrl.cmd_status = 2;
            kuCtrl.byte_counter = 0;
            kuCtrl.buffer[kuCtrl.byte_counter++] = 0x55;
            kuCtrl.buffer[kuCtrl.byte_counter++] = 0xAA;
            kuCtrl.cmd_checksum = 0xFF;
        } else if (kuCtrl.cmd_status == 2) {
            if (kuCtrl.byte_counter == 5) {  // Get length of data
                kuCtrl.cmd_status = 3;
                kuCtrl.data_len = serial_in_byte;
            }
            kuCtrl.cmd_checksum += serial_in_byte;
            kuCtrl.buffer[kuCtrl.byte_counter++] = serial_in_byte;
        } else if ((kuCtrl.cmd_status == 3) && (kuCtrl.byte_counter == (6 + kuCtrl.data_len)) && (kuCtrl.cmd_checksum == serial_in_byte)) {  // Compare checksum and process packet
            kuCtrl.buffer[kuCtrl.byte_counter++] = serial_in_byte;

            char hex_char[(kuCtrl.byte_counter * 2) + 2];
            uint16_t len = kuCtrl.buffer[4] << 8 | kuCtrl.buffer[5];
            // Response_P(PSTR("{\"" D_JSON_TUYA_MCU_RECEIVED "\":{\"Data\":\"%s\",\"Cmnd\":%d"), ToHex_P((unsigned char *)kuCtrl.buffer, kuCtrl.byte_counter, hex_char, sizeof(hex_char)), kuCtrl.buffer[3]);

            if (len > 0) {
                // ResponseAppend_P(PSTR(",\"CmndData\":\"%s\""), ToHex_P((unsigned char *)&kuCtrl.buffer[6], len, hex_char, sizeof(hex_char)));
                if (CMD_STATE == kuCtrl.buffer[3]) {
                    uint16_t dpDataLen = kuCtrl.buffer[8] << 8 | kuCtrl.buffer[9];
                    // ResponseAppend_P(PSTR(",\"DpId\":%d,\"DpIdType\":%d,\"DpIdData\":\"%s\""), kuCtrl.buffer[6], kuCtrl.buffer[7], ToHex_P((unsigned char *)&kuCtrl.buffer[10], dpDataLen, hex_char, sizeof(hex_char)));
                    if (KU_TYPE_STRING == kuCtrl.buffer[7]) {
                        // ResponseAppend_P(PSTR(",\"Type3Data\":\"%.*s\""), dpDataLen, (char *)&kuCtrl.buffer[10]);
                    }
                }
            }

            // ResponseAppend_P(PSTR("}}"));

            // if (Settings.flag3.tuya_serial_mqtt_publish) {  // SetOption66 - Enable TuyaMcuReceived messages over Mqtt
            //     MqttPublishPrefixTopic_P(RESULT_OR_TELE, PSTR(D_JSON_TUYA_MCU_RECEIVED));
            // } else {
            //     AddLog_P(LOG_LEVEL_DEBUG, mqtt_data);
            // }
            // XdrvRulesProcess();

            // if (!kuCtrl.low_power_mode) {
            //     // TuyaNormalPowerModePacketProcess();
            // } else {
            //     // TuyaLowPowerModePacketProcess();
            // }
            KUNormalPowerModePacketProcess();

            kuCtrl.byte_counter = 0;
            kuCtrl.cmd_status = 0;
            kuCtrl.cmd_checksum = 0;
            kuCtrl.data_len = 0;
        }                                                     // read additional packets from KU
        else if (kuCtrl.byte_counter < KU_BUFFER_SIZE - 1) {  // add char to string if it still fits
            kuCtrl.buffer[kuCtrl.byte_counter++] = serial_in_byte;
            kuCtrl.cmd_checksum += serial_in_byte;
            #if DEVELOPEMEMT_MODE
                #if DEBUG_SOFTWARE_SUPPORT == 0
                    if(kuCtrl.byte_counter == (6 + kuCtrl.data_len)){
                        Serial.println("----------------------------");
                        Serial.print("checksum should be -");
                        Serial.println(kuCtrl.cmd_checksum, HEX);
                        Serial.println("----------------------------");
                    }
                #else
                    if(kuCtrl.byte_counter == (6 + kuCtrl.data_len)){
                        DEBUG_PRINT(PSTR("[KU_CONTROLLER] checksum should be - %x \n"), kuCtrl.cmd_checksum);
                    }
                #endif
            #endif
        } else {
            kuCtrl.byte_counter = 0;
            kuCtrl.cmd_status = 0;
            kuCtrl.cmd_checksum = 0;
            kuCtrl.data_len = 0;
        }
        //add a delay of 1000us
        delayMicroseconds(1000);
    }
}

void KUSerialprocessTest(void) {
    uint8_t serial_in_byte;
    while (Serial.available() && ((serial_in_byte = Serial.read()) >= 0)) {
        if (serial_in_byte) {
            DEBUG_PRINT(PSTR("BYTE: %d\n"), serial_in_byte);
        }
    }
}

bool areRealysConfigured() {
    return are_relays_configured;
}

void setRelaysConfigured(bool configured) {
    DEBUG_PRINT(PSTR("[KU_CONTROLLER] Relays Configured \n"));
    are_relays_configured = configured;
}

// void processQueryProduct(char *frame) {
//     unsigned int datLength1 = frame[4];
//     unsigned int datLength2 = frame[5];
//     unsigned int DPID = frame[6];
//     unsigned int DataType = frame[7];

//     DEBUG_PRINT(PSTR("[KU_CONTROLLER] In Process Query Product with DPID: %x \n"), DPID);
//     switch (DPID)
//     {
//     case DPID_PRODUCT_INFO_RELAYS:
//     {
//         if(areRealysConfigured() == false) {
//             unsigned int relayCount1 = frame[8];
//             unsigned int relayCount2 = frame[9];
//             unsigned int relayCount = relayCount1 + relayCount2;
//             for (unsigned int n = 0; n < relayCount; ++n) {
//                 unsigned int relayType = frame[10 + n];
//                 switch(relayType)
//                 {
//                     case RELAY_TYPE_TOGGLE:
//                         DEBUG_PRINT(PSTR("[KU_CONTROLLER] creating relay at %d\n"), n+1);
//                         createRelays(n+1,RELAY_TYPE_TOGGLE);
//                         break;
//                     case RELAY_TYPE_DIMMER:
//                         DEBUG_PRINT(PSTR("[KU_CONTROLLER] creating dimmer at %d\n"), n+1);
//                         createRelays(n+1,RELAY_TYPE_DIMMER);
//                         createDimmers(n+1);
//                         break;
//                     case RELAY_TYPE_CURTAIN:
//                         DEBUG_PRINT(PSTR("[KU_CONTROLLER] creating curtain at %d\n"), n+1);
//                         createRelays(n+1,RELAY_TYPE_CURTAIN);
//                         createCurtains(n+1);
//                         break;
//                     default:
//                         //unknown relay type should throw error back to attiny
//                         break;
//                 }
//             }
//             startAskingHeartBeat();
//             setRelaysConfigured(true);
//         } else {
//             DEBUG_PRINT(PSTR("[KU_CONTROLLER] Relays already configured, not configuring again\n"));
//         }
//         break;
//     }

//     case DPID_PRODUCT_INFO_DIMMING_STEPS:
//     {
//         unsigned int relayCount1 = frame[8];
//         unsigned int relayCount2 = frame[9];
//         unsigned int relayCount = relayCount1 + relayCount2;
//         DEBUG_PRINT(PSTR("[KU_CONTROLLER] creating dimming steps  \n"));
//         for (unsigned int n = 0; n < relayCount; ++n) {
//             uint8_t dimmingStep = frame[10 + n];
//             setDimmingStep(n+1,dimmingStep);
//         }
//         break;
//     }

//     case DPID_PRODUCT_INFO_SENSORS:
//     {
//         unsigned int sensorCount1 = frame[8];
//         unsigned int sensorCount2 = frame[9];
//         unsigned int sensorCount = sensorCount1 + sensorCount2;
//         for (unsigned int n = 0; n < sensorCount; ++n) {
//             unsigned int sensorType = frame[10 + n];
//             switch(sensorType)
//             {
//                 case SENSOR_TYPE_TEMP:

//                     break;

//                 case SENSOR_TYPE_HUMID:

//                     break;

//                 case SENSOR_TYPE_POWER:

//                     break;

//                 default:
//                     //unknown sensor type should throw error back to attiny
//                     break;
//             }
//         }
//         break;
//     }

//     case DPID_PRODUCT_INFO_RELAY_FEATURES:
//     {
//         unsigned int relayFeaturesCount1 = frame[8];
//         unsigned int relayFeaturesCount2 = frame[9];
//         unsigned int relayFeaturesCount = relayFeaturesCount1 + relayFeaturesCount2;
//         for (unsigned int n = 0; n < relayFeaturesCount; ++n) {
//             unsigned int relayFeature = frame[10 + n];
//             setRelayFeature(n+1,relayFeature);
//         }
//         break;
//     }

//     case DPID_PRODUCT_INFO_UNIQUE_ID:
//     {        
//         static uint8_t uniqueId[16];
//         uint8_t uniqueIdCount = frame[8] + frame[9];
//         for (uint8_t n = 0; n < uniqueIdCount; ++n) {
//             uint8_t _uniqueId = frame[10 + n];
//             uniqueId[n] = _uniqueId;
//         }
//         setChildDeviceUniqueId(&uniqueId[0], 16);                 
//         break;
//     }

//     case DPID_PRODUCT_INFO_VERSION:
//     {        
//         static uint8_t devVersion[3];
//         static uint8_t devId[5];
//         uint8_t versionCount = frame[8] + frame[9];
//         for (uint8_t n = 0; n < versionCount; ++n) {
//             uint8_t version = frame[10 + n];
//             if(n < 3) {
//                 devVersion[n] = version;
//             } else {
//                 devId[n-3] = version;
//             }
//         }        
//         setChildDeviceVersion(&devVersion[0], 3);      
//         setChildDeviceId(&devId[0], 5);
//         break;
//     }
    
//     default:
//         //unknown DPID type should throw error back to attiny
//         DEBUG_PRINT(PSTR("[PROD INFO DP] Unknown DPID"));
//         break;
//     }
// }

void processSetDP(char *frame) {
    unsigned int datLength1 = frame[4];
    unsigned int datLength2 = frame[5];
    unsigned int DPID = frame[6];
    unsigned int DataType = frame[7];
    unsigned int relayCount1 = frame[8];
    unsigned int relayCount2 = frame[9];

    unsigned int totDataleLen = datLength1 + datLength2;
    DEBUG_PRINT(PSTR("[KU_CONTROLLER] In Process SET DP with DPID: %x\n"), DPID);

    // if(DPID > CUSTOM_DPIDS_START) {
    //     int dataLen = relayCount1 + relayCount2;
    //     byte buffer[dataLen];
        
    //     for(int i=0; i<dataLen; i++) {
    //         buffer[i] = frame[10+i];
    //         #if DEBUG_KU_CONTROLLER
    //             DEBUG_PRINT(PSTR("values of set DP: %x \n"), buffer[i]);
    //         #endif
    //     }

    //     int32_t intValueFromBuffer = byteArrayToInt(buffer, 0, dataLen);
    //     // storeCustomDPValue(DPID, intValueFromBuffer);
    //     // activeHomePong(true);
    // }

    switch (DPID)
    {
        case DPID_RELAY_STATUS:
        {
            unsigned int subDataLen = relayCount1 + relayCount2;
            unsigned int relayCnt = relayCount();
            for (unsigned int n = 0; n < relayCnt; ++n) {
                unsigned int relayStatus = frame[10 + n];
                relayUpdate((unsigned char)n+1,(unsigned char)relayStatus);
                
            }
            break;
        }

        case DPID_INDEX_RELAY_STATUS:
        {
            unsigned char relayIndex = frame[10];
            unsigned char relayStatus = frame[11];
            relayUpdate((unsigned char)relayIndex,(unsigned char)relayStatus);
                //schReportRelayStatus(relayIndex);
                // relaySave();
                // if(totDataleLen == 6) {
                //     relayMQTT((unsigned char)relayIndex);
                // } else if(totDataleLen == 7) {
                //     unsigned int triggerSourceRel = frame[12];

                //     relayMQTT((unsigned char)relayIndex, triggerSourceRel);
                // } else {
                //     // need to handle this case;
                // }
                // #if MQTT_REPORT_EVENT_TO_HOME
                //     activeHomePong(true);
                // #endif
            
            break;
        }

        case DPID_DIMMER_STATUS:
        {
            //TODO: Implementing Dimmer

            unsigned int subDataLen = relayCount1 + relayCount2;
            unsigned int relayCnt = relayCount();
            for (unsigned int n = 0; n < relayCnt; ++n) {
                unsigned int dimmerStatus = frame[10 + n];
                dimmerUpdate((unsigned char)n+1, (unsigned int)dimmerStatus);
            }
            break;

        }

        case DPID_INDEX_DIMMER_STATUS:

        {
            unsigned int dimmerIndex = frame[10];
            unsigned int dimmerStatus = frame[11];
            dimmerUpdate((unsigned char)dimmerIndex, (unsigned int)dimmerStatus);
            // if(dimmerUpdate(dimmerIndex,dimmerStatus)) {
            //     dimmerSave();


            //     if(totDataleLen == 6) {
            //         dimmerMQTT((unsigned char)dimmerIndex);
            //     } else if(totDataleLen == 7) {
            //         unsigned int triggerSourceDim = frame[12];
            //         dimmerMQTT((unsigned char)dimmerIndex, triggerSourceDim);
            //     } else {
            //         // need to handle this case;
            //     }
            //     #if MQTT_REPORT_EVENT_TO_HOME
            //         activeHomePong(true);
            //     #endif
            // }
            break;
        }

        case DPID_INDEX_CHILD_LOCK:
        {
            // unsigned int childLockIndex = frame[10];
            // unsigned int childLockVal = frame[11];
            // if(getSetting(SETTINGS_CHILD_LOCK_SUPPORT, SETTINGS_CHILD_LOCK_SUPPORT_EN).toInt()) {
            //     setSetting(SETTINGS_CHILD_LOCK_CFG, childLockVal);
            //     saveSettings();
            //     // how to report config values to server
            //     #if MQTT_REPORT_EVENT_TO_HOME
            //         activeHomePong(true);
            //     #endif
            // }
            break;
        }
// #if POWER_PROVIDER == POWER_PROVIDER_SERIAL
//         case DPID_POWER:
//         {
//             // int dataLen = relayCount1 + relayCount2;
//             // uint32_t powerValue = byteArrayToInt((uint8_t *) frame, 10, 10+dataLen);
//             // _activePowerIntake((double) powerValue);
//             // setActivePower((double) powerValue);
//             // activeHomePong(true);
//             // break;
//         }
// #endif
// #if CURTAIN_SUPPORT
//         case DPID_INDEX_CURTAIN_STATUS:
//         {
//             unsigned int curtIndex = frame[10];
//             unsigned int curtStatus = frame[11];
//             setCurtainSwitchState(curtStatus);

//             if(totDataleLen == 6) {
//                 // need to implement this case;
//             } else if(totDataleLen == 7) {
//                 unsigned int triggerSourceCurt = frame[12];
//                 // need to implement this case;
//             } else {
//                  // need to implement this case;
//             }            
//             activeHomePong(true);
//             break;
//         }
// #endif
// #if (IR_LED_TEST == 1)
//         case DPID_IR_TESTING:
//         {
//             unsigned int irLedIndex = frame[10];
//             unsigned int irLedStatus = frame[11];
//             startIrLedTest(irLedIndex, irLedStatus);
//             break;
//         }
// #endif
        default:
            //unknown DPID type should throw error back to attiny
            DEBUG_PRINT(PSTR("[PSETDP] Unknown DPID"));
            break;
    }
}

void KUNormalPowerModePacketProcess() {
    DEBUG_PRINT(PSTR("%d \n"), kuCtrl.data_len);
    for (int i = 0; i < kuCtrl.data_len; i++) {
        DEBUG_PRINT(PSTR(" %x "), kuCtrl.buffer[i]);
    }
    DEBUG_PRINT(PSTR("\n"));
    switch ((uint8_t)kuCtrl.buffer[3]) {
        case CMD_QueryProduct:
            DEBUG_PRINT(PSTR("[KU_CONTROLLER] RX Product info\n"));
            // processQueryProduct(kuCtrl.buffer);
            break;

        case CMD_WiFiResetCfg:
            DEBUG_PRINT(PSTR("[KU_CONTROLLER] RX WiFi reset config \n"));
            settingFactoryReset();
            break;

        case CMD_ReportDP:
            // TO DO:
            // since attiny is reporting to ESP, we set the DP here
            // its confusing need to change the names to make them meaningful
            DEBUG_PRINT(PSTR("[KU_CONTROLLER] RX Set DP \n"));
            processSetDP(kuCtrl.buffer);
            break;

        case CMD_SetDP:
            // TO DO:
            // refer above reportDP case also
            // its confusing need to change the names to make them meaningful
            DEBUG_PRINT(PSTR("[KU_CONTROLLER] You shouldn't be here man! check your DPIDs on both sides \n"));
            DEBUG_PRINT(PSTR("[KU_CONTROLLER] RX Process DP \n"));
            //doesn't make sense to process this command from attiny as it never recives it here
            // cause we'll report to attiny and set there, vice versa
            // processReportDP(kuCtrl.buffer);
            break;

        case CMD_MCU_CONF:
            DEBUG_PRINT(PSTR("KIOT: RX MCU configuration Mode=%d"), kuCtrl.buffer[5]);

            if (kuCtrl.buffer[5] == 2) {  // Processing by ESP module mode
                uint8_t led1_gpio = kuCtrl.buffer[6];
                uint8_t key1_gpio = kuCtrl.buffer[7];
                bool key1_set = false;
                bool led1_set = false;
                // for (uint32_t i = 0; i < sizeof(Settings.my_gp); i++) {
                //     if (Settings.my_gp.io[i] == GPIO_LED1)
                //         led1_set = true;
                //     else if (Settings.my_gp.io[i] == GPIO_KEY1)
                //         key1_set = true;
                // }
                // if (!Settings.my_gp.io[led1_gpio] && !led1_set) {
                //     Settings.my_gp.io[led1_gpio] = GPIO_LED1;
                //     restart_flag = 2;
                // }
                // if (!Settings.my_gp.io[key1_gpio] && !key1_set) {
                //     Settings.my_gp.io[key1_gpio] = GPIO_KEY1;
                //     restart_flag = 2;
                // }
            }
            // TuyaRequestState();
            break;

        default:
            DEBUG_PRINT(PSTR("[KU_CONTROLLER] RX unknown command \n"));
    }
}

void KUQueryPowerInfo() {
    uint16_t payload_len = 4;
    uint8_t payload_buffer[4];
    payload_buffer[0] = DPID_POWER;
    payload_buffer[1] = KU_TYPE_VALUE;
    payload_buffer[2] = 0x00;
    payload_buffer[3] = 0x00;
    DEBUG_PRINT(PSTR("[SERIAL] Power info to controller \n"));
    KUSendCmd(CMD_SetDP, payload_buffer, payload_len);
}

// void storeCustomConfig(JsonObject &customConfig) {
//     if(!customConfig["cmds"].is<JsonArray&>()){
//         DEBUG_PRINT(PSTR("[KU_CONTROLLER] Custom Config Payload is not array\n"));
//         return;
//     }
//     JsonArray &configArray = customConfig["cmds"].as<JsonArray>();
//     File file = LittleFS.open("/customConfig.json", "w");
    
//     // Serialize/store JSON to file
//     if (customConfig.printTo(file) == 0) {
//         Serial.println(F("Failed to write to file"));
//     }
//     DEBUG_PRINT(PSTR("[CUSTOM_CONFIG] Created file size = %d\n"), file.size());

//     file.close();

//     if(configArray.size() <= 0){
//         DEBUG_PRINT(PSTR("[KU_CONTROLLER] Custom Config Payload: No commands found\n"));
//         return;
//     }
// }

// #if MQTT_REPORT_CUSTOM_COMMANDS
// void custom_command_config_keys(JsonBuffer& jsonBuffer, JsonObject& root, char* topic, bool nested_under_topic) {
//     strncpy_P(topic, MQTT_TOPIC_CUSTOM_COMMANDS, MAX_CONFIG_TOPIC_SIZE);
//     if (nested_under_topic) {
//         DynamicJsonBuffer jsonCmdArrayBuffer(200);
//         JsonArray& cmdArray = jsonCmdArrayBuffer.createArray();
//         JsonObject& object = cmdArray.createNestedObject();

//         for (unsigned char i = 0; i < customDps.size(); i++) {            
//             int reportToServer = customDps[i].reportToServer;
//             if (reportToServer) {
//                 DEBUG_PRINT(PSTR("[KU_CONTROLLER] In report to server : %d\n"), i);
//                 String DpId = String(customDps[i].dpId);
//                 String DpVal = String(customDps[i].dpValue).c_str();
//                 object[DpId] = DpVal;   
//                 cmdArray.add(object);
//             }
//         }
//         JsonObject& data = root.createNestedObject(jsonBuffer.strdup(topic));
//         data["customCmds"] = cmdArray;
//     } else {
//         DynamicJsonBuffer jsonCmdArrayBuffer(200);
//         JsonArray& cmdArray = jsonCmdArrayBuffer.createArray();
//         JsonObject& object = cmdArray.createNestedObject();

//         for (unsigned char i = 0; i < customDps.size(); i++) {            
//             int reportToServer = customDps[i].reportToServer;
//             if (reportToServer) {
//                 DEBUG_PRINT(PSTR("[KU_CONTROLLER] In report to server : %d\n"), i);
//                 String DpId = String(customDps[i].dpId);
//                 String DpVal = String(customDps[i].dpValue).c_str();
//                 object[DpId] = DpVal;   
//                 cmdArray.add(object);
//             }
//         }
//         root["customCmds"] = cmdArray;
//     }
// }
// #endif

//utils
int32_t byteArrayToInt(uint8_t *byteArray, uint8_t from, uint8_t to) {
    int32_t result = 0;
    int i;
    for (i = from; i < to; i++) {
        result = (result << 8) + byteArray[i];
    }
    return result;
}

#endif