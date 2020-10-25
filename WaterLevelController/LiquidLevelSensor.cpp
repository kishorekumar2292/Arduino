/**
 * This file contains variables and functions used to 
 * read the analog value from liquid level sensors.
 * Analog Pin A0 is mapped with SUMP
 * Analog Pin A1 is mapped with OHT1 (Used for Cauvery water)
 * Analog Pin A2 is mapped with OHT2 (Used for Bore water)
 */
#include "LiquidLevelSensor.h"

/**
 * Reads analog value of sensor attached to the unit
 * and returns a value with 10 bit precision
 */
int LiquidLevelSensor::readWaterLevelSensor(int unit) {
  int value = 0;
  if(unit == SUMP || unit == OHT1 ||unit == OHT2)
    value = analogRead(unit);
  return value;
}

/**
 * Reads the sensor output and converts it to
 * appropriate level indicator value
 */
int LiquidLevelSensor::getWaterLevel(int unit) {
  int analogVal = readWaterLevelSensor(unit);
  if(analogVal == 0)
    return LVLLO;
  if(analogVal >= 100 && analogVal < 250)
    return LVL25;
  if(analogVal >= 250 && analogVal < 350)
    return LVL50;
  if(analogVal >= 350 && analogVal < 450)
    return LVL75;
  if(analogVal >= 450)
    return LVLHI;
}
