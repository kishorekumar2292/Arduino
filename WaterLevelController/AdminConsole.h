#ifndef AdminConsole_h
#define AdminConsole_h

boolean serialconnected = false;
boolean adminConsoleisActive = false;

void printAdminConsoleMenu() {
  Serial.println("****Welcome to PC Admin Console****");
  Serial.println("1. System Date/Time");
  Serial.println("2. Monoblock Pump and Tank Status");
  Serial.println("3. Submersible Pump and Tank Status");
  Serial.println("4. Exit");
  Serial.print("Enter option: ");
}
#endif
