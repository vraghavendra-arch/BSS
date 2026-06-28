#include "bl6552Sensor.h"
#include "all.h"
#include "SoftwareSerial.h"

#if POWER_PROVIDER == POWER_PROVIDER_BL6552

typedef struct{
  unsigned long timestamp;
  float voltage;
  float current;
  float activePower;
  float reactivePower;
  float apparentPower;
  float powerFactor;
  float energy_delta;
  float energy_total;
  float phase_angle;
  uint8_t state;
} phaseData;

#if DEBUG_BL6552_SOFTWARE_SERIAL
SoftwareSerial bl6552Serial(99, 0);
char* states[] =  { "NO_ERROR", "TIMEOUT_ERR", "BADFRAME_ERR", "CRCFRAME_ERR", "READY", "COMM_ERROR" };
#endif

BL6552 myenergy(&Serial);

phaseData _phaseData[3];

extern bool ReadData;

double _powerCurrent() {
    return (_phaseData[0].current + _phaseData[1].current + _phaseData[2].current)/3;
}

double _powerCurrent(uint8_t phase) {
    return _phaseData[phase].current;
}

double _powerVoltage() {
    return (_phaseData[0].voltage+_phaseData[1].voltage+_phaseData[2].voltage)/3;
}

double _powerVoltage(uint8_t phase) {
    return _phaseData[phase].voltage;
}

double _powerActivePower() {
    return _phaseData[0].activePower + _phaseData[1].activePower + _phaseData[2].activePower;
}

double _powerActivePower(uint8_t phase) {
    return _phaseData[phase].activePower;
}

double _powerApparentPower() {
    return _phaseData[0].apparentPower + _phaseData[1].apparentPower + _phaseData[2].apparentPower;
}

double _powerApparentPower(uint8_t phase) {
    return _phaseData[phase].apparentPower;
}

double _powerReactivePower() {
    return _phaseData[0].reactivePower + _phaseData[1].reactivePower + _phaseData[2].reactivePower;
}

double _powerReactivePower(uint8_t phase) {
    return _phaseData[phase].reactivePower;
}

double _powerPowerFactor() {
    return (_phaseData[0].powerFactor + _phaseData[1].powerFactor + _phaseData[2].powerFactor)/3;
}

double _powerPowerFactor(uint8_t phase) {
    return _phaseData[phase].powerFactor;
}

double _powerEnergy() {
    return _phaseData[0].energy_total + _phaseData[1].energy_total + _phaseData[2].energy_total;
}

double _powerEnergy(uint8_t phase) {
    return _phaseData[phase].energy_total;
}

double _powerPhaseAngle() {
    return (_phaseData[0].phase_angle + _phaseData[1].phase_angle + _phaseData[2].phase_angle)/3;
}

double _powerPhaseAngle(uint8_t phase) {
    return _phaseData[phase].phase_angle;
}

void loadPhaseStruct(uint8_t phase){
    _phaseData[phase].timestamp = millis();
    _phaseData[phase].voltage = myenergy.getVoltageOfPhase(phase+1)==BL6552_ERROR?_phaseData[phase].voltage:myenergy.getVoltageOfPhase(phase+1);
    _phaseData[phase].current = myenergy.getCurrentOfPhase(phase+1)==BL6552_ERROR?_phaseData[phase].current:myenergy.getCurrentOfPhase(phase+1);
    _phaseData[phase].activePower = myenergy.getActivePowerOfPhase(phase+1)==BL6552_ERROR?_phaseData[phase].activePower:myenergy.getActivePowerOfPhase(phase+1);
    _phaseData[phase].apparentPower = myenergy.getApparentPowerOfPhase(phase+1)==BL6552_ERROR?_phaseData[phase].apparentPower:myenergy.getApparentPowerOfPhase(phase+1);
    _phaseData[phase].powerFactor = myenergy.getPowerFactorOfPhase(phase+1)==BL6552_ERROR?_phaseData[phase].powerFactor:myenergy.getPowerFactorOfPhase(phase+1);
    // _phaseData[phase].phase_angle = myenergy.getPhaseAngleOfPhase(phase+1);
    // _phaseData[phase].energy_delta = myenergy.getEnergyDelta(); // we will calculate by our end
    // _phaseData[phase].energy_total = myenergy.getEnergy();      // we will calculate by our end
    _phaseData[phase].reactivePower = myenergy.getReactivePowerOfPhase(phase+1)==BL6552_ERROR?_phaseData[phase].reactivePower:myenergy.getReactivePowerOfPhase(phase+1);
    _phaseData[phase].state = myenergy.getState();
}

