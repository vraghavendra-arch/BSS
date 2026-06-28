#include "hardware.h"

//debug setup////////////////////////////////////////////////
#define DEBUG_PORT   Serial // hardware serial for debugging

#if DEBUG_ENABLE
#define DEBUG_PRINT(...)  DEBUG_PORT.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif
////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

#ifndef MQTT_USE_ASYNC
#define MQTT_USE_ASYNC 1
#endif

#ifndef MQTT_SSL_ENABLED
#define MQTT_SSL_ENABLED 0  // By default ssl will be enabled on MQTT
#endif

#ifndef MQTT_KEEPALIVE
#define MQTT_KEEPALIVE 60
#endif

#define MQTT_RECONNECT_DELAY 10000
#define MQTT_TRY_INTERVAL 30000

#ifndef MQTT_MAX_TRIES
#define MQTT_MAX_TRIES 8
#endif

#ifndef MQTT_SKIP_RETAINED
#define MQTT_SKIP_RETAINED 1
#endif

#ifndef MQTT_SKIP_TIME
#define MQTT_SKIP_TIME 1000
#endif

#ifndef MQTT_STABLE_TIME
#define MQTT_STABLE_TIME 3000
#endif

#ifndef MQTT_RECONNECT_DELAY_MIN
#define MQTT_RECONNECT_DELAY_MIN 10000   // Try to reconnect in 5 seconds upon disconnection
#endif

#ifndef MQTT_RECONNECT_DELAY_STEP
#define MQTT_RECONNECT_DELAY_STEP 10000  // Increase the reconnect delay in 2 seconds after each failed attempt
#endif

#ifndef MQTT_RECONNECT_DELAY_MAX
#define MQTT_RECONNECT_DELAY_MAX 30000   // Set reconnect time to 30 seconds at most
#endif

#ifndef MQTT_HOME_PING_RETAINED
#define MQTT_HOME_PING_RETAINED false
#endif

#define MQTT_USE_JSON 1
#define MAX_CONFIG_TOPIC_SIZE 10

// Config updates
#define MQTT_REPORT_CONFIG_POWER 0

#define MQTT_STATUS_ONLINE "1"
#define MQTT_STATUS_OFFLINE "0"

#define MQTT_ACTION_RESET "reset"

#define MQTT_CONNECT_EVENT 0
#define MQTT_DISCONNECT_EVENT 1
#define MQTT_MESSAGE_EVENT 2

// Custom get and set postfixes
// Use something like "/status" or "/set", with leading slash
#define MQTT_USE_GETTER "/get"
#define MQTT_USE_SETTER "/set"

//mqtt topics and keys
#define MQTT_KEY_DEVICE_ID "DeviceID"
#define MQTT_KEY_ENTITY_ID "entityId"
#define MQTT_KEY_DATE "Date"
#define MQTT_KEY_TIME "Time"
#define MQTT_KEY_VOLTAGE_R "Voltage_R"
#define MQTT_KEY_CURRENT_R "Current_R"
#define MQTT_KEY_POWER_R "Power_R"
#define MQTT_KEY_ENERGY_R "Energy_R"
#define MQTT_KEY_APPARENT_POWER_R "ApparentPower_R"
#define MQTT_KEY_APPARENT_ENERGY_R "Apparent_Energy_R"
#define MQTT_KEY_REACTIVE_POWER_R "ReactivePower_R"
#define MQTT_KEY_REACTIVE_ENERGY_R "Reactive_Energy_R"
#define MQTT_KEY_FREQUENCY_R "Frequency_R"
#define MQTT_KEY_PF_R "PF_R"
#define MQTT_KEY_VOLTAGE_Y "Voltage_Y"
#define MQTT_KEY_CURRENT_Y "Current_Y"
#define MQTT_KEY_POWER_Y "Power_Y"
#define MQTT_KEY_ENERGY_Y "Energy_Y"
#define MQTT_KEY_APPARENT_POWER_Y "ApparentPower_Y"
#define MQTT_KEY_APPARENT_ENERGY_Y "Apparent_Energy_Y"
#define MQTT_KEY_REACTIVE_POWER_Y "ReactivePower_Y"
#define MQTT_KEY_REACTIVE_ENERGY_Y "Reactive_Energy_Y"
#define MQTT_KEY_FREQUENCY_Y "Frequency_Y"
#define MQTT_KEY_PF_Y "PF_Y"
#define MQTT_KEY_VOLTAGE_B "Voltage_B"
#define MQTT_KEY_CURRENT_B "Current_B"
#define MQTT_KEY_POWER_B "Power_B"
#define MQTT_KEY_ENERGY_B "Energy_B"
#define MQTT_KEY_APPARENT_POWER_B "ApparentPower_B"
#define MQTT_KEY_APPARENT_ENERGY_B "Apparent_Energy_B"
#define MQTT_KEY_REACTIVE_POWER_B "ReactivePower_B"
#define MQTT_KEY_REACTIVE_ENERGY_B "Reactive_Energy_B"
#define MQTT_KEY_FREQUENCY_B "Frequency_B"
#define MQTT_KEY_PF_B "PF_B"

