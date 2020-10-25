/**
 * This file contains variables and functions used to 
 * read the analog value from liquid level sensors.
 * Analog Pin A0 is mapped with SUMP
 * Analog Pin A1 is mapped with OHT1 (Used for Cauvery water)
 * Analog Pin A2 is mapped with OHT2 (Used for Bore water)
 */
#ifndef LiquidLevelSensor_h
#define LiquidLevelSensor_h
#include <Arduino.h>

#define SUMP A0
#define OHT1 A1
#define OHT2 A2

enum WATERLEVEL { LVLLO=0, LVL25=25, LVL50=50, LVL75=75, LVLHI=100 };

/**
 * Has functions to read analog value from sensors and
 * output analog values and level indicator outputs
 */
class LiquidLevelSensor {
  private:
    enum WATERLEVEL level;
  public:
    /**
     * Reads analog value of sensor attached to the unit
     * and returns a value with 10 bit precision
     */
    int readWaterLevelSensor(int unit);
    /**
     * Reads the sensor output and converts it to
     * appropriate level indicator value
     */
    int getWaterLevel(int unit);
};

#endif
