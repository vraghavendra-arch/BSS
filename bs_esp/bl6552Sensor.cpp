#if POWER_PROVIDER == POWER_PROVIDER_BL6552
#include "bl6552Sensor.h"


/**
 * @brief Constructor for the BL6552 class.
 * 
 * This constructor initializes the BL6552 object with a given HardwareSerial instance.
 * It sets up the serial communication by starting the HardwareSerial with the specified
 * baud rate and port configuration, and then calls the initialization function.
 * 
 * @param hwSer Pointer to a HardwareSerial object used for serial communication.
 */
BL6552::BL6552(HardwareSerial* hwSer) {
    this->_serial = hwSer;
    static_cast<HardwareSerial*>(_serial)->begin(BL6552_UART_BAUD);
    // _init();
}

/**
 * @brief Destructor for the BL6552 class.
 * 
 * This destructor is called when an object of the BL6552 class is 
 * destroyed. It currently does not perform any specific cleanup 
 * operations, but can be extended in the future if necessary.
 */
BL6552::~BL6552()
{}

void BL6552::begin(){
    _init();
    // _getCoefficients();
}

void BL6552::_init(){
    //enable writing to calibration registers
    writeValue(REG_USR_WRPROT, 0x005555);
    writeValue(0xE1, 0x000950);
    writeValue(0xDD, 0x000D82);
    
    
    //initialize the BL6552 calibration registers here
    writeValue(REG_GAIN1, 0x000000); //current gain to 1
    writeValue(REG_GAIN2, 0x000000); //voltage gain to 1

    writeValue(REG_CFDIV, 0x10); //default: 0x10 -> 1, Used to control the accumulation speed of electric energy pulse counting
    writeValue(REG_MODE1, 0b1<<23); //default: 0x000000 -> 0x000000, 23rd bit -> Current WAVE waveform register output selection, default 0 to select the waveform of the normal effective value channel, and 1 to select the waveform output of the fast effective value channel 
    writeValue(REG_MODE2, 0x000000); //default: 0x000000 -> 0x000000
    writeValue(REG_MODE3, 0x011200); //default: 0x000000 -> 0x000000
    writeValue(REG_RST_ENG, 0x000000); // do not reset the energy counter
    


    //disable writing to calibration registers
    writeValue(0xDD, 0x000000);
    writeValue(0xE1, 0x000000);
    writeValue(REG_USR_WRPROT, 0x000000);

}

void BL6552::setAllConstants(float iS, float iK, float vS, float vK, float pS, float pK, float sS, float sK){
    _current_standard_value = iS;
    _current_k_constant = iK;
    _voltage_standard_value = vS;
    _voltage_k_constant = vK;
    _active_power_standard_value = pS;
    _active_power_k_constant = pK;
    _apparent_power_standard_value = sS;
    _apparent_power_k_constant = sK;

}

void BL6552::setCurrentConstant(float iS, float iK){
    _current_standard_value = iS;
    _current_k_constant = iK;
}

void BL6552::setVoltageConstant(float vS, float vK){
    _voltage_standard_value = vS;
    _voltage_k_constant = vK;
}

void BL6552::setActivePowerConstant(float pS, float pK){
    _active_power_standard_value = pS;
    _active_power_k_constant = pK;
}

void BL6552::setApparentPowerConstant(float sS, float sK){
    _apparent_power_standard_value = sS;
    _apparent_power_k_constant = sK;
}




/**
 * @brief      Get the RMS voltage of a given phase
 *
 * @details    This function reads the RMS voltage of a given phase and returns it
 *             in Volts.
 *
 * @param[in]  phase   The phase number to read the voltage from
 *
 * @return     The RMS voltage of the given phase in Volts, or 0.0 if there is an
 *             error
 */
float BL6552::getVoltageOfPhase(int phase){

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
        return BL6552_ERROR;
        break;
    }

    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }
    return ((float)(((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0])))*(_voltage_standard_value/_voltage_k_constant);

}

