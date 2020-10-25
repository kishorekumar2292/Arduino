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
    char _date_str[16] = {' '};
    char _time_str[16] = {' '};
    char *_dow;
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
    char* getDayOfWeek();    
    char* getDate(DateTime time);
    char* getTime(DateTime time);
  private:
    const char numbers[11] = "0123456789";
};
#endif