#define MQTT_TOPIC_VOLTAGE_PLUG              "voltage"
#define MQTT_TOPIC_CURRENT_PLUG              "current"
#define MQTT_TOPIC_POWER_FACTOR_PLUG         "PF"
#define MQTT_TOPIC_POWER_ACTIVE_PLUG          "power"
#define MQTT_TOPIC_POWER_REACTIVE_PLUG        "reactive"
#define MQTT_TOPIC_POWER_APPARENT_PLUG        "apparent"
#define MQTT_KEY_ENERGY_PLUG                  "energy"

#define MQTT_TOPIC_VOLTAGE              "Voltage"
#define MQTT_TOPIC_CURRENT              "Current"
#define MQTT_TOPIC_POWER_FACTOR         "PF"
#define MQTT_TOPIC_POWER_ACTIVE          "Power"
#define MQTT_TOPIC_POWER_REACTIVE        "Reactive"
#define MQTT_TOPIC_POWER_APPARENT        "Apparent"

#define DEVICE_ID_API     ""
//mqtt
#define MQTT_USERNAME     ""
#define MQTT_PASSWORD
#define MQTT_SERVER       "35.154.62.193"
#define MQTT_PORT         1883
#define MQTT_IDENTIFIER

#define MQTT_EVENT_MESSAGE    1
#define MQTT_EVENT_CONNECT    2

//mqtt topics
#ifndef MQTT_DEVICE_AS
#define MQTT_DEVICE_AS   ""
#endif

#define MQTT_TOPIC_ENERGY  "energy-meter/"MQTT_DEVICE_AS
#define MQTT_TOPIC_ENERGY_PLUG  "smart-switch/data"
#define MQTT_TOPIC_STATUS  "energy-meter/status"//MQTT_DEVICE_AS
#define MQTT_TOPIC_RELAY   "energy-meter/three-phase/command"//"energy-meter/command"
#define MQTT_TOPIC_RELAY_PLUG   "smart-switch/command"
#define MQTT_TOPIC_WILL_STATUS "status"
#define MQTT_TOPIC_TEST    "test"
#define MQTT_TOPIC_OTA  "ota/version"
#define MQTT_TOPIC_OTA_CHECK "ota/check"

#define MQTT_KEY_ENERGY           "Energy"
#define MQTT_KEY_APPARENT_ENERGY  "Apparent_Energy"
#define MQTT_KEY_REACTIVE_ENERGY  "Reactive_Energy"

#define MQTT_OTA_VERSION  "ota_version"


#define POWER_REPORTING_INTERVAL   5 //sec, time interval to report to the server
#define POWER_READ_INTERVAL        1000 //ms, time interval to read power data from the IC


#define EEPROM_DATA_OFFSET   11
#define EEPROM_SIZE          4096

#define SETTING_CONFIG_MODE     "confg"
#define SETTING_OTA_APP_VERSION "version"
#define SETTING_DEVICE_ID       "device_id"
#define SETTING_WIFI_SSID       "ssid0"
#define SETTING_WIFI_PASS       "pass0"
#define SETTINGS_ENABLE_ELEGANT_OTA  "eota"

