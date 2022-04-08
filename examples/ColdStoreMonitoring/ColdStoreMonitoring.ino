// In the minimal configuration only a few thing properties are populated
// Check thingProperties.h to find out which ones.
#define MINIMAL_THINGS_CONFIG

// Defines if an external light sensor is attached and should be used
// This is useful to check e.g. if the light is on in the room containing the cold store
#define USE_EXTERNAL_LIGHT_SENSOR
#define LIGHT_SENSOR_PIN A3


// Enable debugging
#define DEBUG

#if defined(DEBUG)
  #define DEBUG_MESSAGE_LEVEL DBG_VERBOSE
#else
  #define DEBUG_MESSAGE_LEVEL DBG_INFO
#endif

#include <Arduino.h>
#include "thingProperties.h"
#include "PegoController.h"
#if defined(USE_EXTERNAL_LIGHT_SENSOR)
  #include "lightSensor.h"
#endif

// Defines the serial port to be used for debugging
#define SerialPort Serial //Serial1 for debugging on the hardware serial pins

// Defines the baud rate of the serial port for debugging
#define SERIAL_BAUDRATE 19200

// Defines how often the peripheral's register are read in ms
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

void setupCloud(){
  Debug.setDebugOutputStream(&SerialPort);
  
  initProperties(); // Defined in thingProperties.h

  SerialPort.println("Connecting to Arduino IoT cloud...");

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection, true /* enable watch dog */);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(DEBUG_MESSAGE_LEVEL);
  ArduinoCloud.printDebugInfo();
}

/**
 * @brief Sets up the Pego Controller and starts the ModBus connection
 * If the setup is successful the build-in LED should be lit
 * If the setup failed the built-in LED should blinks at a 500ms rate
 */
void setupController(){
  if(!controller.begin()){
    SerialPort.println("Failed to start Modbus RTU Client!");    
    while (true){ blinkLED(500); }
  };  
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  SerialPort.begin(SERIAL_BAUDRATE);
  delay(5000);

  SerialPort.println("Starting the Modbus RTU client...");    
  setupController();

  setupCloud();
}

/**
 * @brief Reads the register values from the Pego controller and saves the values
 * into the cloud variables.
 */
void readValuesFromController(){
  deviceResponsive = controller.responsive();

  if(!deviceResponsive){
    SerialPort.println("Couldn't reach the controller. Power outage?");
    return;
  }

  ambientTemperature = controller.getAmbientTemperature();
  SerialPort.print("Ambient Temperature: ");
  SerialPort.print(ambientTemperature);
  SerialPort.println(" °C\n");

  openDoorAlarmStatus = controller.getOpenDoorAlarmStatus();
  SerialPort.print("Open Door Alarm Status: ");
  SerialPort.println(openDoorAlarmStatus ? "ON\n" : "OFF\n");
  
  temperatureAlarmStatus = controller.getTemperatureAlarmStatus();    
  SerialPort.print("Temperature alarm: ");
  SerialPort.println(temperatureAlarmStatus ? "ON\n" : "OFF\n");
  
  #if !defined(MINIMAL_THINGS_CONFIG)  

    evaporatorTemperature = controller.getEvaporatorTemperature();
    SerialPort.print("Evaporator Temperature: ");
    SerialPort.print(evaporatorTemperature);
    SerialPort.println(" °C\n");

    #ifdef ECP_202
      hotResistanceStatus = controller.getHotResistanceStatus();
      SerialPort.print("Hot Resistance Status: ");
      SerialPort.println(hotResistanceStatus ? "ON\n" : "OFF\n");
      
      standByStatus = controller.getStandByStatus();
      SerialPort.print("Stand By Status: ");
      SerialPort.println(standByStatus ? "ON\n" : "OFF\n");
    #endif
    
    drippingStatus = controller.getDrippingStatus();
    SerialPort.print("Dripping Status: ");
    SerialPort.println(drippingStatus ? "ON\n" : "OFF\n");
    
    coldRoomLightRelayStatus = controller.getColdRoomLightRelayStatus();
    SerialPort.print("Cold Room Light Relay Status: ");
    SerialPort.println(coldRoomLightRelayStatus ? "ON\n" : "OFF\n");
    
    fansRelayStatus = controller.getFansRelayStatus();
    SerialPort.print("Fans Relay Status: ");
    SerialPort.println(fansRelayStatus ? "ON\n" : "OFF\n");
    
    defrostRelayStatus = controller.getDefrostRelayStatus();
    SerialPort.print("Defrost Relay Status: ");
    SerialPort.println(defrostRelayStatus ? "ON\n" : "OFF\n");
    
    compressorRelayStatus = controller.getCompressorRelayStatus();
    SerialPort.print("Compressor Relay Status: ");
    SerialPort.println(compressorRelayStatus ? "ON\n" : "OFF\n");
    
    compressorProtectionStatus = controller.getCompressorProtectionStatus();
    SerialPort.print("Compressor Protection Status: ");
    SerialPort.println(compressorProtectionStatus ? "ON\n" : "OFF\n");
    
    doorSwitchStatus = controller.getDoorSwitchStatus();    
    SerialPort.print("Door open: ");
    SerialPort.println(doorSwitchStatus ? "YES\n" : "NO\n");
    
    #ifdef ECP_202
      lightAlarmStatus = controller.getLightAlarmStatus();
      SerialPort.print("Light Alarm Status: ");
      SerialPort.println(lightAlarmStatus ? "ON\n" : "OFF\n");
      
      compressorProtectionAlarmStatus = controller.getCompressorProtectionAlarmStatus();
      SerialPort.print("Compressor Protection Alarm Status: ");
      SerialPort.println(compressorProtectionAlarmStatus ? "ON\n" : "OFF\n");
      
      manInRoomAlarmStatus = controller.getManInRoomAlarmStatus();
      SerialPort.print("Man In Room Alarm: ");
      SerialPort.println(manInRoomAlarmStatus ? "ON\n" : "OFF\n");
    #endif
    
    eepromErrorStatus = controller.getEEPROMErrorStatus();
    SerialPort.print("EEPROM Error Status: ");
    SerialPort.println(eepromErrorStatus ? "ON\n" : "OFF\n");
    
    evaporatorProbeFaultStatus = controller.getEvaporatorProbeFaultStatus();
    SerialPort.print("Evaporator Probe Fault Status: ");
    SerialPort.println(evaporatorProbeFaultStatus ? "ON\n" : "OFF\n");
    
    ambientProbeFaultStatus = controller.getAmbientProbeFaultStatus();
    SerialPort.print("Ambient Probe Fault Status: ");
    SerialPort.println(ambientProbeFaultStatus ? "ON\n" : "OFF\n");  
  #endif
}

void loop() {
  // Indicate that the Modbus client & IoTCloud connection was started successfully
  digitalWrite(LED_BUILTIN, ArduinoCloud.connected() ? HIGH : LOW);  
  
  static auto lastCheck= millis();

  if (millis() - lastCheck >= REGISTER_UPDATE_INTERVAL) {
    lastCheck = millis();
    readValuesFromController();
    #if defined(USE_EXTERNAL_LIGHT_SENSOR)
      ambientLightStatus = getAmbientLightStatus(LIGHT_SENSOR_PIN);
      SerialPort.print("Ambient Light Status: ");
      SerialPort.println(ambientLightStatus ? "ON\n" : "OFF\n");
    #endif   
  }
  ArduinoCloud.update();
}
