/*
ModBus Function Codes & Registers

Object type | Access | Size | Function code
--------------------------------------
Single input/output "Coil" | read & write | 1-bit 01 / 05 / 15
Single input "Discrete Input" | read only | 1-bit 02
(analog) inputs "Input Register" | read only | 16-bits 04
(analog) inputs/outputs "Holding Register" | read & write | 16-bits 03 / 06 / 16
*/

/*
 Modbus uses a “big-Endian” representation for addresses and data items. 
 This means that when a numerical quantity larger than a single byte is transmitted, 
 the most significant byte is sent first. The Arduino processors are little-endian.
 The conversion is done by the ArduinoModbus library
 */

#include <ArduinoModbus.h>
#include "PegoController.h"
#include "registerdescriptions-ecp-base.h"

#ifdef ECP_202
#include "registerdescriptions-ecp-202.h"
#endif

#ifndef SerialPort
#define SerialPort Serial
#endif

/* 
Defines the amount of time (in ms) during which the controller has to be
unreachable until it transitions into the unresponsive state.
The controller may not always respond to all requests. Hence using a 
threshold give the controller a grace period until being considered unresponsive.
*/
#define RESPONSIVENESS_THRESHOLD 300000

PegoController::PegoController( unsigned long baudRate, uint8_t peripheralID, uint16_t serialConfig) : 
_baudRate(baudRate),
_peripheralID(peripheralID),
_serialConfig(serialConfig)
{}

bool PegoController::begin(){
    _lastResponsive = millis();
    return ModbusRTUClient.begin(_baudRate, _serialConfig);
}

bool PegoController::responsive(){
    auto isResponsive = readModbusRegister(deviceStatusRegister) != READ_ERROR;
    if(isResponsive){
        _lastResponsive = millis();
    }
    return millis() - _lastResponsive < RESPONSIVENESS_THRESHOLD;
};


int16_t PegoController::convertToSignedValue(uint16_t value, RegisterDescription registerEntry){
  if(!registerEntry.requiresConversion) return value;
  if(value >= 0 && value <= 32767) return value;
  if(value >= 32768 && value <= 65535) return value - 65536;
  return 0;
}

int16_t PegoController::readModbusRegister(RegisterDescription registerEntry){      
    if (!ModbusRTUClient.requestFrom(_peripheralID, registerEntry.type, registerEntry.registerNumber, 1 /* Amount of registers */)) {
        SerialPort.print("Failed to read register: ");
        SerialPort.println(registerEntry.registerNumber);
        SerialPort.println(ModbusRTUClient.lastError());
        return READ_ERROR;
    }
    if(ModbusRTUClient.available() == 0){
        SerialPort.println("No values received.");
        return READ_ERROR;
    }
    long rawValue = ModbusRTUClient.read();
    #ifdef DEBUG
        SerialPort.print("RECEIVED BINARY VALUE: ");
        SerialPort.println(rawValue, BIN);    
    #endif
    return convertToSignedValue(rawValue, registerEntry);     
}

bool PegoController::writeModbusRegister(RegisterDescription registerEntry, int16_t value){
    #ifdef DEBUG
    SerialPort.print("SENDING BINARY VALUE: ");
    SerialPort.println(value, BIN);    
    #endif
    
    if(!ModbusRTUClient.beginTransmission(_peripheralID, registerEntry.type, registerEntry.registerNumber, 1)){
        SerialPort.print("Writed operation failed: ");
        SerialPort.println(ModbusRTUClient.lastError());
        return false;
    };

    ModbusRTUClient.write(value);

    if (!ModbusRTUClient.endTransmission()) {
        SerialPort.print("Write operation failed: ");
        SerialPort.println(ModbusRTUClient.lastError());
        return false;
    } else {
        #ifdef DEBUG
        SerialPort.println("Write operation successful.");
        #endif
        return true;
    }
}

byte PegoController::getLSByteBit(int16_t value, byte bit){
  return bitRead(lowByte(value), bit);
}

byte PegoController::getMSByteBit(int16_t value, byte bit){
  return bitRead(highByte(value), bit);
}

void PegoController::setLSByteBit(int16_t *value, byte bit){
    bitSet(*value, bit);
};

void PegoController::setMSByteBit(int16_t *value, byte bit){
    bitSet(*value, bit + 8);
};

float PegoController::applyMultiplicationFactor(int16_t value, RegisterDescription registerEntry){
  return value * registerEntry.multiplicationFactor;
}

int16_t PegoController::unapplyMultiplicationFactor(float value, RegisterDescription registerEntry){
  return value / registerEntry.multiplicationFactor;
}

//////////////////////////////////////////////////