#if DEBUG_ENABLE
void loadRamdomTestValuesToPhaseDataStruct(uint8_t phase){
    _phaseData[phase].timestamp = millis();
    _phaseData[phase].voltage = random(220, 240);
    _phaseData[phase].current = random(800, 10000)/1000.0;
    _phaseData[phase].powerFactor = random(800, 1000)/1000.0;
    _phaseData[phase].apparentPower = _phaseData[phase].voltage * _phaseData[phase].current;
    _phaseData[phase].activePower = _phaseData[phase].apparentPower * _phaseData[phase].powerFactor;
    _phaseData[phase].energy_delta = random(100, 1000);
    _phaseData[phase].energy_total = random(100, 1000);
    _phaseData[phase].phase_angle = random(0, 120);
    _phaseData[phase].reactivePower = _phaseData[phase].apparentPower * sqrt(1-pow(_phaseData[phase].powerFactor, 2));
}
#endif

void retrieveMeterCalibConstants(){
    float iS = getSetting(BL6552_CURRENT_STANDARD_ADDR, CURRENT_STANDARD_VALUE).toFloat();
    float iK = getSetting(BL6552_CURRENT_K_CONSTANT_ADDR, CURRENT_K_CONSTANT).toFloat();

    float vS = getSetting(BL6552_VOLTAGE_STANDARD_ADDR, VOLTAGE_STANDARD_VALUE).toFloat();
    float vK = getSetting(BL6552_VOLTAGE_K_CONSTANT_ADDR, VOLTAGE_K_CONSTANT).toFloat();

    float pS = getSetting(BL6552_ACTIVE_POWER_STANDARD_ADDR, ACTIVE_POWER_STANDARD_VALUE).toFloat();
    float pK = getSetting(BL6552_ACTIVE_POWER_K_CONSTANT_ADDR, ACTIVE_POWER_K_CONSTANT).toFloat();

    float sS = getSetting(BL6552_APPARENT_POWER_STANDARD_ADDR, APPARENT_POWER_STANDARD_VALUE).toFloat();
    float sK = getSetting(BL6552_APPARENT_POWER_K_CONSTANT_ADDR, APPARENT_POWER_K_CONSTANT).toFloat();



    myenergy.setAllConstants(iS, iK, vS, vK, pS, pK, sS, sK);

}


void powerBl6552Setup(void){
 
    myenergy.begin();

    #if DEBUG_BL6552_SOFTWARE_SERIAL
    bl6552Serial.begin(9600);
    #endif
    retrieveMeterCalibConstants();

    loopRegister(powerBl6552Loop);
}

void powerBl6552Loop(void){

    static int _powerReadLast = 0;
    static int _powerReportLast = 0;
    // static uint8_t _phase = 0;

    if(millis() - _powerReadLast > POWER_READ_INTERVAL){ //for reading from the IC
        _powerReadLast = millis();

        for(uint8_t _phase = 0; _phase<PHASE_COUNT; _phase++){  
            loadPhaseStruct(_phase);  //for real values
            // loadRamdomTestValuesToPhaseDataStruct(_phase); //for testing random values

            #if DEBUG_BL6552_SOFTWARE_SERIAL
            bl6552Serial.println("\n[PHASE " + String(_phase+1)+"]");
            bl6552Serial.println("VOLTAGE = " + String(_phaseData[_phase].voltage));
            bl6552Serial.println("CURRENT = " + String(_phaseData[_phase].current));
            bl6552Serial.println("ACTIVE POWER = "+String(_phaseData[_phase].activePower));
            bl6552Serial.println("REACTIVE POWER = " + String(_phaseData[_phase].reactivePower));
            bl6552Serial.println("APPARANT POWER = " + String(_phaseData[_phase].apparentPower));
            bl6552Serial.println("ENERGY DELTA = " + String(_phaseData[_phase].energy_delta));
            bl6552Serial.println("ENERGY = " + String(_phaseData[_phase].energy_delta));
            bl6552Serial.println("PHASE ANGLE = " + String(_phaseData[_phase].phase_angle));
            bl6552Serial.println("POWER FACTOR = " + String(_phaseData[_phase].powerFactor));
            bl6552Serial.println("STATE = " + String(states[_phaseData[_phase].state]));
            bl6552Serial.println();
            #endif

        }
        ReadData = true;

        // _phase++;
        // if(_phase > 3-1){
        //     _phase = 0;
        // }
    }



}

#endif //POWER_PROVIDER_BL6552
