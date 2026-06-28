#define DEBUG_ENABLE 0

#if defined(BCT_3PHASE)
    #define HOSTNAME  "BCT100C1-3Phase"
    #define MQTT_DEVICE_AS   "three-phase"
    #define DEVICE_ID_GET_HTTP_LINK   "pmd/get3pEMId"
    #define DEVICE_ID_GET_HTTP_DATA   "&DeviceID=Test2&ctType=BCT100C1"
    #define ENTITY_TYPE "em3p"
    #define RELAY1_NUM 1
    #define RELAY1_PIN 99
    #define SERIAL_BAUDRATE   4800
    #define RELAY_PROVIDER   RELAY_PROVIDER_SOFTWARE_SERIAL
    #define SOFTWARE_SERIAL_RX   4
    #define SOFTWARE_SERIAL_TX   5
    #define DEBUG_BL6552_SOFTWARE_SERIAL 0
    #define POWER_PROVIDER POWER_PROVIDER_BL6552
    #define PHASE_COUNT 3
    #define DEVICE_TYPE DEVICE_TYPE_ENERGY_METER

    #define BUTTON1_NUM          1
    #define BUTTON1_PIN          12
    #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH
    #define BUTTON1_RELAY 1

    #define CURRENT_STANDARD_VALUE    2.19//8.00//0.8000
    #define CURRENT_K_CONSTANT        174132.0//638679.0//51372
    #define ACTIVE_POWER_STANDARD_VALUE 519.525096//1955.12
    #define ACTIVE_POWER_K_CONSTANT    102790.00//388261.00
    #define APPARENT_POWER_STANDARD_VALUE 520.5567//1955
    #define APPARENT_POWER_K_CONSTANT    103178.333//388732.00

#elif defined(BKCT_3PHASE)
    #define HOSTNAME  "BKCT100C1-3Phase"
    #define MQTT_DEVICE_AS   "three-phase"
    #define DEVICE_ID_GET_HTTP_LINK   "pmd/get3pEMId"
    #define DEVICE_ID_GET_HTTP_DATA   "&DeviceID=Test2&ctType=BKCT60C"
    #define ENTITY_TYPE "em3p"
    #define RELAY1_NUM 1
    #define RELAY1_PIN 99
    #define SERIAL_BAUDRATE   4800
    #define RELAY_PROVIDER   RELAY_PROVIDER_SOFTWARE_SERIAL
    #define SOFTWARE_SERIAL_RX   4
    #define SOFTWARE_SERIAL_TX   5
    #define DEBUG_BL6552_SOFTWARE_SERIAL 0
    #define POWER_PROVIDER POWER_PROVIDER_BL6552
    #define PHASE_COUNT  3
    #define DEVICE_TYPE DEVICE_TYPE_ENERGY_METER

    // #define DEVICE_TYPE DEVICE_TYPE_ENERGY_METER

    #define BUTTON1_NUM          1
    #define BUTTON1_PIN          12
    #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH
    #define BUTTON1_RELAY 1

    #define CURRENT_STANDARD_VALUE    0.8567 //2.19//8.00//0.8000
    #define CURRENT_K_CONSTANT        54374.33 //174132.0//638679.0//51372
    #define ACTIVE_POWER_STANDARD_VALUE 201.3627 //519.525096//1955.12
    #define ACTIVE_POWER_K_CONSTANT    31764.00 //102790.00//388261.00
    #define ACTIVE_POWER_CONSTANT      ACTIVE_POWER_STANDARD_VALUE/ACTIVE_POWER_K_CONSTANT
    #define APPARENT_POWER_STANDARD_VALUE 203.3967 //520.5567//1955
    #define APPARENT_POWER_K_CONSTANT    31843.3333 //103178.333//388732.00
    #define APPARENT_POWER_CONSTANT      APPARENT_POWER_STANDARD_VALUE/APPARENT_POWER_K_CONSTANT

    
