#include "energyMeter.h"
#include "SoftwareSerial.h"


// extern SoftwareSerial mySerial;

/**
 * @brief Constructor for the Meter class.
 * 
 * This constructor initializes the Meter object with a given SoftwareSerial instance.
 * It sets up the serial communication by starting the SoftwareSerial with the specified
 * baud rate and port configuration, and then calls the initialization function.
 * 
 * @param hwSer Pointer to a SoftwareSerial object used for serial communication.
 */
Meter::Meter(HardwareSerial* hwSer) {
    this->_serial = hwSer;
    static_cast<HardwareSerial*>(_serial)->begin(Meter_UART_BAUD);
    // _init();
}

/**
 * @brief Destructor for the Meter class.
 * 
 * This destructor is called when an object of the Meter class is 
 * destroyed. It currently does not perform any specific cleanup 
 * operations, but can be extended in the future if necessary.
 */
Meter::~Meter()
{}

void Meter::begin(){
    _init();
}

void Meter::_init(){
    //enable writing to calibration registers
    writeValue(REG_USR_WRPROT, 0x005555);
    writeValue(0xE1, 0x000950);
    writeValue(0xDD, 0x000D82);
    
    
    //initialize the Meter calibration registers here
    writeValue(REG_GAIN1, 0x000000); //current gain to 1
    writeValue(REG_GAIN2, 0x000000); //voltage gain to 1

    writeValue(REG_CFDIV, 0x10); //default: 0x10 -> 1, Used to control the accumulation speed of electric energy pulse counting
    writeValue(REG_MODE1, 0b1<<23); //default: 0x000000 -> 0x000000, 23rd bit -> Current WAVE waveform register output selection, default 0 to select the waveform of the normal effective value channel, and 1 to select the waveform output of the fast effective value channel 
    writeValue(REG_MODE2, 0x000000); //default: 0x000000 -> 0x000000
    writeValue(REG_MODE3, 0x011200); //default: 0x000000 -> 0x000000
    writeValue(REG_RST_ENG, 0x000000); // do not reset the energy counter


    writeValue(REG_CFDIV, 0x08); //default: 0x10 -> 1, Used to control the accumulation speed of electric energy pulse counting
    writeValue(REG_RST_ENG, 0xFFFFFF); // reset the energy counter after reading the registers


    //disable writing to calibration registers
    writeValue(0xDD, 0x000000);
    writeValue(0xE1, 0x000000);
    writeValue(REG_USR_WRPROT, 0x000000);

}


/**
 * @brief Get the voltage of the specified phase (1-3)
 * @param phase The phase to read the voltage from
 * @return The voltage of the specified phase in Volts, or 0 if an error occurred
 */
float Meter::getVoltageOfPhase(int phase){

    switch (phase)
    {
    case 1:
        readValue(REG_VRMS_A);
        break;
    case 2:
        readValue(REG_VRMS_B);
        break;
    case 3:
        readValue(REG_VRMS_C);
        break;
    
    default:
        return 0.00;
        break;
    }

    
    if(getState() != NO_ERROR){
        return 0.00;
    }
    
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*VOLTAGE_CONSTANT;//*1.227E-4;//*STANDERED_VOLTAGE_VALUE)/v_a_coefficient;

}


/**
 * @brief Get the current of the specified phase (1-3)
 * @param phase The phase to read the current from
 * @return The current of the specified phase in Amperes, or 0 if an error occurred
 */
float Meter::getCurrentOfPhase(int phase){
    switch (phase)
    {
    case 1:
        readValue(REG_IRMS_A);
        break;
    case 2:
        readValue(REG_IRMS_B);
        break;
    case 3:
        readValue(REG_IRMS_C);
        break;
    default:
        return 0.00;
        break;
    }

    if(getState() != NO_ERROR){
        return 0.00;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*CURRENT_CONSTANT;//*2.01022447695E-5;//*STANDERED_VOLTAGE_VALUE)/i_a_coefficient;
}


/**
 * @brief      Get the Active Power of a given phase (1-3)
 *
 * @details    This function reads the Active Power of a given phase and returns it
 *             in a converted format.
 *
 * @param[in]  phase   The phase number to read the Active Power from
 *
 * @return     The Active Power of the given phase in converted format, or 0.00 if there is an
 *             error
 */
float Meter::getActivePowerOfPhase(int phase){
    uint32_t activepower;
    bool invert = false;
    switch(phase){
        case 1:
            readValue(REG_WATT_A);
            break;
        case 2:
            readValue(REG_WATT_B);
            break;
        case 3:
            readValue(REG_WATT_C);
            break;
        default:
            return 0.00;
            break;
    }

    if(getState() != NO_ERROR){
        return 0.00;
    }
    activepower = (((uint32_t)_rawHolder[2])<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]);
    invert = (activepower>>23) != 0;
    if(invert){
        activepower = ((~activepower)&0xFFFFFF) + 1;
    }
    return (float)activepower*ACTIVE_POWER_CONSTANT;//*1.03450279865E-2;//STANDERED_VOLTAGE_VALUE;

}


