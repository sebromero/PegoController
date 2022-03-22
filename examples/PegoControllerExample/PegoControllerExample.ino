/*
  Modbus RTU Pego ECP 300 Expert Client Example
  Author: Sebastian Romero

  Circuit:
   - MKR board
   - MKR 485 shield
   - Pego ECP 300 Expert
     - ISO GND connected to "Common Digital Inputs" of the Pego ECP (Modbus RTU slave) (Solid blue) 
     - Y connected to A of the Pego ECP (Terminal 7) (Green/White)
     - Z connected to B of the Pego ECP (Terminal 8) (Solid green)
          
     - Jumper positions
       - A \/\/ B set to OFF
       - FULL set to OFF
       - Z \/\/ Y set to ON
*/

/* Documentation available from 
 * http://www.pego.it/en/applications/cold-rooms/ecp-300-expert-vd/113#boards
 * http://www.pego.it/en/products/electrical-boards/expert-series/ecp-300-expert-vd/113#boards
 * 
 * http://www.pego.it/images/stories/PDF/doc_tecnica/m_ecp300_202_expert_en.pdf
 * http://www.pego.it/images/stories/PDF/doc_tecnica/MODBUS-RTU_ECP202_EN.pdf
 */

/* ECP EXPERT Configuration (See instruction manual):
 * SEr = 1 -> Modbus protocol 
 * Bdr = 7 -> 19200 baud (default: 5 -> 9600 baud)
 * Prt = 0 (default: 0) -> No Modbus parity check
 * Ad = 1 (default: 0) -> Net address 1 ÷ 247
 * 
 * To change the settings:
 * Press UP + DOWN + LIGHT for 3 seconds
 * Press UP / DOWN to select the variable that you want to change
 * Press SET (looks like a grid) + UP/DOWN to change the value
 * Press UP + DOWN for 3 sec to exit
 * Press the Standby button for 1 sec to go back to normal operating mode
 */

#include "PegoController.h"

// Defines how often the peripheral's registers are read in ms
#define REGISTER_UPDATE_INTERVAL 30000

// Defines the baudrate for the serial connection between the board and the Pego device
#define RS485_BAUDRATE 19200

PegoController controller = PegoController(RS485_BAUDRATE);

/**
 * @brief Blinks an LED at a given interval
 * @param interval The interval in milliseconds
 * @param pin The pin where the LED is connected. Defaults to LED_BUILTIN
 */
