
#include "Arduino.h"
#include "SoftwareSerial.h"


//Electrical parameter register (read only)
#define REG_IC_WAVE           0x02  // C-phase current waveform register
#define REG_IB_WAVE           0x03  // B-phase current waveform register
#define REG_IA_WAVE           0x04  // A-phase current waveform register
#define REG_IN_WAVE           0x05  // Neutral current waveform register

#define REG_VA_WAVE           0x08  // A phase voltage waveform register
#define REG_VB_WAVE           0x09  // B phase voltage waveform register
#define REG_VC_WAVE           0x0A  // C phase voltage waveform register

#define REG_IRMS_C            0x0D  // C-phase current RMS register, unsigned
#define REG_IRMS_B            0x0E  // B-phase current RMS register, unsigned
#define REG_IRMS_A            0x0F  // A-phase current RMS register, unsigned
#define REG_IN_RMS            0x10  // Neutral current RMS register, unsigned

#define REG_VRMS_A            0x13  // A-phase voltage RMS register, unsigned
#define REG_VRMS_B            0x14  // B-phase voltage RMS register, unsigned
#define REG_VRMS_C            0x15  // C-phase voltage RMS register, unsigned

#define REG_IC_FAST_RMS       0x18  // C-phase current fast RMS register, unsigned
#define REG_IB_FAST_RMS       0x19  // B-phase current fast RMS register, unsigned
#define REG_IA_FAST_RMS       0x1A  // A-phase current fast RMS register, unsigned
#define REG_IN_FAST_RMS       0x1B  // Neutral current fast RMS register, unsigned

#define REG_VA_FAST_RMS       0x1E  // A phase voltage fast RMS register, unsigned
#define REG_VB_FAST_RMS       0x1F  // B phase voltage fast RMS register, unsigned
#define REG_VC_FAST_RMS       0x20  // C phase voltage fast RMS register, unsigned

#define REG_WATT_A            0x22  // A phase active power register
#define REG_WATT_B            0x23  // B phase active power register
#define REG_WATT_C            0x24  // C phase active power register
#define REG_WATT              0x25  // Combined phase active power register

#define REG_VA_A              0x26  // A phase apparent power register
#define REG_VA_B              0x27  // B phase apparent power register
#define REG_VA_C              0x28  // C phase apparent power register
#define REG_VA                0x29  // Combined phase apparent power register

#define REG_FVAR_A            0x2A  // A phase (fundamental wave) reactive power register
#define REG_FVAR_B            0x2B  // B phase (fundamental wave) reactive power register
#define REG_FVAR_C            0x2C  // C phase (fundamental wave) reactive power register
#define REG_FVAR              0x2D  // Combined phase (fundamental wave) reactive power register

#define REG_PERIOD            0x2E  // Line voltage frequency period register (optional channel)

#define REG_CF_A_CNT          0x2F  // A phase active pulse count, unsigned
#define REG_CF_B_CNT          0x30  // B phase active pulse count, unsigned
#define REG_CF_C_CNT          0x31  // C phase active pulse count, unsigned
#define REG_CF_CNT            0x32  // Combined phase active pulse count, unsigned

#define REG_CFP_A_CNT         0x33  // A phase positive active pulse count, unsigned
#define REG_CFP_B_CNT         0x34  // B phase positive active pulse count, unsigned
#define REG_CFP_C_CNT         0x35  // C phase positive active pulse count, unsigned
#define REG_CFP_CNT           0x36  // Combined phase positive active pulse count, unsigned

#define REG_CFN_A_CNT         0x37  // A phase negative active pulse count, unsigned
#define REG_CFN_B_CNT         0x38  // B phase negative active pulse count, unsigned
#define REG_CFN_C_CNT         0x39  // C phase negative active pulse count, unsigned
#define REG_CFN_CNT           0x3A  // Combined phase negative active pulse count, unsigned

#define REG_CFQ_A_CNT         0x3B  // A phase reactive pulse count, unsigned
#define REG_CFQ_B_CNT         0x3C  // B phase reactive pulse count, unsigned
#define REG_CFQ_C_CNT         0x3D  // C phase reactive pulse count, unsigned
#define REG_CFQ_CNT           0x3E  // Combined phase reactive pulse count, unsigned

