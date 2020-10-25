#ifndef AdminConsole_h
#define AdminConsole_h

boolean serialconnected = false;
boolean adminConsoleisActive = false;

void printAdminConsoleMenu() {
  Serial.println("****Welcome to PC Admin Console****");
  Serial.println("1. System Date/Time");
  Serial.println("2. Pump 1");
  Serial.println("3. Pump 2");
  Serial.println("4. Exit");
  Serial.print("Enter option: ");
}
#endif