#elif defined(YCT_3PHASE)
    #define HOSTNAME  "YCT100C1-3Phase"
    #define MQTT_DEVICE_AS   "three-phase"
    #define DEVICE_ID_GET_HTTP_LINK   "pmd/get3pEMId"
    #define DEVICE_ID_GET_HTTP_DATA   "&DeviceID=Test2&ctType=YCT100C"
    #define ENTITY_TYPE "em3p"
    #define RELAY1_NUM 1
    #define RELAY1_PIN 99
    #define SERIAL_BAUDRATE   4800
    #define RELAY_PROVIDER   RELAY_PROVIDER_SOFTWARE_SERIAL
    #define SOFTWARE_SERIAL_RX   4
    #define SOFTWARE_SERIAL_TX   5
    #define DEBUG_BL6552_SOFTWARE_SERIAL 0
    #define POWER_PROVIDER POWER_PROVIDER_BL6552
    #define PHASE_COUNT  3
    #define DEVICE_TYPE DEVICE_TYPE_ENERGY_METER

    #define BUTTON1_NUM          1
    #define BUTTON1_PIN          12
    #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH
    #define BUTTON1_RELAY 1

    #define CURRENT_STANDARD_VALUE    0.86 //2.19//8.00//0.8000
    #define CURRENT_K_CONSTANT        54374.33 //174132.0//638679.0//51372
    #define ACTIVE_POWER_STANDARD_VALUE 201.3627 //519.525096//1955.12
    #define ACTIVE_POWER_K_CONSTANT    31764.00 //102790.00//388261.00
    #define ACTIVE_POWER_CONSTANT      ACTIVE_POWER_STANDARD_VALUE/ACTIVE_POWER_K_CONSTANT
    #define APPARENT_POWER_STANDARD_VALUE 203.3967 //520.5567//1955
    #define APPARENT_POWER_K_CONSTANT    31843.3333 //103178.333//388732.00
    #define APPARENT_POWER_CONSTANT      APPARENT_POWER_STANDARD_VALUE/APPARENT_POWER_K_CONSTANT

#elif defined(ENERGY_METER_1) //single phase energy meter
    #define HOSTNAME  "BSEM-1Phase" //change device name
    #define MQTT_DEVICE_AS   "single-phase"
    #define DEVICE_ID_GET_HTTP_LINK   "bsp/getBSPId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=Test1&bspType=VIRTUAL"
    #define ENTITY_TYPE "bsp"
    #define RELAY1_NUM 1
    #define RELAY1_PIN 99
    #define SERIAL_BAUDRATE   4800
    #define RELAY_PROVIDER   RELAY_PROVIDER_SOFTWARE_SERIAL
    #define SOFTWARE_SERIAL_RX   4
    #define SOFTWARE_SERIAL_TX   5
    #define DEBUG_BL6552_SOFTWARE_SERIAL 0
    #define POWER_PROVIDER POWER_PROVIDER_BL0940
    #define DEVICE_TYPE DEVICE_TYPE_ENERGY_METER

    #define BUTTON1_NUM          1
    #define BUTTON1_PIN          12
    #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH
    #define BUTTON1_RELAY 1

#elif defined(TUNABLE_LIGHT)
    #define HOSTNAME  "BS-TUNABLE-LIGHT" //change device name
    #define MQTT_DEVICE_AS   "tunable-light"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"  //change this
    #define DEVICE_ID_GET_HTTP_DATA   "&switchType=SW1DL" //change this 
    #define ENTITY_TYPE "light"
    #define SERIAL_BAUDRATE   4800
    #define RELAY1_NUM 1
    #define RELAY1_PIN 99
    #define LIGHT_PROVIDER LIGHT_PROVIDER_PWM
    #define LIGHT_PWM_PIN1 4
    #define LIGHT_PWM_PIN2 5
    #define LIGHT_PWM_PIN3 12
    #define LIGHT_PWM_PIN4 13
    
    #define INITIAL_DELAY 0
    #define ELEGANT_OTA_SUPPORT  1

    