#define SETTING_POWER_ENERGY_TOTAL    "kwh"
#define SETTING_POWER_ENERGY_APPARENT  "kvah"
#define SETTING_POWER_ENERGY_REACTIVE  "kvarh"

#define SETTING_LIGHT_BRIGHTNESS "lght"

#define RELAY_PROVIDER_SERIAL             1
#define RELAY_PROVIDER_SOFTWARE_SERIAL    2
#define RELAY_PROVIDER_ESP                3
#define RELAY_PROVIDER_TOUCH_PANEL_SERIAL 4

#define LONG_LONG_PRESS_DURATION  15000 //ms
#define LONG_PRESS_DURATION  5000 //ms
#define NORMAL_PRESS_DURATION  10 //ms

#define BL6552_CURRENT_STANDARD_ADDR           "iS"
#define BL6552_CURRENT_K_CONSTANT_ADDR         "iK"
#define BL6552_VOLTAGE_STANDARD_ADDR           "vS"
#define BL6552_VOLTAGE_K_CONSTANT_ADDR         "vK"
#define BL6552_ACTIVE_POWER_STANDARD_ADDR      "pS"
#define BL6552_ACTIVE_POWER_K_CONSTANT_ADDR    "pK"
#define BL6552_APPARENT_POWER_STANDARD_ADDR    "sS"
#define BL6552_APPARENT_POWER_K_CONSTANT_ADDR  "sK"
#define BL6552_REACTIVE_POWER_STANDARD_ADDR    "qS"
#define BL6552_REACTIVE_POWER_K_CONSTANT_ADDR  "qK


// -----------------------------------------------------------------------------
// WIFI & WEB
// -----------------------------------------------------------------------------
#ifndef WIFI_RECONNECT_INTERVAL
#define WIFI_RECONNECT_INTERVAL 30000        // If could not connect to WIFI, retry after this time in ms
#endif

#define WIFI_RECONNECT_LONG_INTERVAL 240000  // 4 minutes
#define WIFI_CONNECT_TIMEOUT 30000           // Connection Timeout
#define GRATUITIOUS_ARP_DELAY 20000          // Delay between continous arp packets. For resolving some issue related to dropping of wifi.

#define WIFI_MAX_NETOWRK_DISCOVERY 30
#define ADMIN_PASS "coral@bharat231"
#define FORCE_CHANGE_PASS 1

#ifndef WIFI_OUTPUT_POWER
#define WIFI_OUTPUT_POWER 20
#endif

// #define HTTP_USERNAME           "admin"
// #define WS_BUFFER_SIZE          5
// #define WS_TIMEOUT              1800000
#define WEB_PORT 80  // HTTP port
// #define DNS_PORT                53

// #define WEB_MODE_NORMAL         0
// #define WEB_MODE_PASSWORD       1

// for kiot red ap_mode is already defined - AP_MODE_ALONE
#include "JustWifi.h"
#ifndef AP_MODE
#define AP_MODE AP_MODE_ONLY_IF_NOT_AVAILABLE  //AP_MODE_ALONE //  // AP_MODE_ALONE (If you want the device to into ap mode if could not connect to the network.)
#endif

#define API_ENABLED true
#define API_BUFFER_SIZE 100     // Size of the buffer for HTTP GET API responses
#define API_REAL_TIME_VALUES 1  // Show filtered/median values by default (0 => median, 1 => real time)

#define WIFI_SLEEP_MODE WIFI_NONE_SLEEP  // WIFI_NONE_SLEEP, WIFI_LIGHT_SLEEP or WIFI_MODEM_SLEEP
#ifndef WIFI_SCAN_NETWORKS
#define WIFI_SCAN_NETWORKS 0  // Set it true only for Kiot red
#endif

#ifndef WIFI_MAX_NETWORKS
#define WIFI_MAX_NETWORKS 1
#endif

