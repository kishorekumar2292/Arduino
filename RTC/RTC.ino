#include <Wire.h>
#include "DS3231.h"

RTClib rtc;
DS3231 clock;

int serialbyte=0;
DateTime currentTime;

byte d,m,y,dow,hh,mm,ss;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  Serial.println("----Menu----");
  Serial.println("1.Now");
  Serial.println("2.Set Time");
  Serial.print("Choice: ");
}

void loop() {
  if(Serial.available()>0)
  {
    serialbyte = Serial.read();
    Serial.print((char)serialbyte);
    Serial.println('\n');
    
    switch(serialbyte)
    {
      case '1':
        printCurrentTime();
        break;
      case '2':
        Serial.println("Set Date Time");
        getDateTime(d, m, y, dow, hh, mm, ss);        
        clock.setClockMode(false);  // set to 24h
        clock.setYear(y);
        clock.setMonth(m);
        clock.setDate(d);
        clock.setDoW(dow);
        clock.setHour(hh);
        clock.setMinute(mm);
        clock.setSecond(ss);
        Serial.println("Time set!");
        printCurrentTime();
        break;
      default:
        Serial.print("Invalid choice");
    }

    Serial.println('\n');
    Serial.println("----Menu----");
    Serial.println("1.Now");
    Serial.println("2.Set Time");
    Serial.print("Choice: ");
  }
}

void printCurrentTime()
{
  currentTime = rtc.now();
  Serial.print("Now: ");
  printTime(currentTime);
  char dow[7][3] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  Serial.print(" ");
  Serial.print(clock.getDoW());
  Serial.println();
}

void errorInvalidMsg()
{
  Serial.println("Error: Invalid value. Enter a valid value!");
}

int getDay()
{
  int d;
  Serial.print("Day: ");
  d = getIntInput();
  if(!(d > 0 && d <=31))
  {
    errorInvalidMsg();
    d = getDay();
  }
  return d;
}

int getMonth()
{
  int m;
  Serial.print("Month: ");
  m = getIntInput();
  if(!(m > 0 && m <=12))
  {
    errorInvalidMsg();
    m = getMonth();
  }
  return m;
}

int getYear()
{
  int y;
  Serial.print("Year: ");
  y = getIntInput();
  if(!(y > 0))
  {
    errorInvalidMsg();
    y = getYear();
  }
  return y;
}

int getDoW()
{
  int dow;
  Serial.print("Day of week: ");
  dow = getIntInput();
  if(!(dow >= 1 && dow <= 7))
  {
    errorInvalidMsg();
    dow = getDoW();
  }
  return dow;
}

int getHour()
{
  int hh;
  Serial.print("Hour: ");
  hh = getIntInput();
  if(!(hh >= 0 && hh <= 23))
  {
    errorInvalidMsg();
    hh = getHour();
  }
  return hh;
}

int getMinute()
{
  int mm;
  Serial.print("Min: ");
  mm = getIntInput();
  if(!(mm >= 0 && m <= 59))
  {
    errorInvalidMsg();
    mm = getMinute();
  }
  return mm;
}

int getSecond()
{
  int ss;
  Serial.print("Sec: ");
  ss = getIntInput();
  if(!(ss >= 0 && d <= 59))
  {
    errorInvalidMsg();
    ss = getSecond();
  }
  return ss;
}

void getDateTime(byte &d, byte &m, byte &y, byte &dow, byte &hh, byte &mm, byte &ss)
{
  d = (byte)getDay();
  m = (byte)getMonth();
  y = (byte)getYear();
  dow = (byte)getDoW();
  hh = (byte)getHour();
  mm = (byte)getMinute();
  ss = (byte)getSecond();
}

void printTime(DateTime time) 
{
  if(currentTime.day() < 10)
    Serial.print('0');
  Serial.print(currentTime.day(), DEC);
  Serial.print("-");
  if(currentTime.month() < 10)
    Serial.print('0');
  Serial.print(currentTime.month(), DEC);
  Serial.print("-");
  Serial.print(currentTime.year(), DEC);
  Serial.print(" ");
  if(currentTime.hour() < 10)
    Serial.print('0');
  Serial.print(currentTime.hour(), DEC);
  Serial.print(":");
  if(currentTime.minute() < 10)
    Serial.print('0');
  Serial.print(currentTime.minute(), DEC);
  Serial.print(":");
  if(currentTime.second() < 10)
    Serial.print('0');
  Serial.print(currentTime.second(), DEC);
}

int getIntInput()
{
  int input=0;
  int value=-1;
  while(true)
  {
    if(Serial.available() > 0)
    {
      input = Serial.read();
      if(input >= 48 && input <= 57)
      {
        if(value <= 0)
        {
          value = input-48;
        }
        else
        {
          value = (value * 10) + (input - 48);
        }
      }
      if(input == 13)
        break;
      
      Serial.print((char)input);
    }
  }
  Serial.println();
  return value;
}