#elif defined(RGBW_LIGHT)
    #define HOSTNAME "RGBW-LIGHT" // change device name
    #define MQTT_DEVICE_AS "rgbw-light"
    #define DEVICE_ID_GET_HTTP_LINK "bss/getBSSId"      // change this
    #define DEVICE_ID_GET_HTTP_DATA "&switchType=SW1DL" // change this
    #define ENTITY_TYPE "light"
    #define SERIAL_BAUDRATE 4800
    #define RELAY1_NUM 1
    #define RELAY1_PIN 99
    #define LIGHT_PROVIDER LIGHT_PROVIDER_PWM
    #define RED_PIN 4
    #define GREEN_PIN 5
    #define BLUE_PIN 12
    #define WHITE_PIN 13

    #define INITIAL_DELAY 0
    #define ELEGANT_OTA_SUPPORT 1

#elif defined(TOUCH_PANEL_4)
    #define HOSTNAME  "BS-TOUCH-PANEL-4" //change device name
    #define MQTT_DEVICE_AS   "touch-panel"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=FGT4S"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_ESP

    #define RELAY1_NUM 1
    #define RELAY2_NUM 2
    #define RELAY3_NUM 3
    #define RELAY4_NUM 4
    
    #define RELAY1_PIN 16
    #define RELAY2_PIN 12
    #define RELAY3_PIN 2
    #define RELAY4_PIN 15
    
    #define BUTTON1_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON1_CLICK BUTTON_MODE_NONE
    #define BUTTON1_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON1_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON2_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON2_CLICK BUTTON_MODE_NONE
    #define BUTTON2_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON2_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON2_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON3_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON3_CLICK BUTTON_MODE_NONE
    #define BUTTON3_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON3_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON3_LNGCLICK BUTTON_MODE_RESET


    #define BUTTON4_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON4_CLICK BUTTON_MODE_NONE
    #define BUTTON4_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON4_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON4_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON5_PRESS   BUTTON_MODE_NONE
    #define BUTTON5_CLICK BUTTON_MODE_TOGGLE
    #define BUTTON5_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON5_LNGLNGCLICK BUTTON_MODE_NONE
    #define BUTTON5_LNGCLICK BUTTON_MODE_NONE

    #define BUTTON_DBLCLICK_DELAY 100
    #define BUTTON_DEBOUNCE_DELAY 30
    #define BUTTON_LNGCLICK_DELAY   5000
    #define BUTTON_LNGLNGCLICK_DELAY 9000

    #define BUTTON1_PIN 4
    #define BUTTON2_PIN 0
    #define BUTTON3_PIN 5
    #define BUTTON4_PIN 3
    #define BUTTON5_PIN 13

    #define BUTTON1_RELAY 1
    #define BUTTON2_RELAY 2
    #define BUTTON3_RELAY 3
    #define BUTTON4_RELAY 4
    #define BUTTON5_RELAY 1

    // Do not set this to pull up, theic works on different logic levels. On touching the button, the button will go high.
    #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON2_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON3_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON4_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON5_MODE BUTTON_SWITCH | BUTTON_DEFAULT_HIGH


    #define LED_PROVIDER LED_PROVIDER_NEOPIXEL
    #define USE_UART_PINS_AS_GPIO  1
    #define NUMPIXELS_COUNT  4  
    #define LED_PIN 1        // Pin where the NeoPixels are connected

    #define NEOPIXEL_LED1_INDEX 0
    #define NEOPIXEL_LED2_INDEX 1
    #define NEOPIXEL_LED3_INDEX 3
    #define NEOPIXEL_LED4_INDEX 2

    #define ELEGANT_OTA_SUPPORT  1

    // #define SERIAL_BAUDRATE   9600

