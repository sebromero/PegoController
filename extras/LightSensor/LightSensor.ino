  
int light_sensor = A3; 
#define LIGHT_ON_THRESHOLD 25
 
void setup() {
  Serial.begin(9600); //begin Serial Communication
}
 
void loop() {
  analogReference(AR_INTERNAL2V23);
  int raw_light = analogRead(light_sensor); // read the raw value from light_sensor pin (A3)
  //Serial.println(raw_light);
  analogReference(AR_DEFAULT);
  int light = map(raw_light, 0, 940, 0, 100); // map the value from 0, 1023 to 0, 100
 
  Serial.print("Light level: "); 
  Serial.println(light); // print the light value in Serial Monitor
 
  delay(250); // add a delay to only read and print every 1 second
}
