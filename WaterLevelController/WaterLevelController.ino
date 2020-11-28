#include "RTC.h"
#include "PumpRelay.h"
#include "TouchSwitch.h"
#include "LiquidLevelSensor.h"
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

static int SIGLOCK=0;

volatile byte P1 = LOW, P2 = LOW;
DateTime P1StartTS, P2StartTS, P1StopTS, P2StopTS;
DateTime sumpTS, tank1TS, tank2TS;
volatile boolean P1Auto = true, P2Auto = true;
volatile int sump, tank1, tank2;
String command, BTRESCODE="";

/**
 * Gets bluetooth serial output, parses it and executes a command.
 * It echos the status to serial terminal
 */
void readBlueToothCommand() {
  if(BlueTooth.available()) {
    command = BlueTooth.readString();
    if(command == "?ping") {
      BlueTooth.print("OK\r");
    }
    else if(command == "?sump") {
      btPrintLevel(sump, sumpTS);
    }
    else if(command == "?tank1") {
      btPrintLevel(tank1, tank1TS);
    }
    else if(command == "?tank2") {
      btPrintLevel(tank2, tank2TS);
    }
    else if(command == "?pump1") {
      btPrintStatus(P1, P1Auto);
    }
    else if(command == "?pump1TS") {
      btPrintTS(P1StartTS, P1StopTS);
    }
    else if(command == "!pump1=on") {
      BTRESCODE = "";
      touchswitch.setSwitch1(HIGH);
      while(BTRESCODE == ""){
        customDelayHandle(100);
      }
      BlueTooth.print(BTRESCODE + "\r");
    }
    else if(command == "!pump1=off") {
      BTRESCODE = "";
      touchswitch.setSwitch1(LOW);
      while(BTRESCODE == ""){
        customDelayHandle(100);
      }
      BlueTooth.print(BTRESCODE + "\r");
    }
    else if(command == "!pump1=auto") {
      P1Auto = true;
      BlueTooth.print("OK\r");
    }
    else if(command == "!pump1=manual") {
      P1Auto = false;
      BlueTooth.print("OK\r");
    }
    else if(command == "?pump2") {
      btPrintStatus(P2, P2Auto);
    }
    else if(command == "?pump2TS") {
      btPrintTS(P2StartTS, P2StopTS);
    }
    else if(command == "!pump2=on") {
      BTRESCODE = "";
      touchswitch.setSwitch2(HIGH);
      while(BTRESCODE == ""){
        customDelayHandle(100);
      }
      BlueTooth.print(BTRESCODE + "\r");
    }
    else if(command == "!pump2=off") {
      BTRESCODE = "";
      touchswitch.setSwitch2(LOW);
      while(BTRESCODE == ""){
        customDelayHandle(100);
      }
      BlueTooth.print(BTRESCODE + "\r");
    }
    else if(command == "!pump2=auto") {
      P2Auto = true;
      BlueTooth.print("OK\r");
    }
    else if(command == "!pump2=manual") {
      P2Auto = false;
      BlueTooth.print("OK\r");
    }
    else {
      BlueTooth.print("BTC404\r");
    }
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
/**
 * Fail Safe defines maximum time taken (min) for each level to reach.
 * When sensors fail to send data after the pump switches ON we safely
 * OFF them to avoid water wastage. This is not an accurate time but a
 * mere fail safe mechanism.
 */
const int monoblockFailSafeTime[4] = {12,9,6,3};  //0->100:12min; 25->100:9min; 50->100:6min; 75->100:3min;
long monoblockFailSafeLimit;
const int submersibleFailSafeTime[4] = {16,12,8,4}; //0->100:16min; 25->100:12min; 50->100:8min; 75->100:4min; 
long submersibleFailSafeLimit;
/**
 * Should be called while pump is set to ON. It determines the total number of seconds it would take
 * to reach the LVLHI from the current tank LEVEL.
 */
void monoblockSetLimit() {
  switch (tank1) {
    case LVLLO:
      monoblockFailSafeLimit = monoblockFailSafeTime[0] * 60; //max_min x sec = total_seconds to run
      break;
    case LVL25:
      monoblockFailSafeLimit = monoblockFailSafeTime[1] * 60; //max_min x sec = total_seconds to run
      break;
    case LVL50:
      monoblockFailSafeLimit = monoblockFailSafeTime[2] * 60; //max_min x sec = total_seconds to run
      break;
    case LVL75:
      monoblockFailSafeLimit = monoblockFailSafeTime[3] * 60; //max_min x sec = total_seconds to run
      break;
  }
}
/**
 * A TimedAction instance is created with this function to decrement the counter every 1000 millis 
 */
void monoblockDoTimer() {
  monoblockFailSafeLimit = monoblockFailSafeLimit > 0 ? --monoblockFailSafeLimit : 0;
}
/**
 * Checks if the timer expired to safely turn off the pump
 */
boolean isMonoblockRunLimitReached() {
  return monoblockFailSafeLimit == 0 ? true : false;
}
TimedAction monoblockTimer = TimedAction(1000, monoblockDoTimer);

/**
 * Should be called while pump is set to ON. It determines the total number of seconds it would take
 * to reach the LVLHI from the current tank LEVEL.
 */
void submersibleSetLimit() {
  switch (tank2) {
    case LVLLO:
      submersibleFailSafeLimit = submersibleFailSafeTime[0] * 60; //max_min x sec = total_seconds to run
      break;
    case LVL25:
      submersibleFailSafeLimit = submersibleFailSafeTime[1] * 60; //max_min x sec = total_seconds to run
      break;
    case LVL50:
      submersibleFailSafeLimit = submersibleFailSafeTime[2] * 60; //max_min x sec = total_seconds to run
      break;
    case LVL75:
      submersibleFailSafeLimit = submersibleFailSafeTime[3] * 60; //max_min x sec = total_seconds to run
      break;
  }
}
/**
 * A TimedAction instance is created with this function to decrement the counter every 1000 millis 
 */
void submersibleDoTimer() {
  submersibleFailSafeLimit = submersibleFailSafeLimit > 0 ? --submersibleFailSafeLimit : 0;
}
/**
 * Checks if the timer expired to safely turn off the pump
 */
boolean isSubmersibleRunLimitReached() {
  return submersibleFailSafeLimit == 0 ? true : false;
}
TimedAction submersibleTimer = TimedAction(1000, submersibleDoTimer);

/**
 * Repeatedly looks for the status of sensors and perform appropriate action
 */
void monoblockPumpCheck() {
  if(SIGLOCK==0) {
    /*Acquiring lock to restrict other thread operation*/
    SIGLOCK++;
    
    /*Read sensor values*/
    if(sump != sensor.getWaterLevel(SUMP)) {
      sump = sensor.getWaterLevel(SUMP);
      sumpTS = rtc.getCurrentTime();
    }
    if(tank1 != sensor.getWaterLevel(OHT1)) {
      tank1 = sensor.getWaterLevel(OHT1);
      tank1TS = rtc.getCurrentTime();
    }
    if(P1 == HIGH)
      monoblockTimer.check();
    /**
     * When PumpStateOn && ( tank1 == Full || sump == Empty)
     * Set AutoModeOn, PumpOff
     * 
     * AUTOSTOP
     *  - OVERFLOW
     *  - SUMP EMPTY
     */
    if(P1 == HIGH && (isMonoblockRunLimitReached() || tank1 >= LVLHI || sump <= LVLLO)) {
      touchswitch.setSwitch1(LOW);
      relay.monoblockPumpOff();
      monoblockTimer.disable();
      P1StopTS = rtc.getCurrentTime();
      P1Auto = true;
      P1 = LOW;
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
        touchswitch.setSwitch1(HIGH);
        relay.monoblockPumpOn();
        monoblockSetLimit();
        monoblockTimer.reset();
        monoblockTimer.enable();
        P1StartTS = rtc.getCurrentTime();
        P1 = HIGH;
      }
      /**
       * When PumpStateOn && TouchSwitchLow && AutoModeOn
       * Set AutoModeOff, PumpOff
       * 
       * MANUALSTOP
       */
      if(P1 == HIGH && touchswitch.getSwitchState(SW1) == LOW) {
        touchswitch.setSwitch1(LOW);
        relay.monoblockPumpOff();
        monoblockTimer.disable();
        P1StopTS = rtc.getCurrentTime();
        P1Auto = false;
        P1 = LOW;
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
          touchswitch.setSwitch1(HIGH);
          relay.monoblockPumpOn();
          monoblockSetLimit();
          monoblockTimer.reset();
          monoblockTimer.enable();
          P1StartTS = rtc.getCurrentTime();
          P1 = HIGH;
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
        touchswitch.setSwitch1(HIGH);
        relay.monoblockPumpOn();
        monoblockSetLimit();
        monoblockTimer.reset();
        monoblockTimer.enable();
        P1StartTS = rtc.getCurrentTime();
        P1 = HIGH;      
        P1Auto = true;
        BTRESCODE = "OK";
      }
      if(P1 == HIGH && touchswitch.getSwitchState(SW1) == LOW) {
        relay.monoblockPumpOff();
        monoblockTimer.disable();
        P1StartTS = rtc.getCurrentTime();
        P1 = LOW;
        touchswitch.setSwitch1(LOW);
        P1Auto = true;
        BTRESCODE = "OK";
      }
    }
    /*Releasing lock for other threads*/
    SIGLOCK--;
  }
}

/**
 * Repeatedly looks for the status of sensors and perform appropriate action
 */
void submersiblePumpCheck() {
  if(SIGLOCK==0) {
    /*Acquiring lock to restrict other thread operation*/
    SIGLOCK++;

    /*Read sensor values*/
    if(tank2 != sensor.getWaterLevel(OHT2)) {
      tank2 = sensor.getWaterLevel(OHT2);
      tank2TS = rtc.getCurrentTime();
    }
    if(P2 == HIGH)
      submersibleTimer.check();
    /**
     * When tank2 >= FULL && PumpStateOn
     * AUTOSTOP
     */
    if(P2 == HIGH && (isSubmersibleRunLimitReached() || tank2 >= LVLHI)) {
      touchswitch.setSwitch2(LOW);
      relay.submersiblePumpOff();
      submersibleTimer.disable();
      P2StopTS = rtc.getCurrentTime();
      P2Auto = true;
      P2 = LOW;
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
        touchswitch.setSwitch2(HIGH);
        relay.submersiblePumpOn();
        submersibleSetLimit();
        submersibleTimer.reset();
        submersibleTimer.enable();
        P2StartTS = rtc.getCurrentTime();
        P2 = HIGH;      
      }
      /**
       * When PumpStateOn && TouchSwitchLow
       * Set AutoModeOff, PumpOff
       * 
       * MANUALSTOP
       */
      if(P2 == HIGH && touchswitch.getSwitchState(SW2) == LOW) {
        touchswitch.setSwitch2(LOW);
        relay.submersiblePumpOff();
        submersibleTimer.disable();
        P2StopTS = rtc.getCurrentTime();
        P2Auto = false;
        P2 = LOW;
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
          touchswitch.setSwitch2(HIGH);
          relay.submersiblePumpOn();
          submersibleSetLimit();
          submersibleTimer.reset();
          submersibleTimer.enable();
          P2StopTS = rtc.getCurrentTime();
          P2Auto = false;
          P2 = HIGH;        
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
        touchswitch.setSwitch2(HIGH);
        relay.submersiblePumpOn();
        submersibleSetLimit();
        submersibleTimer.reset();
        submersibleTimer.enable();
        P2StartTS = rtc.getCurrentTime();
        P2 = HIGH;      
        P2Auto = true;
        BTRESCODE = "OK";
      }
      /**
       * When PumpStateOn && touch switch 2 is LOW
       * AutoModeOn
       * PumpOff
       */
      if(P2 == HIGH && touchswitch.getSwitchState(SW2) == LOW) {
        touchswitch.setSwitch2(LOW);
        relay.submersiblePumpOff();
        submersibleTimer.disable();
        P2StartTS = rtc.getCurrentTime();
        P2 = LOW;      
        P2Auto = true;
        BTRESCODE = "OK";
      }
    }
  
    /*Releasing lock for other threads*/
    SIGLOCK--;
  }
}

