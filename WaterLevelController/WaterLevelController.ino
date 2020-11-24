#include "RTC.h"
#include "PumpRelay.h"
#include "TouchSwitch.h"
#include "LiquidLevelSensor.h"
#include "AdminConsole.h"
#include "Themes.h"
#include "DS3231.h"

#include <TimedAction.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial BlueTooth(4, 5);

LiquidLevelSensor sensor;
TouchSwitch touchswitch;
RealTimeClock rtc;
PumpRelay relay;

volatile byte P1 = LOW, P2 = LOW;
DateTime P1StartTS, P2StartTS, P1StopTS, P2StopTS;
DateTime sumpTS, tank1TS, tank2TS;
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
  /*
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
        Serial.print(rtc.getTimeStampStr(rtc.getCurrentTime()));
        Serial.print(", ");
        Serial.print(rtc.getDayOfWeek());
        Serial.print(" | ");
        Serial.print(rtc.getTemperature());
        Serial.println(" ^c");
        Serial.println("");
        break;
      case '2':
        Serial.println(String("Sump:" + String(sump) + "% | Tank:" + String(tank1) + "%"));
        Serial.println(String("Pump Mode:" + String(P1Auto ? "Auto" : "Manual") + " | Pump Status:" + String(P1 == HIGH ? "ON" : "OFF")));
        Serial.println("");
        break;
      case '3':
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
  */
}
/**
 * Repeatedly looks for the status of sensors and perform appropriate action
 */
void monoblockPumpCheck() {
  /*Read sensor values*/
  if(sump != sensor.getWaterLevel(SUMP)) {
    sump = sensor.getWaterLevel(SUMP);
    sumpTS = rtc.getCurrentTime();
  }
  if(tank1 != sensor.getWaterLevel(OHT1)) {
    tank1 = sensor.getWaterLevel(OHT1);
    tank1TS = rtc.getCurrentTime();
  }
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
    P1StopTS = rtc.getCurrentTime();
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
    if(tank1 > LVLLO && tank1 < LVL50 && P1 == LOW && sump > LVLLO) {
      relay.monoblockPumpOn();
      P1StartTS = rtc.getCurrentTime();
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
      P1StopTS = rtc.getCurrentTime();
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
        P1StartTS = rtc.getCurrentTime();
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
      P1StartTS = rtc.getCurrentTime();
      P1 = HIGH;
      touchswitch.setSwitch1(HIGH);
      P1Auto = true;
      BTRESCODE = "OK";
    }
    if(P1 == HIGH && touchswitch.getSwitchState(SW1) == LOW) {
      relay.monoblockPumpOff();
      P1StartTS = rtc.getCurrentTime();
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
  if(tank2 != sensor.getWaterLevel(OHT2)) {
    tank2 = sensor.getWaterLevel(OHT2);
    tank2TS = rtc.getCurrentTime();
  }
  /**
   * When tank2 >= FULL && PumpStateOn
   * AUTOSTOP
   */
  if(tank2 >= LVLHI && P2 == HIGH) {
    relay.submersiblePumpOff();
    P2StopTS = rtc.getCurrentTime();
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
    if(tank2 > LVLLO && tank2 < LVL50 && P2 == LOW) {
      relay.submersiblePumpOn();
      P2StartTS = rtc.getCurrentTime();
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
      P2StopTS = rtc.getCurrentTime();
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
        P2StopTS = rtc.getCurrentTime();
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
      P2StartTS = rtc.getCurrentTime();
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
      P2StartTS = rtc.getCurrentTime();
      P2 = LOW;
      touchswitch.setSwitch2(LOW);
      P2Auto = true;
      BTRESCODE = "OK";
    }
  }
}
void monoblockPumpStatus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Monoblock Pump ");
  for(int i=0; i<10; i++) {
    lcd.setCursor(0,1);
    lcd.print(String("Pump:" + String(P1==HIGH?"ON  ":"OFF")));
    lcd.setCursor(10,1);
    lcd.print(String("Mode:" + String(P1Auto?"A":"M")));
    customDelayHandle(300);
  }
}
void submersiblePumpStatus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Submersible Pump");
  for(int i=0; i<10; i++) {
    lcd.setCursor(0,1);
    lcd.print(String("Pump:" + String(P2==HIGH?"ON  ":"OFF")));
    lcd.setCursor(10,1);
    lcd.print(String("Mode:" +String(P2Auto?"A":"M")));
    customDelayHandle(300);
  }
}

TimedAction monoblockPumpAction = TimedAction(100, monoblockPumpCheck);
TimedAction submersiblePumpAction = TimedAction(100, submersiblePumpCheck);
TimedAction bluetoothAction = TimedAction(100, getBlueToothCommand);
//TimedAction consoleAction = TimedAction(50, getSerialData);

