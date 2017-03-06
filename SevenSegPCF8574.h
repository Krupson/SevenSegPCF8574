#ifndef SevenSegPCF8574_h
#define SevenSegPCF8574_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

typedef struct {
  uint8_t pin;
  uint8_t state;
} SevenSegPCF8574_Digit;

class SevenSegPCF8574 {
  protected:
    uint8_t expanderPins[8];
    SevenSegPCF8574_Digit * digits;
    uint8_t digitsCount;
    uint8_t address;

  public:
    static const uint8_t DIGITS[11];
    static const uint8_t MINUS;
  
    SevenSegPCF8574(uint8_t address, uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t E, uint8_t F, uint8_t G, uint8_t DP);

    bool addDigit(uint8_t pin);

    uint8_t getDigitsCount() const;

    bool setDigit(uint8_t index, uint8_t value, bool displayDecimalPoint);

    bool setDigit(uint8_t index, uint8_t value);

    bool setDecimalPoint(uint8_t index, bool isOn);

    bool setDouble(double number, uint8_t decimalPoints);

    bool setInteger(int number);

    void show();

    void printCode(int index);
};

#endif