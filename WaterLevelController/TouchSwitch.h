/**
 *TouchSwitch library is to handle the capacitive touch switch
 *interrupts to Arduino and set the output to LEDs.
 *
 *toggleS1() and toggleS2() functions are the interrupt service
 *routines that changes a volatile global variable
 */
#ifndef TouchSwitch_h
#define TouchSwitch_h
#include <Arduino.h>
#include <TimedAction.h>

#define SW1 2
#define SW2 3
#define LED1 11
#define LED2 12

void toggleS1();
void toggleS2();

class TouchSwitch {
  public:
    TouchSwitch();
    int getSwitchState(int swlabel);
    void setSwitch1(byte val);
    void setSwitch2(byte val);
};

#endif
