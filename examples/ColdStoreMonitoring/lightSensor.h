#include <Arduino.h>

#ifndef SerialPort
#define SerialPort Serial
#endif

#define SENSOR_MAX_VALUE 940 // Max value measured in sun light
 
bool getAmbientLightStatus(uint8_t sensorPin, uint8_t threshold = 25) {
    #if defined(ARDUINO_ARCH_SAMD)
        analogReference(AR_INTERNAL2V23);
    #endif
    int rawSensorValue = analogRead(sensorPin);
    analogReference(AR_DEFAULT);
    int lightLevel = map(rawSensorValue, 0, SENSOR_MAX_VALUE, 0, 100);
    #if defined(DEBUG)
        SerialPort.print("Light level: "); 
        SerialPort.println(lightLevel); // print the light value in Serial Monitor
    #endif
    return lightLevel >= threshold;
}