#define WIFI_RSSI_1M -30          // Calibrate it with your router reading the RSSI at 1m
#define WIFI_PROPAGATION_CONST 4  // This is typically something between 2.7 to 4.3 (free space is 2)

// This option builds the firmware with the web interface embedded.
// You first have to build the data.h file that holds the contents
// of the web interface by running "gulp buildfs_embed"

#ifndef HTTP_CONF_SUPPORT
#define HTTP_CONF_SUPPORT   1 // set http config by default
#endif

#if HTTP_CONF_SUPPORT
#define HTTP_CONF_RETRY_GAP 12000
// TEMPORARY
#define HTTP_FETCH_RPC_RETRY_GAP 30000
#endif

#define DEVICE_ID()  getSetting(SETTING_DEVICE_ID, "")
#define IDENTIFIER()  getSetting(SETTING_DEVICE_ID, HOSTNAME)

#define POWER_PROVIDER_NONE  0
#define POWER_PROVIDER_BL6552  1  //3 phase energy metering IC
#define POWER_PROVIDER_BL0940  2  //single phase energy metering IC
#define UPDATE_CHECK_INTERVAL  1800000//3600000 // 3 minutes, time interval to check for updates. Need to check as the GET() function is blocking the code.
#define MQTT_RECONNECT_MAX_ATTEMPTS 10 // Maximum number of attempts to reconnect to the MQTT server.


#define UPTIME_OVERFLOW 4294967295

#define LIGHT_PROVIDER_NONE 0
#define LIGHT_PROVIDER_PWM 1
#define LIGHT_PROVIDER_WS2812 2



#define DEVICE_TYPE_ENERGY_METER  1
#define DEVICE_TYPE_SWITCH        2
#define DEVICE_TYPE_SENSOR        3
#define DEVICE_TYPE_LIGHT         4



#define CMD_WIFI_RESET 0x04
#define CMD_MCU_CONF 0x02
#define CMD_STATE 0x07

#define CMD_SET 0x06
#define CMD_GET 0x07
// 55 aa 00 06 00 05 01 01 00 01   01
#define BYTE_1 0x55
#define BYTE_2 0xAA
#define BYTE_3 0x00
#define BYTE_4 CMD_SET

#define KU_TYPE_BOOL 0x01
#define KU_TYPE_VALUE 0x02
#define KU_TYPE_STRING 0x03
#define KU_TYPE_ENUM 0x04

#define KU_BUFFER_SIZE 256

//command DPIDS
#define CMD_Heartbeat           0x00
#define CMD_QueryProduct        0x01
#define CMD_QueryMode           0x02
#define CMD_WiFiStatus          0x03
#define CMD_WiFiResetCfg        0x04
#define CMD_WiFiResetSelect     0x05
#define CMD_SetDP               0x06
#define CMD_ReportDP            0x07
#define CMD_QueryDP             0x08
#define CMD_OTAInit             0x0a
#define CMD_OTATransmit         0x0b
#define CMD_LocalTime           0x1c
#define CMD_WiFiTest            0x0e

//DPIDs
#define DPID_PRODUCT_INFO_RELAYS              0x11
#define DPID_PRODUCT_INFO_DIMMING_STEPS       0x12
#define DPID_PRODUCT_INFO_SENSORS             0x13
#define DPID_PRODUCT_INFO_RELAY_FEATURES      0x14
#define DPID_PRODUCT_INFO_VERSION             0x15
#define DPID_PRODUCT_INFO_UNIQUE_ID           0x16

#define DPID_EEPROM_ADDRESS_VALUE     0x00
#define DPID_RELAY_STATUS             0x01
#define DPID_INDEX_RELAY_STATUS       0x02
#define DPID_DIMMER_STATUS            0x03
#define DPID_INDEX_DIMMER_STATUS      0x04
#define DPID_RELAY_RGB_BACKLGT        0x05
#define DPID_RELAY_INDX_FOR_VS        0x06
#define DPID_INDEX_CURTAIN_STATUS     0x09
#define DPID_FAULT_ZCD                0x0a
#define DPID_FAULT_MCU_STUCK          0x0b
#define DPID_INDEX_CHILD_LOCK         0x0c

