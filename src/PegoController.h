#include "RegisterDescription.h"
#include <limits.h>
#include <float.h>
#include <Arduino.h>

#define RS485_DEFAULT_BAUD_RATE 9600
#define RS485_DEFAULT_SERIAL_CONFIG SERIAL_8N1

#define READ_ERROR SHRT_MIN
#define READ_ERROR_FLOAT FLT_MIN

#define DEFAULT_PERIPHERAL_ID 1
#define ECP_202 // Enables the ECP 202 base features

class PegoController {
private:
    // The peripheral's ModBus address
    uint8_t _peripheralID;

    unsigned long _lastResponsive;

    // The baud rate for the RS485 connection
    unsigned long _baudRate;

    // The serial configuration for the RS485 connection
    uint16_t _serialConfig;

    /**
     * @brief Returns the requested bit from the least significant byte (little endian)     
     * @param value A two byte (word) value
     * @param bit The bit index that shall be read starting from 0 (= least significant bit).
     * @return 1 or 0 depending on the bit value.
     */
    byte getLSByteBit(int16_t value, byte bit);

    /**
     * @brief Returns the requested bit from the most significant byte (little endian)     
     * @param value A two byte (word) value
     * @param bit The bit index that shall be read starting from 0 (= least significant bit).
     * @return 1 or 0 depending on the bit value.
     */
    byte getMSByteBit(int16_t value, byte bit);

    /**
     * @brief Sets the requested bit from the least significant byte (little endian) to 1
     * @param value A two byte (word) value
     * @param bit The bit index that shall be set to 1 starting from 0 (= least significant bit).     
     */
    void setLSByteBit(int16_t *value, byte bit);

    /**
     * @brief Sets the requested bit from the most significant byte (little endian) to 1
     * @param value A two byte (word) value
     * @param bit The bit index that shall be set to 1 starting from 0 (= least significant bit).     
     */
    void setMSByteBit(int16_t *value, byte bit);

protected:
    /**
     * @brief Converts the input for signed values if necessary.
     * Conversion procedure:
     * - If the value contained in the register is included between 0 and 32767, it represents a positive or null number (the results is the value itself)
     * - If the value contained in the register is included between 32768 and 65535, it represents a negative number (the results is the register value ‐ 65536)
     * @param value The register value to be converted
     * @param description A RegisterDescription object providing the info about the requested register.
     * @return int16_t 
     */
    int16_t convertToSignedValue(uint16_t value, RegisterDescription description);
    
public:
    /**
     * @brief Construct a new Pego Controller object     
     * @param baudRate The baud rate to be used for the serial communication. Default: 9600
     * @param peripheralID The ModBus server ID / peripheral ID of the Pego device. Default: 1
     * @param serialConfig The serial configuration to be used with ArduinoModbus. Default: SERIAL_8N1 @see https://www.arduino.cc/en/ArduinoModbus/ArduinoModbus
     */
    PegoController(unsigned long baudRate = RS485_DEFAULT_BAUD_RATE, uint8_t peripheralID = DEFAULT_PERIPHERAL_ID, uint16_t serialConfig = RS485_DEFAULT_SERIAL_CONFIG);    
    
    /**
     * @brief Starts the communication with the Pego controller device over ModBus.
     * It uses the serial communication options provided via constructor.
     * @return True if the Modbus RTU Client was started successfully. False, if it failed.
     */
    bool begin();

    /**
     * @brief Executed a dummy read request to figure out if the device is responsive.
     * @return true if the device responded to the request, false otherwise.
     */
    bool responsive();

    /**
     * @brief Reads a word (2byte) value from the device's register and converts it to a signed number if necessary.
     * Note that this function does not apply any multiplication factor.     
     * @param description A RegisterDescription object providing the info about the requested register.
     * @return int16_t The numeric value read from the register in little endian format.
     */
    int16_t readModbusRegister(RegisterDescription description);

    /**
     * @brief Writes a word (2byte) value to the device's register.
     * Note that this function does not apply any multiplication factor.
     * @param description A RegisterDescription object providing the info about the requested register.     
     * @return A boolean indicating whether the write operation was successful.
     */
    bool writeModbusRegister(RegisterDescription description, int16_t value);