#define REG_CFQ1_CNT          0x3F  // The first quadrant reactive pulse count, unsigned
#define REG_CFQ2_CNT          0x40  // The second quadrant reactive pulse count, unsigned
#define REG_CFQ3_CNT          0x41  // The third quadrant reactive pulse count, unsigned
#define REG_CFQ4_CNT          0x42  // The fourth quadrant reactive pulse count, unsigned

#define REG_CFS_A_CNT         0x43  // A phase pulse count apparent, unsigned
#define REG_CFS_B_CNT         0x44  // B phase pulse count apparent, unsigned
#define REG_CFS_C_CNT         0x45  // C phase pulse count apparent, unsigned
#define REG_CFS_CNT           0x46  // Combined phase pulse count apparent, unsigned

#define REG_PF_A              0x47  // A phase power factor register
#define REG_PF_B              0x48  // B phase power factor register
#define REG_PF_C              0x49  // C phase power factor register
#define REG_PF                0x4A  // Combined phase power factor register

#define REG_LINE_WATTHR       0x4B  // Line cycle cumulative active energy register
#define REG_LINE_VARHR        0x4C  // Line cycle cumulative reactive energy register

#define REG_SIGN              0x4D  // CF sign bit

#define REG_ANGLE_AB          0x4E  // Waveform angle register of voltage A phase and voltage B phase
#define REG_ANGLE_BC          0x4F  // Waveform angle register of voltage B phase and voltage C phase
#define REG_ANGLE_AC          0x50  // Waveform angle register of voltage A phase and voltage C phase

#define REG_ANGLE_A           0x51  // A phase voltage and current waveform angle register
#define REG_ANGLE_B           0x52  // B phase voltage and current waveform angle register
#define REG_ANGLE_C           0x53  // C phase voltage and current waveform angle register

#define REG_STATUS1           0x54  // Interrupt status register 1, unsigned
#define REG_STATUS2           0x55  // Interrupt status register 2, unsigned

#define REG_I_SUM             0x56  // The sum of the instantaneous waveforms of the three-phase current
#define REG_I_SUM_RMS         0x57  // The RMS of the three-phase current instantaneous waveform sum, unsigned
#define REG_I_SUM_FAST_RMS    0x58  // The fast RMS of the three-phase current instantaneous waveform sum, unsigned

#define REG_VAR_A             0x5A  // A phase (full wave) reactive power register
#define REG_VAR_B             0x5B  // B phase (full wave) reactive power register
#define REG_VAR_C             0x5C  // C phase (full wave) reactive power register
#define REG_VAR               0x5D  // Combined phase (full wave) reactive power register


//Calibration Registers 1 (read and write)
#define REG_GAIN1              0x60  // Channel PGA gain adjustment register, [11:8]: C-phase current; [15:12]: B-phase current; [19:16]: A-phase current; [23:20]: Neutral line current
#define REG_GAIN2              0x61  // Channel PGA gain adjustment register, [11:8]: Phase A voltage; [15:12]: Phase B voltage; [19:16]: Phase C voltage

#define REG_IRMS_P1            0x62  // The angle difference segment point defines P1, which satisfies IRMSmin<P1<P2<IRMSmax
#define REG_IRMS_P2            0x63  // The angle difference segment point defines P2, which satisfies IRMSmin<P1<P2<IRMSmax

#define REG_IA_PHCAL           0x64  // Phase calibration of Phase A Current channel
#define REG_IB_PHCAL           0x65  // Phase calibration of Phase B Current channel
#define REG_IC_PHCAL           0x66  // Phase calibration of Phase C Current channel

#define REG_VA_PHCAL           0x67  // Phase calibration of Phase A Voltage channel
#define REG_VB_PHCAL           0x68  // Phase calibration of Phase B Voltage channel
#define REG_VC_PHCAL           0x69  // Phase calibration of Phase C Voltage channel

#define REG_VAR_PHCAL_I        0x6A  // Reactive power phase correction (fine tuning) for the A-phase and B-phase current channel in reactive power calculation
#define REG_VAR_PHCAL_V        0x6B  // Reactive power phase correction (fine adjustment) for the A-phase, B-phase, and C-phase voltage channel in reactive power calculation