void blinkLED(uint16_t interval, uint16_t pin = LED_BUILTIN){
  digitalWrite(pin, HIGH);
  delay(interval);
  digitalWrite(pin, LOW);
  delay(interval);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(5000);

  Serial.println("Starting the Modbus RTU client...");    
  if(!controller.begin()){
    Serial.println("Failed to start Modbus RTU Client!");
    
    while (true){
      blinkLED(500);
    }
  };
  
  // Indicate that the Modbus client was started successfully
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  auto transmissionTimestamp = millis();

  // ANALOG INPUTS REGISTERS

  float ambientTemperature = controller.getAmbientTemperature();
  Serial.print("Ambient Temperature: ");
  Serial.print(ambientTemperature);
  Serial.println(" °C\n");

  float evaporatorTemperature = controller.getEvaporatorTemperature();
  Serial.print("Evaporator Temperature: ");
  Serial.print(evaporatorTemperature);
  Serial.println(" °C\n");
  
  // PARAMETER REGISTERS

  float temperatureSetPoint = controller.getTemperatureSetPoint();
  Serial.print("Temperature Set Point: ");
  Serial.print(temperatureSetPoint);
  Serial.println(" °C\n");
  
  float temperatureDifferential = controller.getTemperatureDifferential();
  Serial.print("Temperature Differential: ");
  Serial.print(temperatureDifferential);
  Serial.println(" °C\n");
  
  int16_t defrostingPeriod = controller.getDefrostingPeriod();
  Serial.print("Defrosting Period: ");
  Serial.print(defrostingPeriod);
  Serial.println(" hours\n");
  
  int16_t endOfDefrostingTemperature = controller.getEndOfDefrostingTemperature();
  Serial.print("End Of Defrosting Temperature: ");
  Serial.print(endOfDefrostingTemperature);
  Serial.println(" °C\n");
  
  int16_t maxDefrostingDuration = controller.getMaxDefrostingDuration();
  Serial.print("Max Defrosting Duration: ");
  Serial.print(maxDefrostingDuration);
  Serial.println(" minutes\n");
  
  int16_t drippingDuration = controller.getDrippingDuration();
  Serial.print("Dripping Duration: ");
  Serial.print(drippingDuration);
  Serial.println(" minutes\n");
  
  int16_t fansStopDurationPostDefrosting = controller.getFansStopDurationPostDefrosting();
  Serial.print("Fans Stop Duration Post Defrosting: ");
  Serial.print(fansStopDurationPostDefrosting);
  Serial.println(" minutes\n");
  
  int16_t temperatureAlarmMinimumThreshold = controller.getTemperatureAlarmMinimumThreshold();
  Serial.print("Temperature Alarm Minimum Threshold: ");
  Serial.print(temperatureAlarmMinimumThreshold);
  Serial.println(" °C\n");
  
  int16_t temperatureAlarmMaximumThreshold = controller.getTemperatureAlarmMaximumThreshold();
  Serial.print("Temperature Alarm Maximum Threshold: ");
  Serial.print(temperatureAlarmMaximumThreshold);
  Serial.println(" °C\n");
  
  int16_t fansStatusWithStoppedCompressor = controller.getFansStatusWithStoppedCompressor();
  Serial.print("Fans Status With Stopped Compressor: ");
  Serial.print(fansStatusWithStoppedCompressor);
  Serial.println("\n");
  
  bool fansStopInDefrosting = controller.getFansStopInDefrosting();
  Serial.print("Fans Stop In Defrosting: ");
  Serial.print(fansStopInDefrosting);
  Serial.println(fansStopInDefrosting ? "Enabled\n" : "Disabled\n");
  
  bool evaporatorProbeExclusion = controller.getEvaporatorProbeExclusion();
  Serial.print("Evaporator Probe Exclusion: ");
  Serial.print(evaporatorProbeExclusion);
  Serial.println(evaporatorProbeExclusion ? "Enabled\n" : "Disabled\n");
  
  int16_t temperatureAlarmSignalingDelay = controller.getTemperatureAlarmSignalingDelay();
  Serial.print("Temperature Alarm Signaling Delay: ");
  Serial.print(temperatureAlarmSignalingDelay);
  Serial.println(" minutes\n");
  
  int16_t compressorReStartingDelay = controller.getCompressorReStartingDelay();
  Serial.print("Compressor Re Starting Delay: ");
  Serial.print(compressorReStartingDelay);
  Serial.println(" minutes\n");
  
  float ambientProbeCalibration = controller.getAmbientProbeCalibration();
  Serial.print("Ambient Probe Calibration: ");
  Serial.print(ambientProbeCalibration);
  Serial.println(" °C\n");
  
  int16_t compressorSafetyTimeForDoorSwitch = controller.getCompressorSafetyTimeForDoorSwitch();
  Serial.print("Compressor Safety Time For Door Switch: ");
  Serial.print(compressorSafetyTimeForDoorSwitch);
  Serial.println(" minutes\n");
  
  int16_t compressorRestartTimeAfterDoorOpening = controller.getCompressorRestartTimeAfterDoorOpening();
  Serial.print("Compressor Restart Time After Door Opening: ");
  Serial.print(compressorRestartTimeAfterDoorOpening);
  Serial.println(" minutes\n");
  
  int16_t fansBlockageTemperature = controller.getFansBlockageTemperature();
  Serial.print("Fans Blockage Temperature: ");
  Serial.print(fansBlockageTemperature);
  Serial.println(" °C\n");
  
  int16_t differentialOnFansBlockage = controller.getDifferentialOnFansBlockage();
  Serial.print("Differential On Fans Blockage: ");
  Serial.print(differentialOnFansBlockage);
  Serial.println(" °C\n");
  
  int16_t temperatureSetPointMinimumLimit = controller.getTemperatureSetPointMinimumLimit();
  Serial.print("Temperature Set Point Minimum Limit: ");
  Serial.print(temperatureSetPointMinimumLimit);
  Serial.println(" °C\n");
  
  int16_t temperatureSetPointMaximumLimit = controller.getTemperatureSetPointMaximumLimit();
  Serial.print("Temperature Set Point Maximum Limit: ");
  Serial.print(temperatureSetPointMaximumLimit);
  Serial.println(" °C\n");

  #ifdef ECP_202

  int16_t temperatureSettingForAuxRelay = controller.getTemperatureSettingForAuxRelay();
  Serial.print("Temperature Setting For AuxRelay: ");
  Serial.print(temperatureSettingForAuxRelay);
  Serial.println(" °C\n");

  bool defrostAtPowerOnStatus = controller.getDefrostAtPowerOnStatus();
  Serial.print("Defrost At Power On Status: ");
  Serial.println(defrostAtPowerOnStatus ? "Enabled\n" : "Disabled\n");

  bool smartDefrostStatus = controller.getSmartDefrostStatus();
  Serial.print("Smart Defrost Status: ");
  Serial.println(smartDefrostStatus ? "Enabled\n" : "Disabled\n");

  int16_t smartDefrostSetpoint = controller.getSmartDefrostSetpoint();
  Serial.print("Smart Defrost Setpoint: ");
  Serial.print(smartDefrostSetpoint);
  Serial.println(" °C\n");

  int16_t durationOfCompressorOnTimeWithFaultyAmbientProbe = controller.getDurationOfCompressorOnTimeWithFaultyAmbientProbe();
  Serial.print("Duration Of Compressor On Time With Faulty Ambient Probe: ");
  Serial.print(durationOfCompressorOnTimeWithFaultyAmbientProbe);
  Serial.println(" min\n");

  int16_t durationOfCompressorOffTimeWithFaultyAmbientProbe = controller.getDurationOfCompressorOffTimeWithFaultyAmbientProbe();
  Serial.print("Duration Of Compressor Off Time With Faulty Ambient Probe: ");
  Serial.print(durationOfCompressorOffTimeWithFaultyAmbientProbe);
  Serial.println(" min\n");

  float getCorrectionFactorForTheSETButtonDuringNightOperation = controller.getCorrectionFactorForTheSETButtonDuringNightOperation();
  Serial.print("Get Correction Factor For The SET Button During Night Operation: ");
  Serial.print(getCorrectionFactorForTheSETButtonDuringNightOperation);
  Serial.println(" °C\n");

  int16_t buzzerEnableStatus = controller.getBuzzerEnableStatus();
  Serial.print("Buzzer Enable Status: ");
  Serial.println(buzzerEnableStatus ? "Enabled\n" : "Disabled\n");

  int16_t evaporatorFansActivationForAirRecirculation = controller.getEvaporatorFansActivationForAirRecirculation();
  Serial.print("Evaporator Fans Activation For Air Recirculation: ");
  Serial.print(evaporatorFansActivationForAirRecirculation);
  Serial.println(" min\n");

  int16_t evaporatorFansDurationForAirRecirculation = controller.getEvaporatorFansDurationForAirRecirculation();
  Serial.print("Evaporator Fans Duration For Air Recirculation: ");
  Serial.print(evaporatorFansDurationForAirRecirculation);
  Serial.println(" sec\n");

  int16_t thermostatFunctioningMode = controller.getThermostatFunctioningMode();
  Serial.print("Thermostat Functioning Mode: ");
  Serial.print(thermostatFunctioningMode);
  Serial.println("\n");

  int16_t defrostType = controller.getDefrostType();
  Serial.print("Defrost Type: ");
  Serial.print(defrostType);
  Serial.println("\n");

  int16_t displayViewingDuringDefrost = controller.getDisplayViewingDuringDefrost();
  Serial.print("Display Viewing During Defrost: ");
  Serial.print(displayViewingDuringDefrost);
  Serial.println("\n");

  int16_t input1Setting = controller.getInput1Setting();
  Serial.print("Input1 Setting : ");
  Serial.print(input1Setting);
  Serial.println("\n");

  int16_t input2Setting = controller.getInput2Setting();
  Serial.print("Input2 Setting : ");
  Serial.print(input2Setting);
  Serial.println("\n");

  int16_t auxiliaryRelay1Control = controller.getAuxiliaryRelay1Control();
  Serial.print("Auxiliary Relay1 Control: ");
  Serial.print(auxiliaryRelay1Control);
  Serial.println("\n");

  int16_t auxiliaryRelay2Control = controller.getAuxiliaryRelay2Control();
  Serial.print("Auxiliary Relay2 Control: ");
  Serial.print(auxiliaryRelay2Control);
  Serial.println("\n");

  #endif

  // INPUTS / OUTPUTS / ALARMS STATUS REGISTERS
  
  #ifdef ECP_202
  bool hotResistanceStatus = controller.getHotResistanceStatus();
  Serial.print("Hot Resistance Status: ");
  Serial.println(hotResistanceStatus ? "ON\n" : "OFF\n");

  bool standByStatus = controller.getStandByStatus();
  Serial.print("Stand By Status: ");
  Serial.println(standByStatus ? "ON\n" : "OFF\n");
  #endif

  bool drippingStatus = controller.getDrippingStatus();
  Serial.print("Dripping Status: ");
  Serial.println(drippingStatus ? "ON\n" : "OFF\n");
  
  bool coldRoomLightRelayStatus = controller.getColdRoomLightRelayStatus();
  Serial.print("Cold Room Light Relay Status: ");
  Serial.println(coldRoomLightRelayStatus ? "ON\n" : "OFF\n");
  
  bool fansRelayStatus = controller.getFansRelayStatus();
  Serial.print("Fans Relay Status: ");
  Serial.println(fansRelayStatus ? "ON\n" : "OFF\n");
  
  bool defrostRelayStatus = controller.getDefrostRelayStatus();
  Serial.print("Defrost Relay Status: ");
  Serial.println(defrostRelayStatus ? "ON\n" : "OFF\n");
  
  bool compressorRelayStatus = controller.getCompressorRelayStatus();
  Serial.print("Compressor Relay Status: ");
  Serial.println(compressorRelayStatus ? "ON\n" : "OFF\n");
  
  #ifdef ECP_202

  bool nightDigitalInputStatus = controller.getNightDigitalInputStatus();
  Serial.print("Night Digital Input Status: ");
  Serial.println(nightDigitalInputStatus ? "ON\n" : "OFF\n");

  bool remoteStopDefrostStatus = controller.getRemoteStopDefrostStatus();
  Serial.print("Remote Stop Defrost Status: ");
  Serial.println(remoteStopDefrostStatus ? "ON\n" : "OFF\n");

  bool remoteStartDefrostStatus = controller.getRemoteStartDefrostStatus();
  Serial.print("Remote Start Defrost Status: ");
  Serial.println(remoteStartDefrostStatus ? "ON\n" : "OFF\n");

  bool remoteStandByStatus = controller.getRemoteStandByStatus();
  Serial.print("Remote Stand By Status: ");
  Serial.println(remoteStandByStatus ? "ON\n" : "OFF\n");

  bool pumpDownInputStatus = controller.getPumpDownInputStatus();
  Serial.print("Pump Down Input Status: ");
  Serial.println(pumpDownInputStatus ? "ON\n" : "OFF\n");

  #endif

  bool manInColdRoomAlarmStatus = controller.getManInColdRoomAlarmStatus();
  Serial.print("Man In Cold Room Alarm Status: ");
  Serial.println(manInColdRoomAlarmStatus ? "ON\n" : "OFF\n");
  
  bool compressorProtectionStatus = controller.getCompressorProtectionStatus();
  Serial.print("Compressor Protection Status: ");
  Serial.println(compressorProtectionStatus ? "ON\n" : "OFF\n");

  bool doorSwitchStatus = controller.getDoorSwitchStatus();    
  Serial.print("Door open: ");
  Serial.println(doorSwitchStatus ? "YES\n" : "NO\n");

  #ifdef ECP_202

  bool lightAlarmStatus = controller.getLightAlarmStatus();
  Serial.print("Light Alarm Status: ");
  Serial.println(lightAlarmStatus ? "ON\n" : "OFF\n");

  bool compressorProtectionAlarmStatus = controller.getCompressorProtectionAlarmStatus();
  Serial.print("Compressor Protection Alarm Status: ");
  Serial.println(compressorProtectionAlarmStatus ? "ON\n" : "OFF\n");

  bool manInRoomAlarmStatus = controller.getManInRoomAlarmStatus();
  Serial.print("Man In Room Alarm: ");
  Serial.println(manInRoomAlarmStatus ? "ON\n" : "OFF\n");

  bool lowTemperatureAlarmStatus = controller.getLowTemperatureAlarmStatus();
  Serial.print("Low Temperature Alarm Status: ");
  Serial.println(lowTemperatureAlarmStatus ? "ON\n" : "OFF\n");

  bool highTemperatureAlarmStatus = controller.getHighTemperatureAlarmStatus();
  Serial.print("High Temperature Alarm Status: ");
  Serial.println(highTemperatureAlarmStatus ? "ON\n" : "OFF\n");

  #endif

  bool openDoorAlarmStatus = controller.getOpenDoorAlarmStatus();
  Serial.print("Open Door Alarm Status: ");
  Serial.println(openDoorAlarmStatus ? "ON\n" : "OFF\n");

  bool temperatureAlarmStatus = controller.getTemperatureAlarmStatus();    
  Serial.print("Temperature alarm: ");
  Serial.println(temperatureAlarmStatus ? "ON\n" : "OFF\n");
  
  bool eepromErrorStatus = controller.getEEPROMErrorStatus();
  Serial.print("EEPROM Error Status: ");
  Serial.println(eepromErrorStatus ? "ON\n" : "OFF\n");
  
  bool evaporatorProbeFaultStatus = controller.getEvaporatorProbeFaultStatus();
  Serial.print("Evaporator Probe Fault Status: ");
  Serial.println(evaporatorProbeFaultStatus ? "ON\n" : "OFF\n");
  
  bool ambientProbeFaultStatus = controller.getAmbientProbeFaultStatus();
  Serial.print("Ambient Probe Fault Status: ");
  Serial.println(ambientProbeFaultStatus ? "ON\n" : "OFF\n");
  
  bool defrostForcingStatus = controller.getDefrostForcingStatus();
  Serial.print("Defrost Forcing Status: ");
  Serial.println(defrostForcingStatus ? "ON\n" : "OFF\n");
  
  bool coldRoomLightKeyStatus = controller.getColdRoomLightKeyStatus();
  Serial.print("Cold Room Light Key Status: ");
  Serial.println(coldRoomLightKeyStatus ? "ON\n" : "OFF\n");
  
  bool deviceStandByStatus = controller.getDeviceStandByStatus();
  Serial.print("Stand By Status: ");
  Serial.println(deviceStandByStatus ? "ON\n" : "OFF\n");

  ///////////////////////////////////////////////
  Serial.println("\n-----------------------------");
  Serial.print("Time to retrieve registers: ");
  Serial.println(String(millis() - transmissionTimestamp) + "ms");
  Serial.println("-----------------------------\n");

  delay(REGISTER_UPDATE_INTERVAL);
}