// ANALOG INPUTS

float PegoController::getAmbientTemperature(){
    int16_t value = readModbusRegister(ambientTemperatureRegister);
    if(value == READ_ERROR) return READ_ERROR_FLOAT;
    return applyMultiplicationFactor(value, ambientTemperatureRegister);
}

float PegoController::getEvaporatorTemperature(){
    int16_t value = readModbusRegister(evaporatorTemperatureRegister);
    if(value == READ_ERROR) return READ_ERROR_FLOAT;
    return applyMultiplicationFactor(value, evaporatorTemperatureRegister);
}

// PARAMETERS

float PegoController::getTemperatureSetPoint(){
    int16_t value = readModbusRegister(temperatureSetPointRegister);
    if(value == READ_ERROR) return READ_ERROR_FLOAT;
    return applyMultiplicationFactor(value, temperatureSetPointRegister);
};

bool PegoController::setTemperatureSetPoint(float value){
    int16_t intValue = unapplyMultiplicationFactor(value, temperatureSetPointRegister);
    return writeModbusRegister(temperatureSetPointRegister, intValue);
};

float PegoController::getTemperatureDifferential(){
    int16_t value = readModbusRegister(temperatureDifferentialRegister);
    if(value == READ_ERROR) return READ_ERROR_FLOAT;
    return applyMultiplicationFactor(value, temperatureDifferentialRegister);
};

bool PegoController::setTemperatureDifferential(float value){
    int16_t intValue = unapplyMultiplicationFactor(value, temperatureDifferentialRegister);
    return writeModbusRegister(temperatureDifferentialRegister, intValue);
};

int16_t PegoController::getDefrostingPeriod(){
    return readModbusRegister(defrostingPeriodRegister);
};

bool PegoController::setDefrostingPeriod(int16_t value){
    return writeModbusRegister(defrostingPeriodRegister, value);
};

int16_t PegoController::getEndOfDefrostingTemperature(){
    return readModbusRegister(endOfDefrostingTemperatureRegister);
};

bool PegoController::setEndOfDefrostingTemperature(int16_t value){
    return writeModbusRegister(endOfDefrostingTemperatureRegister, value);
};

int16_t PegoController::getMaxDefrostingDuration(){
    return readModbusRegister(maxDefrostingDurationRegister);
};

bool PegoController::setMaxDefrostingDuration(int16_t value){
    return writeModbusRegister(maxDefrostingDurationRegister, value);
};

int16_t PegoController::getDrippingDuration(){
    return readModbusRegister(drippingDurationRegister);
};

bool PegoController::setDrippingDuration(int16_t value){
    return writeModbusRegister(drippingDurationRegister, value);
};

int16_t PegoController::getFansStopDurationPostDefrosting(){
    return readModbusRegister(fansStopDurationPostDefrostingRegister);
};

bool PegoController::setFansStopDurationPostDefrosting(int16_t value){
    return writeModbusRegister(fansStopDurationPostDefrostingRegister, value);
};

int16_t PegoController::getTemperatureAlarmMinimumThreshold(){
    return readModbusRegister(temperatureAlarmMinimumThresholdRegister);
};

bool PegoController::setTemperatureAlarmMinimumThreshold(int16_t value){
    return writeModbusRegister(temperatureAlarmMinimumThresholdRegister, value);
};

int16_t PegoController::getTemperatureAlarmMaximumThreshold(){
    return readModbusRegister(temperatureAlarmMaximumThresholdRegister);
};

bool PegoController::setTemperatureAlarmMaximumThreshold(int16_t value){
    return writeModbusRegister(temperatureAlarmMaximumThresholdRegister, value);
};

int16_t PegoController::getFansStatusWithStoppedCompressor(){
    return readModbusRegister(fansStatusWithStoppedCompressorRegister);
};

bool PegoController::setFansStatusWithStoppedCompressor(int16_t value){
    return writeModbusRegister(fansStatusWithStoppedCompressorRegister, value);
};

bool PegoController::getFansStopInDefrosting(){
    return readModbusRegister(fansStopInDefrostingRegister) == 1;
};

bool PegoController::setFansStopInDefrosting(int16_t value){
    return writeModbusRegister(fansStopInDefrostingRegister, value);
};

bool PegoController::getEvaporatorProbeExclusion(){
    return readModbusRegister(evaporatorProbeExclusionRegister) == 1;
};

bool PegoController::setEvaporatorProbeExclusion(int16_t value){
    return writeModbusRegister(evaporatorProbeExclusionRegister, value);
};

int16_t PegoController::getTemperatureAlarmSignalingDelay(){
    return readModbusRegister(temperatureAlarmSignalingDelayRegister);
};