/**
 * @brief      Get the RMS current of a given phase
 *
 * @details    This function reads the RMS current of a given phase and returns it
 *             in Amperes.
 *
 * @param[in]  phase   The phase number to read the current from
 *
 * @return     The RMS current of the given phase in Amperes, or 0.0 if there is an
 *             error
 */
float BL6552::getCurrentOfPhase(int phase){
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
        return BL6552_ERROR;
        break;
    }

    if(getState() != NO_ERROR){ 
        return BL6552_ERROR;
    }
    return ((float)(((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0])))*(_current_standard_value/_current_k_constant);
}

/**
 * @brief      Get the Active Power of a given phase
 *
 * @details    This function reads the Active Power of a given phase and returns it
 *             in Watts.
 *
 * @param[in]  phase   The phase number to read the Active Power from
 *
 * @return     The Active Power of the given phase in Watts, or 0.0 if there is an
 *             error
 */
float BL6552::getActivePowerOfPhase(int phase){
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
            return BL6552_ERROR;
            break;
    }

    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }
    activepower = (((uint32_t)_rawHolder[2])<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]);
    invert = (activepower>>23) != 0;
    if(invert){
        activepower = ((~activepower)&0xFFFFFF) + 1;
    }
    return ((float)activepower)*(_active_power_standard_value/_active_power_k_constant);

}

/**
 * @brief      Get the Reactive Power of a given phase
 *
 * @details    This function reads the Reactive Power of a given phase and returns it
 *             in VAr.
 *
 * @param[in]  phase   The phase number to read the Reactive Power from
 *
 * @return     The Reactive Power of the given phase in VAr, or 0.0 if there is an
 *             error
 */
float BL6552::getReactivePowerOfPhase(int phase){
    uint32_t apparentpower;
    bool invert = false;
    float pf = getPowerFactorOfPhase(phase);
    if(pf==BL6552_ERROR){
        return BL6552_ERROR;
    }
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
            return BL6552_ERROR;
            break;
    }

    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }
    apparentpower = (((uint32_t)_rawHolder[2])<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]);
    invert = (apparentpower>>23) != 0;
    if(invert){
        apparentpower = ((~apparentpower)&0xFFFFFF) + 1;
    }

    float sinePhi = sqrt(1- pow(pf, 2));
    return ((float)apparentpower)*(_apparent_power_standard_value/_apparent_power_k_constant)*sinePhi;
}

/**
 * @brief      Get the Apparent Power of a given phase
 *
 * @details    This function reads the Apparent Power of a given phase and returns ituu6
 *             in VA.
 *
 * @param[in]  phase   The phase number to read the Apparent Power from
 *
 * @return     The Apparent Power of the given phase in VA, or 0.0 if there is an
 *             error
 */
float BL6552::getApparentPowerOfPhase(int phase){
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
            return BL6552_ERROR;
            break;
    }

    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }
    apparentpower = (((uint32_t)_rawHolder[2])<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]);
    invert = (apparentpower>>23) != 0;
    if(invert){
        apparentpower = ((~apparentpower)&0xFFFFFF) + 1;
    }
    return ((float)apparentpower)*(_apparent_power_standard_value/_apparent_power_k_constant);

}

/**
 * @brief      Get the Line frequency of the system
 *
 * @details    This function reads the Line frequency of the system and returns it
 *             in Hz.
 *
 * @return     The Line frequency of the system in Hz, or 0.0 if there is an
 *             error
 */
float BL6552::getLineFrequency(){
    readValue(REG_PERIOD);

    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }

    return 10000000.00/(((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0])*1.00);
}


/**
 * @brief      Get the Power factor of a given phase
 *
 * @details    This function reads the Power factor of a given phase and returns it
 *             as a float between 0.00 and 1.0.
 *
 * @param[in]  phase   The phase number to read the Power factor from
 *
 * @return     The Power factor of the given phase, or 0.00 if there is an
 *             error
 */
