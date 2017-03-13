#include <Wire.h>
#include <SevenSegPCF8574.h>

/*
 * SevenSegPCF8574 ShiftString
 * This example sketch shifts long string trough display.
 * 
 * If all address pins are connected to GND then the expander address is:
 * - 0x20 for PCF8574
 * - 0x38 for PCF8574A
 * 
 * Connect SevenSeg A to PCF8574 P0, B to P1 etc.
 */
 
SevenSegPCF8574 display = SevenSegPCF8574(0x38, 0, 1, 2, 3, 4, 5, 6, 7);

unsigned long currentMillis = 0;
float elapsedTime = 0.0;

void setup() {
  Wire.begin();
  display.addDigit(2); // Digit 1 anode connected to Arduino pin 2
  display.addDigit(3); // Digit 2 anode connected to Arduino pin 3
  display.addDigit(4); // Digit 3 anode connected to Arduino pin 4
  display.addDigit(5); // Digit 4 anode connected to Arduino pin 5
}

void loop() {
  display.shiftString("This is long string", 500);
  display.show();
}
