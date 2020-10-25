/**
 * 
 */
#ifndef TouchSwitch_h
#define TouchSwitch_h
#include <Arduino.h>

#define SW1 2
#define SW2 3
#define LED1 11
#define LED2 12

volatile byte TSW1 = LOW;
volatile byte TSW2 = LOW;

void toggleS1() {
  TSW1 = !TSW1;
  digitalWrite(LED1, TSW1);
}
 
void toggleS2() {
  TSW2 = !TSW2;
  digitalWrite(LED2, TSW2);
}

class TouchSwitch {
  public:
    TouchSwitch() {
      //Set digital PIN 2 for input and attach interrupt
      pinMode(SW1, INPUT);
      attachInterrupt(digitalPinToInterrupt(SW1), toggleS1, RISING);
      pinMode(LED1, OUTPUT);
      digitalWrite(LED1, LOW);
      //Set digital PIN 3 for input and attach interrupt
      pinMode(SW2, INPUT);
      attachInterrupt(digitalPinToInterrupt(SW2), toggleS2, RISING);
      pinMode(LED2, OUTPUT);
      digitalWrite(LED2, LOW);
    }

    int getSwitchState(int swlabel) {
      return swlabel == SW1 ? TSW1 : TSW2;
    }

    void setSwitch1(byte val) {
      TSW1 = val;
      digitalWrite(LED1, val);
    }
    
    void setSwitch2(byte val) {
      TSW2 = val;
      digitalWrite(LED2, val);
    }
};

#endif