bool PegoController::setTemperatureAlarmSignalingDelay(int16_t value){
    return writeModbusRegister(temperatureAlarmSignalingDelayRegister, value);
};

int16_t PegoController::getCompressorReStartingDelay(){
    return readModbusRegister(compressorReStartingDelayRegister);
};

bool PegoController::setCompressorReStartingDelay(int16_t value){
    return writeModbusRegister(compressorReStartingDelayRegister, value);
};

float PegoController::getAmbientProbeCalibration(){
    int16_t value = readModbusRegister(ambientProbeCalibrationRegister);
    if(value == READ_ERROR) return READ_ERROR_FLOAT;
    return applyMultiplicationFactor(value, ambientProbeCalibrationRegister);
};

bool PegoController::setAmbientProbeCalibration(float value){
    int16_t intValue = unapplyMultiplicationFactor(value, ambientProbeCalibrationRegister);
    return writeModbusRegister(ambientProbeCalibrationRegister, intValue);
};

int16_t PegoController::getCompressorSafetyTimeForDoorSwitch(){
    return readModbusRegister(compressorSafetyTimeForDoorSwitchRegister);
};

bool PegoController::setCompressorSafetyTimeForDoorSwitch(int16_t value){
    return writeModbusRegister(compressorSafetyTimeForDoorSwitchRegister, value);
};

int16_t PegoController::getCompressorRestartTimeAfterDoorOpening(){
    return readModbusRegister(compressorRestartTimeAfterDoorOpeningRegister);
};

bool PegoController::setCompressorRestartTimeAfterDoorOpening(int16_t value){
    return writeModbusRegister(compressorRestartTimeAfterDoorOpeningRegister, value);
};

int16_t PegoController::getFansBlockageTemperature(){
    return readModbusRegister(fansBlockageTemperatureRegister);
};

bool PegoController::setFansBlockageTemperature(int16_t value){
    return writeModbusRegister(fansBlockageTemperatureRegister, value);
};

int16_t PegoController::getDifferentialOnFansBlockage(){
    return readModbusRegister(differentialOnFansBlockageRegister);
};

bool PegoController::setDifferentialOnFansBlockage(int16_t value){
    return writeModbusRegister(differentialOnFansBlockageRegister, value);
};

int16_t PegoController::getTemperatureSetPointMinimumLimit(){
    return readModbusRegister(temperatureSetPointMinimumLimitRegister);
};

bool PegoController::setTemperatureSetPointMinimumLimit(int16_t value){
    return writeModbusRegister(temperatureSetPointMinimumLimitRegister, value);
};

int16_t PegoController::getTemperatureSetPointMaximumLimit(){
    return readModbusRegister(temperatureSetPointMaximumLimitRegister);
};

bool PegoController::setTemperatureSetPointMaximumLimit(int16_t value){
    return writeModbusRegister(temperatureSetPointMaximumLimitRegister, value);
};

#ifdef ECP_202

int16_t PegoController::getTemperatureSettingForAuxRelay(){
    return readModbusRegister(temperatureSettingForAuxRelayRegister);
};

bool PegoController::setTemperatureSettingForAuxRelay(int16_t value){
    return writeModbusRegister(temperatureSettingForAuxRelayRegister, value);
};

bool PegoController::getDefrostAtPowerOnStatus(){
    return readModbusRegister(defrostAtPowerOnStatusRegister) == 1;
};

bool PegoController::setDefrostAtPowerOnStatus(bool value){
    return writeModbusRegister(defrostAtPowerOnStatusRegister, value ? 1 : 0);
};

bool PegoController::getSmartDefrostStatus(){
    return readModbusRegister(smartDefrostStatusRegister) == 1;
};

bool PegoController::setSmartDefrostStatus(bool value){
    return writeModbusRegister(smartDefrostStatusRegister, value ? 1 : 0);
};

int16_t PegoController::getSmartDefrostSetpoint(){
    return readModbusRegister(smartDefrostSetpointRegister);
};

bool PegoController::setSmartDefrostSetpoint(int16_t value){
    return writeModbusRegister(smartDefrostSetpointRegister, value);
};

int16_t PegoController::getDurationOfCompressorOnTimeWithFaultyAmbientProbe(){
    return readModbusRegister(durationOfCompressorOnTimeWithFaultyAmbientProbeRegister);
};

bool PegoController::setDurationOfCompressorOnTimeWithFaultyAmbientProbe(int16_t value){
    return writeModbusRegister(durationOfCompressorOnTimeWithFaultyAmbientProbeRegister, value);
};

