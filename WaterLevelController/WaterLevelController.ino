#include "RTC.h"
#include "PumpRelay.h"
#include "TouchSwitch.h"
#include "LiquidLevelSensor.h"
#include "AdminConsole.h"
#include "Themes.h"

#include <TimedAction.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial BlueTooth(4, 5);

LiquidLevelSensor sensor;
TouchSwitch touchswitch;
RealTimeClock rtc;
PumpRelay relay(10, 9, 8); //RA, RB1, RB2

volatile byte P1 = LOW, P2 = LOW;
DateTime P1Start, P1Stop, P2Start, P2Stop;
volatile boolean P1Auto = true, P2Auto = true;
volatile int sump, tank1, tank2;
String btcommand, BTRESCODE="";

/**
 * Gets bluetooth serial output, parses it and executes a command.
 * It echos the status to serial terminal
 */
void getBlueToothCommand() {
  if(BlueTooth.available()) {
    btcommand = BlueTooth.readString();
    parseBTCommand(btcommand);
  }
}
/**
 * Serial for configuration through PC
 */
void getSerialData() {
  if(Serial && !serialconnected) {
    serialconnected = true;
    adminConsoleisActive = true;
    printAdminConsoleMenu();
  }
  if(Serial.available() > 0 && adminConsoleisActive) {
    int indata = Serial.read();
    Serial.println(indata-48);
    
    switch(indata) {
      case '1':
        Serial.print("Current date/time is "); 
        Serial.print(rtc.getDate(rtc.getCurrentTime()));
        Serial.print(" ");
        Serial.print(rtc.getTime(rtc.getCurrentTime()));
        Serial.print(", ");
        Serial.print(rtc.getDayOfWeek());
        Serial.print(" | ");
        Serial.print(rtc.getTemperature());
        Serial.println(" ^c");
        Serial.println("");
        break;
      case '2':
        Serial.println("Cauvery water sump, tank & pump status");
        Serial.println(String("Sump:" + String(sump) + "% | Tank:" + String(tank1) + "%"));
        Serial.println(String("Pump Mode:" + String(P1Auto ? "Auto" : "Manual") + " | Pump Status:" + String(P1 == HIGH ? "ON" : "OFF")));
        Serial.println("");
        break;
      case '3':
        Serial.println("Bore water tank & pump status");
        Serial.println(String("Tank:" + String(tank2) + "%"));
        Serial.println(String("Pump Mode:" + String(P2Auto ? "Auto" : "Manual") + " | Pump Status:" + String(P2 == HIGH ? "ON" : "OFF")));
        Serial.println("");
        break;
      case '4':
        adminConsoleisActive = false;
        Serial.println("Good Bye :)");
        break;
      default:
        Serial.println("Invalid input!");
        Serial.println("");
    }
    if(adminConsoleisActive)
      printAdminConsoleMenu();
  }
}
/**
 * Repeatedly looks for the status of sensors and perform appropriate action
 */
void monoblockPumpCheck() {
  /*Read sensor values*/
  sump = sensor.getWaterLevel(SUMP);
  tank1 = sensor.getWaterLevel(OHT1);
  /**
   * When PumpStateOn && ( tank1 == Full || sump == Empty)
   * Set AutoModeOn, PumpOff
   * 
   * AUTOSTOP
   *  - OVERFLOW
   *  - SUMP EMPTY
   */
  if(P1 == HIGH && (tank1 >= LVLHI || sump <= LVLLO)) {
    relay.monoblockPumpOff();
    P1Stop = rtc.getCurrentTime();
    P1Auto = true;
    P1 = LOW;
    touchswitch.setSwitch1(LOW);
  }
  /**
   * Functions when Auto Mode ON
   */
  if(P1Auto) {
    /**
     * When tank1 between OnThreshold && PumpStateOff && AutoModeOn && Sump more than empty
     * Set PumpOn
     * 
     * AUTOSTART
     */
    if(tank1 <= LVL50 && P1 == LOW && sump > LVLLO) {
      relay.monoblockPumpOn();
      P1Start = rtc.getCurrentTime();
      P1 = HIGH;
      touchswitch.setSwitch1(HIGH);
    }
    /**
     * When PumpStateOn && TouchSwitchLow && AutoModeOn
     * Set AutoModeOff, PumpOff
     * 
     * MANUALSTOP
     */
    if(P1 == HIGH && touchswitch.getSwitchState(SW1) == LOW) {
      relay.monoblockPumpOff();
      P1Stop = rtc.getCurrentTime();
      P1Auto = false;
      P1 = LOW;
      touchswitch.setSwitch1(LOW);
      BTRESCODE = "OK";
    }
    /**
     * When PumpStateOff && TouchSwitchHigh
     * Set the PumpStateOn, Disable AutoMode
     * 
     * MANUALSTART
     */
    if(P1 == LOW && touchswitch.getSwitchState(SW1) == HIGH) {
      if(sump > LVLLO && tank1 < LVLHI) {
        relay.monoblockPumpOn();
        P1Start = rtc.getCurrentTime();
        P1 = HIGH;
        touchswitch.setSwitch1(HIGH);
        P1Auto = false;
        BTRESCODE = "OK";
      } 
      else if (sump <= LVLLO) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Sump is empty!");
        lcd.setCursor(0,1);
        lcd.print("Can't start pump");
        P1 = LOW;
        touchswitch.setSwitch1(LOW);
        BTRESCODE = "NOTOK:SUMP EMPTY";
      }
      else if (tank1 >= LVLHI) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tank is full!");
        lcd.setCursor(0,1);
        lcd.print("Can't start pump");
        P1 = LOW;
        touchswitch.setSwitch1(LOW);
        BTRESCODE = "NOTOK:TANK FULL";
      }
    }  
  }
  /**
   * When AutoModeOff
   * Display message: Touch SW1 to reset AutoModeOn
   */
  else {
    /**
     * When PumpStateOff && touch switch 1 is triggered
     * Reset AutoModeOn
     */
    if(P1 == LOW && touchswitch.getSwitchState(SW1) == HIGH) {
      relay.monoblockPumpOn();
      P1Start = rtc.getCurrentTime();
      P1 = HIGH;
      touchswitch.setSwitch1(HIGH);
      P1Auto = true;
      BTRESCODE = "OK";
    }
    if(P1 == HIGH && touchswitch.getSwitchState(SW1) == LOW) {
      relay.monoblockPumpOff();
      P1Start = rtc.getCurrentTime();
      P1 = LOW;
      touchswitch.setSwitch1(LOW);
      P1Auto = true;
      BTRESCODE = "OK";
    }
  }
}
/**
 * Repeatedly looks for the status of sensors and perform appropriate action
 */
