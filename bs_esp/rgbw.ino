#include "all.h"
#include "animations.h"
#if defined(RGBW_LIGHT)
uint8_t brightness = 255;
// Max PWM value
const uint16_t PWM_RES = 255;
volatile uint16_t pwmTick = 0;
volatile uint8_t redDuty = 0;
volatile uint8_t greenDuty = 0;
volatile uint8_t blueDuty = 0;
volatile uint8_t whiteDuty = 0;

uint8_t writeDimmValue = 0;
uint8_t dimmValue = 0;
bool disableTransition = false;

uint8_t colorComponents[4] = {0};
uint8_t currentColor[4] = {0};   // currently displayed
uint8_t targetColor[4]  = {0};   // MQTT requested color
////////////////////////////////////////////////////////////
// 
// String animationModel = "";




// const uint32_t HOLD_TIME = 3000;
// const uint32_t FADE_TIME = 2000;

////////////////////////////////////////////////////////////

// static uint8_t currentPaletteIndex = 0;
// static uint32_t colorStartTime = 0;
// static bool transitionPhase = false;

void IRAM_ATTR pwmCycle() {
    pwmTick = (pwmTick + 1) % (PWM_RES + 1);
  
    digitalWrite(RED_PIN, redDuty > pwmTick ? LOW : HIGH);
    digitalWrite(GREEN_PIN, greenDuty > pwmTick ? LOW : HIGH);
    digitalWrite(BLUE_PIN, blueDuty > pwmTick ? LOW : HIGH);
    digitalWrite(WHITE_PIN, whiteDuty > pwmTick ? LOW : HIGH);
}


uint8_t getBrightness(){
    return brightness;
}


bool getLightState(){
    return brightness > 0;
}

void setLightState(bool state){
    if(state){
        dimmValue = brightness;
    }else{
        dimmValue = 0;
    }
    disableTransition = true;
}

void setBrightness(uint8_t val){
    brightness = val;
    dimmValue = brightness;

    DEBUG_PRINT("Setting brightness to: %d\n", brightness);
    setSetting(SETTING_LIGHT_BRIGHTNESS, String(brightness));
    saveSettings(false);
    setColours(); 
}



void getLightString(JsonDocument& doc){
    // doc["entityId"] = DEVICE_ID();
    doc["brightness"] = String(getBrightness());

    #if LIGHT_HAS_COLOUR
    doc["colour"] = String(getColour());
    #endif

}

// const uint8_t northernLights[][3] =
// {
//     {127,255,0},
//     {0,200,83},
//     {0,191,165},
//     {0,229,255},
//     {41,121,255},
//     {170,0,255}
// };




void lightMqttCallback(uint8_t event, const char* topic, const char* payload, unsigned int length){
    // if(strcmp(topic, MQTT_TOPIC_RELAY)!=0) return;
    DEBUG_PRINT("Light mqtt callback & event: %d\n", event);
    if(event==MQTT_EVENT_CONNECT){
        DEBUG_PRINT("Relay mqtt callback connect\n");
        #if defined(ENERGY_METER_1)
        mqtt_subscribe(MQTT_TOPIC_RELAY_PLUG);

        #elif POWER_PROVIDER == POWER_PROVIDER_BL6552
        
        char topic[50];
        String deviceid = getSetting("device_id", "");
        sprintf(topic, "%s/%s", MQTT_TOPIC_RELAY, deviceid.c_str());
        
        mqtt_subscribe(MQTT_TOPIC_RELAY);
        #else
        char topic[50];
        String deviceid = getSetting("device_id", "");
        snprintf(topic, sizeof(topic), "%s/%s/light/command", MQTT_DEVICE_AS, deviceid.c_str());
        mqtt_subscribe(topic);
        #endif

        return;
    }

    if(event==MQTT_EVENT_MESSAGE){
        DEBUG_PRINT("Relay mqtt callback message\n");
        char topic[50];
        String deviceid = getSetting("device_id", "");
        snprintf(topic, sizeof(topic), "%s/%s/light/command", MQTT_DEVICE_AS, deviceid.c_str());
        if(strcmp(topic, topic)!=0) return;
    }


    StaticJsonDocument<200> root;
    DeserializationError error = deserializeJson(root, payload);
    if (error){
        DEBUG_PRINT("deserializeJson() failed: %s\n", error.f_str());
        return;
    }

    if(root.containsKey("entityId")){
        String deviceid = root["entityId"];
        if(strcmp(deviceid.c_str(), DEVICE_ID().c_str())!=0) return;
    }

    const char* type = root["type"];
   
    if(strcmp(type, "brightness")==0){
        setBrightness(root["value"].as<uint8_t>());
    }
    if(strcmp(type, "colour")==0){
        // JsonArray multi_light = root["colour"];
        JsonArray multi_light = root["colour"].as<JsonArray>();
        String output;
        serializeJson(multi_light, output);
        DEBUG_PRINT("multi_light: %s\n", output.c_str());

        // animationRunning = false;
        stopAnimation();    

        setSetting("last_mode", "manual");
        saveSettings(false);

        setSetting(SETTING_RGBW_LIGHTS, output);
        saveSettings(false);

        targetColor[0] = multi_light[0];
        targetColor[1] = multi_light[1];
        targetColor[2] = multi_light[2];
        targetColor[3] = multi_light[3];
        
    }
    
   if(strcmp(type, "animations")==0)
{
    String model =
        root["model"].as<String>();

    startAnimation(model);

    setSetting("last_mode", "animation");
    setSetting("anim_model", model);

    saveSettings(false);
    }
}
void setColours(){
    
    // float scale = writeDimmValue / 255.0;  // or dimmValue depending on whether you're using transition
    // redDuty   = colorComponents[0] * scale;
    // greenDuty = colorComponents[1] * scale;
    // blueDuty  = colorComponents[2] * scale;
    // whiteDuty = colorComponents[3] * scale;
    float scale = writeDimmValue / 255.0;

    redDuty   = currentColor[0] * scale;
    greenDuty = currentColor[1] * scale;
    blueDuty  = currentColor[2] * scale;
    whiteDuty = currentColor[3] * scale;

    DEBUG_PRINT("Color components - R: %d, G: %d, B: %d, W: %d\n", redDuty, greenDuty, blueDuty, whiteDuty);
   
   
}
void RetrieveLastState(){
    String default_settings = getSetting(SETTING_RGBW_LIGHTS, SETTING_RGBW_LEDS);
    DEBUG_PRINT("Raw RGBW setting string: %s\n", default_settings.c_str());

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, default_settings);
    if (err) {
        DEBUG_PRINT("Deserialization failed: %s\n", err.f_str());
        return;
    }

    JsonArray led_array = doc.as<JsonArray>();
    if (led_array.size() != 4) {
        DEBUG_PRINT("Invalid color array size\n");
        return;
    }

    currentColor[0] = led_array[0];
    currentColor[1] = led_array[1];
    currentColor[2] = led_array[2];
    currentColor[3] = led_array[3];

    targetColor[0] = currentColor[0];
    targetColor[1] = currentColor[1];
    targetColor[2] = currentColor[2];
    targetColor[3] = currentColor[3];

    DEBUG_PRINT("Retrieved color components - R: %d, G: %d, B: %d, W: %d\n", 
                colorComponents[0], colorComponents[1], colorComponents[2], colorComponents[3]);

    writeDimmValue = brightness;
    disableTransition = true;
    setColours();
}

 

