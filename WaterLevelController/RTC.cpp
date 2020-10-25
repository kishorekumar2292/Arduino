/**
 * 
 */

#include "RTC.h"

float RealTimeClock::getTemperature() {
  return dsmodule.getTemperature();
}

DateTime RealTimeClock::getCurrentTime() {
  return rtc.now();
}

char* RealTimeClock::getDayOfWeek() {
  char dow[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  _dow = (char *) dow[dsmodule.getDoW()];
  return _dow;
}
    
char* RealTimeClock::getDate(DateTime time) {
  char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  int day = time.day();
  _date_str[1] = numbers[day%10]; day /= 10;  _date_str[0] = numbers[day%10];
  _date_str[2] = '.';
  int mon = time.month()-1;
  char *thisMonth = months[mon];
  _date_str[3] = thisMonth[0]; _date_str[4] = thisMonth[1]; _date_str[5] = thisMonth[2];
  _date_str[6] = '.';
  int year = time.year();
  _date_str[10] = numbers[year%10]; year /= 10; 
  _date_str[9] = numbers[year%10]; year /= 10; 
  _date_str[8] = numbers[year%10]; year /= 10; 
  _date_str[7] = numbers[year%10]; 
  return _date_str;
}

char* RealTimeClock::getTime(DateTime time) {
  int hr = time.hour();
  int mm = time.minute();
  char mer[2];
  if(hr < 13) {
    mer[0] = 'a';
    mer[1] = 'm';
  }
  if(hr > 12) {
    hr -= 12;
    mer[0] = 'p';
    mer[1] = 'm';
  }
  _time_str[1] = numbers[hr%10]; hr /= 10; _time_str[0] = numbers[hr%10];
  _time_str[2] = ':';
  _time_str[4] = numbers[mm%10]; mm /= 10; _time_str[3] = numbers[mm%10];
  _time_str[5] = mer[0]; _time_str[6] = mer[1];
  return _time_str;
}