/**
 * @brief      Reads the Reactive Power of a given phase and returns it in a converted format.
 *
 * @details    This function reads the Reactive Power of a given phase and returns it in a converted
 *             format. The conversion factor is derived from the datasheet and the power factor of the
 *             phase is used to calculate the reactive power.
 *
 * @param[in]  phase   The phase number to read the Reactive Power from
 *
 * @return     The Reactive Power of the given phase in converted format, or 0.00 if there is an
 *             error
 */
float Meter::getReactivePowerOfPhase(int phase){
    uint32_t apparentpower;
    bool invert = false;
    float pf = getPowerFactorOfPhase(phase);
    switch(phase){
        case 1:
            readValue(REG_VAR_A);
            break;
        case 2:
            readValue(REG_VAR_B);
            break;
        case 3:
            readValue(REG_VAR_C);
            break;
        default:
            return 0.00;
            break;
    }

    if(getState() != NO_ERROR){
        return 0.00;
    }
    apparentpower = (((uint32_t)_rawHolder[2])<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]);
    invert = (apparentpower>>23) != 0;
    if(invert){
        apparentpower = ((~apparentpower)&0xFFFFFF) + 1;
    }

    float sinePhi = sqrt(1- pow(pf, 2));
    return (float)apparentpower*APPARENT_POWER_CONSTANT*sinePhi;//*1.03450279865E-2;//STANDERED_VOLTAGE_VALUE;
}


/**
 * @brief      Get the Apparent Power of a given phase in converted format
 *
 * @details    This function reads the Apparent Power of a given phase and returns it
 *             in a converted format. The apparent power is the vector sum of the
 *             active and reactive power of the given phase.
 *
 * @param[in]  phase   The phase number to read the Apparent Power from
 *
 * @return     The Apparent Power of the given phase in converted format, or 0.00 if there is an
 *             error
 */
float Meter::getApparentPowerOfPhase(int phase){
    uint32_t apparentpower;
    bool invert = false;
    switch(phase){
        case 1:
            readValue(REG_VA_A);
            break;
        case 2:
            readValue(REG_VA_B);
            break;
        case 3:
            readValue(REG_VA_C);
            break;
        default:
            return 0.00;
            break;
    }

    if(getState() != NO_ERROR){
        return 0.00;
    }
    apparentpower = (((uint32_t)_rawHolder[2])<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]);
    invert = (apparentpower>>23) != 0;
    if(invert){
        apparentpower = ((~apparentpower)&0xFFFFFF) + 1;
    }
    return (float)apparentpower*APPARENT_POWER_CONSTANT;//*1.03450279865E-2;//STANDERED_VOLTAGE_VALUE;

}

/**
 * @brief      Get the line frequency of the meter
 *
 * @details    This function reads the line frequency from the meter and returns it
 *             in Hz. The line frequency is the frequency of the AC line voltage
 *             that the meter is measuring.
 *
 * @return     The line frequency of the meter in Hz, or 0.00 if there is an
 *             error
 */
float Meter::getLineFrequency(){
    readValue(REG_PERIOD);

    if(getState() != NO_ERROR){
        return 0.00;
    }

    return 10000000.00/(((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0])*1.00);
}


/**
 * @brief      Get the Power Factor of a given phase
 *
 * @details    This function reads the Power Factor of a given phase from the meter and returns it
 *             in a converted format. The power factor is a value between 0 and 1 that shows how
 *             much of the power is reactive and how much is active. A power factor of 1.00 means
 *             that the power is purely active and a power factor of 0.00 means that the power is
 *             purely reactive.
 *
 * @param[in]  phase   The phase number to read the Power Factor from
 *
 * @return     The Power Factor of the given phase in converted format, or 0.00 if there is an
 *             error
 */
float Meter::getPowerFactorOfPhase(int phase){
    uint32_t powerFactor;
    bool invert = false;
    switch(phase){
        case 1:
            readValue(REG_PF_A);
            break;
        case 2:
            readValue(REG_PF_B);
            break;
        case 3:
            readValue(REG_PF_C);
            break;
        default:
            return 0.00;
            break;
    }

    if(getState() != NO_ERROR){
        return 0.00;
    }
    powerFactor = (((uint32_t)_rawHolder[2])<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]);
    invert = (powerFactor>>23) != 0;
    if(invert){
        powerFactor = ((~powerFactor)&0xFFFFFF) + 1;
    }

    float pf =  ((float)powerFactor * 1.00)/pow(2, 23);
    pf = pf>1 ? 1 : pf;
    return pf;
}