int16_t PegoController::getDurationOfCompressorOffTimeWithFaultyAmbientProbe(){
    return readModbusRegister(durationOfCompressorOffTimeWithFaultyAmbientProbeRegister);
};

bool PegoController::setDurationOfCompressorOffTimeWithFaultyAmbientProbe(int16_t value){
    return writeModbusRegister(durationOfCompressorOffTimeWithFaultyAmbientProbeRegister, value);
};

float PegoController::getCorrectionFactorForTheSETButtonDuringNightOperation(){
    int16_t value = readModbusRegister(correctionFactorForTheSETButtonDuringNightOperationRegister);
    if(value == READ_ERROR) return READ_ERROR_FLOAT;
    return applyMultiplicationFactor(value, correctionFactorForTheSETButtonDuringNightOperationRegister);
};

bool PegoController::setCorrectionFactorForTheSETButtonDuringNightOperation(float value){
    int16_t intValue = unapplyMultiplicationFactor(value, correctionFactorForTheSETButtonDuringNightOperationRegister);
    return writeModbusRegister(correctionFactorForTheSETButtonDuringNightOperationRegister, intValue);
};

bool PegoController::getBuzzerEnableStatus(){
    return readModbusRegister(buzzerEnableStatusRegister) == 1;
};

bool PegoController::setBuzzerEnableStatus(bool value){
    return writeModbusRegister(buzzerEnableStatusRegister, value ? 1 : 0);
};

int16_t PegoController::getEvaporatorFansActivationForAirRecirculation(){
    return readModbusRegister(evaporatorFansActivationForAirRecirculationRegister);
};

bool PegoController::setEvaporatorFansActivationForAirRecirculation(int16_t value){
    return writeModbusRegister(evaporatorFansActivationForAirRecirculationRegister, value);
};

int16_t PegoController::getEvaporatorFansDurationForAirRecirculation(){
    return readModbusRegister(evaporatorFansDurationForAirRecirculationRegister);
};

bool PegoController::setEvaporatorFansDurationForAirRecirculation(int16_t value){
    return writeModbusRegister(evaporatorFansDurationForAirRecirculationRegister, value);
};

int16_t PegoController::getThermostatFunctioningMode(){
    return readModbusRegister(thermostatFunctioningModeRegister);
};

int16_t PegoController::getDefrostType(){
    return readModbusRegister(defrostTypeRegister);
};

int16_t PegoController::getDisplayViewingDuringDefrost(){
    return readModbusRegister(displayViewingDuringDefrostRegister);
};

int16_t PegoController::getInput1Setting(){
    return readModbusRegister(input1SettingRegister);
};

int16_t PegoController::getInput2Setting(){
    return readModbusRegister(input2SettingRegister);
};

int16_t PegoController::getAuxiliaryRelay1Control(){
    return readModbusRegister(auxiliaryRelay1ControlRegister);
};

int16_t PegoController::getAuxiliaryRelay2Control(){
    return readModbusRegister(auxiliaryRelay2ControlRegister);
};

#endif


// INPUTS / OUTPUTS / ALARMS STATUS REGISTERS

// # Output Status Register

#ifdef ECP_202
bool PegoController::getHotResistanceStatus(){
    int16_t outputStatus = readModbusRegister(outputStatusRegister);
    return getLSByteBit(outputStatus, 6) == 1;
};

bool PegoController::getStandByStatus(){
    int16_t outputStatus = readModbusRegister(outputStatusRegister);
    return getLSByteBit(outputStatus, 5) == 1;
};
#endif

bool PegoController::getDrippingStatus(){
    int16_t outputStatus = readModbusRegister(outputStatusRegister);
    return getLSByteBit(outputStatus, 4) == 1;
};

bool PegoController::getColdRoomLightRelayStatus(){
    int16_t outputStatus = readModbusRegister(outputStatusRegister);
    return getLSByteBit(outputStatus, 3) == 1;
};

bool PegoController::getFansRelayStatus(){
    int16_t outputStatus = readModbusRegister(outputStatusRegister);
    return getLSByteBit(outputStatus, 2) == 1;
};

bool PegoController::getDefrostRelayStatus(){
    int16_t outputStatus = readModbusRegister(outputStatusRegister);
    return getLSByteBit(outputStatus, 1) == 1;
};

bool PegoController::getCompressorRelayStatus(){
    int16_t outputStatus = readModbusRegister(outputStatusRegister);
    return getLSByteBit(outputStatus, 0) == 1;
};


// # Input Status Register

#ifdef ECP_202
bool PegoController::getNightDigitalInputStatus(){
    int16_t inputStatus = readModbusRegister(inputStatusRegister);
    return getLSByteBit(inputStatus, 7) == 1;
};