#elif defined(TOUCH_PANEL_2)
    #define HOSTNAME  "BS-TOUCH-PANEL-2" //change device name
    #define MQTT_DEVICE_AS   "touch-panel"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=FGT2S"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_ESP

    #define RELAY1_NUM 1
    #define RELAY2_NUM 2
    
    #define RELAY1_PIN 2
    #define RELAY2_PIN 12
    
    #define BUTTON1_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON1_CLICK BUTTON_MODE_NONE
    #define BUTTON1_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON1_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON2_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON2_CLICK BUTTON_MODE_NONE
    #define BUTTON2_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON2_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON2_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON3_PRESS   BUTTON_MODE_NONE
    #define BUTTON3_CLICK BUTTON_MODE_TOGGLE
    #define BUTTON3_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON3_LNGLNGCLICK BUTTON_MODE_NONE
    #define BUTTON3_LNGCLICK BUTTON_MODE_NONE

    #define BUTTON_DBLCLICK_DELAY 100
    #define BUTTON_DEBOUNCE_DELAY 30
    #define BUTTON_LNGCLICK_DELAY   5000
    #define BUTTON_LNGLNGCLICK_DELAY 9000

    #define BUTTON1_PIN 0
    #define BUTTON2_PIN 3
    #define BUTTON3_PIN 13

    #define BUTTON1_RELAY 1
    #define BUTTON2_RELAY 2
    #define BUTTON3_RELAY 1

    // Do not set this to pull up, theic works on different logic levels. On touching the button, the button will go high.
    #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON2_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON3_MODE BUTTON_SWITCH | BUTTON_DEFAULT_HIGH


    #define LED_PROVIDER LED_PROVIDER_NEOPIXEL
    #define USE_UART_PINS_AS_GPIO  1
    #define NUMPIXELS_COUNT   2
    #define LED_PIN 1        // Pin where the NeoPixels are connected
    

    #define NEOPIXEL_LED1_INDEX 0
    #define NEOPIXEL_LED2_INDEX 1

    #define ELEGANT_OTA_SUPPORT  1

    // #define SERIAL_BAUDRATE   9600


#elif defined(TOUCH_PANEL_4P1)
    #define HOSTNAME  "BS-TOUCH-PANEL-4P1" //change device name
    #define MQTT_DEVICE_AS   "touch-panel"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=FGT4S1F"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH
    #define RELAY_PROVIDER  RELAY_PROVIDER_TOUCH_PANEL_SERIAL
    #define RELAY1_NUM 1
    #define RELAY1_PIN 99
    #define RELAY2_NUM 2
    #define RELAY2_PIN 99
    #define RELAY3_NUM 3
    #define RELAY3_PIN 99
    #define RELAY4_NUM 4
    #define RELAY4_PIN 99

    #define RELAY5_NUM 5
    #define DIMMER_NUM 5
    #define TOUCH_PANEL_SERIAL_COMMANDS 1
    #define SERIAL_BAUDRATE   9600

#elif defined(TOUCH_PANEL_10)
    #define HOSTNAME  "BS-TOUCH-PANEL-10S" //change device name
    #define MQTT_DEVICE_AS   "touch-panel"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=FGT10S"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH
    #define ENTITY_TYPE "switch"
    #define RELAY_PROVIDER  RELAY_PROVIDER_TOUCH_PANEL_SERIAL
    #define RELAY1_NUM 1
    #define RELAY1_PIN 99
    #define RELAY2_NUM 2
    #define RELAY2_PIN 99
    #define RELAY3_NUM 3
    #define RELAY3_PIN 99
    #define RELAY4_NUM 4
    #define RELAY4_PIN 99
    #define RELAY5_NUM 5
    #define RELAY5_PIN 99
    #define RELAY6_NUM 6
    #define RELAY6_PIN 99
    #define RELAY7_NUM 7
    #define RELAY7_PIN 99
    #define RELAY8_NUM 8
    #define RELAY8_PIN 99
    #define RELAY9_NUM 9
    #define RELAY9_PIN 99
    #define RELAY10_NUM 10
    #define RELAY10_PIN 99

    #define TOUCH_PANEL_SERIAL_COMMANDS 1
    #define SERIAL_BAUDRATE   9600

