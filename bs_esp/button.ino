/* 
Usage of special macros

#define USE_SWITCH_FOR_RESET    1
#define BUTTON_DBL_CLICK_COUNT  7

 */
// -----------------------------------------------------------------------------
// BUTTON
// -----------------------------------------------------------------------------

#include <DebounceEvent.h>
#include <vector>
#include "all.h"

typedef struct {
    DebounceEvent* button;
    unsigned long actions;
    unsigned int relayID;
} button_t;

std::vector<button_t> _buttons;
unsigned long btnDelay;
unsigned long btnDebDel;
bool manual_override = false;

#ifdef MQTT_TOPIC_BUTTON
void buttonMQTT(unsigned char id, uint8_t event) {
    if (id >= _buttons.size()) return;
    char payload[2];
    sprintf(payload, "%d", event);
    mqttSend(MQTT_TOPIC_BUTTON, id, payload);
}
#endif

int buttonFromRelay(unsigned int relayID) {
    for (unsigned int i = 0; i < _buttons.size(); i++) {
        if (_buttons[i].relayID == relayID) return i;
    }
    return -1;
}

bool buttonState(unsigned char id) {
    if (id >= _buttons.size()) return false;
    return _buttons[id].button->pressed();
}

unsigned char buttonAction(unsigned char id, unsigned char event) {
    if (id >= _buttons.size()) return BUTTON_MODE_NONE;
    unsigned long actions = _buttons[id].actions;
    if (event == BUTTON_EVENT_PRESSED) return (actions)&0x0F;
    if (event == BUTTON_EVENT_CLICK) return (actions >> 4) & 0x0F;
    if (event == BUTTON_EVENT_DBLCLICK) return (actions >> 8) & 0x0F;
    if (event == BUTTON_EVENT_LNGCLICK) return (actions >> 12) & 0x0F;
    if (event == BUTTON_EVENT_LNGLNGCLICK) return (actions >> 16) & 0x0F;
    return BUTTON_MODE_NONE;
}

unsigned long buttonStore(unsigned char pressed, unsigned char click, unsigned char dblclick, unsigned char lngclick, unsigned char lnglngclick) {
    unsigned int value;
    value = pressed;
    value += click << 4;
    value += dblclick << 8;
    value += lngclick << 12;
    value += lnglngclick << 16;
    return value;
}

uint8_t mapEvent(uint8_t event, uint8_t count, uint16_t length) {
    // DEBUG_PRINT(PSTR("%d %d %d\n"), BUTTON_DBL_CLICK_COUNT, count, event);
    if (event == EVENT_PRESSED) return BUTTON_EVENT_PRESSED;
#if USE_SWITCH_FOR_RESET
    if (event == EVENT_CHANGED) return BUTTON_EVENT_CLICK;
#else
    if (event == EVENT_CHANGED) return BUTTON_EVENT_CLICK;
#endif
    if (event == EVENT_RELEASED) {
        if (count == 1) {
            if (length > BUTTON_LNGLNGCLICK_DELAY) return BUTTON_EVENT_LNGLNGCLICK;
            if (length > BUTTON_LNGCLICK_DELAY) return BUTTON_EVENT_LNGCLICK;
            return BUTTON_EVENT_CLICK;
        }
        if (count == BUTTON_DBL_CLICK_COUNT) return BUTTON_EVENT_DBLCLICK;
    } else if (event == EVENT_SWITCH_FINISHED) {
        if (count == BUTTON_DBL_CLICK_COUNT) return BUTTON_EVENT_DBLCLICK;
#if BUTTON_ACTION_MULTIPLE
        if (count == BUTTON_TRIPLE_CLICK_COUNT) return BUTTON_EVENT_LNGLNGCLICK;
#endif
    }
    return BUTTON_EVENT_NONE;
}

void buttonEvent(unsigned int id, unsigned char event) {
    DEBUG_PRINT(PSTR("[BUTTON] Pressed #%d, event: %d\n"), id, event);
    if (event == 0) return;

    unsigned char action = buttonAction(id, event);
    DEBUG_PRINT(PSTR("[BUTTON] Button action %d \n"), action);

    if (action == BUTTON_MODE_TOGGLE) {      
        if (_buttons[id].relayID > 0) {
            // relayToggle(_buttons[id].relayID - 1);
            bool _toggle = false;
            button_t * btn = &_buttons[id];
            if(manual_override && btn->button->getButtonMode() == BUTTON_SWITCH){
                // If the button is not in default position means it is in on position so Turn on relay. Otherwise turn off. 
                if(_buttons[id].relayID, btn->button->getDefaultStatus() != digitalRead(btn->button->getButtonPin())) {
                    // If relay is off - Toggle it
                    if(!relayStatus(btn->relayID)){
                        _toggle = true;
                    }
                }else{
                    // If relay is on - Toggle it
                    if(relayStatus(btn->relayID)){
                        _toggle = true;
                    }
                }
            }else{
                _toggle = true;
            }
            if(_toggle){
                    relayToggle(_buttons[id].relayID);
                // #if MQTT_REPORT_EVENT_TO_HOME
                //     activeHomePong(true);
                // #endif
            }
        }
    }
    
    if (action == BUTTON_MODE_AP) {;}//createLongAP();
    if (action == BUTTON_MODE_RESET) {
        deferredReset(100, CUSTOM_RESET_HARDWARE);
    }
    // if (action == BUTTON_MODE_PULSE) relayPulseToggle();
    if (action == BUTTON_MODE_FACTORY) {
        DEBUG_PRINT(PSTR("\n\nFACTORY RESET\n\n"));
        settingFactoryReset();
    }
    if(action == BUTTON_MODE_CUSTOMACT){
        DEBUG_PRINT("CUSTOM ACTION \n");
        buttonCustomAction(id);
    }
    if(action == BUTTON_MODE_INCREASE_SPEED){increaseDimmingValue();}
    if(action == BUTTON_MODE_DECREASE_SPEED){decreaseDimmingValue();}
}

