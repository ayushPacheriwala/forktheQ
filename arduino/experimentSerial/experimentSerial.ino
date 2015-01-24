#include <hardSerialLCD.h>

HardSerialLCD LCD;

void setup() {
  LCD.begin();
}

void loop() {
  LCD.clear();
  delay(10);
  LCD.print('n');
  delay(1000);
  LCD.print('m');
  delay(1000);
}
