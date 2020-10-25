/**
 * 
 */

#ifndef RTC_h
#define RTC_h

#include "DS3231.h"

class RealTimeClock {
  public:
    RTClib rtc;
    DS3231 dsmodule;
    char _date_str[16] = {' '};
    char _time_str[16] = {' '};
    char *_dow;
    char figures[10] = "0123456789";
    byte degree[8] = {
      B00100,
      B01010,
      B00100,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000
    };

    float getTemperature() {
      return dsmodule.getTemperature();
    }

    DateTime getCurrentTime() {
      return rtc.now();
    }

    char* getDayOfWeek() {
      char *dow[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
      _dow = dow[dsmodule.getDoW()];
      return _dow;
    }
    
    char* getDate(DateTime time) {
      char months[12][3] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
      int day = time.day();
      _date_str[1] = figures[day%10]; day /= 10;  _date_str[0] = figures[day%10];
      _date_str[2] = '.';
      int mon = time.month()-1;
      char *thisMonth = months[mon];
      _date_str[3] = thisMonth[0]; _date_str[4] = thisMonth[1]; _date_str[5] = thisMonth[2];
      _date_str[6] = '.';
      int year = time.year();
      _date_str[10] = figures[year%10]; year /= 10; 
      _date_str[9] = figures[year%10]; year /= 10; 
      _date_str[8] = figures[year%10]; year /= 10; 
      _date_str[7] = figures[year%10]; 
      return _date_str;
    }

    char* getTime(DateTime time) {
      int hr = time.hour();
      int mm = time.minute();
      char *mer;
      if(hr < 13) {
        mer = "am";
      }
      if(hr > 12) {
        hr -= 12;
        mer = "pm";
      }
      _time_str[1] = figures[hr%10]; hr /= 10; _time_str[0] = figures[hr%10];
      _time_str[2] = ':';
      _time_str[4] = figures[mm%10]; mm /= 10; _time_str[3] = figures[mm%10];
      _time_str[5] = mer[0]; _time_str[6] = mer[1];
      return _time_str;
    }
};
#endif
