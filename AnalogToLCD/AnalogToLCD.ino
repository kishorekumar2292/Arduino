#include  <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
int analogPin = A0;

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Value:");
  lcd.setCursor(0,1);
  lcd.print("Level:");
}

void loop()
{
  int value = analogRead(analogPin);
  lcd.setCursor(6,0);
  lcd.print(value);
  lcd.setCursor(6,1);
  lcd.print(getLevel(value));
  delay(1000);  
}

int getLevel(int analogVal) {
  if(analogVal == 0)
    return 0;
  if(analogVal >= 100 && analogVal < 250)
    return 1;
  if(analogVal >= 250 && analogVal < 350)
    return 2;
  if(analogVal >= 350 && analogVal < 450)
    return 3;
  if(analogVal >= 450)
    return 4;
}
