#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"

#if !defined(MINIMAL_THINGS_CONFIG)
CloudTemperature evaporatorTemperature;
bool ambientProbeFaultStatus;
bool coldRoomLightRelayStatus;
bool compressorProtectionAlarmStatus;
bool compressorProtectionStatus;
bool compressorRelayStatus;
bool defrostRelayStatus;
bool doorSwitchStatus;
bool drippingStatus;
bool eepromErrorStatus;
bool evaporatorProbeFaultStatus;
bool fansRelayStatus;
bool hotResistanceStatus;
bool lightAlarmStatus;
bool manInRoomAlarmStatus;
bool standByStatus;
#endif

bool deviceResponsive;
bool openDoorAlarmStatus;
bool temperatureAlarmStatus;
CloudTemperature ambientTemperature;

#if defined(USE_EXTERNAL_LIGHT_SENSOR)
bool ambientLightStatus;
#endif

void initProperties(){

  #if !defined(MINIMAL_THINGS_CONFIG)  
  ArduinoCloud.addProperty(evaporatorTemperature, READ, ON_CHANGE, NULL, 1);
  ArduinoCloud.addProperty(ambientProbeFaultStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(coldRoomLightRelayStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(compressorProtectionAlarmStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(compressorProtectionStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(compressorRelayStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(defrostRelayStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(doorSwitchStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(drippingStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(eepromErrorStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(evaporatorProbeFaultStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(fansRelayStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(hotResistanceStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(lightAlarmStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(manInRoomAlarmStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(standByStatus, READ, ON_CHANGE, NULL);
  #endif

  ArduinoCloud.addProperty(deviceResponsive, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(openDoorAlarmStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(temperatureAlarmStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(ambientTemperature, READ, ON_CHANGE, NULL, 1);  

  #if defined(USE_EXTERNAL_LIGHT_SENSOR)  
  ArduinoCloud.addProperty(ambientLightStatus, READ, ON_CHANGE, NULL);
  #endif
}

#if defined(SECRET_APN)
  // GSM configuration
  const char GPRS_APN[]      = SECRET_APN;
  const char PINNUMBER[]     = SECRET_PIN;
  const char GPRS_LOGIN[]    = SECRET_USERNAME;
  const char GPRS_PASSWORD[] = SECRET_PASSWORD;
  GSMConnectionHandler ArduinoIoTPreferredConnection(PINNUMBER, GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD);

#elif defined(SECRET_APP_EUI)
  const char APPEUI[]      = SECRET_APP_EUI;
  const char APPKEY[]     = SECRET_APP_KEY;
  LoRaConnectionHandler ArduinoIoTPreferredConnection(APPEUI, APPKEY, _lora_band::EU868);

#elif defined(SECRET_SSID)
  const char SSID[]      = SECRET_SSID;
  const char PASS[]     = SECRET_PASS;
  WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
#else
  #error No IoT Cloud configuration found.
#endif