#elif defined(NODE_SWITCH_1)
    #define HOSTNAME  "BS-NODE-SWITCH-1" //change device name
    #define MQTT_DEVICE_AS   "node-switch"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=RTHD"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_ESP

    #define RELAY1_NUM 1

    #define RELAY1_PIN 13

    #define BUTTON1_PIN 14
    
    #define BUTTON1_RELAY 1
    
    #define BUTTON1_PRESS   BUTTON_MODE_NONE
    #define BUTTON1_CLICK BUTTON_MODE_TOGGLE
    #define BUTTON1_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGLNGCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGCLICK BUTTON_MODE_NONE

    #define BUTTON1_MODE BUTTON_SWITCH | BUTTON_DEFAULT_HIGH
    // #define BUTTON2_MODE BUTTON_SWITCH | BUTTON_DEFAULT_HIGH

    // #define TOUCH_PANEL_SERIAL_COMMANDS 1

    #define ELEGANT_OTA_SUPPORT  1

    // #define SERIAL_BAUDRATE   115200

#elif defined(NODE_SWITCH_1P1)
    #define HOSTNAME  "BS-NODE-SWITCH-1P1" //change device name
    #define MQTT_DEVICE_AS   "node-switch"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=RT1S"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_ESP

    #define RELAY1_NUM 1
    #define RELAY2_NUM 2

    #define RELAY1_PIN 5
    #define RELAY2_PIN 4

    #define BUTTON1_PIN 12
    #define BUTTON2_PIN 13
    
    #define BUTTON1_RELAY 1
    #define BUTTON2_RELAY 2
    
    #define BUTTON1_PRESS   BUTTON_MODE_NONE
    #define BUTTON1_CLICK BUTTON_MODE_TOGGLE
    #define BUTTON1_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGLNGCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGCLICK BUTTON_MODE_NONE

    #define BUTTON2_PRESS   BUTTON_MODE_NONE
    #define BUTTON2_CLICK BUTTON_MODE_TOGGLE
    #define BUTTON2_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON2_LNGLNGCLICK BUTTON_MODE_NONE
    #define BUTTON2_LNGCLICK BUTTON_MODE_NONE

    #define BUTTON1_MODE BUTTON_SWITCH | BUTTON_DEFAULT_HIGH
    #define BUTTON2_MODE BUTTON_SWITCH | BUTTON_DEFAULT_HIGH

    // #define TOUCH_PANEL_SERIAL_COMMANDS 1

    #define ELEGANT_OTA_SUPPORT  1

    // #define SERIAL_BAUDRATE   115200

#elif defined(NODE_SWITCH_2)
    #define HOSTNAME  "BS-NODE-SWITCH-2" //change device name
    #define MQTT_DEVICE_AS   "node-switch"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=RT2S"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_ESP

    #define RELAY1_NUM 1
    #define RELAY2_NUM 2

    #define RELAY1_PIN 5
    #define RELAY2_PIN 13

    #define DIMMER_NUM 2

    #define BUTTON1_PIN 12
    #define BUTTON2_PIN 14
    
    #define BUTTON1_RELAY 1
    #define BUTTON2_RELAY 2
    
    #define BUTTON1_PRESS   BUTTON_MODE_NONE
    #define BUTTON1_CLICK BUTTON_MODE_TOGGLE
    #define BUTTON1_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGLNGCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGCLICK BUTTON_MODE_NONE

    #define BUTTON2_PRESS   BUTTON_MODE_NONE
    #define BUTTON2_CLICK BUTTON_MODE_TOGGLE
    #define BUTTON2_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON2_LNGLNGCLICK BUTTON_MODE_NONE
    #define BUTTON2_LNGCLICK BUTTON_MODE_NONE

    #define BUTTON1_MODE BUTTON_SWITCH | BUTTON_DEFAULT_HIGH
    #define BUTTON2_MODE BUTTON_SWITCH | BUTTON_DEFAULT_HIGH

    // #define TOUCH_PANEL_SERIAL_COMMANDS 1

    #define ELEGANT_OTA_SUPPORT  1

    // #define SERIAL_BAUDRATE   115200