bool PegoController::getRemoteStopDefrostStatus(){
    int16_t inputStatus = readModbusRegister(inputStatusRegister);
    return getLSByteBit(inputStatus, 6) == 1;
};

bool PegoController::getRemoteStartDefrostStatus(){
    int16_t inputStatus = readModbusRegister(inputStatusRegister);
    return getLSByteBit(inputStatus, 5) == 1;
};

bool PegoController::getRemoteStandByStatus(){
    int16_t inputStatus = readModbusRegister(inputStatusRegister);
    return getLSByteBit(inputStatus, 4) == 1;
};

bool PegoController::getPumpDownInputStatus(){
    int16_t inputStatus = readModbusRegister(inputStatusRegister);
    return getLSByteBit(inputStatus, 3) == 1;
};
#endif

bool PegoController::getManInColdRoomAlarmStatus(){
    int16_t inputStatus = readModbusRegister(inputStatusRegister);
    return getLSByteBit(inputStatus, 2) == 1;
};

bool PegoController::getCompressorProtectionStatus(){
    int16_t inputStatus = readModbusRegister(inputStatusRegister);
    return getLSByteBit(inputStatus, 1) == 1;
};

bool PegoController::getDoorSwitchStatus(){
    int16_t inputStatus = readModbusRegister(inputStatusRegister);
    return getLSByteBit(inputStatus, 0) == 1;
};


// # Alarm Status Register

#ifdef ECP_202
bool PegoController::getLightAlarmStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getMSByteBit(alarmStatus, 1) == 1;
};

bool PegoController::getCompressorProtectionAlarmStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getMSByteBit(alarmStatus, 0) == 1;
};

bool PegoController::getManInRoomAlarmStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getLSByteBit(alarmStatus, 7) == 1;
};

bool PegoController::getOpenDoorAlarmStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getLSByteBit(alarmStatus, 6) == 1;
};

bool PegoController::getLowTemperatureAlarmStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getLSByteBit(alarmStatus, 5) == 1;
};

bool PegoController::getHighTemperatureAlarmStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getLSByteBit(alarmStatus, 4) == 1;
};

bool PegoController::getTemperatureAlarmStatus(){
    return getLowTemperatureAlarmStatus() || getHighTemperatureAlarmStatus();
};

#else
bool PegoController::getOpenDoorAlarmStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getLSByteBit(alarmStatus, 4) == 1;
};

bool PegoController::getTemperatureAlarmStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getLSByteBit(alarmStatus, 3) == 1;
};
#endif

bool PegoController::getEEPROMErrorStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getLSByteBit(alarmStatus, 2) == 1;
};

bool PegoController::getEvaporatorProbeFaultStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getLSByteBit(alarmStatus, 1) == 1;
};

bool PegoController::getAmbientProbeFaultStatus(){
    int16_t alarmStatus = readModbusRegister(alarmStatusRegister);
    return getLSByteBit(alarmStatus, 0) == 1;
};

// DEVICE STATUS
// # Device Status Register

bool PegoController::getDefrostForcingStatus(){
    int16_t deviceStatus = readModbusRegister(deviceStatusRegister);
    return getLSByteBit(deviceStatus, 2) == 1;
};

bool PegoController::setDefrostForcingStatus(bool value){
    int16_t registerValue = 0;
    setMSByteBit(&registerValue, 2);
    if(value) setLSByteBit(&registerValue, 2); // 1 = defrost, 0 = non‐defrost
    return writeModbusRegister(deviceStatusRegister, registerValue);
};

bool PegoController::getColdRoomLightKeyStatus(){
    int16_t deviceStatus = readModbusRegister(deviceStatusRegister);
    return getLSByteBit(deviceStatus, 1) == 1;
};

bool PegoController::setColdRoomLightKeyStatus(bool value){
    int16_t registerValue = 0;
    setMSByteBit(&registerValue, 1);
    if(value) setLSByteBit(&registerValue, 1); // 1 = active cold room light, 0 = non‐active cold room light
    return writeModbusRegister(deviceStatusRegister, registerValue);
};

bool PegoController::getDeviceStandByStatus(){
    int16_t deviceStatus = readModbusRegister(deviceStatusRegister);
    return getLSByteBit(deviceStatus, 0) == 1;
};

bool PegoController::setDeviceStandByStatus(bool value){
    int16_t registerValue = 0;
    setMSByteBit(&registerValue, 0);
    if(value) setLSByteBit(&registerValue, 0); // 0 = ON, 1 = stand‐by
    return writeModbusRegister(deviceStatusRegister, registerValue);
};