// SETTERS

// setTemperatureSetPoint(float value);
// setTemperatureDifferential(float value);
// setDefrostingPeriod(int16_t value);
// setEndOfDefrostingTemperature(int16_t value);
// setMaxDefrostingDuration(int16_t value);
// setDrippingDuration(int16_t value);
// setFansStopDurationPostDefrosting(int16_t value);
// setTemperatureAlarmMinimumThreshold(int16_t value);
// setTemperatureAlarmMaximumThreshold(int16_t value);
// setFansStatusWithStoppedCompressor(int16_t value);
// setFansStopInDefrosting(int16_t value);
// setEvaporatorProbeExclusion(int16_t value);
// setTemperatureAlarmSignalingDelay(int16_t value);
// setCompressorReStartingDelay(int16_t value);
// setAmbientProbeCalibration(float value);
// setCompressorSafetyTimeForDoorSwitch(int16_t value);
// setCompressorRestartTimeAfterDoorOpening(int16_t value);
// setFansBlockageTemperature(int16_t value);
// setDifferentialOnFansBlockage(int16_t value);
// setTemperatureSetPointMinimumLimit(int16_t value);
// setTemperatureSetPointMaximumLimit(int16_t value);
// setTemperatureSettingForAuxRelay(int16_t value);
// setDefrostAtPowerOnStatus(bool value);
// setSmartDefrostStatus(bool value);
// setSmartDefrostSetpoint(int16_t value);
// setDurationOfCompressorOnTimeWithFaultyAmbientProbe(int16_t value);
// setDurationOfCompressorOffTimeWithFaultyAmbientProbe(int16_t value);
// setCorrectionFactorForTheSETButtonDuringNightOperation(float value);
// setBuzzerEnableStatus(bool value);
// setEvaporatorFansActivationForAirRecirculation(int16_t value);
// setEvaporatorFansDurationForAirRecirculation(int16_t value);
// setDefrostForcingStatus(bool value);
// setColdRoomLightKeyStatus(bool value);
// setDeviceStandByStatus(bool value);
