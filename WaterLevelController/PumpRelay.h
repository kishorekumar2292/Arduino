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

#define RELAYA 10
#define RELAYB1 9
#define RELAYB2 8

class PumpRelay {
  public:
    /**
     * Initializes the pins as digital output
     */
    PumpRelay() {
      /**
       * Set PIN for monoblock pump as output
       * Initialize with HIGH so relay is open
       */
      pinMode(RELAYA, OUTPUT);
      digitalWrite(RELAYA, LOW);
      /**
       * Set PINs for submersible pump as output
       * Initialize both PINs as HIGH so relay is open
       */
      pinMode(RELAYB1, OUTPUT);
      pinMode(RELAYB2, OUTPUT);
      digitalWrite(RELAYB1, LOW);
      digitalWrite(RELAYB2, LOW);
    }
    
    /**
     * Switches both relays to ON initailly and 
     * cuts of starter capacitor after 1s
     */
    void submersiblePumpOn() {
      digitalWrite(RELAYB1, HIGH);
      digitalWrite(RELAYB2, HIGH);
      delay(1000);
      digitalWrite(RELAYB2, LOW);
    }
    
    /**
     * Cuts of the main relay of submersible pump
     */
    void submersiblePumpOff() {
      digitalWrite(RELAYB1, LOW);
    }
    
    /**
     * Sets Relay A ON
     */
    void monoblockPumpOn() {
      digitalWrite(RELAYA, HIGH);
    }
    
    /**
     * Sets Relay A OFF
     */
    void monoblockPumpOff() {
      digitalWrite(RELAYA, LOW);
    }
};

#endif
