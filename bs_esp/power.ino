#include "ArduinoJson.h"
#include "meanfilter.h"
#include "all.h"
#include "Ticker.h"
#if POWER_PROVIDER != POWER_PROVIDER_NONE

Ticker tick; //to avoid loosing energy value

uint64_t powerActiveEnergyDelta;
uint64_t powerActiveEnergyTotal;

uint64_t powerApparentEnergyDelta;
uint64_t powerApparentEnergyTotal;

uint64_t powerReactiveEnergyDelta;
uint64_t powerReactiveEnergyTotal;

volatile bool resettingEnergy = false;


uint32_t _powerReportingInterval = POWER_REPORTING_INTERVAL;
bool ReadData  = false;

MeanPowerFilter _filter_apparent = MeanPowerFilter();
MeanPowerFilter _filter_reactive = MeanPowerFilter();
MeanPowerFilter _filter_active = MeanPowerFilter();
MeanPowerFilter _filter_voltageA = MeanPowerFilter();
MeanPowerFilter _filter_voltageB = MeanPowerFilter();
MeanPowerFilter _filter_voltageC = MeanPowerFilter();
MeanPowerFilter _filter_currentA = MeanPowerFilter();
MeanPowerFilter _filter_currentB = MeanPowerFilter();
MeanPowerFilter _filter_currentC = MeanPowerFilter();

void tickerCallback(){
    
    //To avoid loosing of energy values, ticker is being used
    /*
    converting in watts minute to save
    */
    double active = _filter_active.mean(true);
    double apparent = _filter_apparent.mean(true);
    double reactive = _filter_reactive.mean(true);
   
    powerActiveEnergyDelta = (uint64_t)((active)*((double)_powerReportingInterval/60.)); // (Watts / 1000) x (seconds / 3600) = kWatts x Hour
    powerActiveEnergyTotal += powerActiveEnergyDelta;

    powerApparentEnergyDelta = (uint64_t)(apparent)*((double)_powerReportingInterval/60.); // (VA / 1000) x (seconds / 3600) = kVA x Hour
    powerApparentEnergyTotal += powerApparentEnergyDelta;

    powerReactiveEnergyDelta = (uint64_t)(reactive)*((double)_powerReportingInterval/60.); // (VAR / 1000) x (seconds / 3600) = kVAR x Hour
    powerReactiveEnergyTotal += powerReactiveEnergyDelta;

    if(resettingEnergy) return;
    setSetting(SETTING_POWER_ENERGY_TOTAL, powerActiveEnergyTotal);
    setSetting(SETTING_POWER_ENERGY_APPARENT, powerApparentEnergyTotal);
    setSetting(SETTING_POWER_ENERGY_REACTIVE, powerReactiveEnergyTotal);
    saveSettings(true);
}
void accumulateData(){
    #if POWER_PROVIDER == POWER_PROVIDER_BL6552
    if(_powerVoltage(0) <= 0 || _powerVoltage(1) <= 0 || _powerVoltage(2) <= 0) return;
    if(_powerCurrent(0) <= 0 || _powerCurrent(1) <= 0 || _powerCurrent(2) <= 0) return;
    if(_powerActivePower(0) < 0 || _powerActivePower(1) < 0 || _powerActivePower(2) < 0) return;
    if(_powerApparentPower(0) < 0 || _powerApparentPower(1) < 0 || _powerApparentPower(2) < 0) return;
    if(_powerReactivePower(0) < 0 || _powerReactivePower(1) < 0 || _powerReactivePower(2) < 0) return;
    _filter_voltageA.add(_powerVoltage(0));
    _filter_voltageB.add(_powerVoltage(1));
    _filter_voltageC.add(_powerVoltage(2));
    _filter_currentA.add(_powerCurrent(0));
    _filter_currentB.add(_powerCurrent(1));
    _filter_currentC.add(_powerCurrent(2));
    #endif
    _filter_active.add(_powerActivePower());
    _filter_apparent.add(_powerApparentPower());
    _filter_reactive.add(_powerReactivePower());
}