#elif defined(NODE_SWITCH_2P1)
    #define HOSTNAME  "BS-NODE-SWITCH-2P1" //change device name
    #define MQTT_DEVICE_AS   "node-switch"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=RT2S1D"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_SERIAL

    #define RELAY1_NUM 1
    #define RELAY2_NUM 2

    #define RELAY3_NUM 3
    #define DIMMER_NUM 3
    
    // #define TOUCH_PANEL_SERIAL_COMMANDS 1

    #define ELEGANT_OTA_SUPPORT  1

    #define SERIAL_BAUDRATE   115200

    #elif defined(NODE_SWITCH_4P1)
    #define HOSTNAME  "BS-NODE-SWITCH-4P1" //change device name
    #define MQTT_DEVICE_AS   "node-switch"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=RT4S1D"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_SERIAL

    #define RELAY1_NUM 1
    #define RELAY2_NUM 2

    #define RELAY3_NUM 3
    #define RELAY4_NUM 4
    #define RELAY5_NUM 5
    #define DIMMER_NUM 5
    
    // #define TOUCH_PANEL_SERIAL_COMMANDS 1

    #define ELEGANT_OTA_SUPPORT  1

    #define SERIAL_BAUDRATE   115200

    #elif defined(NODE_SWITCH_4)
    #define HOSTNAME  "BS-NODE-SWITCH-4" //change device name
    #define MQTT_DEVICE_AS   "node-switch"
    #define DEVICE_ID_GET_HTTP_LINK   "bss/getBSSId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=TEST&switchType=RT4S"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_SERIAL

    #define RELAY1_NUM 1
    #define RELAY2_NUM 2

    #define RELAY3_NUM 3
    #define RELAY4_NUM 4
   
    // #define TOUCH_PANEL_SERIAL_COMMANDS 1

    #define ELEGANT_OTA_SUPPORT  1

    #define SERIAL_BAUDRATE   115200

#elif defined(MODULAR_TOUCH_4)
    #define HOSTNAME  "BS-MODULAR-TOUCH-4" //change device name
    #define MQTT_DEVICE_AS   "modular-touch"
    #define DEVICE_ID_GET_HTTP_LINK   "bsp/getBSPId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=Test1&bspType=VIRTUAL"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_ESP

    #define RELAY1_NUM 1
    #define RELAY2_NUM 2
    #define RELAY3_NUM 3
    #define RELAY4_NUM 4
    
    #define RELAY1_PIN 12
    #define RELAY2_PIN 13
    #define RELAY3_PIN 14
    #define RELAY4_PIN 15
    
    #define BUTTON1_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON1_CLICK BUTTON_MODE_NONE
    #define BUTTON1_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON1_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON2_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON2_CLICK BUTTON_MODE_NONE
    #define BUTTON2_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON2_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON2_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON3_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON3_CLICK BUTTON_MODE_NONE
    #define BUTTON3_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON3_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON3_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON4_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON4_CLICK BUTTON_MODE_NONE
    #define BUTTON4_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON4_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON4_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON_DBLCLICK_DELAY 100
    #define BUTTON_DEBOUNCE_DELAY 30
    #define BUTTON_LNGCLICK_DELAY   5000
    #define BUTTON_LNGLNGCLICK_DELAY 9000

    #define BUTTON1_PIN 4
    #define BUTTON2_PIN 5
    #define BUTTON3_PIN 0
    #define BUTTON4_PIN 2

    #define BUTTON1_RELAY 1
    #define BUTTON2_RELAY 2
    #define BUTTON3_RELAY 3
    #define BUTTON4_RELAY 4

    // Do not set this to pull up, theic works on different logic levels. On touching the button, the button will go high.
    #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON2_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON3_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON4_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP

    #define LED_PROVIDER LED_PROVIDER_NEOPIXEL
    #define USE_UART_PINS_AS_GPIO  1
    #define NUMPIXELS_COUNT   4
    #define LED_PIN 1        // Pin where the NeoPixels are connected

    #define NEOPIXEL_LED1_INDEX 1
    #define NEOPIXEL_LED2_INDEX 0
    #define NEOPIXEL_LED3_INDEX 2
    #define NEOPIXEL_LED4_INDEX 3

    #define ELEGANT_OTA_SUPPORT  1

    // #define SERIAL_BAUDRATE   9600