#define REG_IC_RMSGN           0x6D  // Phase C Current RMS gain adjust
#define REG_IB_RMSGN           0x6E  // Phase B Current RMS gain adjust
#define REG_IA_RMSGN           0x6F  // Phase A Current RMS gain adjust
#define REG_IN_RMSGN           0x70  // Neutral Current RMS gain adjust

#define REG_VA_RMSGN           0x73  // Phase A Voltage RMS gain adjust
#define REG_VB_RMSGN           0x74  // Phase B Voltage RMS gain adjust
#define REG_VC_RMSGN           0x75  // Phase C Voltage RMS gain adjust

#define REG_IC_RMSOS           0x78  // Phase C Current RMS offset
#define REG_IB_RMSOS           0x79  // Phase B Current RMS offset
#define REG_IA_RMSOS           0x7A  // Phase A Current RMS offset
#define REG_IN_RMSOS           0x7B  // Neutral Current RMS offset

#define REG_VA_RMSOS           0x7E  // Phase A Voltage RMS offset
#define REG_VB_RMSOS           0x7F  // Phase B Voltage RMS offset
#define REG_VC_RMSOS           0x80  // Phase C Voltage RMS offset

#define REG_WA_LOS_A           0x82  // [23:12] Corresponding to Phase A active power small signal compensation register, complement
#define REG_WA_LOS_B           0x83  // [23:12] Corresponding to Phase B active power small signal compensation register, complement
#define REG_WA_LOS_C           0x84  // [23:12] Corresponding to Phase C active power small signal compensation register, complement

#define REG_VAR_LOS_A          0x82  // [11:0] Corresponding to Phase A reactive power small signal compensation register, complement
#define REG_VAR_LOS_B          0x83  // [11:0] Corresponding to Phase B reactive power small signal compensation register, complement
#define REG_VAR_LOS_C          0x84  // [11:0] Corresponding to Phase C reactive power small signal compensation register, complement

#define REG_FVAR_LOS_A         0x85  // FVAR_LOS_A: [11:0] Corresponding to the reactive power small signal compensation register, complement.
#define REG_FVAR_LOS_B         0x86  // FVAR_LOS_B: [11:0] Corresponding to the reactive power small signal compensation register, complement.
#define REG_FVAR_LOS_C         0x87  // FVAR_LOS_C: [11:0] Corresponding to the reactive power small signal compensation register, complement.

#define REG_VAR_CREEP_WA_CREEP 0x88  // VAR_CREEP/WA_CREEP: [23:12] Reactive anti-creeping power threshold register; [11:0] Active anti-creeping power threshold register.
#define REG_VAR_CREEP2_WA_CREEP2 0x89  // VAR_CREEP2/WA_CREEP2: [23:12] Combined reactive power anti-creeping power threshold register; [11:0] Combined active anti-creeping power threshold register.
#define REG_REVP_CREEP_RMS_CREEP 0x8A  // REVP_CREEP/RMS_CREEP: [23:12] Reverse indication threshold register REVP_CREEP; [11:0] Effective value small signal threshold register RMS_CREEP.

#define REG_FAST_RMS_CTRL      0x8B  // FAST_RMS_CTRL: [23:21] Channel fast effective value register refresh time, half cycle and N cycle can be selected; [20:0] Channel fast effective value threshold register.
#define REG_PKLVL_V_PKLVL      0x8C  // PKLVL/V_PKLVL: [23:12] Current peak value threshold register L_PKLVL; [11:0] Voltage peak threshold register V_PKLVL.
#define REG_ISUMLVL            0x8D  // ISUMLVL: For the current comparison threshold register, select NLRMS to compare with the ISUMLVL register.
#define REG_SAGCYC_ZXTOUT      0x8E  // SAGCYC/ZXTOUT: [23:16] SAG period register SAGCYC; [15:0] Zero-crossing time-out register ZXTOUT.
#define REG_SAGLVL_LINECYC     0x8F  // SAGLVL/LINECYC: [23:12] SAG threshold register SAGLVL; [11:0] Line energy accumulation cycle number register LINECYC.
#define REG_IN_PHCAL           0x90  // IN_PHCAL: Phase calibration of Phase N current channel.
#define REG_ISUM_RMSGN         0x91  // ISUM_RMSGN: Corresponding channel effective value gain adjustment register.
#define REG_ISUM_RMSOS         0x92  // ISUM_RMSOS: Corresponding channel effective value offset correction register.
#define REG_ADC_PD             0x93  // ADC_PD: 7 channels ADC enable control; when set to 1, the corresponding channel ADC is closed.

