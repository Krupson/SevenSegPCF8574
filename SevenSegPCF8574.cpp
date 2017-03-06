#include <Arduino.h>
#include <Wire.h>
#include <SevenSegPCF8574.h>

const uint8_t SevenSegPCF8574::DIGITS[11] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11110110, // 9
  0b00000000  // NONE
};

const uint8_t SevenSegPCF8574::MINUS = 0b00000010;

SevenSegPCF8574::SevenSegPCF8574(uint8_t address, uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t E, uint8_t F, uint8_t G, uint8_t DP) {
  this -> address = address;
  this -> digitsCount = 0;
  this -> digits = NULL;
  
  this -> expanderPins[0] = A;
  this -> expanderPins[1] = B;
  this -> expanderPins[2] = C;
  this -> expanderPins[3] = D;
  this -> expanderPins[4] = E;
  this -> expanderPins[5] = F;
  this -> expanderPins[6] = G;
  this -> expanderPins[7] = DP;
  
  Wire.begin();
}

bool SevenSegPCF8574::addDigit(uint8_t pin) {
  if(this -> digits == NULL) {
    this -> digits = (SevenSegPCF8574_Digit *) calloc(4, sizeof(SevenSegPCF8574_Digit));
    if(this -> digits == NULL) {
      return false;
    }
  } else if(this -> digitsCount >= 32) {
    return false;
  } else if(this -> digitsCount >= sizeof(this -> digits) / sizeof(SevenSegPCF8574_Digit)) {
    void * temp = realloc(this -> digits, (sizeof(SevenSegPCF8574_Digit) * this -> digitsCount) + 4);
    if(temp == NULL) {
      return false;
    }
    this -> digits = (SevenSegPCF8574_Digit *) temp;
  }

  for(uint8_t x = 0; x < this -> digitsCount; x++) {
    if(this -> digits[x].pin == pin) return false;
  }
  
  this -> digits[this -> digitsCount].pin = pin;
  this -> digits[this -> digitsCount].state = 0;
  this -> digitsCount++;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  return true;
}

uint8_t SevenSegPCF8574::getDigitsCount() const {
  return this -> digitsCount;
}

bool SevenSegPCF8574::setDigit(uint8_t index, uint8_t value, bool displayDecimalPoint) {
  if(!setDigit(index, value)) return false;
  setDecimalPoint(index, displayDecimalPoint);
  return true;
}

bool SevenSegPCF8574::setDigit(uint8_t index, uint8_t value) {
  if(index >= this -> digitsCount) {
    return false;
  }
  this -> digits[index].state = value;
  return true;
}

bool SevenSegPCF8574::setDecimalPoint(uint8_t index, bool isOn) {
  if(index >= this -> digitsCount) {
    return false;
  }
  if(isOn) {
    this -> digits[index].state |= 0b00000001;
  } else {
    this -> digits[index].state &= 0b11111110;
  }
}

bool SevenSegPCF8574::setDouble(double number, uint8_t decimalPoints) {
  bool result = setInteger((int) (number * pow(10, decimalPoints)));
  if(decimalPoints > 0) {
  	result = result && setDecimalPoint(this -> digitsCount - decimalPoints - 1, true);
  }
  return result;
}

bool SevenSegPCF8574::setInteger(int number) {
  bool sign = false;
  if(number < 0) {
  	number *= -1;
  	sign = true;
  }

  int x;
  for(x = this -> digitsCount - 1; x >= 0 && number != 0; x--) {
    setDigit(x, DIGITS[number % 10]);
    number /= 10;
  }

  if(x >= 0 && sign) {
  	setDigit(x--, MINUS);
  }

  for(x; x >=0; x--) {
  	setDigit(x, DIGITS[10]);
  }

  return number <= 0;
}

void SevenSegPCF8574::show() {
  for(int x = 0; x < this -> digitsCount; x++) {
    Wire.beginTransmission(this -> address);
    Wire.write(0b11111111);
    Wire.endTransmission();
    
    for(int y = 0; y < this -> digitsCount; y++) {
      digitalWrite(this -> digits[y].pin, x == y);
    }

    uint8_t code = 0;
    bool tempBit = 0;
    for(int y = 0; y < 8; y++) {
      tempBit = (this -> digits[x].state >> y) & 1;
      code ^= (-tempBit ^ code) & (1 << expanderPins[7-y]);
    }
    
    Wire.beginTransmission(this -> address);
    Wire.write(~code);
    Wire.endTransmission();
    
    delay(3);
  }
}

void SevenSegPCF8574::printCode(int index) {
  Serial.println(this -> digits[index].state, BIN);
}