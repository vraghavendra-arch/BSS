#include "all.h"
#include "bl0940Sensor.h"
#include "SoftwareSerial.h"

#if POWER_PROVIDER == POWER_PROVIDER_BL0940
#if DEBUG_BL0940_SOFTWARE_SERIAL
SoftwareSerial bl0940Serial(POWER_SOFT_SERIAL_RX, POWER_SOFT_SERIAL_TX); // RX and TX pins of ESP8266
char* states[] =  { "NO_ERROR", "TIMEOUT_ERR", "BADFRAME_ERR", "CRCFRAME_ERR", "READY", "COMM_ERROR" };
#endif
#define BL0940_SERIAL_PORT Serial
bl0940 myenergy;

double _powerCurrent() {
    return (myenergy.getCurrent()<0)?myenergy.getCurrent()*(-1):myenergy.getCurrent();
}

double _powerVoltage() {
    return myenergy.getVoltage();
}

double _powerActivePower() {
    return (myenergy.getActivePower()<0)?myenergy.getActivePower()*(-1):myenergy.getActivePower();
}

double _powerApparentPower() {
    return (myenergy.getApparentPower()<0)?myenergy.getApparentPower()*(-1):myenergy.getApparentPower();
}

double _powerReactivePower() {
    return (myenergy.getReactivePower()<0)?myenergy.getReactivePower()*(-1):myenergy.getReactivePower();
}

double _powerPowerFactor() {
    return (myenergy.getPowerFactor(false)<0)?myenergy.getPowerFactor(false)*(-1):myenergy.getPowerFactor(false);
}

double _powerEnergy() {
    return (myenergy.getEnergy());
}

void powerSetupBl0940(){
    double _shunt_resistance = getSetting("sr", BL0940_SHUNT_RESISTANCE).toDouble();
    #if DEBUG_BL0940_SOFTWARE_SERIAL
    bl0940Serial.begin(9600);
    #endif

    myenergy.setRMSUpdate(BL0940_RMS_REG_UPDATE_RATE_800MS);
    myenergy.setACFrequency(BL0940_AC_FREQ_50HZ);
    myenergy.writeModeRegister();

    // settingsRegisterCommand(F("METERSHUNT"), [](Embedis* e) {
    //     if (e->argc < 1) {
    //         return e->response(Embedis::ARGS_ERROR);
    //     }
    //     if (e->argc > 1) {
    //         double value = atof((e->argv[1]));
    //         setResistance(value);
    //     }
    //     DEBUG_MSG_P(PSTR("Shunt Resistance: %f\n"), getResistance());
    //     DEBUG_MSG_P(PSTR("Done\n"));
    // });

    // settingsRegisterCommand(F("METERCT_TB"), [](Embedis* e) {
    //     if (e->argc < 2) {
    //         return e->response(Embedis::ARGS_ERROR);
    //     }
    //     double turn = atof((e->argv[1]));
    //     if (e->argc > 2) {
    //         double burden = atof((e->argv[2]));
    //         double shunt = (float)(((float)burden*1000)/turn);
    //         setResistance(shunt);
    //     }
    //     // DEBUG_PRINT(PSTR("Shunt Resistance: %f\n"), getResistance());
    //     // DEBUG_PRINT(PSTR("Done\n"));
    // });

    myenergy.setResistance(_shunt_resistance);
    loopRegister(powerLoopBl0940);
}

void powerLoopBl0940()
{
    static uint32_t _powerlast = 0;

    if(millis() - _powerlast >= 1000){
        myenergy.readValues();
        // _power_newdata = true;

        #if DEBUG_BL0940_SOFTWARE_SERIAL
        bl0940Serial.println("\nACTIVE POWER = " + String(myenergy.getActivePower()));
        bl0940Serial.println("CURRENT = " + String(myenergy.getCurrent()));
        bl0940Serial.println("VOLTAGE = " + String(myenergy.getVoltage()));
        bl0940Serial.println("ENERGY = " + String(myenergy.getEnergy(myenergy.getEnergy())));
        bl0940Serial.println("TEMPERATURE = " + String(myenergy.getTemperature()));
        bl0940Serial.println("APPARANT POWER = " + String(myenergy.getApparentPower()));
        bl0940Serial.println("ENERGY DELTA = " + String(myenergy.getEnergyDelta()));
        bl0940Serial.println("PHASE ANGLE = " + String(myenergy.getPhaseAngle()));
        bl0940Serial.println("POWER FACTOR = " + String(myenergy.getPowerFactor(false)));
        bl0940Serial.println("REACTIVE POWER = " + String(myenergy.getReactivePower()));
        bl0940Serial.println("STATE = " + String(states[myenergy.getState()]));
        #endif
        
        // // delay(3000);
        _powerlast = millis();
        verifySystemStability(); //sometimes the Serial buffer creates problem, reset communication
    }

}

void verifySystemStability(){
    if(_powerCurrent()>10000){
        BL0940_SERIAL_PORT.begin(4800);
    }
}

#endif