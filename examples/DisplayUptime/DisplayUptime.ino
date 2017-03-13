#include <Wire.h>
#include <SevenSegPCF8574.h>

/*
 * SevenSegPCF8574 DisplayUptime
 * This example sketch displays uptime of Arduino in seconds with one decimal point.
 * 
 * If all address pins are connected to GND then the expander address is:
 * - 0x20 for PCF8574
 * - 0x38 for PCF8574A
 * 
 * Connect SevenSeg A to PCF8574 P0, B to P1 etc.
 */
 
const int LED = 13;
SevenSegPCF8574 display = SevenSegPCF8574(0x38, 0, 1, 2, 3, 4, 5, 6, 7);

unsigned long currentMillis = 0;
float elapsedTime = 0.0;

void setup() {
  Wire.begin();
  display.addDigit(2); // Digit 1 anode connected to Arduino pin 2
  display.addDigit(3); // Digit 2 anode connected to Arduino pin 3
  display.addDigit(4); // Digit 3 anode connected to Arduino pin 4
  display.addDigit(5); // Digit 4 anode connected to Arduino pin 5
  pinMode(LED, OUTPUT);
}

void loop() {
  if(millis() - currentMillis >= 100) {
    currentMillis = millis();
    elapsedTime += 0.1;
    digitalWrite(LED, display.setDouble(elapsedTime)); // Let the built-in led indicate if setDouble returned true/false
  }
  display.show();
}
