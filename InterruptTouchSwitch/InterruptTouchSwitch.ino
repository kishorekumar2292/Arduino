#include  <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const byte sw1 = 2;
const byte sw2 = 3;
volatile byte s1op = LOW;
volatile byte s2op = LOW;

void setup()
{
  pinMode(sw1, INPUT);
  attachInterrupt(digitalPinToInterrupt(sw1), toggleS1, RISING);
  pinMode(sw2, INPUT);
  attachInterrupt(digitalPinToInterrupt(sw2), toggleS2, RISING);

  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Bore Pump:");  
  lcd.setCursor(0,1);
  lcd.print("Cauvery Pump:");  
}

void loop()
{
  lcd.setCursor(10,0);
  lcd.print(s1op==0?"OFF":"ON ");
  lcd.setCursor(13,1);
  lcd.print(s2op==0?"OFF":"ON ");
  //delay(1000);
}

void toggleS1() {
  s1op = !s1op;
}

void toggleS2() {
  s2op = !s2op;
}