/**
 * Copies the raw data from the latest read operation into the given array.
 *
 * @param[in]  arr  The array to copy the data into. The array must be at least 6 bytes long.
 */
void Meter::getRawData(uint8_t* arr){
    memcpy(arr, _rawHolder, 6);
}

/**
 * @brief      Get the value of a given register in the meter
 *
 * @details    This function reads the value of a given register from the meter and returns it
 *             in a raw format.
 *
 * @param[in]  regAddr  The address of the register to read
 *
 * @return     The value of the given register in raw format, or 0 if there is an
 *             error
 */
uint32_t Meter::getRegistorValue(uint8_t regAddr){
    readValue(regAddr);
    return ((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]);
}

/**
 * @brief      Get the RMS Voltage of a given phase in raw format
 *
 * @details    This function reads the RMS Voltage of a given phase and returns it
 *             in a raw format.
 *
 * @param[in]  phase   The phase number to read the RMS Voltage from
 *
 * @return     The RMS Voltage of the given phase in raw format, or 0.00 if there is an
 *             error
 */
double Meter::getVoltageRaw(uint8_t phase){
    switch (phase)
    {
    case 1:
        readValue(REG_VRMS_A);
        break;
    case 2:
        readValue(REG_VRMS_B);
        break;
    case 3:
        readValue(REG_VRMS_C);
        break;
    
    default:
        return 0.00;
        break;
    }

    if(getState() != NO_ERROR){
        return 0.00;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]));
}

/**
 * @brief      Get the RMS current of a given phase in raw format
 *
 * @details    This function reads the RMS current of a given phase and returns it
 *             in a raw format.
 *
 * @param[in]  phase   The phase number to read the current from
 *
 * @return     The RMS current of the given phase in raw format, or 0.00 if there is an
 *             error
 */
double Meter::getCurrentRaw(uint8_t phase){
    switch (phase)
    {
    case 1:
        readValue(REG_IRMS_A);
        break;
    case 2:
        readValue(REG_IRMS_B);
        break;
    case 3:
        readValue(REG_IRMS_C);
        break;
    default:
        return 0.00;
        break;
    }

    if(getState() != NO_ERROR){
        return 0.00;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]));
}

/**
 * @brief      Get the Active Power of a given phase in raw format
 *
 * @details    This function reads the Active Power of a given phase and returns it
 *             in a raw format.
 *
 * @param[in]  phase   The phase number to read the Active Power from
 *
 * @return     The Active Power of the given phase in raw format, or 0.00 if there is an
 *             error
 */
