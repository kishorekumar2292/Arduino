#include <SoftwareSerial.h>

SoftwareSerial BlueTooth(4,5); /* (Rx,Tx) */

void setup() {
  BlueTooth.begin(9600);  /* Define baud rate for software serial communication */
  Serial.begin(9600); /* Define baud rate for serial communication */
}

void loop() {
  if(BlueTooth.available()) {
    Serial.write(BlueTooth.read());
  }
  if(Serial.available()) {
    BlueTooth.write(Serial.read());
  }
}