void submersiblePumpCheck() {
  /*Read sensor values*/
  tank2 = sensor.getWaterLevel(OHT2);
  /**
   * When tank2 >= FULL && PumpStateOn
   * AUTOSTOP
   */
  if(tank2 >= LVLHI && P2 == HIGH) {
    relay.submersiblePumpOff();
    P2Stop = rtc.getCurrentTime();
    P2Auto = true;
    P2 = LOW;
    touchswitch.setSwitch2(LOW);
  }
  /**
   * When AUTOMODEON
   */
  if(P2Auto) {
    /**
     * When tank2 <= OnThreshold && PumpStateOff
     * Set PumpOn
     * 
     * AUTOSTART
     */
    if(tank2 <= LVL50 && P2 == LOW) {
      relay.submersiblePumpOn();
      P2Start = rtc.getCurrentTime();
      P2 = HIGH;
      touchswitch.setSwitch2(HIGH);
    }
    /**
     * When PumpStateOn && TouchSwitchLow
     * Set AutoModeOff, PumpOff
     * 
     * MANUALSTOP
     */
    if(P2 == HIGH && touchswitch.getSwitchState(SW2) == LOW) {
      relay.submersiblePumpOff();
      P2Stop = rtc.getCurrentTime();
      P2Auto = false;
      P2 = LOW;
      touchswitch.setSwitch2(LOW);
      BTRESCODE = "OK";
    }
    /**
     * When PumpStateOff && TouchSwitchHigh
     * Set AutoModeOff, PumpOn
     * 
     * MANUALSTART
     */
    if(P2 == LOW && touchswitch.getSwitchState(SW2) == HIGH) {
      if(tank2 < LVLHI) {
        relay.submersiblePumpOn();
        P2Stop = rtc.getCurrentTime();
        P2Auto = false;
        P2 = HIGH;
        touchswitch.setSwitch2(HIGH);
        BTRESCODE = "OK";
      }
      else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tank is full!");
        lcd.setCursor(0,1);
        lcd.print("Can't start pump");
        P2 = LOW;
        touchswitch.setSwitch2(LOW);
        BTRESCODE = "NOTOK:TANK FULL";
      }
    }
  }  
  /**
   * When AutoModeOff
   * Display message: Touch SW2 to reset AutoModeOn
   */
  else {
    /**
     * When PumpStateOff && touch switch 2 is HIGH
     * AutoModeOn
     * PumpOn
     */
    if(P2 == LOW && touchswitch.getSwitchState(SW2) == HIGH) {
      relay.submersiblePumpOn();
      P2Start = rtc.getCurrentTime();
      P2 = HIGH;
      touchswitch.setSwitch2(HIGH);
      P2Auto = true;
      BTRESCODE = "OK";
    }
    /**
     * When PumpStateOn && touch switch 2 is LOW
     * AutoModeOn
     * PumpOff
     */
    if(P2 == HIGH && touchswitch.getSwitchState(SW2) == LOW) {
      relay.submersiblePumpOff();
      P2Start = rtc.getCurrentTime();
      P2 = LOW;
      touchswitch.setSwitch2(LOW);
      P2Auto = true;
      BTRESCODE = "OK";
    }
  }
}
void monoblockPumpMode() {
  if(!P1Auto) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PUMP1->MANUAL");
    lcd.setCursor(0,1);
    lcd.print("Touch SW1->AUTO");
    customDelayHandle(2000);
  }
}
void submersiblePumpMode() {
  if(!P2Auto) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PUMP2->MANUAL");
    lcd.setCursor(0,1);
    lcd.print("Touch SW2->AUTO");
    customDelayHandle(2000);
  }
}

