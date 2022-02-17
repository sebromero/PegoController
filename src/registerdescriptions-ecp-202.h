#pragma once

#include <ArduinoModbus.h>
#include "RegisterDescription.h"

// # ECP 202 EXPERT REGISTERS
RegisterDescription temperatureSettingForAuxRelayRegister = {HOLDING_REGISTERS, 789, true, 1};
RegisterDescription defrostAtPowerOnStatusRegister = {HOLDING_REGISTERS, 790, false, 1};
RegisterDescription smartDefrostStatusRegister = {HOLDING_REGISTERS, 791, false, 1};
RegisterDescription smartDefrostSetpointRegister = {HOLDING_REGISTERS, 792, true, 1};
RegisterDescription durationOfCompressorOnTimeWithFaultyAmbientProbeRegister = {HOLDING_REGISTERS, 793, false, 1};
RegisterDescription durationOfCompressorOffTimeWithFaultyAmbientProbeRegister = {HOLDING_REGISTERS, 794, false, 1};
RegisterDescription correctionFactorForTheSETButtonDuringNightOperationRegister = {HOLDING_REGISTERS, 795, false, 0.1};
RegisterDescription buzzerEnableStatusRegister = {HOLDING_REGISTERS, 796, false, 1};
RegisterDescription evaporatorFansActivationForAirRecirculationRegister = {HOLDING_REGISTERS, 797, false, 1};
RegisterDescription evaporatorFansDurationForAirRecirculationRegister = {HOLDING_REGISTERS, 798, false, 1};
RegisterDescription thermostatFunctioningModeRegister = {HOLDING_REGISTERS, 512, false, 1};
RegisterDescription defrostTypeRegister = {HOLDING_REGISTERS, 513, false, 1};
RegisterDescription displayViewingDuringDefrostRegister = {HOLDING_REGISTERS, 514, false, 1};
RegisterDescription input1SettingRegister = {HOLDING_REGISTERS, 515, false, 1};
RegisterDescription input2SettingRegister = {HOLDING_REGISTERS, 516, false, 1};
RegisterDescription auxiliaryRelay1ControlRegister = {HOLDING_REGISTERS, 517, false, 1};
RegisterDescription auxiliaryRelay2ControlRegister = {HOLDING_REGISTERS, 518, false, 1};