#define REG_MODE1              0x96  // MODE1: User mode selection register 1.
#define REG_MODE2              0x97  // MODE2: User mode selection register 2.
#define REG_MODE3              0x98  // MODE3: User mode selection register 3.
#define REG_MASK1              0x9A  // MASK1: User mode selection register.
#define REG_MASK2              0x9B  // Interrupt mask register, which controls whether an interrupt generates a valid IRQ2 output, please refer to the description of "Interrupt Mask Register" for details 
#define REG_RST_ENG            0x9D  // Energy clearing setting register, see the description of "Energy clearing setting register" for details 
#define REG_USR_WRPROT         0x9E  // User write protection setting register.
#define REG_SOFT_RESET         0x9F  // When the input is 5A5A5A, reset the electrical parameter register; When the input is 55AA55, the calibration register is reset: Reg60~reg9F, RegA0~RegD0

//calibration register 2 (read and write)
#define REG_IC_CHGN  0xA1  // Current C channel gain adjustment register, complement
#define REG_IB_CHGN  0xA2  // Current B channel gain adjustment register, complement
#define REG_IA_CHGN  0xA3  // Current A channel gain adjustment register, complement
#define REG_IN_CHGN  0xA4  // Current N channel gain adjustment register, complement

#define REG_VA_CHGN  0xA7  // Voltage A channel gain adjustment register, complement
#define REG_VB_CHGN  0xA8  // Voltage B channel gain adjustment register, complement
#define REG_VC_CHGN  0xA9  // Voltage C channel gain adjustment register, complement

#define REG_IC_CHOS  0xAC  // Current C channel offset adjustment register, complement
#define REG_IB_CHOS  0xAD  // Current B channel offset adjustment register, complement
#define REG_IA_CHOS  0xAE  // Current A channel offset adjustment register, complement
#define REG_IN_CHOS  0xAF  // Current N channel offset adjustment register, complement

#define REG_VA_CHOS  0xB2  // Voltage A channel offset adjustment register, complement
#define REG_VB_CHOS  0xB3  // Voltage B channel offset adjustment register, complement
#define REG_VC_CHOS  0xB4  // Voltage C channel offset adjustment register, complement

#define REG_WATTGN_A  0xB6  // A phase active power gain adjustment register, complement
#define REG_WATTGN_B  0xB7  // B-phase active power gain adjustment register, complement
#define REG_WATTGN_C  0xB8  // C-phase active power gain adjustment register, complement

#define REG_VARGN_A  0xB9  // A phase reactive power gain adjustment register, complement
#define REG_VARGN_B  0xBA  // B-phase reactive power gain adjustment register, complement
#define REG_VARGN_C  0xBB  // C-phase reactive power gain adjustment register, complement

#define REG_FVARGN_A  0xBC  // A phase fundamental reactive power gain adjustment register, complement
#define REG_FVARGN_B  0xBD  // B-phase fundamental reactive power gain adjustment register, complement
#define REG_FVARGN_C  0xBE  // C-phase fundamental reactive power gain adjustment register, complement

#define REG_VAGN_A  0xBF  // A phase apparent power gain adjustment register, complement
#define REG_VAGN_B  0xC0  // B-phase apparent power gain adjustment register, complement
#define REG_VAGN_C  0xC1  // C-phase apparent power gain adjustment register, complement

#define REG_WATTOS_A  0xC2  // A phase active power offset adjustment register, complement
#define REG_WATTOS_B  0xC3  // B-phase active power offset adjustment register, complement
#define REG_WATTOS_C  0xC4  // C-phase active power offset adjustment register, complement

#define REG_VAROS_A  0xC5  // A phase reactive power offset adjustment register, complement
#define REG_VAROS_B  0xC6  // B-phase reactive power offset adjustment register, complement
#define REG_VAROS_C  0xC7  // C-phase reactive power offset adjustment register, complement

#define REG_FVAROS_A  0xC8  // A phase fundamental reactive power offset adjustment register, complement
#define REG_FVAROS_B  0xC9  // B-phase fundamental reactive power offset adjustment register, complement
#define REG_FVAROS_C  0xCA  // C-phase fundamental reactive power offset adjustment register, complement

