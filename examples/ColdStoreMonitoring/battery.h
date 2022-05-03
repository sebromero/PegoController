// Use voltmeter to determine battery voltage when fully charged
float batteryLevel(float battery_voltage = 4.3){    
    analogReadResolution(10); // 10bit resolution
    analogReference(AR_INTERNAL1V0);
    // AR_DEFAULT: the default analog reference of 3.3V
    // AR_INTERNAL1V0: a built-in 1.0V reference

    // read the input on analog pin 0:
    int sensorValue = analogRead(ADC_BATTERY);

    // Convert the analog reading (which goes from 0 - 1023)
    float voltage = sensorValue * (battery_voltage / 1023.0);
    // print out the value you read:
    // Serial.print("Voltage: ");
    // Serial.print(voltage);
    // Serial.println("V");

    float battery_percentage = ((voltage * 100) / battery_voltage);

    // Serial.print("Battery Percentage: ");
    // Serial.print(battery_percentage);
    // Serial.println("%");

    analogReference(AR_DEFAULT);
    return battery_percentage;
}

bool batteryConnected(){
    return batteryLevel() > 5;
}