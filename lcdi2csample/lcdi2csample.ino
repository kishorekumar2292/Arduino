#include  <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
}

void loop()
{
  char string[] = "Hello World! Kishore Vijayakumar";
  int len = sizeof(string)/sizeof(string[0]);
  int exlen = len-16;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(string);
  for(int i=0;i<exlen;i++) 
  {
    delay(500);
    lcd.scrollDisplayLeft();
  }
  delay(2000);  
}