#define REG_VAOS_A  0xCB  // A phase apparent power offset adjustment register, complement
#define REG_VAOS_B  0xCC  // B-phase apparent power offset adjustment register, complement
#define REG_VAOS_C  0xCD  // C-phase apparent power offset adjustment register, complement

#define REG_CFDIV  0xCE  // Active CF scaling register
#define REG_AT_SEL  0xCF  // AT1–3 logic output pin configuration

//*********************************************************************************************** */
//Constants
#define VOLTAGE_STANDARD_VALUE    233.8                      //239.6333
#define VOLTAGE_K_CONSTANT        2448899.4               //1456904.27576                    //2502632.33
#define VOLTAGE_CONSTANT          VOLTAGE_STANDARD_VALUE/VOLTAGE_K_CONSTANT

#define CURRENT_STANDARD_VALUE    6.7324                  //2.19
#define CURRENT_K_CONSTANT        526959.2                   //174132.0
#define CURRENT_CONSTANT          CURRENT_STANDARD_VALUE/CURRENT_K_CONSTANT

#define ACTIVE_POWER_STANDARD_VALUE    1570.65                 //519.525096
#define ACTIVE_POWER_K_CONSTANT     307289.75               // 102790.00
#define ACTIVE_POWER_CONSTANT      ACTIVE_POWER_STANDARD_VALUE/ACTIVE_POWER_K_CONSTANT

#define APPARENT_POWER_STANDARD_VALUE 520.5567
#define APPARENT_POWER_K_CONSTANT    103178.333
#define APPARENT_POWER_CONSTANT      APPARENT_POWER_STANDARD_VALUE/APPARENT_POWER_K_CONSTANT



//Communication command data
#define Meter_UART_BAUD               4800
#define Meter_DEFAULT_PORT_CONFIG     SERIAL_8N1

#define Meter_READ_CMD   0x35
#define Meter_WRITE_CMD  0xCA

#define Meter_TIMEOUT    50 //ms
#define Meter_MAX_FRAME  16

#define Meter_HEAD_POSE     0
#define Meter_ADD_POSE      1
#define Meter_DATA_L_POSE   2
#define Meter_DATA_M_POSE   3
#define Meter_DATA_H_POSE   4
#define Meter_CRC_POSE      5

#define STANDERED_VOLTAGE_VALUE 230 // [TODO] need to see this value
enum bl6522States { NO_ERROR, TIMEOUT_ERR, BADFRAME_ERR, CRCFRAME_ERR, READY, COMM_ERROR };

typedef struct
{
    uint8_t Bytes;
    uint8_t Index;
    uint8_t Payload[Meter_MAX_FRAME];
    uint64_t lastRcv;
    uint64_t lastUpdate;
} frame_t;

class Meter 
{
    public:
        Meter(HardwareSerial* hwSer); // Constructor
        ~Meter(); // Destructor

        // void begin();
        bool readValue(uint8_t regAddr);
        void writeValue(uint8_t regAddr, uint32 value);
        void getRawData(uint8_t *arr);
        uint8_t getState();
        void begin();
        float getVoltageOfPhase(int phase);
        float getCurrentOfPhase(int phase);
        float getActivePowerOfPhase(int phase);
        float getReactivePowerOfPhase(int phase);
        float getApparentPowerOfPhase(int phase);
        float getPowerFactorOfPhase(int phase);
        float getLineFrequency();

        double getVoltageRaw(uint8_t phase);
        double getCurrentRaw(uint8_t phase);
        double getActivePowerRaw(uint8_t phase);
        double getReactivePowerRaw(uint8_t phase);
        double getApparentPowerRaw(uint8_t phase);
        double getPowerFactorRaw(uint8_t phase);
        double getPhaseAngleRaw(uint8_t phase);
        uint32_t getRegistorValue(uint8_t regAddr);
        
    
    private:
        Stream* _serial;
        bl6522States _state;
        uint8_t _rawHolder[5];
        frame_t _frame;

        void _sendFrame();
        void _receiveFrame();
        bool _crcCheck();
        uint8_t _getCrc(uint8_t* arr);
        void _init();
};