#elif defined(MODULAR_TOUCH_2)
    #define HOSTNAME  "BS-MODULAR-TOUCH-2" //change device name
       #define MQTT_DEVICE_AS   "modular-touch"
       #define DEVICE_ID_GET_HTTP_LINK   "bsp/getBSPId"
       #define DEVICE_ID_GET_HTTP_DATA   "&plugId=Test1&bspType=VIRTUAL"
       #define ENTITY_TYPE "switch"
       #define DEVICE_TYPE DEVICE_TYPE_SWITCH
   
       #define RELAY_PROVIDER  RELAY_PROVIDER_ESP
   
       #define RELAY1_NUM 1
       #define RELAY2_NUM 2
   
       
       #define RELAY1_PIN 13
       #define RELAY2_PIN 15
       
       #define BUTTON1_PRESS   BUTTON_MODE_TOGGLE
       #define BUTTON1_CLICK BUTTON_MODE_NONE
       #define BUTTON1_DBLCLICK BUTTON_MODE_NONE
       #define BUTTON1_LNGLNGCLICK BUTTON_MODE_FACTORY
       #define BUTTON1_LNGCLICK BUTTON_MODE_RESET
   
       #define BUTTON2_PRESS   BUTTON_MODE_TOGGLE
       #define BUTTON2_CLICK BUTTON_MODE_NONE
       #define BUTTON2_DBLCLICK BUTTON_MODE_NONE
       #define BUTTON2_LNGLNGCLICK BUTTON_MODE_FACTORY
       #define BUTTON2_LNGCLICK BUTTON_MODE_RESET
   
       #define BUTTON_DBLCLICK_DELAY 100
       #define BUTTON_DEBOUNCE_DELAY 30
       #define BUTTON_LNGCLICK_DELAY   5000
       #define BUTTON_LNGLNGCLICK_DELAY 9000
   
       #define BUTTON1_PIN 4
       #define BUTTON2_PIN 0
   
   
       #define BUTTON1_RELAY 1
       #define BUTTON2_RELAY 2
   
   
       // Do not set this to pull up, theic works on different logic levels. On touching the button, the button will go high.
       #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
       #define BUTTON2_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
   
       #define LED_PROVIDER LED_PROVIDER_NEOPIXEL
       // #define USE_UART_PINS_AS_GPIO  1
       #define NUMPIXELS_COUNT   2
       #define LED_PIN 5        // Pin where the NeoPixels are connected
   
       #define NEOPIXEL_LED1_INDEX 0
       #define NEOPIXEL_LED2_INDEX 1
   
       #define ELEGANT_OTA_SUPPORT  1
   
       // #define SERIAL_BAUDRATE   9600
   
