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
  this -> shift = 0;
  this -> lastShift = 0;

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

uint8_t SevenSegPCF8574::getDigitCode(uint8_t digit) const {
  if(digit <= 10) return DIGITS[digit];
  return 0;
}

uint8_t SevenSegPCF8574::getLetterCode(char letter) const {
	if(letter >= 48 && letter <= 57) return getDigitCode(letter - 48);
	if(letter >= 65 && letter <= 90) letter += 32;

	switch(letter) {
		case 'a': return 0b11101110;
		case 'b': return 0b00111110;
		case 'c': return 0b10011100;
		case 'd': return 0b01111010;
		case 'e': return 0b10011110;
		case 'f': return 0b10001110;
		case 'g': return 0b11110110;
		case 'h': return 0b00101110;
		case 'i': return 0b01100000;
		case 'j': return 0b01111000;
		//case 'k': return 0b01101110;
		case 'l': return 0b00011100;
		case 'm': return 0b10101000;
		case 'n': return 0b00101010;
		case 'o': return 0b11111100;
		case 'p': return 0b11001110;
		case 'q': return 0b11100110;
		case 'r': return 0b00001010;
		case 's': return 0b10110110;
		case 't': return 0b00011110;
		case 'u': return 0b01111100;
		case 'v': return 0b00111000;
		case 'w': return 0b01010100;
		//case 'x': return 0b01101110;
		case 'y': return 0b01100110;
		case 'z': return 0b11011010;
		
		case ' ': return 0b00000000;
		case '.':
		case ',': return 0b00000001;
	}

	return 0b00010000;
}

bool SevenSegPCF8574::setDigit(uint8_t index, uint8_t value, bool displayDecimalPoint) {
  if(!setDigit(index, value)) return false;
  setDecimalPoint(index, displayDecimalPoint);
  return true;
}

bool SevenSegPCF8574::setDigit(uint8_t index, uint8_t value) {
  if(index >= this -> digitsCount) return false;
  this -> digits[index].state = value;
  return true;
}

bool SevenSegPCF8574::setDecimalPoint(uint8_t index, bool displayDecimalPoint) {
  if(index >= this -> digitsCount) {
    return false;
  }
  if(displayDecimalPoint) {
    this -> digits[index].state |= 0b00000001;
  } else {
    this -> digits[index].state &= 0b11111110;
  }
}

bool SevenSegPCF8574::setDouble(double number, uint8_t decimalPoints) {
  bool result;
  if(number == 0.0) {
  	result = setInteger(0, decimalPoints + 1);
  } else if(number < 1.0) {
  	result = setInteger((int) (number * pow(10, decimalPoints)), max(decimalPoints, 1));
  } else {
  	result = setInteger((int) (number * pow(10, decimalPoints)));
  }

  if(decimalPoints > 0) {
  	result = result && setDecimalPoint(this -> digitsCount - decimalPoints - 1, true);
  }
  return result;
}

bool SevenSegPCF8574::setDouble(double number) {
  return setDouble(number, 1);
}

bool SevenSegPCF8574::setInteger(int number) {
  return setInteger(number, 0);
}

bool SevenSegPCF8574::setInteger(int number, uint8_t leadingZeros) {
  bool sign = false;
  if(number < 0) {
  	number *= -1;
  	sign = true;
  }

  int x;
  for(x = this -> digitsCount - 1; x >= 0 && number != 0; x--) {
    setDigit(x, getDigitCode(number % 10));
    number /= 10;
  }

  while(leadingZeros > 0 && x >= 0) {
  	setDigit(x--, getDigitCode(0));
  	leadingZeros--;
  }

  if(x >= 0 && sign) {
  	setDigit(x--, MINUS);
  }

  while(x >= 0) {
  	setDigit(x--, getDigitCode(10));
  }

  return number <= 0;
}


bool SevenSegPCF8574::setString(String text) {
	int x;
	for(x = 0; x < text.length() && x < getDigitsCount(); x++) {
		setDigit(x, getLetterCode(text.charAt(x)));
	}

	while(x < getDigitsCount()) {
		setDigit(x++, 0);
	}

	return text.length() <= getDigitsCount();
}

void SevenSegPCF8574::shiftString(String text, int duration) {
  if(text.length() <= getDigitsCount()) {
  	setString(text);
  	return;
  }

  if(millis() - lastShift >= duration) {
  	for(int x = 0; x < getDigitsCount(); x++) text += " ";
  	text = text.substring(shift++);

  	if(text.length() == 0) shift = 0;
  	setString(text);
  	lastShift = millis();
  }
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
    
    delay(2);
  }
}