TimedAction monoblockPumpAction = TimedAction(100, monoblockPumpCheck);
TimedAction submersiblePumpAction = TimedAction(100, submersiblePumpCheck);
TimedAction bluetoothAction = TimedAction(100, getBlueToothCommand);
TimedAction consoleAction = TimedAction(50, getSerialData);

void setup() {
  /*Initializes BlueTooth serial*/
  BlueTooth.begin(9600);
  while(!BlueTooth) {}
  /*Initializes serial terminal*/
  Serial.begin(9600);
  while(!Serial) {}
  /*Initializes LCD and display WELCOME message*/
  initLCD();
}

void loop() {
  displayDateTimeTemp();
  monoblockPumpStatus();
  submersiblePumpStatus();
  monoblockPumpMode();
  submersiblePumpMode();
}

void customDelayHandle(long duration) {
  for(int i=0; i<duration; i++) {
    consoleAction.check();
    bluetoothAction.check();
    monoblockPumpAction.check();
    submersiblePumpAction.check();
    delay(1);
  }
}
void initLCD() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4,0);
  lcd.print("WELCOME!");
  logonMusic();
  delay(500);
  lcd.clear();
  lcd.createChar(0, rtc.degree);
}

void displayDateTimeTemp() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(rtc.getDate(rtc.getCurrentTime()));
  lcd.setCursor(13,0);
  lcd.print(rtc.getDayOfWeek());
  lcd.setCursor(0,1);
  lcd.print(rtc.getTime(rtc.getCurrentTime()));
  lcd.setCursor(10,1);
  lcd.print(rtc.getTemperature());
  lcd.setCursor(14,1);
  lcd.write(byte(0));
  lcd.print("c");
  delay(4000);
}

/**
 * Monoblock Pump
 * Cauvery water
 */
void monoblockPumpStatus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  Cauvery Water ");
  lcd.setCursor(0,1);
  lcd.print(" Monoblock Pump ");
  customDelayHandle(2000);
  /*Read sensor values*/
  sump = sensor.getWaterLevel(SUMP);
  tank1 = sensor.getWaterLevel(OHT1);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SUMP  TANK  PUMP");
  lcd.setCursor(0,1);  lcd.print(sump);
  lcd.setCursor(3,1);  lcd.print("%");
  lcd.setCursor(6,1);  lcd.print(tank1);
  lcd.setCursor(9,1);  lcd.print("%");
  for(int i=0; i<100; i++) {
    lcd.setCursor(12,1);
    lcd.print(P1==HIGH?"ON  ":"OFF");
    customDelayHandle(30);
  }
}
/**
 * Submersible Pump
 * Bore Water
 */
void submersiblePumpStatus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Bore Water  ");
  lcd.setCursor(0,1);
  lcd.print("Submersible Pump");
  customDelayHandle(2000);
  /*Read sensor values*/
  tank2 = sensor.getWaterLevel(OHT2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TANK    UG PUMP");
  lcd.setCursor(0,1);  lcd.print(tank2);
  lcd.setCursor(3,1);  lcd.print("%");
  for(int i=0; i<100; i++) {
    lcd.setCursor(9,1);
    lcd.print(P2==HIGH?"ON  ":"OFF");
    customDelayHandle(30);
  }
}

void parseBTCommand(String command) {
  if(command == "pump1=?") {
    BlueTooth.print(String("pump1=" + String(P1==HIGH?"on":"off") + "\r"));
  }
  else if(command == "pump1=on") {
    BTRESCODE = "";
    touchswitch.setSwitch1(HIGH);
    while(BTRESCODE == ""){
      customDelayHandle(100);
    }
    BlueTooth.print(BTRESCODE);
    BlueTooth.print("\r");
  }
  else if(command == "pump1=off") {
    BTRESCODE = "";
    touchswitch.setSwitch1(LOW);
    while(BTRESCODE == ""){
      customDelayHandle(100);
    }
    BlueTooth.print(BTRESCODE);
    BlueTooth.print("\r");
  }
  else if(command == "pump2=?") {
    BlueTooth.print(String("pump2=" + String(P2==HIGH?"on":"off") + "\r"));
  }  
  else if(command == "pump2=on") {
    BTRESCODE = "";
    touchswitch.setSwitch2(HIGH);
    while(BTRESCODE == ""){
      customDelayHandle(100);
    }
    BlueTooth.print(BTRESCODE);
    BlueTooth.print("\r");
  }
  else if(command == "pump2=off") {
    BTRESCODE = "";
    touchswitch.setSwitch2(LOW);
    while(BTRESCODE == ""){
      customDelayHandle(100);
    }
    BlueTooth.print(BTRESCODE);
    BlueTooth.print("\r");
  }
  else {
    BlueTooth.print("BTC404\r");
  }
}