#define DPID_LIGHT_DIMMER_FLICK_CONFIG 0x0f
#define DPID_POWER                    0x07
#define DPID_IR_TESTING               0x08
#define CUSTOM_DPIDS_START            0x20

#define SETTING_TOUCH_PANEL_BKLT    "bklt"
#define SETTING_TOUCH_PANEL_BACKLIGHT  "[255, 00, 00, 255, 255, 255, 100, 10, 100]"

#define PING_INTERVAL  20000 //ms

#define SETTING_RGBW_LIGHTS    "colour"
#define SETTING_RGBW_LEDS     "[153,0,150,0]"

#define SETTING_TOUCH_PANEL_LEDS   "led"
#define SETTING_TOUCH_PANEL_LEDS_LIGHTS_DEFAULT  "[120, 16, 255,00,00,255,255,0,0]"
// #define PING_INTERVAL  20000 //ms

#if DEBUG_ENABLE
#define ELEGANT_OTA_SUPPORT 0
#endif

#define LED_PROVIDER_NONE 0
#define LED_PROVIDER_NEOPIXEL 1
#define LED_PROVIDER_BI_COLOR 2
#define LED_PROVIDER_SINGLE_LED 3


//--------------------------------------------------------------------------------
// BUTTON
//--------------------------------------------------------------------------------
#ifndef BUTTON_DEBOUNCE_DELAY
#define BUTTON_DEBOUNCE_DELAY 35
#endif

#ifndef BUTTON_DBLCLICK_DELAY
#define BUTTON_DBLCLICK_DELAY 1000
#endif

#ifndef BUTTON_LNGCLICK_DELAY
#define BUTTON_LNGCLICK_DELAY 3000
#endif

#ifndef BUTTON_LNGLNGCLICK_DELAY
#define BUTTON_LNGLNGCLICK_DELAY 5000
#endif

#define BUTTON_EVENT_NONE 0
#define BUTTON_EVENT_PRESSED 1
#define BUTTON_EVENT_CLICK 2
#define BUTTON_EVENT_DBLCLICK 3
#define BUTTON_EVENT_LNGCLICK 4
#define BUTTON_EVENT_LNGLNGCLICK 5

#define BUTTON_MODE_NONE 0
#define BUTTON_MODE_TOGGLE 1
#define BUTTON_MODE_AP 2
#define BUTTON_MODE_RESET 3
#define BUTTON_MODE_PULSE 4
#define BUTTON_MODE_FACTORY 5
#define BUTTON_MODE_CUSTOMACT   6
#define BUTTON_MODE_INCREASE_SPEED 7
#define BUTTON_MODE_DECREASE_SPEED 8


#ifndef BUTTON_DEFAULT_MODE
#define BUTTON_DEFAULT_MODE BUTTON_MODE_TOGGLE
#endif

//--------------------------------------------------------------------------------
// RESET
//--------------------------------------------------------------------------------

#define CUSTOM_RESET_HARDWARE 1
#define CUSTOM_RESET_WEB 2
#define CUSTOM_RESET_TERMINAL 3
#define CUSTOM_RESET_MQTT 4
#define CUSTOM_RESET_RPC 5
#define CUSTOM_RESET_OTA 6
#define CUSTOM_RESET_UPGRADE 7
#define CUSTOM_RESET_FACTORY 8
#define CUSTOM_RESET_WIFI_CONFIG  9


#define EEPROM_CUSTOM_RESET    "rr"

//dimmer
#define DIMMER_PROVIDER_NONE 0
#define DIMMER_PROVIDER_ESP 1
#define DIMMER_PROVIDER_SERIAL 2
#define DIMMER_PROVIDER_SOFTSERIAL 3
#define DIMMER_PROVIDER_TOUCH_PANEL_SERIAL 4

// SWITCH MODE
#define SWITCH_MODE                 "VSO"
#define SWITCH_MANUAL_OVERRIDE_DISABLED     1
#define SWITCH_MANUAL_OVERRIDE_ENABLED      2