void setup() {
  /*Initializes BlueTooth serial*/
  BlueTooth.begin(9600);
  while(!BlueTooth) {}
  /*Initializes serial terminal*/
  //Serial.begin(9600);
  //while(!Serial) {}
  /*Initializes LCD and display WELCOME message*/
  initLCD();

  P1StartTS = P1StopTS = P2StartTS = P2StopTS = rtc.getCurrentTime();
}

void loop() {
  displayDateTimeTemp();
  cauveryWaterStatus();
  monoblockPumpStatus();
  boreWaterStatus();  
  submersiblePumpStatus();
}

void customDelayHandle(long duration) {
  for(int i=0; i<duration; i++) {
    //consoleAction.check();
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
  DateTime now = rtc.getCurrentTime();
  lcd.setCursor(0,0);
  lcd.print(rtc.getDateStr(now));
  lcd.setCursor(13,0);
  lcd.print(rtc.getDayOfWeek());
  lcd.setCursor(0,1);
  lcd.print(rtc.getTimeStr(now));
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
void cauveryWaterStatus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  Cauvery Water ");
  lcd.setCursor(1,1);  lcd.print(String("S:" + String(sump,DEC) + "%"));
  lcd.setCursor(9,1);  lcd.print(String("T:" + String(tank1,DEC) + "%"));
  customDelayHandle(2500);
}
/**
 * Submersible Pump
 * Bore Water
 */
void boreWaterStatus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Bore Water  ");
  lcd.setCursor(3,1);  lcd.print(String("Tank: " + String(tank2,DEC) + "%"));
  customDelayHandle(2500);
}

void parseBTCommand(String command) {
  if(command == "ping") {
    BlueTooth.print("OK\r");
  }
  else if(command == "sump") {
    btPrintLevel(sump, sumpTS);
  }
  else if(command == "tank1") {
    btPrintLevel(tank1, tank1TS);
  }
  else if(command == "tank2") {
    btPrintLevel(tank2, tank2TS);
  }
  else if(command == "pump1=?") {
    btPrintStatus(P1, P1Auto);
  }
  else if(command == "pump1TS") {
    btPrintTS(P1StartTS, P1StopTS);
  }
  else if(command == "pump1=on") {
    BTRESCODE = "";
    touchswitch.setSwitch1(HIGH);
    while(BTRESCODE == ""){
      customDelayHandle(100);
    }
    BlueTooth.print(BTRESCODE + "\r");
  }
  else if(command == "pump1=off") {
    BTRESCODE = "";
    touchswitch.setSwitch1(LOW);
    while(BTRESCODE == ""){
      customDelayHandle(100);
    }
    BlueTooth.print(BTRESCODE + "\r");
  }
  else if(command == "pump1=auto") {
    P1Auto = true;
    BlueTooth.print("OK\r");
  }
  else if(command == "pump1=manual") {
    P1Auto = false;
    BlueTooth.print("OK\r");
  }
  else if(command == "pump2=?") {
    btPrintStatus(P2, P2Auto);
  }
  else if(command == "pump2TS") {
    btPrintTS(P2StartTS, P2StopTS);
  }
  else if(command == "pump2=on") {
    BTRESCODE = "";
    touchswitch.setSwitch2(HIGH);
    while(BTRESCODE == ""){
      customDelayHandle(100);
    }
    BlueTooth.print(BTRESCODE + "\r");
  }
  else if(command == "pump2=off") {
    BTRESCODE = "";
    touchswitch.setSwitch2(LOW);
    while(BTRESCODE == ""){
      customDelayHandle(100);
    }
    BlueTooth.print(BTRESCODE + "\r");
  }
  else if(command == "pump2=auto") {
    P2Auto = true;
    BlueTooth.print("OK\r");
  }
  else if(command == "pump2=manual") {
    P2Auto = false;
    BlueTooth.print("OK\r");
  }
  else {
    BlueTooth.print("BTC404\r");
  }
}

void btPrintLevel(int level, DateTime timestamp) {
  BlueTooth.print("Level=" + String(level,DEC) + ",StatusTS=" + rtc.getTimeStampStr(timestamp) + "\r");
}

void btPrintTS(DateTime start, DateTime stop) {
  BlueTooth.print("StartTS=" + rtc.getTimeStampStr(start) + ",StopTS=" + rtc.getTimeStampStr(stop) + "\r");
}

void btPrintStatus(byte state, boolean mode) {
  BlueTooth.print("State=" + String(state==HIGH?"ON":"OFF") + ",Mode=" + String(mode?"A":"M") + "\r");
}