TimedAction monoblockPumpAction = TimedAction(100, monoblockPumpCheck);
TimedAction submersiblePumpAction = TimedAction(100, submersiblePumpCheck);
TimedAction bluetoothAction = TimedAction(100, readBlueToothCommand);

void setup() {
  /*Initializes BlueTooth serial*/
  BlueTooth.begin(9600);
  while(!BlueTooth) {}
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
  for(long i=0; i<duration; i++) {
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
  customDelayHandle(4000);
}

/**
 * Cauvery water
 * Monoblock Pump 
 */
void cauveryWaterStatus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  Cauvery Water ");
  lcd.setCursor(1,1);  lcd.print(String("S:" + String(sump,DEC) + "%"));
  lcd.setCursor(9,1);  lcd.print(String("T:" + String(tank1,DEC) + "%"));
  customDelayHandle(2500);
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

/**
 * Bore Water
 * Submersible Pump
 */
void boreWaterStatus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Bore Water  ");
  lcd.setCursor(3,1);  lcd.print(String("Tank: " + String(tank2,DEC) + "%"));
  customDelayHandle(2500);
}
void submersiblePumpStatus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Submersible Pump");
  for(int i=0; i<10; i++) {
    lcd.setCursor(0,1);
    lcd.print(String("Pump:" + String(P2==HIGH?"ON  ":"OFF")));
    lcd.setCursor(10,1);
    lcd.print(String("Mode:" + String(P2Auto?"A":"M")));
    customDelayHandle(300);
  }
}