#elif defined(MODULAR_TOUCH_1)
    #define HOSTNAME  "BS-MODULAR-TOUCH-1" //change device name
    #define MQTT_DEVICE_AS   "modular-touch"
    #define DEVICE_ID_GET_HTTP_LINK   "bsp/getBSPId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=Test1&bspType=VIRTUAL"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_ESP

    #define RELAY1_NUM 1
    
    #define RELAY1_PIN 14
    
    #define BUTTON1_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON1_CLICK BUTTON_MODE_NONE
    #define BUTTON1_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON1_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON_DBLCLICK_DELAY 100
    #define BUTTON_DEBOUNCE_DELAY 30
    #define BUTTON_LNGCLICK_DELAY   5000
    #define BUTTON_LNGLNGCLICK_DELAY 9000

    #define BUTTON1_PIN 4

    #define BUTTON1_RELAY 1

    // Do not set this to pull up, theic works on different logic levels. On touching the button, the button will go high.
    #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP

    #define LED_PROVIDER LED_PROVIDER_NEOPIXEL
    // #define USE_UART_PINS_AS_GPIO  1
    #define NUMPIXELS_COUNT   1
    #define LED_PIN 5        // Pin where the NeoPixels are connected

    #define NEOPIXEL_LED1_INDEX 0

    #define ELEGANT_OTA_SUPPORT  1

    // #define SERIAL_BAUDRATE   9600

#elif defined(MODULAR_TOUCH_FAN)
    #define HOSTNAME  "BS-MODULAR-TOUCH-FAN" //change device name
    #define MQTT_DEVICE_AS   "modular-touch"
    #define DEVICE_ID_GET_HTTP_LINK   "bsp/getBSPId"
    #define DEVICE_ID_GET_HTTP_DATA   "&plugId=Test1&bspType=VIRTUAL"
    #define ENTITY_TYPE "switch"
    #define DEVICE_TYPE DEVICE_TYPE_SWITCH

    #define RELAY_PROVIDER  RELAY_PROVIDER_ESP
    #define DIMMER_PROVIDER  DIMMER_PROVIDER_ESP

    #define RELAY1_NUM 1
    #define DIMMER_NUM 1

    #define DIMMER_PIN1 15 // 15 1.1
    #define DIMMER_PIN2 12 // 12 2.2
    #define DIMMER_PIN3 13 // 13 3.3
    #define DIMMER_PIN4 14 // 14full
    
    #define BUTTON1_PRESS   BUTTON_MODE_TOGGLE
    #define BUTTON1_CLICK BUTTON_MODE_NONE
    #define BUTTON1_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON1_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON1_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON2_PRESS   BUTTON_MODE_DECREASE_SPEED
    #define BUTTON2_CLICK BUTTON_MODE_NONE
    #define BUTTON2_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON2_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON2_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON3_PRESS   BUTTON_MODE_INCREASE_SPEED
    #define BUTTON3_CLICK BUTTON_MODE_NONE
    #define BUTTON3_DBLCLICK BUTTON_MODE_NONE
    #define BUTTON3_LNGLNGCLICK BUTTON_MODE_FACTORY
    #define BUTTON3_LNGCLICK BUTTON_MODE_RESET

    #define BUTTON_DBLCLICK_DELAY 100
    #define BUTTON_DEBOUNCE_DELAY 30
    #define BUTTON_LNGCLICK_DELAY   5000
    #define BUTTON_LNGLNGCLICK_DELAY 9000

    #define BUTTON1_PIN 2
    #define BUTTON2_PIN 0
    #define BUTTON3_PIN 4

    #define BUTTON1_RELAY 1

    // Do not set this to pull up, theic works on different logic levels. On touching the button, the button will go high.
    #define BUTTON1_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON2_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    #define BUTTON3_MODE BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP

    #define LED_PROVIDER LED_PROVIDER_NEOPIXEL
    // #define USE_UART_PINS_AS_GPIO  1
    #define NUMPIXELS_COUNT   8
    #define LED_PIN 5        // Pin where the NeoPixels are connected

    #define NEOPIXEL_LED1_INDEX 0

    #define ELEGANT_OTA_SUPPORT  1

    // #define SERIAL_BAUDRATE   9600



#else
#error "Wrong device"
#endif
