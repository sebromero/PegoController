/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
}

// the loop function runs over and over again forever
void loop() {
  float battery_voltage = 4.3; // use voltmeter

  analogReadResolution(10); // 10bit resolution
  analogReference(AR_INTERNAL1V0); 
  // AR_DEFAULT: the default analog reference of 3.3V 
  // AR_INTERNAL1V0: a built-in 1.0V reference
  // read the input on analog pin 0:
  int sensorValue = analogRead(ADC_BATTERY);
  // Convert the analog reading (which goes from 0 - 1023) 
  float voltage = sensorValue * (battery_voltage / 1023.0);
  // print out the value you read:
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println("V");

  float battery_percentage = ((voltage * 100) / battery_voltage);

  Serial.print("Batery Percentage: ");
  Serial.print(battery_percentage);
  Serial.println("%");

  analogReference(AR_DEFAULT);
}