void resetEnergyValue(){
    resettingEnergy = true;
    powerActiveEnergyTotal = 0;

    powerApparentEnergyTotal = 0;

    powerReactiveEnergyTotal = 0;

    setSetting(SETTING_POWER_ENERGY_TOTAL, powerActiveEnergyTotal);
    setSetting(SETTING_POWER_ENERGY_APPARENT, powerApparentEnergyTotal);
    setSetting(SETTING_POWER_ENERGY_REACTIVE, powerReactiveEnergyTotal);
    saveSettings(true);
    resettingEnergy = false;
}

void powerReport(void){
    // powerActiveEnergyDelta = (_powerActivePower()/1000)*((double)_powerReportingInterval/3600.); // (Watts / 1000) x (seconds / 3600) = kWatts x Hour
    // powerActiveEnergyTotal += powerActiveEnergyDelta;

    // powerApparentEnergyDelta = (_powerApparentPower()/1000)*((double)_powerReportingInterval/3600.); // (VA / 1000) x (seconds / 3600) = kVA x Hour
    // powerApparentEnergyTotal += powerApparentEnergyDelta;

    // powerReactiveEnergyDelta = (_powerReactivePower()/1000)*((double)_powerReportingInterval/3600.); // (VAR / 1000) x (seconds / 3600) = kVAR x Hour
    // powerReactiveEnergyTotal += powerReactiveEnergyDelta;

    // setSetting(SETTING_POWER_ENERGY_TOTAL, powerActiveEnergyTotal);
    // setSetting(SETTING_POWER_ENERGY_APPARENT, powerApparentEnergyTotal);
    // setSetting(SETTING_POWER_ENERGY_REACTIVE, powerReactiveEnergyTotal);
    // saveSettings(false);
    float voltageA = _filter_voltageA.mean();
    float voltageB = _filter_voltageB.mean();
    float voltageC = _filter_voltageC.mean();
    float currentA = _filter_currentA.mean();
    float currentB = _filter_currentB.mean();
    float currentC = _filter_currentC.mean();

    StaticJsonDocument<1024> root;
    #if defined(ENERGY_METER_1)
    root[MQTT_KEY_ENTITY_ID] = DEVICE_ID();
    #else
    root[MQTT_KEY_DEVICE_ID] = DEVICE_ID();
    #endif
    
    #if PHASE_COUNT == 3
    root[MQTT_KEY_VOLTAGE_R] = String(voltageA);
    root[MQTT_KEY_CURRENT_R] = String(currentA);
    root[MQTT_KEY_POWER_R] = String(_powerActivePower(0));
    root[MQTT_KEY_APPARENT_POWER_R] = String(_powerApparentPower(0));
    root[MQTT_KEY_REACTIVE_POWER_R] = String(_powerReactivePower(0));
    // root[MQTT_KEY_FREQUENCY_R] = _powerFrequency(0);
    root[MQTT_KEY_PF_R] = String(_powerPowerFactor(0));

    root[MQTT_KEY_VOLTAGE_Y] = String(voltageB);
    root[MQTT_KEY_CURRENT_Y] = String(currentB);
    root[MQTT_KEY_POWER_Y] = String(_powerActivePower(1));
    root[MQTT_KEY_APPARENT_POWER_Y] = String(_powerApparentPower(1));
    root[MQTT_KEY_REACTIVE_POWER_Y] = String(_powerReactivePower(1));
    // root[MQTT_KEY_FREQUENCY_Y] = _powerFrequency(1);
    root[MQTT_KEY_PF_Y] = String(_powerPowerFactor(1));

    root[MQTT_KEY_VOLTAGE_B] = String(voltageC);
    root[MQTT_KEY_CURRENT_B] = String(currentC);
    root[MQTT_KEY_POWER_B] = String(_powerActivePower(2));
    root[MQTT_KEY_APPARENT_POWER_B] = String(_powerApparentPower(2));
    root[MQTT_KEY_REACTIVE_POWER_B] = String(_powerReactivePower(2));
    // root[MQTT_KEY_FREQUENCY_B] = _powerFrequency(2);
    root[MQTT_KEY_PF_B] = String(_powerPowerFactor(2));

    root[MQTT_KEY_ENERGY] = String(((double)powerActiveEnergyTotal/(60000.)),3);
    root[MQTT_KEY_APPARENT_ENERGY] = String((double)powerApparentEnergyTotal/(60000.),3);
    root[MQTT_KEY_REACTIVE_ENERGY] = String((double)powerReactiveEnergyTotal/(60000.),3);

    
  
    #else
    #if defined(ENERGY_METER_1)
    root[MQTT_TOPIC_VOLTAGE_PLUG] = String(_powerVoltage());
    root[MQTT_TOPIC_CURRENT_PLUG] = String(_powerCurrent());
    root[MQTT_TOPIC_POWER_FACTOR_PLUG] = String(_powerPowerFactor());
    root[MQTT_TOPIC_POWER_ACTIVE_PLUG] = String(_powerActivePower());
    root[MQTT_TOPIC_POWER_APPARENT_PLUG] = String(_powerApparentPower());
    root[MQTT_TOPIC_POWER_REACTIVE_PLUG] = String(_powerReactivePower());
    root[MQTT_KEY_ENERGY_PLUG] = String(powerActiveEnergyTotal);
    root[MQTT_OTA_VERSION] = String(getSetting(SETTING_OTA_APP_VERSION, "0"));


   
    #else
    root[MQTT_TOPIC_VOLTAGE] = String(_powerVoltage());
    root[MQTT_TOPIC_CURRENT] = String(_powerCurrent());
    root[MQTT_TOPIC_POWER_FACTOR] = String(_powerPowerFactor());
    root[MQTT_TOPIC_POWER_ACTIVE] = String(_powerActivePower());
    root[MQTT_TOPIC_POWER_APPARENT] = String(_powerApparentPower());
    root[MQTT_TOPIC_POWER_REACTIVE] = String(_powerReactivePower());
    root[MQTT_KEY_ENERGY] = String(powerActiveEnergyTotal);
    root[MQTT_KEY_APPARENT_ENERGY] = String(powerApparentEnergyTotal);
    root[MQTT_KEY_REACTIVE_ENERGY] = String(powerReactiveEnergyTotal);
    root[MQTT_OTA_VERSION] = String(getSetting(SETTING_OTA_APP_VERSION, "0"));


    #endif
    #endif

    String output;
    serializeJson(root, output);

    if(mqttConnected()){
        #if defined(ENERGY_METER_1)
        mqtt_publish(MQTT_TOPIC_ENERGY_PLUG, output.c_str());
        #else
        mqtt_publish(MQTT_TOPIC_ENERGY, output.c_str());
        #endif
    }

}


void powerSetup(void){
    powerActiveEnergyTotal = getSetting(SETTING_POWER_ENERGY_TOTAL, 0.00).toFloat();
    powerApparentEnergyTotal = getSetting(SETTING_POWER_ENERGY_APPARENT, 0.00).toFloat();
    powerReactiveEnergyTotal = getSetting(SETTING_POWER_ENERGY_REACTIVE, 0.00).toFloat();
    
    #if POWER_PROVIDER == POWER_PROVIDER_BL6552
    powerBl6552Setup();
   #endif

   #if POWER_PROVIDER == POWER_PROVIDER_BL0940
   powerSetupBl0940();
   #endif

    tick.attach(_powerReportingInterval, tickerCallback);

    loopRegister(powerLoop);
}

void powerLoop(void){
    static int _powerReportLast = 0;

    if(millis() - _powerReportLast > POWER_REPORTING_INTERVAL*1000){ //for reporting to the server
        _powerReportLast = millis();
        powerReport();
    }

    if(ReadData == true){
        accumulateData();
        ReadData = false;
    }
}

#endif