void lightSetup() {
    brightness = getSetting(SETTING_LIGHT_BRIGHTNESS, "100").toInt();
    dimmValue = brightness;
    initAnimation();

    String mode =
        getSetting("last_mode", "manual");

    if(mode == "animation")
    {
        String model =
            getSetting(
                "anim_model",
                "northernlights"
            );

        startAnimation(model);

        DEBUG_PRINT(
            "Restored animation: %s\n",
            model.c_str()
        );
    }
    else
    {
        RetrieveLastState();
    }
    mqttRegisterCallback(lightMqttCallback);

    timer1_isr_init();
    timer1_attachInterrupt(pwmCycle);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
    int Loop_time=50000/500;
    timer1_write(250);

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(WHITE_PIN, OUTPUT);

    // RetrieveLastState();

    // mqttRegisterCallback(lightMqttCallback);
    loopRegister(lightLoop);

}

void lightLoop() {

    runAnimation();
    lightTransitionLoop();

    
}

// void lightTransitionLoop(){
//     static int transitionLast = 0;

//     if(disableTransition){
//         disableTransition = false;
//         writeDimmValue = dimmValue;
//         return;
//     }

//     if(millis() - transitionLast < 25) return;
//     transitionLast = millis();

//     bool changed = false;

//     // Brightness transition
//     if(writeDimmValue < dimmValue)
//     {
//         writeDimmValue++;
//         changed = true;
//     }
//     else if(writeDimmValue > dimmValue)
//     {
//         writeDimmValue--;
//         changed = true;
//     }

//     // Colour transition
//     for(int i=0;i<4;i++)
//     {
//         if(currentColor[i] < targetColor[i])
//         {
//             currentColor[i]++;
//             changed = true;
//         }
//         else if(currentColor[i] > targetColor[i])
//         {
//             currentColor[i]--;
//             changed = true;
//         }
//     }

//     if(changed)
//     {
//         setColours();
//     }

// }

    void lightTransitionLoop()
    {
        static uint32_t transitionLast = 0;

        if(disableTransition)
        {
            disableTransition = false;
            writeDimmValue = dimmValue;
            return;
        }

        if(millis() - transitionLast < 10)
            return;

        transitionLast = millis();

        bool changed = false;

        if(writeDimmValue < dimmValue)
        {
            writeDimmValue++;
            changed = true;
        }
        else if(writeDimmValue > dimmValue)
        {
            writeDimmValue--;
            changed = true;
        }

        for(int i = 0; i < 4; i++)
        {
            if(currentColor[i] < targetColor[i])
            {
                currentColor[i] =
                min(
                    (int)currentColor[i] + 2,
                    (int)targetColor[i]
                );
                changed = true;
            }
            else if(currentColor[i] > targetColor[i])
            {
                currentColor[i] =
                max(
                    (int)currentColor[i] - 2,
                    (int)targetColor[i]
                );
                changed = true;
            }
        }

        if(changed)
            setColours();
    }

#endif