#pragma once

#include <ArduinoModbus.h>
#include "RegisterDescription.h"

// # ECP BASE & ECP EXPERT REGISTERS

// ANALOG INPUTS REGISTERS
RegisterDescription ambientTemperatureRegister = {HOLDING_REGISTERS, 256, true, 0.1 };
RegisterDescription evaporatorTemperatureRegister = {HOLDING_REGISTERS, 257, true, 0.1 };

// PARAMETER REGISTERS
RegisterDescription temperatureSetPointRegister = {HOLDING_REGISTERS, 768, false, 0.1 };
RegisterDescription temperatureDifferentialRegister = {HOLDING_REGISTERS, 769, false, 0.1 }; 
RegisterDescription defrostingPeriodRegister = {HOLDING_REGISTERS, 770, false, 1 };
RegisterDescription endOfDefrostingTemperatureRegister = {HOLDING_REGISTERS, 771, true, 1 }; 
RegisterDescription maxDefrostingDurationRegister = {HOLDING_REGISTERS, 772, false, 1 }; 
RegisterDescription drippingDurationRegister = {HOLDING_REGISTERS, 773, false, 1 };
RegisterDescription fansStopDurationPostDefrostingRegister = {HOLDING_REGISTERS, 774, false, 1 };
RegisterDescription temperatureAlarmMinimumThresholdRegister = {HOLDING_REGISTERS, 775, true, 1 };
RegisterDescription temperatureAlarmMaximumThresholdRegister = {HOLDING_REGISTERS, 776, true, 1 };
RegisterDescription fansStatusWithStoppedCompressorRegister = {HOLDING_REGISTERS, 777, false, 1 };
RegisterDescription fansStopInDefrostingRegister = {HOLDING_REGISTERS, 778, false, 1 };
RegisterDescription evaporatorProbeExclusionRegister = {HOLDING_REGISTERS, 779, false, 1 };
RegisterDescription temperatureAlarmSignalingDelayRegister = {HOLDING_REGISTERS, 780, false, 1 };
RegisterDescription compressorReStartingDelayRegister = {HOLDING_REGISTERS, 781, false, 1 };
RegisterDescription ambientProbeCalibrationRegister = {HOLDING_REGISTERS, 782, true, 0.1 };
RegisterDescription compressorSafetyTimeForDoorSwitchRegister = {HOLDING_REGISTERS, 783, false, 1 };
RegisterDescription compressorRestartTimeAfterDoorOpeningRegister = {HOLDING_REGISTERS, 784, false, 1 };
RegisterDescription fansBlockageTemperatureRegister = {HOLDING_REGISTERS, 785, true, 1 };
RegisterDescription differentialOnFansBlockageRegister = {HOLDING_REGISTERS, 786, false, 1 };
RegisterDescription temperatureSetPointMinimumLimitRegister = {HOLDING_REGISTERS, 787, true, 1 };
RegisterDescription temperatureSetPointMaximumLimitRegister = {HOLDING_REGISTERS, 788, true, 1 };

// INPUTS / OUTPUTS / ALARMS STATUS REGISTERS
RegisterDescription outputStatusRegister = {HOLDING_REGISTERS, 1280, false, 1 };
RegisterDescription inputStatusRegister = {HOLDING_REGISTERS, 1281, false, 1 };
RegisterDescription alarmStatusRegister = {HOLDING_REGISTERS, 1282, false, 1 };
RegisterDescription deviceStatusRegister = {HOLDING_REGISTERS, 1536, false, 1 };