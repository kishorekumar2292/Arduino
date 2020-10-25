

int pin = A0;
int val;

void setup() {
  Serial.begin(9600);

}

void loop() {
  if(Serial.available()) {
    Serial.read();
    val = analogRead(pin);
    Serial.println(val);
  }
}