float BL6552::getPowerFactorOfPhase(int phase){
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
            return BL6552_ERROR;
            break;
    }

    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }
    powerFactor = (((uint32_t)_rawHolder[2])<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]);
    invert = (powerFactor>>23) != 0;
    if(invert){
        powerFactor = ((~powerFactor)&0xFFFFFF) + 1;
    }

    float pf =  (((float)powerFactor) * 1.00)/pow(2, 23);
    pf = pf>1 ? 1 : pf;
    return pf;

}

float BL6552::getEnergyDelta(){
    readValue(REG_LINE_WATTHR);
    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*1.00;
}

/**
 * @brief      Get the Total Energy in Wh
 *
 * @details    This function reads the Total Energy in Wh and returns it
 *
 * @return     The Total Energy in Wh, or 0.00 if there is an error
 */
float BL6552::getEnergyTotal(){
    readValue(REG_LINE_WATTHR);
    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*1.00;
}


/**
 * @brief      Get the phase angle of a given phase
 *
 * @details    This function reads the phase angle of a given phase and returns it
 *             in degrees.
 *
 * @param[in]  phase   The phase number to read the phase angle from
 *
 * @return     The phase angle of the given phase in degrees, or 0.00 if there is an
 *             error
 */
float BL6552::getPhaseAngleOfPhase(int phase){
    int phaseAngle;
    switch(phase){
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
            return BL6552_ERROR;
            break;
    }

    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }

    return ((float)(((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0])))*360*getLineFrequency()/500000;
}

/**
 * @brief      Get the raw data from the sensor
 *
 * @details    This function copies the raw data from the sensor into the provided
 *             array. The array must be at least 6 bytes long.
 *
 * @param[out] arr   The array to copy the raw data into
 */
