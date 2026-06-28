#include "EEPROM_Rotate.h"
#include "all.h"

EEPROM_Rotate EEPROMr;

uint32_t eepromCurrent() {
    return EEPROMr.current();
}

void eepromBackup(uint32_t index) {
    EEPROMr.backup(index);
}

void eepromSetup(){
    EEPROMr.size(4);

    EEPROMr.offset(EEPROM_DATA_OFFSET);
    EEPROMr.begin(EEPROM_SIZE);
}



