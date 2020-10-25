#include <LiquidCrystal_I2C.h>
#include "DS3231.h"

RTClib rtc;
DS3231 clock;
LiquidCrystal_I2C lcd(0x27,16,2); //Initialize LCD for 16x2 at address 0x27

DateTime currentTime;
float temperature;
char str[16] = {'0'};

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {

  currentTime = rtc.now();
  temperature = clock.getTemperature();
  lcd.setCursor(0,0);
  lcd.print(timestamp(currentTime));
  lcd.setCursor(0,1);
  lcd.print("Temp:");
  lcd.setCursor(5,1);
  lcd.print(temperature);
  delay(1000);
  
}

char* timestamp(DateTime time)
{
  char figures[] = "0123456789"; 
  int day = time.day();
  str[1] = figures[day%10]; day /= 10;  str[0] = figures[day%10];
  str[2] = '/';
  int mon = time.month();
  str[4] = figures[mon%10]; mon /= 10;  str[3] = figures[mon%10];
  str[5] = '/';
  int year = time.year();
  str[7] = figures[year%10]; year /= 10; str[6] = figures[year%10];
  str[8] = ' ';
  int hr = time.hour();
  str[10] = figures[hr%10]; hr /= 10; str[9] = figures[hr%10];
  str[11] = ':';
  int mm = time.minute();
  str[13] = figures[mm%10]; mm /= 10; str[12] = figures[mm%10];
  return str;
}
