#include "all.h"
#include "string.h"
#include "Ticker.h"

Ticker deffered;

String rightString(const char *s, size_t length) {
    String str;
    str.reserve(length);
    for (size_t i = 0; i < length; i++) str += s[i];
    return str;
}

unsigned char resetReason() {
    unsigned char status = getSetting(EEPROM_CUSTOM_RESET, 0).toInt();
    return status;
}

void resetReason(unsigned char reason) {
    setSetting(EEPROM_CUSTOM_RESET, reason);
    // EEPROMr.commit();
    saveSettings(true);
}

void deferredReset(uint16_t _time_ms, uint8_t reason){
    deffered.once_ms(_time_ms, [reason](){
        resetReason(reason);
        ESP.restart();
    });
}
