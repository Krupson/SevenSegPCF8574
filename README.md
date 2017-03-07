# SevenSegPCF8574
An open-source Arduino/ESP8266 library to interface multiple seven-segment displays with PCF8574 expander.

# How to use
### Partslist
* Arduino Uno / ESP8266 (NodeMCU)
* PCF8574 expander
* SevenSegment display (necessarily with common anode)

### Wiring
1. Connect PCF8574 I/O pins to your display's pins A,B,C,D,E,F,G,DP through current limiting resistors.
2. Connect anodes to any of microcontroller's digital pins.

# Basic methods
### SevenSegPCF8574(uint8_t address, uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t E, uint8_t F, uint8_t G, uint8_t DP)
Object constructor. You have to specify I2C address that is used by your expander and PCF8574's pin numbers wired to display's cathodes.

### bool addDigit(uint8_t pin)
By default no digits are specified. You have to manually assiociate digit index with corresponding uC pin. If everything goes fine method returns true. Note that at first you have to add the most significant digit.

### bool setDigit(uint8_t index, uint8_t value, bool displayDecimalPoint)
You can set all the digits separately if you want. All you have to do is to give digit index and value coded in binary (see **Encoding** section). Optionally you can light up decimal point by setting displayDecimalPoint to true.
Returns true if everything is ok.

### bool setDouble(double number, uint8_t decimalPoints)
Display double value. You can specify number and decimal points (default is 1). Returns true if whole number fits the display.

### bool setInteger(int number)
Just like setDouble but with integers. Returns true if whole number fits the display.

### bool setString(String text)
Display text. Returns true if whole text fits the display.

### void shiftString(String text, int duration)
Like setString, but if text doesn't fit the display it is shifted left every *duration* milliseconds. This method has to be executed in loop() without any delays between iterations.

### void show()
This method drives the display (multiplexing). This method has to be executed in loop() without any delays between iterations.

# Encoding
As a **value** parameter you can pass one of predefined symbols or define your own.

### Using predefined symbols
Just call getDigitCode(uint8_t digit) or getLetterCode(char letter) to get the digit/letter code. Then you can pass result as *value* to setDigit method.
To get digit code you can also use SevenSegPCF8574::DIGITS[x] constant array where x is the digit you want do display. If x == 10 then the whole digit goes black.
There is also a constant called MINUS which displays minus sign.

### Define your own symbols
Encoding is very simple. You have 8 bit unsigned integer (uint8_t) and 8 LEDs per digit to drive.
The most significant bit drives segment A. The least significant bit drives segment DP.
* Bit 7 <-> A
* Bit 6 <-> B
* Bit 5 <-> C
* Bit 4 <-> D
* Bit 3 <-> E
* Bit 2 <-> F
* Bit 1 <-> G
* Bit 0 <-> DP

If bit for specific segment is set to 1 then it will light up. Otherwise it will go black.

*For example to light up only segments D and G on most significant digit just call*
```
display.setDigit(0, 0b00010010);
```