void buttonCustomAction(uint8_t button_id){
    // TODO: later introduce something like registering callbacks and calling those for the given button. 
    // Temporarily 
    disbaleTxAsGpioAndInitSerial();
}

// IMPORTANT: This function should not be called in production after device has been mounted to the device.
void disbaleTxAsGpioAndInitSerial(){
    // Important - Need to clear buttons
    // otherwise button event will be fired for the button attached totx/rx pin.
    _buttons.clear();

    pinMode(3, FUNCTION_0);
    pinMode(1, FUNCTION_0);
    // serialDisabled(false);
    // Serial.begin(SERIAL_BAUDRATE);
    // Temporary - DEBUG
    
}

void buttonSetup() {
    buttonConfigure();
    loopRegister(buttonLoop);
    // afterConfigParseRegister(buttonConfigure);
}

void buttonLoop() {
    for (unsigned int i = 0; i < _buttons.size(); i++) {
        if (unsigned char event = _buttons[i].button->loop()) {
            unsigned char count = _buttons[i].button->getEventCount();
            unsigned long length = _buttons[i].button->getEventLength();
            unsigned char mapped = mapEvent(event, count, length);
            // DEBUG_PRINT(PSTR("Mapped Event %d \n"), mapped);
            buttonEvent(i, mapped);
        }
    }

}


void buttonConfigure(){
    btnDelay = getSetting("btnDelay", BUTTON_DBLCLICK_DELAY).toInt();
    btnDebDel = getSetting("btnDebDelay", BUTTON_DEBOUNCE_DELAY).toInt();
    // setManualOverride(true);
    // BUTTON DELAY SHOULD BE ATLEST TWICE OF Btn Debounce Delay.
    if (btnDebDel > btnDelay / 2) {
        btnDelay = 2 * btnDebDel;
    }
    if (_buttons.size() > 0) {
        _buttons.clear();
    }

#ifdef BUTTON1_PIN
    {
        unsigned int actions = buttonStore(BUTTON1_PRESS, BUTTON1_CLICK, BUTTON1_DBLCLICK, BUTTON1_LNGCLICK, BUTTON1_LNGLNGCLICK);
        _buttons.push_back({new DebounceEvent(BUTTON1_PIN, BUTTON1_MODE, btnDebDel, btnDelay), actions, BUTTON1_RELAY});
    }
#endif
#ifdef BUTTON2_PIN
    {
        unsigned int actions = buttonStore(BUTTON2_PRESS, BUTTON2_CLICK, BUTTON2_DBLCLICK, BUTTON2_LNGCLICK, BUTTON2_LNGLNGCLICK);
        _buttons.push_back({new DebounceEvent(BUTTON2_PIN, BUTTON2_MODE, btnDebDel, btnDelay), actions, BUTTON2_RELAY});
    }
#endif
#ifdef BUTTON3_PIN
    {
        unsigned int actions = buttonStore(BUTTON3_PRESS, BUTTON3_CLICK, BUTTON3_DBLCLICK, BUTTON3_LNGCLICK, BUTTON3_LNGLNGCLICK);
        _buttons.push_back({new DebounceEvent(BUTTON3_PIN, BUTTON3_MODE, btnDebDel, btnDelay), actions, BUTTON3_RELAY});
    }
#endif
#ifdef BUTTON4_PIN
    {
        unsigned int actions = buttonStore(BUTTON4_PRESS, BUTTON4_CLICK, BUTTON4_DBLCLICK, BUTTON4_LNGCLICK, BUTTON4_LNGLNGCLICK);
        _buttons.push_back({new DebounceEvent(BUTTON4_PIN, BUTTON4_MODE, btnDebDel, btnDelay), actions, BUTTON4_RELAY});
    }
#endif  
#ifdef BUTTON5_PIN
    {
        unsigned int actions = buttonStore(BUTTON5_PRESS, BUTTON5_CLICK, BUTTON5_DBLCLICK, BUTTON5_LNGCLICK, BUTTON5_LNGLNGCLICK);
        _buttons.push_back({new DebounceEvent(BUTTON5_PIN, BUTTON5_MODE, btnDebDel, btnDelay), actions, BUTTON5_RELAY});
    }
#endif        
        // setManualOverride(false);
        DEBUG_PRINT(PSTR("[BUTTON] Number of buttons: %d\n"), _buttons.size());
}