void BL6552::getRawData(uint8_t* arr){
    memcpy(arr, _rawHolder, 6);
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
double BL6552::getVoltageRaw(uint8_t phase){
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
        return BL6552_ERROR;
        break;
    }

    if(getState() != NO_ERROR){
        return BL6552_ERROR;
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
double BL6552::getCurrentRaw(uint8_t phase){
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
        return BL6552_ERROR;
        break;
    }

    if(getState() != NO_ERROR){
        return BL6552_ERROR;   
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
double BL6552::getActivePowerRaw(uint8_t phase){
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
            return BL6552_ERROR;
            break;
    }

    if(getState() != NO_ERROR){
        return BL6552_ERROR;
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
double BL6552::getReactivePowerRaw(uint8_t phase){
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
            return BL6552_ERROR;
            break;
    }
    if(getState() != NO_ERROR){
        return BL6552_ERROR;
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
double BL6552::getApparentPowerRaw(uint8_t phase){
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
            return BL6552_ERROR;
            break;
    }
    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*1.00;
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
double BL6552::getPowerFactorRaw(uint8_t phase){
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
            return BL6552_ERROR;
            break;
    }
    if(getState() != NO_ERROR){
        return BL6552_ERROR;
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
double BL6552::getPhaseAngleRaw(uint8_t phase){
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
            return BL6552_ERROR;
            break;
    }
    
    if(getState() != NO_ERROR){
        return BL6552_ERROR;
    }
    return (((uint32_t)_rawHolder[2]<<16 | (uint32_t)_rawHolder[1]<<8 | (uint32_t)_rawHolder[0]))*1.00;
}

/**
 * @brief Retrieves the current state of the BL6552 device.
 * 
 * This function returns the value of the private member variable 
 * _state, which represents the current operational state of the 
 * BL6552 device. The state is returned as an 8-bit unsigned integer.
 * 
 * @return uint8_t The current state of the BL6552 device.
 */
uint8_t BL6552::getState(){
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
bool BL6552::readValue(uint8_t regAddr){
    _frame.Bytes = 6;
    _frame.Index = 2;
    _frame.Payload[BL6552_HEAD_POSE] = BL6552_READ_CMD;
    _frame.Payload[BL6552_ADD_POSE] = regAddr;
    _sendFrame();
    _receiveFrame();

    return _state == NO_ERROR;
}

/**
 * @brief Send prepared frame to chip
 * 
 * This function sends a frame to the BL6552 device, using the serial
 * interface provided by the _serial member variable. The frame to be
 * sent is stored in the member variable _frame, and the number of bytes
 * to send is stored in the Bytes member of the _frame variable.
 */
void BL6552::_sendFrame(){
    _serial->write(_frame.Payload, _frame.Index);
}

/**
 * @brief Receive a frame from the BL6552 device.
 * 
 * This function receives a frame from the BL6552 device, using the serial
 * interface provided by the _serial member variable. The frame is stored
 * in the member variable _frame, and the number of bytes received is stored
 * in the Bytes member of the _frame variable.
 * 
 * The function returns immediately if the frame is received successfully
 * (i.e., no error occurred). If an error occurs, the function sets the
 * internal state to the corresponding error value and returns.
 */
void BL6552::_receiveFrame(){
    int bytesAvailable;
    _frame.lastRcv = millis();
    while( (_frame.Index < _frame.Bytes) && !((millis()-_frame.lastRcv) > BL6552_TIMEOUT))
    {
        bytesAvailable = _serial->available();
        if( bytesAvailable > 0)
        {
            for( int bytesIndex=0; bytesIndex<bytesAvailable; bytesIndex++)
            {
                _frame.Payload[_frame.Index++] = (uint8_t)_serial->read();
            }
            _frame.lastRcv = millis();         //Reset timeout counter
        }
    }

    if(millis() - _frame.lastRcv > BL6552_TIMEOUT){
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

    memcpy(_rawHolder, _frame.Payload+BL6552_DATA_L_POSE, 3);
    _state = NO_ERROR;


}

/**
 * @brief Check frame checksum
 * 
 * This function checks the checksum of a received frame, by summing all
 * bytes except for the header and the checksum byte itself, and then
 * inverting the result (i.e., it calculates the two's complement of the
 * sum). It then compares this calculated checksum to the one stored in
 * the frame (at position BL6552_CRC_POSE). If the two values match, the
 * function returns true, otherwise false.
 * 
 * @return true if the checksum matches, false otherwise
 */
bool BL6552::_crcCheck(){
    uint8_t crc = 0;

    for(uint8_t i= BL6552_ADD_POSE; i<=BL6552_DATA_H_POSE; i++){
        crc += _frame.Payload[i];
    }
    crc = ~crc;


    return (crc == _frame.Payload[BL6552_CRC_POSE]);
    
}

/**
 * @brief Write a value to a register
 * 
 * This function sends a frame to the device to write a value to a register.
 * The frame is constructed from the register address and the value to be written.
 * The checksum is calculated and placed in the frame, and the frame is then
 * sent to the device.
 * 
 * @param regAddr Register address to write to
 * @param value Value to write to the register
 */
void BL6552::writeValue(uint8_t regAddr, uint32 value){
    _frame.Bytes = 6;
    _frame.Index = 1;
    _frame.Payload[BL6552_HEAD_POSE] = BL6552_WRITE_CMD;
    _frame.Payload[BL6552_ADD_POSE] = regAddr;
    _frame.Payload[BL6552_DATA_L_POSE] = value&0xff;
    _frame.Payload[BL6552_DATA_M_POSE] = (value>>8)&0xff;
    _frame.Payload[BL6552_DATA_H_POSE] = (value>>16)&0xff;
    _frame.Payload[BL6552_CRC_POSE] = _getCrc(_frame.Payload);
    _frame.Index = _frame.Bytes;
    _sendFrame();
}

/**
 * @brief Calculate checksum for a given array
 * 
 * This function calculates the checksum for a given array by summing all the
 * bytes in the array and then inverting the result (i.e., taking the two's
 * complement of the sum). The first byte of the array is skipped, as it is
 * reserved for the command byte.
 * 
 * @param arr The array to calculate the checksum for
 * @return The calculated checksum
 */
uint8_t BL6552::_getCrc(uint8_t* arr){
    uint8_t crc = 0;
    for(uint8_t i = _frame.Index; i<(_frame.Bytes-1); i++){
        crc += arr[i];
    }
    crc = ~crc;
    return crc;
}

#endif
