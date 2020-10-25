/**
 * This file contains variables and functions used to 
 * control the relays attached to pumps.
 * Digital pins 9 & 10 are used in combination for the
 * submersible pump starter.
 * Digital pin 11 is used to control the relay attached
 * with monoblock pump
 */

#ifndef PumpRelay_h
#define PumpRelay_h

#include <Arduino.h>

class PumpRelay {
  private:
    int RELAYA;
    int RELAYB1, RELAYB2;
  public:
    /**
     * Initializes the pins as digital output
     */
    PumpRelay(int rA, int rB1, int rB2);
    /**
     * Switches both relays to ON initailly and 
     * cuts of starter capacitor after 1s
     */
    void submersiblePumpOn();
    /**
     * Cuts of the main relay of submersible pump
     */
    void submersiblePumpOff();
    /**
     * Sets Relay A ON
     */
    void monoblockPumpOn();
    /**
     * Sets Relay A OFF
     */
    void monoblockPumpOff();
};

#endif
