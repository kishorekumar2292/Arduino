/**
 * 
 */

#ifndef RTC_h
#define RTC_h

#include "DS3231.h"

class RealTimeClock {
  private:
    RTClib rtc;
    DS3231 dsmodule;
  public:
    uint8_t degree[8] = {
      B00100,
      B01010,
      B00100,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000
    };

    float getTemperature();
    DateTime getCurrentTime();
    String getDayOfWeek();    
    String getDateStr(DateTime time);
    String getTimeStr(DateTime time);
    String getTimeStampStr(DateTime dt);
  private:
    const char numbers[11] = "0123456789";
};
#endif