double Meter::getActivePowerRaw(uint8_t phase){
    switch (phase){
        case 1:
            readValue(REG_WATT_A);
            break;
        case 2:
            readValue(REG_WATT_B);
            break;
        case 3:
            readValue(REG_WATT_C);
            break;
        default:
            return 0.00;
            break;
    }

    if(getState() != NO_ERROR){
        return 0.00;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*1.00;
}

/**
 * @brief      Get the Reactive Power of a given phase in raw format
 *
 * @details    This function reads the Reactive Power of a given phase and returns it
 *             in a raw format.
 *
 * @param[in]  phase   The phase number to read the Reactive Power from
 *
 * @return     The Reactive Power of the given phase in raw format, or 0.00 if there is an
 *             error
 */
double Meter::getReactivePowerRaw(uint8_t phase){
    switch (phase){
        case 1:
            readValue(REG_VAR_A);
            break;
        case 2:
            readValue(REG_VAR_B);
            break;
        case 3:
            readValue(REG_VAR_C);
            break;
        default:
            return 0.00;
            break;
    }
    if(getState() != NO_ERROR){
        return 0.00;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*1.00;
}

/**
 * @brief      Get the Apparent Power of a given phase in raw format
 *
 * @details    This function reads the Apparent Power of a given phase and returns it
 *             in a raw format.
 *
 * @param[in]  phase   The phase number to read the Apparent Power from
 *
 * @return     The Apparent Power of the given phase in raw format, or 0.00 if there is an
 *             error
 */
double Meter::getApparentPowerRaw(uint8_t phase){
    switch (phase){
        case 1:
            readValue(REG_VA_A);
            break;
        case 2:
            readValue(REG_VA_B);
            break;
        case 3:
            readValue(REG_VA_C);
            break;
        default:
            return 0.00;
            break;
    }
    if(getState() != NO_ERROR){
        return 0.00;
    }
    return (((uint32_t)(uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*1.00;
}

/**
 * @brief      Get the Power factor of a given phase in raw format
 *
 * @details    This function reads the Power factor of a given phase and returns it
 *             in a raw format.
 *
 * @param[in]  phase   The phase number to read the Power factor from
 *
 * @return     The Power factor of the given phase in raw format, or 0.00 if there is an
 *             error
 */
double Meter::getPowerFactorRaw(uint8_t phase){
    switch (phase){
        case 1:
            readValue(REG_PF_A);
            break;
        case 2:
            readValue(REG_PF_B);
            break;
        case 3:
            readValue(REG_PF_C);
            break;
        default:
            return 0.00;
            break;
    }
    if(getState() != NO_ERROR){
        return 0.00;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*1.00;
}

/**
 * @brief      Get the Phase angle of a given phase in raw format
 *
 * @details    This function reads the Phase angle of a given phase and returns it
 *             in a raw format.
 *
 * @param[in]  phase   The phase number to read the Phase angle from
 *
 * @return     The Phase angle of the given phase in raw format, or 0.00 if there is an
 *             error
 */
double Meter::getPhaseAngleRaw(uint8_t phase){
    switch (phase){
        case 1:
            readValue(REG_ANGLE_A);
            break;
        case 2:
            readValue(REG_ANGLE_B);
            break;
        case 3:
            readValue(REG_ANGLE_C);
            break;
        default:
            return 0.00;
            break;
    }
    
    if(getState() != NO_ERROR){
        return 0.00;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*1.00;
}


/**
 * @brief Retrieves the current state of the Meter device.
 * 
 * This function returns the value of the private member variable 
 * _state, which represents the current operational state of the 
 * Meter device. The state is returned as an 8-bit unsigned integer.
 * 
 * @return uint8_t The current state of the Meter device.
 */
uint8_t Meter::getState(){
    return _state;
}

/**
 * @brief Reads a value from a specified register address.
 * 
 * This function sets up a frame to read a value from the given register address,
 * sends the frame, and then receives the response frame. It returns true if the
 * operation was successful (i.e., no error occurred), otherwise false.
 * 
 * @param regAddr The register address to read from.
 * @return true if the read operation was successful, false otherwise.
 */
bool Meter::readValue(uint8_t regAddr){
    _frame.Bytes = 6;
    _frame.Index = 2;
    _frame.Payload[Meter_HEAD_POSE] = Meter_READ_CMD;
    _frame.Payload[Meter_ADD_POSE] = regAddr;
    _sendFrame();
    _receiveFrame();

    return _state == NO_ERROR;
}

void Meter::_sendFrame(){
    _serial->write(_frame.Payload, _frame.Index);
}

void Meter::_receiveFrame(){
    int bytesAvailable;
    _frame.lastRcv = millis();
    while( (_frame.Index < _frame.Bytes) && !((millis()-_frame.lastRcv) > Meter_TIMEOUT))
    {
        bytesAvailable = _serial->available();
        if( bytesAvailable > 0)
        {
            for( int bytesIndex=0; bytesIndex<bytesAvailable; bytesIndex++)
            {
                _frame.Payload[_frame.Index++] = (uint8_t)_serial->read();
                // Serial.println(_frame.Payload[_frame.Index-1], HEX);
            }
            _frame.lastRcv = millis();         //Reset timeout counter
        }
    }

    if(millis() - _frame.lastRcv > Meter_TIMEOUT){
        _state = TIMEOUT_ERR;
        return;
    }

    if(_frame.Index != _frame.Bytes){
        _state = BADFRAME_ERR;
        return;
    }

    if(!_crcCheck()){
        _state = CRCFRAME_ERR;
        return;
    }

    memcpy(_rawHolder, _frame.Payload+Meter_DATA_L_POSE, 3);
    _state = NO_ERROR;


}

bool Meter::_crcCheck(){
    uint8_t crc = 0;

    for(uint8_t i= Meter_ADD_POSE; i<=Meter_DATA_H_POSE; i++){
        crc += _frame.Payload[i];
    }
    crc = ~crc;


    return (crc == _frame.Payload[Meter_CRC_POSE]);
    
}

void Meter::writeValue(uint8_t regAddr, uint32 value){
    _frame.Bytes = 6;
    _frame.Index = 1;
    _frame.Payload[Meter_HEAD_POSE] = Meter_WRITE_CMD;
    _frame.Payload[Meter_ADD_POSE] = regAddr;
    _frame.Payload[Meter_DATA_L_POSE] = value&0xff;
    _frame.Payload[Meter_DATA_M_POSE] = (value>>8)&0xff;
    _frame.Payload[Meter_DATA_H_POSE] = (value>>16)&0xff;
    _frame.Payload[Meter_CRC_POSE] = _getCrc(_frame.Payload);
    _frame.Index = _frame.Bytes;
    _sendFrame();
}

uint8_t Meter::_getCrc(uint8_t* arr){
    uint8_t crc = 0;
    for(uint8_t i = _frame.Index; i<(_frame.Bytes-1); i++){
        crc += arr[i];
    }
    crc = ~crc;
    return crc;
}