    /**
     * @brief Applies the multiplication factor to a register value definded by the register description.
     * This is necessary to convert the integer values transferred over the wire into floats.
     * @param value The value retrieved from the register
     * @param description A RegisterDescription object providing the info about the requested register.
     * @return The register value multiplied by the pre-defined factor.
     */
    float applyMultiplicationFactor(int16_t value, RegisterDescription description);

    /**
     * @brief Un-applies the multiplication factor to a register value definded by the register description.
     * This is necessary to convert the float values back to integers to transfer them over the wire.
     * @param value The value to be written to the register
     * @param description A RegisterDescription object providing the info about the requested register.
     * @return The value divided by the pre-defined factor.
     */
    int16_t unapplyMultiplicationFactor(float value, RegisterDescription registerEntry);

    // ANALOG INPUTS REGISTERS

    /**
     * @brief Get the Ambient Temperature of the device
     * - Unit: °C
     * - Resolution: 0,1°C
     * - Range: -45°C .. +99°C
     * - Values > +99°C indicate broken probe
     * @return the temperature in °C
     */
    float getAmbientTemperature();

    /**
     * @brief Get the Evaporator Temperature of the device
     * - Unit: °C
     * - Resolution: 0,1°C
     * - Range: -45°C .. +99°C
     * - Values > +99°C indicate broken probe
     * @return the temperature in °C
     */
    float getEvaporatorTemperature();

    // PARAMETER REGISTERS
    
    /**
     * @brief Get the Temperature Set Point. This is the target temperature of the cold store.
     * - Unit: °C
     * - Resolution: 0.1 °C steps, with sign 
     * - Range: LSE..HSE
     * @return the temperature in °C 
     */
    float getTemperatureSetPoint();

    /**
     * @brief Set the Temperature Set Point. This is the target temperature of the cold store.
     * - Unit: °C
     * - Resolution: 0.1 °C steps, with sign 
     * - Range: LSE..HSE
     * @param value The temperature in °C.
     * @return true if the value was updated succssfully, false otherwise.
     */
    bool setTemperatureSetPoint(float value);
    
    /**
     * @brief Get the Temperature Differential. The value refers to the main set point.
     * - Device Variable: r0
     * - Unit: °C
     * - Resolution: 0.1 °C steps
     * - Range: 0.2..10.0 °C
     * @return the temperature in °C  
     */
    float getTemperatureDifferential();

    /**
     * @brief Set the Temperature Differential. The value refers to the main set point
     * - Device Variable: r0
     * - Unit: °C
     * - Resolution: 0.1 °C steps
     * - Range: 0.2..10.0 °C
     * @param value The temperature in °C. 
     * @return true if the value was updated succssfully, false otherwise.
     */
    bool setTemperatureDifferential(float value);
    
    /**
    * @brief Get the defrosting period, the interval for defrost. If d0 = 0 the cyclic defrosts are disabled 
    * - Device Variable: d0
    * - Unit: hours
    * - Resolution: 1 hour steps
    * - Range: 0..24 hours (0 = disabled)
    * @return the period in hours
    */
    int16_t getDefrostingPeriod();

    /**
     * @brief Set the Defrosting period, the interval for defrost. If d0 = 0 the cyclic defrosts are disabled 
    * - Device Variable: d0
    * - Unit: hours
    * - Resolution: 1 hour steps
    * - Range: 0..24 hours (0 = disabled)     
     * @param value The period in hours
     * @return true if the value was updated succssfully, false otherwise.
     */
    bool setDefrostingPeriod(int16_t value);
    
    /**
    * @brief Get the end temperature of the defrosting process. 
    * Defrost is not carried out if the temperature read by the defrost probe is greater than the value of d2.
    * (If the probe is broken defrost is performed based on time)
    * - Device Variable: d2
    * - Unit: °C
    * - Resolution: 1 °C steps, with sign
    * - Range: -35..+45 °C
    * @return the temperature in °C
    */
    int16_t getEndOfDefrostingTemperature();

    /**
    * @brief Set the end temperature of the defrosting process. 
    * Defrost is not carried out if the temperature read by the defrost probe is greater than the value of d2.
    * (If the probe is broken defrost is performed based on time)
    * - Device Variable: d2
    * - Unit: °C
    * - Resolution: 1 °C steps, with sign
    * - Range: -35..+45 °C
    * @param value The temperature in degree celsius
    * @return true if the value was updated succssfully, false otherwise.
    */
    bool setEndOfDefrostingTemperature(int16_t value);
    
