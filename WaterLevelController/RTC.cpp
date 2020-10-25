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

String RealTimeClock::getDayOfWeek() {
  String _dowStr[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  return _dowStr[dsmodule.getDoW() - 1];
}
    
String RealTimeClock::getDateStr(DateTime time) {
  String months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  String _dtStr = String(String(time.day(),DEC) + "." + months[time.month() - 1] + "." + String(time.year(), DEC));
  return _dtStr;
}

String RealTimeClock::getTimeStr(DateTime time) {
  int hour = time.hour();
  int minute = time.minute();
  String ampm = hour < 12 ? String("am") : String("pm");
  hour = hour > 12 ? hour - 12 : hour;
  String hr = hour < 10 ? String("0" + String(hour,DEC)) : String(hour,DEC);
  String mm = minute < 10 ? String("0" + String(minute,DEC)) : String(minute,DEC);
  String _tsStr = String(hr + ":" + mm + ampm);
  return _tsStr;
}

String RealTimeClock::getTimeStampStr(DateTime dt) {
  String _timeStamp = String(String(dt.hour(), DEC) + ":" + String(dt.minute(), DEC) + ":" + String(dt.second(), DEC) + " " + String(dt.day(), DEC) + "/" + String(dt.month(), DEC) + "/" + String(dt.year(), DEC));
  return _timeStamp;
}