    /**
    * @brief Get the maximum defrosting duration
    * - Device Variable: d3
    * - Unit: minutes
    * - Resolution: 1 minute steps.
    * - Range: 1..240 minutes
    * @return the duration in minutes
    */
    int16_t getMaxDefrostingDuration();

    /**
     * @brief Set the maximum defrosting duration
     * 
     * @param value the duration in minutes
     * @return true if the value was updated succssfully, false otherwise.
     */
    bool setMaxDefrostingDuration(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: d7
    * - Unit: minutes
    * - Resolution: 1 minute steps.
    * - Range: 0..10 minutes
    * @return the duration in minutes
    */
    int16_t getDrippingDuration();

    /**
     * @brief Set the dripping duration
     * 
     * @param value The duration in minutes (0 = disabled)
     * @return true if the value was updated succssfully, false otherwise.
     */
    bool setDrippingDuration(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: F5
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getFansStopDurationPostDefrosting();
    bool setFansStopDurationPostDefrosting(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: A1
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getTemperatureAlarmMinimumThreshold();
    bool setTemperatureAlarmMinimumThreshold(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: A2
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getTemperatureAlarmMaximumThreshold();
    bool setTemperatureAlarmMaximumThreshold(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: F3
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getFansStatusWithStoppedCompressor();
    bool setFansStatusWithStoppedCompressor(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: F4
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    bool getFansStopInDefrosting();
    bool setFansStopInDefrosting(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: dE
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    bool getEvaporatorProbeExclusion();
    bool setEvaporatorProbeExclusion(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: ALd
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getTemperatureAlarmSignalingDelay();
    bool setTemperatureAlarmSignalingDelay(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: C1
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getCompressorReStartingDelay();
    bool setCompressorReStartingDelay(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: CAL
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    float getAmbientProbeCalibration();
    bool setAmbientProbeCalibration(float value);
    
    /**
    * @brief brief description
    * - Device Variable: doC
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getCompressorSafetyTimeForDoorSwitch();
    bool setCompressorSafetyTimeForDoorSwitch(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: tdo
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getCompressorRestartTimeAfterDoorOpening();
    bool setCompressorRestartTimeAfterDoorOpening(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: FSt
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getFansBlockageTemperature();
    bool setFansBlockageTemperature(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: Fd
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getDifferentialOnFansBlockage();
    bool setDifferentialOnFansBlockage(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: LSE
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getTemperatureSetPointMinimumLimit();
    bool setTemperatureSetPointMinimumLimit(int16_t value);
    
    /**
    * @brief brief description
    * - Device Variable: HSE
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getTemperatureSetPointMaximumLimit();
    bool setTemperatureSetPointMaximumLimit(int16_t value);

    #ifdef ECP_202
    
    /**
    * @brief brief description
    * - Device Variable: StA
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getTemperatureSettingForAuxRelay();
    bool setTemperatureSettingForAuxRelay(int16_t value);

    /**
    * @brief brief description
    * - Device Variable: dPo
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    bool getDefrostAtPowerOnStatus();
    bool setDefrostAtPowerOnStatus(bool value);

    /**
    * @brief brief description
    * - Device Variable: dSE
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    bool getSmartDefrostStatus();
    bool setSmartDefrostStatus(bool value);

    /**
    * @brief brief description
    * - Device Variable: dSt
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    //  (if dSE=1)
    int16_t getSmartDefrostSetpoint();
    bool setSmartDefrostSetpoint(int16_t value);

    /**
    * @brief brief description
    * - Device Variable: CE1
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getDurationOfCompressorOnTimeWithFaultyAmbientProbe();
    bool setDurationOfCompressorOnTimeWithFaultyAmbientProbe(int16_t value);

    /**
    * @brief brief description
    * - Device Variable: CE2
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getDurationOfCompressorOffTimeWithFaultyAmbientProbe();
    bool setDurationOfCompressorOffTimeWithFaultyAmbientProbe(int16_t value);

    /**
    * @brief brief description
    * - Device Variable: nSC
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    float getCorrectionFactorForTheSETButtonDuringNightOperation();
    bool setCorrectionFactorForTheSETButtonDuringNightOperation(float value);

    /**
    * @brief brief description
    * - Device Variable: bEE
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    bool getBuzzerEnableStatus();
    bool setBuzzerEnableStatus(bool value);

    /**
    * @brief brief description
    * - Device Variable: F6
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getEvaporatorFansActivationForAirRecirculation();
    bool setEvaporatorFansActivationForAirRecirculation(int16_t value);

    /**
    * @brief brief description
    * - Device Variable: F7
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getEvaporatorFansDurationForAirRecirculation();
    bool setEvaporatorFansDurationForAirRecirculation(int16_t value);

    /**
    * @brief brief description
    * - Device Variable: mOd
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getThermostatFunctioningMode();

    /**
    * @brief brief description
    * - Device Variable: d1
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getDefrostType();

    /**
    * @brief brief description
    * - Device Variable: dFd
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getDisplayViewingDuringDefrost();

    /**
    * @brief brief description
    * - Device Variable: In1
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getInput1Setting();

    /**
    * @brief brief description
    * - Device Variable: In2
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getInput2Setting();
    
    /**
    * @brief brief description
    * - Device Variable: AU1
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getAuxiliaryRelay1Control();

    /**
    * @brief brief description
    * - Device Variable: AU2
    * - Unit: 
    * - Resolution: 
    * - Range: 
    * @return
    */
    int16_t getAuxiliaryRelay2Control();

    #endif


    // INPUTS / OUTPUTS / ALARMS STATUS REGISTERS

    // # Output Status Register
    #ifdef ECP_202
    bool getHotResistanceStatus();
    bool getStandByStatus();
    #endif

    bool getDrippingStatus();
    bool getColdRoomLightRelayStatus();
    bool getFansRelayStatus();
    bool getDefrostRelayStatus();
    bool getCompressorRelayStatus();


    // # Input Status Register

    #ifdef ECP_202
    //  (energy saving)
    bool getNightDigitalInputStatus();
    bool getRemoteStopDefrostStatus();
    bool getRemoteStartDefrostStatus();
    bool getRemoteStandByStatus();
    bool getPumpDownInputStatus();
    #endif

    // Status Code: (E8)
    bool getManInColdRoomAlarmStatus();

    // Status Code: (EC)
    bool getCompressorProtectionStatus();

    /**
     * @brief Get the status of the Door Switch
     * @return true if the door switch is open, meaning the door is open or 
     * false if the door switch is closed, meaning the door is closed.
     */
    bool getDoorSwitchStatus();

    // # Alarm Status Register

    #ifdef ECP_202
    // Status Code: (E9)
    bool getLightAlarmStatus();

    // Status Code: (Ec)
    bool getCompressorProtectionAlarmStatus();
    
    // Status Code: (E8)
    bool getManInRoomAlarmStatus();
    
    // Status Code: (EL)
    bool getLowTemperatureAlarmStatus();
    
    // Status Code: (EH)
    bool getHighTemperatureAlarmStatus();
    #endif

    /**
     * @brief Get the status of the Temperature Alarm
     * If the temperature falls beyond the range defined on the device the alarm will turn on.
     * @return true If the temperature alarm is on
     * @return false If the temperature alarm is off
     */
    bool getTemperatureAlarmStatus();

    // Status Code: (Ed)
    bool getOpenDoorAlarmStatus();
    
    // Status Code: (E2)
    bool getEEPROMErrorStatus();

    // Status Code: (E1)
    bool getEvaporatorProbeFaultStatus();

    // Status Code: (E0)
    bool getAmbientProbeFaultStatus();

    // # Device Status Register
    
    //1 = defrost
    //0 = non‐defrost
    bool getDefrostForcingStatus();
    bool setDefrostForcingStatus(bool value);
    
    //1 = active cold room light
    //0 = non‐active cold room light
    bool getColdRoomLightKeyStatus();
    bool setColdRoomLightKeyStatus(bool value);

    //1 = stand‐by 
    //0 = ON
    bool getDeviceStandByStatus();
    bool setDeviceStandByStatus(bool value);
};
